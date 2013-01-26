#include "PCH.h"
#include "TextBox.h"
#include "cWidgetRenderer.h"
#include "cWidgetSkin.h"
#include "Text.h"
#include "Button.h"

MY_UI::Controls::TextBox::TextBox(cWidgetBase* parent): cWidgetBase(parent) { 
	SetControlBounds(Utilities::Rect(0, 0, static_cast<int>(MY_UI::Internal::WidgetSkin->TextBox.width), static_cast<int>(MY_UI::Internal::WidgetSkin->TextBox.height)));
	SetClientBounds(Utilities::Rect(0, 0, static_cast<int>(MY_UI::Internal::WidgetSkin->TextBox.width-3.0f), static_cast<int>(MY_UI::Internal::WidgetSkin->TextBox.height-3.0f)));

	Box= new MY_UI::Controls::Button(this);
	Box->Text->SetText("");
	Box->Set_UnPressed_Texture(MY_UI::Internal::WidgetSkin->TextBox);
	Box->Set_Hovered_Texture(MY_UI::Internal::WidgetSkin->TextBox_Hovered);
	Box->SetImageBorderSize(2, 2);
	Box->OnKeyDown.Add(std::bind(&MY_UI::Controls::TextBox::KeyDown, this));
	Box->OnHoverEnter.Add(std::bind(&MY_UI::Controls::TextBox::Toggle_TextCursor, this));
	Box->OnHoverExit.Add(std::bind(&MY_UI::Controls::TextBox::Toggle_TextCursor, this));
	Box->OnLeftClicked.Add(std::bind(&MY_UI::Controls::TextBox::Move_Caret_To_Mouse, this));
	Box->OnLeftDoubleClick.Add(std::bind(&MY_UI::Controls::TextBox::Select_All_Text, this));

	SetClientBounds(Utilities::Rect(3, 3, static_cast<int>(MY_UI::Internal::WidgetSkin->TextBox.width-3.0f), static_cast<int>(MY_UI::Internal::WidgetSkin->TextBox.height-3.0f)));

	Text = new MY_UI::Controls::Text(this);
	SetSizeToContents();	
}
MY_UI::Controls::TextBox::~TextBox(){ 
	SetFocus(false);
}
void MY_UI::Controls::TextBox::SetText(std::string txt){
	Text->SetText(txt);
	Text->SetPos(0,0);
	Internal_TextBox.CaretIndex =0;
	UpdateCaret();
}
std::string MY_UI::Controls::TextBox::GetText() const {
	return Text->GetText();
}
void MY_UI::Controls::TextBox::SetFocus(bool focus){
	MY_UI::Controls::cWidgetBase::SetFocus(focus);
	auto root = MY_UI::Internal::RootWidget;
	if(focus) {
		root->Caret->Internal_Widget.Parent = this;
		Move_Caret_To_Index();
		root->Caret->SetHidden(false);
	} else {
		if(root->Caret->Internal_Widget.Parent == this){
			root->Caret->Internal_Widget.Parent = 0;
			DeCouple(root->Caret);
			root->Caret->SetHidden(true);
		}
		Internal_TextBox.CaretTimer =0;
	}
}

void MY_UI::Controls::TextBox::SetSize(int x, int y){
	MY_UI::Controls::cWidgetBase::SetSize(x, y);
	Box->SetSize(x, y);
	Text->SetSize(x, y);
}

void MY_UI::Controls::TextBox::KeyDown(){
	auto skin = MY_UI::Internal::WidgetSkin;
	Utilities::Point p, tpos;
	if(Current_Key == KEY_LEFT) Move_Caret_Left();
	else if(Current_Key == KEY_RIGHT) Move_Caret_Right();
	else {
		if(   ( (Current_Key >= KEY_0) & (Current_Key <= KEY_9) ) | ((Current_Key >= KEY_A) & (Current_Key <= KEY_Z)) | (Current_Key == KEY_SPACE) ){
			std::string t = Text->GetText();
			if(static_cast<int>(t.size()) +1 >= Internal_TextBox.MaxCharacters) return;// do not allow more characters
			char c;
			if(ShiftDown | CapsLock) c = UpperChars[Current_Key];
			else c = LowerChars[Current_Key];
			t.insert(t.begin() + Internal_TextBox.CaretIndex, c);
			Text->SetText(t);
			Move_Caret_Right();
		}else if (Current_Key == KEY_BACK){
			std::string t = Text->GetText();
			if(Internal_TextBox.CaretIndex == 0 || t.size() == 0) return;
			Move_Caret_Left();// move before the deletion
			t.erase(t.begin() + Internal_TextBox.CaretIndex);// erase the character from the string of text
			Text->SetText(t);// set the text
		}else if (Current_Key == KEY_DELETE){
			std::string t = Text->GetText();
			if(Internal_TextBox.CaretIndex == t.size()) return;
			t.erase(t.begin() + Internal_TextBox.CaretIndex);
			Text->SetText(t);
			UpdateCaret();
		}
	}
	if(Current_Key == KEY_RETURN) OnReturnPressed.Call(this);
	MY_UI::Controls::cWidgetBase::KeyUp();
	
}


