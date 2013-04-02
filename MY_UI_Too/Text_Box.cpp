#include "PCH.h"
#include "Text_Box.h"
#include "Widget.h"
#include "ISkin.h"
#include "Text.h"
#include "IRenderer.h"
#include "IInput.h"
#include "Common.h"

MY_UI_Too::Controls::Text_Box::Text_Box(IWidget* parent): Widget(parent) {
	_Carret_Index=0;
	Set_Size(Utilities::Point(162, 29));

	_Selected_UVs=_UVs_No_Focus = Internal::UI_Skin->Get_Text_Box_No_Focus();
	_UVs_Focus = Internal::UI_Skin->Get_Text_Box_Focus();

	Set_Name("Text_Box");
	Text=new MY_UI_Too::Controls::Text(this);
	Text->Set_Font_Size(20);
	Text->Set_Text("blank");
	Text->Set_Pos(Utilities::Point(5, 5));

}
MY_UI_Too::Controls::Text_Box::~Text_Box(){

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
	default:
		if(Current_Key>=(START_CHAR-1) && Current_Key<= END_CHAR) {// bounds check
			Text->Set_Text(Text->Get_Text()+(char)Current_Key);
			_Carret_Index+=1;
		} 
		break;
	};
}

void MY_UI_Too::Controls::Text_Box::Set_Focus(bool focus){
	MY_UI_Too::Controls::Widget::Set_Focus(focus);
	if(!focus) _Selected_UVs = _UVs_No_Focus;
	else _Selected_UVs = _UVs_Focus;
}
void MY_UI_Too::Controls::Text_Box::Mouse_Left_Down(){
	MY_UI_Too::Controls::Widget::Mouse_Left_Down();
	_Selected_UVs = _UVs_Focus;
}
void MY_UI_Too::Controls::Text_Box::Mouse_Left_Up(){
	MY_UI_Too::Controls::Widget::Mouse_Left_Up();
	_Selected_UVs = _UVs_Focus;
}
void MY_UI_Too::Controls::Text_Box::Mouse_Entered(){
	MY_UI_Too::Controls::Widget::Mouse_Entered();
	_Selected_UVs = _UVs_Focus;
	Internal::Input->SetCursor(Cursor_Types::Text);
}
void MY_UI_Too::Controls::Text_Box::Mouse_Exited(){
	MY_UI_Too::Controls::Widget::Mouse_Exited();
	if(!_Internals.Focus) _Selected_UVs = _UVs_No_Focus;
	else _Selected_UVs = _UVs_Focus;
	Internal::Input->SetCursor(Cursor_Types::Standard);
}
void MY_UI_Too::Controls::Text_Box::Draw(){
	Internal::Renderer->DrawTexturedRect_NoClip(Internal::UI_Skin->Get_Skin(), _Selected_UVs, Utilities::Rect(_Internals.Absolute_TL.x , _Internals.Absolute_TL.y, _Internals.Size.x, _Internals.Size.y) );
	Text->Draw();
}
MY_UI_Too::Interfaces::IWidget* MY_UI_Too::Controls::Text_Box::Hit(){
	Utilities::Rect rect(_Internals.Absolute_TL.left, _Internals.Absolute_TL.top, _Internals.Size.x, _Internals.Size.y);
	if(rect.Intersect(Utilities::Point(New_MousePosx, New_MousePosy))) return this;
	return nullptr;
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
