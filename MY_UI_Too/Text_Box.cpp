#include "PCH.h"
#include "Text_Box.h"
#include "ISkin.h"
#include "Text.h"
#include "IInput.h"
#include "Common.h"

MY_UI_Too::Controls::Text_Box::Text_Box(IWidget* parent): Widget(parent) {
	_Carret_Index=0;
	Set_Size(Utilities::Point(162, 29));

	Set_Name("Text_Box");
	Text=new MY_UI_Too::Controls::Text(this);
	Text->Set_Font_Size(20);
	Text->Set_Text("");
	Text->Set_Pos(Utilities::Point(5, 5));
	Text->Set_Hitable(false);

}
void MY_UI_Too::Controls::Text_Box::Set_Focus(bool focus){
	MY_UI_Too::Controls::Widget::Set_Focus(focus);
	if(focus) _Timer.Start();
}
void  MY_UI_Too::Controls::Text_Box::Dec_Caret(){
	if(_Carret_Index<=0) return;
	_Carret_Index-=1;
}
void  MY_UI_Too::Controls::Text_Box::Inc_Caret(){
	if(_Carret_Index>=Text->Get_Text().size()) return;
	_Carret_Index+=1;
}

void MY_UI_Too::Controls::Text_Box::Key_Up(){
	MY_UI_Too::Controls::Widget::Key_Up();
	switch(Current_Key){
	case(8)://backspace
		BackSpace();
		break;
	case(9)://tab
		Tab();
		break;
	case(10)://newline
		NewLine();
		break;
	case(13)://Execute
		Enter();
		break;
	case(KEY_DELETE)://delete key
		Delete();
		break;
	case(KEY_LEFT):
		Dec_Caret();
		break;
	case(KEY_RIGHT):
		Inc_Caret();
		break;
	case(KEY_UP):
	case(KEY_DOWN):
		break;
	default:
		if(Current_Key>=(START_CHAR-1) && Current_Key<= END_CHAR) {// bounds check
			std::string str = Text->Get_Text();
			str.insert(str.begin()+_Carret_Index, (char)Current_Key);
			Text->Set_Text(str);
			_Carret_Index+=1;
		} 
		break;
	};
}

void MY_UI_Too::Controls::Text_Box::Mouse_Entered(){
	MY_UI_Too::Controls::Widget::Mouse_Entered();
	Internal::Input->SetCursor(Cursor_Types::Text);
}

void MY_UI_Too::Controls::Text_Box::Draw(MY_UI_Too::Interfaces::ISkin* skin){
	skin->Draw_Text_Box(Get_Focus(), Utilities::Rect(_Internals.Absolute_TL.x , _Internals.Absolute_TL.y, _Internals.Size.x, _Internals.Size.y) );
	Text->Draw(skin);
	static bool lastloopcaret = true;
	if(Get_Focus()) {
		if(_Timer.Advance()) lastloopcaret =!lastloopcaret;
		if(lastloopcaret) Text->Draw_Caret(skin, _Carret_Index);
	}
}

void MY_UI_Too::Controls::Text_Box::BackSpace(){
	if(_Carret_Index==0) return;
	std::string t = Text->Get_Text();
	if(t.size()==0) return;
	t.erase(_Carret_Index-1, 1);
	Text->Set_Text(t);
	_Carret_Index-=1;
}
void MY_UI_Too::Controls::Text_Box::Tab(){
	Text->Set_Text(Text->Get_Text()+(char)Current_Key);
	_Carret_Index+=1;
}
void MY_UI_Too::Controls::Text_Box::Delete(){
	std::string t = Text->Get_Text();
	if(t.size()==0) return;
	t.erase(_Carret_Index, 1);
	Text->Set_Text(t);
}
void MY_UI_Too::Controls::Text_Box::NewLine(){
	OUTPUT_DEBUG_MSG("Newline is not supported for a standard Textbox");
}
void MY_UI_Too::Controls::Text_Box::Enter(){
	OUTPUT_DEBUG_MSG("Enter on Textbox.. Callback here");
}
