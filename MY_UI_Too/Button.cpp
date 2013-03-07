#include "PCH.h"
#include "Button.h"
#include "IRenderer.h"
#include "ISkin.h"

MY_UI_Too::Controls::Button::Button(IWidget* parent): Widget(parent) {
	Set_Control_Bounds(Utilities::Rect(0, 0, 100, 30));
	_State = 0;
}
MY_UI_Too::Controls::Button::~Button(){

}
void MY_UI_Too::Controls::Button::Set_Focus(bool focus){
	MY_UI_Too::Controls::Widget::Set_Focus(focus);
	if(!focus) _State =0;
}
void MY_UI_Too::Controls::Button::Mouse_Left_Down(){
	MY_UI_Too::Controls::Widget::Mouse_Left_Down();
	_State=2;
}
void MY_UI_Too::Controls::Button::Mouse_Left_Up(){
	MY_UI_Too::Controls::Widget::Mouse_Left_Up();
	_State=1;
}
void MY_UI_Too::Controls::Button::Mouse_Entered(){
	MY_UI_Too::Controls::Widget::Mouse_Entered();
	_State = 1;
}
void MY_UI_Too::Controls::Button::Mouse_Exited(){
	MY_UI_Too::Controls::Widget::Mouse_Exited();
	_State =0;
}
void MY_UI_Too::Controls::Button::Draw(){
	Interfaces::ITexture* skin = Internal::UI_Skin->Get_Skin();
	Utilities::UVs uvs;
	if(_State==0)uvs = Internal::UI_Skin->Get_Up_Button();
	else if(_State==1)uvs = Internal::UI_Skin->Get_Hovered_Button();
	else if(_State==2)uvs = Internal::UI_Skin->Get_Down_Button();

	Internal::Renderer->DrawTexturedRect_NoClip(skin, uvs, Get_Control_Bounds());
}