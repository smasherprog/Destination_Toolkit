#include "PCH.h"
#include "Button.h"
#include "IRenderer.h"
#include "ISkin.h"
#include "Text.h"
#include "Input.h"

MY_UI_Too::Controls::Button::Button(IWidget* parent): Widget(parent) {
	Set_Size(Utilities::Point(100, 30));
	_Cust_MouseDown=_Cust_MouseHovered=_Cust_MouseUp=false;

	_Selected_UVs=_UVs_MouseUp = Internal::UI_Skin->Get_Up_Button();
	_UVs_MouseHovered = Internal::UI_Skin->Get_Hovered_Button();
	_UVs_MouseDown = Internal::UI_Skin->Get_Down_Button();
	Set_Name("Button");
	Text=new MY_UI_Too::Controls::Text(this);
	Text->Set_Font_Size(20);
	Text->Set_Text("Button");
	Text->Align_Center();

}
MY_UI_Too::Controls::Button::~Button(){
	if(_Cust_MouseDown) Internal::UI_Skin->Remove_From_Skin(_UVs_MouseDown);
	if(_Cust_MouseHovered) Internal::UI_Skin->Remove_From_Skin(_UVs_MouseHovered);
	if(_Cust_MouseUp) Internal::UI_Skin->Remove_From_Skin(_UVs_MouseUp);
}

void MY_UI_Too::Controls::Button::Set_Text(std::string txt){
	Text->Set_Text(txt);
	Text->Align_Center();
}
void MY_UI_Too::Controls::Button::Set_Focus(bool focus){
	MY_UI_Too::Controls::Widget::Set_Focus(focus);
	if(!focus) _Selected_UVs = _UVs_MouseUp;
}
void MY_UI_Too::Controls::Button::Mouse_Left_Down(){
	MY_UI_Too::Controls::Widget::Mouse_Left_Down();
	_Selected_UVs = _UVs_MouseDown;
}
void MY_UI_Too::Controls::Button::Mouse_Left_Up(){
	MY_UI_Too::Controls::Widget::Mouse_Left_Up();
	_Selected_UVs = _UVs_MouseUp;
}
void MY_UI_Too::Controls::Button::Mouse_Entered(){
	MY_UI_Too::Controls::Widget::Mouse_Entered();
	_Selected_UVs = _UVs_MouseHovered;
}
void MY_UI_Too::Controls::Button::Mouse_Exited(){
	MY_UI_Too::Controls::Widget::Mouse_Exited();
	_Selected_UVs = _UVs_MouseUp;
}
void MY_UI_Too::Controls::Button::Draw(){
	Internal::Renderer->DrawTexturedRect_NoClip(Internal::UI_Skin->Get_Skin(), _Selected_UVs, Utilities::Rect(_Internals.Absolute_TL.x , _Internals.Absolute_TL.y, _Internals.Size.x, _Internals.Size.y) );
	Text->Draw();
}
MY_UI_Too::Interfaces::IWidget* MY_UI_Too::Controls::Button::Hit(){
	Utilities::Rect rect(_Internals.Absolute_TL.left, _Internals.Absolute_TL.top, _Internals.Size.x, _Internals.Size.y);
	if(rect.Intersect(Utilities::Point(New_MousePosx, New_MousePosy))) return this;
	return nullptr;
}



void MY_UI_Too::Controls::Button::Set_MouseDownImage(std::string file){
	MY_UI_Too::Interfaces::ITexture* tex = Internal::Renderer->LoadTexture(file);
	if(tex != nullptr) {
		_UVs_MouseDown = Internal::UI_Skin->Add_To_Skin(tex);
		_Cust_MouseDown= true;
	} else {
		Interfaces::ITexture* skin = Internal::UI_Skin->Get_Skin();
		_UVs_MouseDown = Internal::UI_Skin->Get_Down_Button();
		_Cust_MouseDown= false;
	}
	delete tex;// the texture is no longer needed since it was copied to the skin
}

void MY_UI_Too::Controls::Button::Set_MouseUpImage(std::string file){
	MY_UI_Too::Interfaces::ITexture* tex = Internal::Renderer->LoadTexture(file);
	if(tex != nullptr) {
		_UVs_MouseUp = Internal::UI_Skin->Add_To_Skin(tex);
		_Cust_MouseUp= true;
	} else {
		Interfaces::ITexture* skin = Internal::UI_Skin->Get_Skin();
		_UVs_MouseUp = Internal::UI_Skin->Get_Up_Button();
		_Cust_MouseUp= false;
	}
	delete tex;// the texture is no longer needed since it was copied to the skin
}
void MY_UI_Too::Controls::Button::Set_MouseHoveredImage(std::string file){
	MY_UI_Too::Interfaces::ITexture* tex = Internal::Renderer->LoadTexture(file);
	if(tex != nullptr) {
		_UVs_MouseHovered = Internal::UI_Skin->Add_To_Skin(tex);
		_Cust_MouseHovered= true;
	} else {
		Interfaces::ITexture* skin = Internal::UI_Skin->Get_Skin();
		_UVs_MouseHovered = Internal::UI_Skin->Get_Hovered_Button();
		_Cust_MouseHovered= false;
	}
	delete tex;// the texture is no longer needed since it was copied to the skin
}	