void MY_UI::Controls::TextBox::Draw_Clip(){
	Box->Draw_NoClip();

	Utilities::Rect& clip = Internal_Widget.Clientpos;// this is the clip bounds of this window
	MY_UI::Internal::WidgetRenderer->StartClip(clip);
	
	if(Internal_TextBox.PasswordBox){
		std::string ast;
		for(size_t i =0; i< Text->GetText().size(); i++){
			ast.push_back('*');
		}
		std::string copy(Text->GetText());
		Text->SetText(ast);
		Text->Draw_Clip();
		Text->SetText(copy);
	} else Text->Draw_Clip();

	if(GetFocus()){// the caret should be drawn
		Image* caret = MY_UI::Internal::RootWidget->Caret;
		if(caret->GetHidden()){// the caret is hidden 
			if(Utilities::GetTime() - Internal_TextBox.CaretTimer > CARETFLASHRATE/3){// set the Caret to not hidden
				caret->SetHidden(false);
				Internal_TextBox.CaretTimer = Utilities::GetTime();
			}
		} else {
			if(Utilities::GetTime() - Internal_TextBox.CaretTimer > CARETFLASHRATE){// set the Caret to hidden
				caret->SetHidden(true);
				Internal_TextBox.CaretTimer = Utilities::GetTime();
			}
		}
		caret->SetColor(Utilities::Color(255, 0, 0, 0));
		caret->Draw_NoClip();
	}	
	MY_UI::Internal::WidgetRenderer->EndClip();
}
void MY_UI::Controls::TextBox::Draw_NoClip(){
	Draw_Clip();// textboxes should be drawn with clipping for text overflow
}
void MY_UI::Controls::TextBox::SetSelectable(bool selectedable){
	Text->SetSelectable(selectedable);
	Box->OnLeftDoubleClick.Add(std::bind(&MY_UI::Controls::TextBox::Select_All_Text, this));
}
void MY_UI::Controls::TextBox::Move_Caret_To_Mouse(){
	Internal_TextBox.CaretIndex =0;
	float curpos = static_cast<float>(Text->Internal_Widget.AbsolutePosition.left);// get the absolute position
	auto skin = MY_UI::Internal::WidgetSkin;
	const std::string& st = Text->GetText();
	while(Internal_TextBox.CaretIndex < st.size()){
		if(curpos > New_MousePosx) {
			Move_Caret_Left();
			break;// stop here 
		}
		curpos += skin->GetCharacterWidth(st[Internal_TextBox.CaretIndex], Text->GetFontSize());
		Move_Caret_Right();
	}
	UpdateCaret();
}
void MY_UI::Controls::TextBox::Move_Caret_To_Index(){
	Internal_TextBox.CaretIndex = Utilities::Clamp<unsigned int>(Internal_TextBox.CaretIndex, 0, static_cast<unsigned int>(Text->GetText().size()));
	UpdateCaret();
}

void MY_UI::Controls::TextBox::Move_Caret_Left(){
	if(Internal_TextBox.CaretIndex ==0) return;
	int prev = Internal_TextBox.CaretIndex;
	Internal_TextBox.CaretIndex = Utilities::Clamp<unsigned int>(Internal_TextBox.CaretIndex - 1, 0, static_cast<unsigned int>(Text->GetText().size()));// the caret can go to one passed the end
	auto skin = MY_UI::Internal::WidgetSkin;
	Utilities::Point cpos = MY_UI::Internal::RootWidget->Caret->GetPos();
	Utilities::Point tpos = Text->GetPos();

	if(cpos.x <= 4){// only move the text if the caret is at the beginning of the box
		Utilities::Point p = skin->GetCharacterSize(Text->GetText()[Internal_TextBox.CaretIndex], Text->GetFontSize());
		Text->SetPos(tpos.x + p.left, tpos.y);// move the text to the left	
	}
	
	UpdateCaret();
}
void MY_UI::Controls::TextBox::Move_Caret_Right(){
	if(Internal_TextBox.CaretIndex == Text->GetText().size()) return;// already at the end
	auto skin = MY_UI::Internal::WidgetSkin;
	int prev = Internal_TextBox.CaretIndex;
	Internal_TextBox.CaretIndex = Utilities::Clamp<unsigned int>(Internal_TextBox.CaretIndex + 1, 0, static_cast<unsigned int>(Text->GetText().size()));// the caret can go to one passed the end
	
	Utilities::Point oldcaretpos = MY_UI::Internal::RootWidget->Caret->GetPos();
	UpdateCaret();// move the caret
	Utilities::Point newcaretpos = MY_UI::Internal::RootWidget->Caret->GetPos();
	Utilities::Point boxsize = Box->GetSize();
	if(newcaretpos.x >= boxsize.x - 4){// if the caret goes outside the box, move the text by the size of the caracter at index -1
		Utilities::Point p = skin->GetCharacterSize(Text->GetText()[Internal_TextBox.CaretIndex-1], Text->GetFontSize());

		Utilities::Point textpos = Text->GetPos();
		textpos.x -= p.x;
		Text->SetPos(textpos);// move the text to the left
		UpdateCaret();
	}
}
void MY_UI::Controls::TextBox::Select_All_Text(){

}
void MY_UI::Controls::TextBox::UpdateCaret(){
	std::string st;
	Internal_TextBox.CaretIndex = Utilities::Clamp<unsigned int>(Internal_TextBox.CaretIndex, 0, static_cast<unsigned int>(Text->GetText().size()));
	if(Internal_TextBox.PasswordBox){
		for(auto beg = Text->GetText().begin(); beg != Text->GetText().begin() + Internal_TextBox.CaretIndex; beg++){
			st.push_back('*');
		}
	} else {
		for(auto beg = Text->GetText().begin(); beg != Text->GetText().begin() + Internal_TextBox.CaretIndex; beg++){
			st.push_back(*beg);
		}
	}

	auto skin = MY_UI::Internal::WidgetSkin;
	Utilities::Point p = skin->GetTextSize(st, Text->GetFontSize());
	auto root = MY_UI::Internal::RootWidget;
	root->Caret->SetPos(Text->GetPos().x + 1 + p.left, 0);

}
void MY_UI::Controls::TextBox::Toggle_TextCursor(){
	if(Utilities::GetCursor() == Utilities::Text) Utilities::SetCursor(Utilities::Standard);
	else Utilities::SetCursor(Utilities::Text);
}

