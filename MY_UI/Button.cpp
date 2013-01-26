#include "PCH.h"
#include "Button.h"
#include "cWidgetSkin.h"
#include "Text.h"
#include "cWidgetUtilities.h"
#include "Image.h"
#include "cWidgetEvents.h"

MY_UI::Controls::Button::Button(cWidgetBase* parent): Image(parent) { 
	auto skin = MY_UI::Internal::WidgetSkin;
	Set_UnPressed_Texture(skin->Medium_Button_UnPressed);
	Set_Pressed_Texture(skin->Medium_Button_Pressed);
	Set_Hovered_Texture(skin->Medium_Button_UnPressed_Hovered);
	SetControlBounds(Utilities::Rect(0, 0, static_cast<int>(skin->Medium_Button_Pressed.width), static_cast<int>(skin->Medium_Button_Pressed.height)));
	SetClientBounds(Utilities::Rect(0, 0, static_cast<int>(skin->Medium_Button_Pressed.width), static_cast<int>(skin->Medium_Button_Pressed.height)));
	SetSize(static_cast<int>(skin->Medium_Button_Pressed.width), static_cast<int>(skin->Medium_Button_Pressed.height));
	SetImageBorderSize(2, 2);
	
	Text = new MY_UI::Controls::Text(this);
	SetPos(0, 0);
	Show_UnPressed_Texture();
	OnHoverEnter.Add(std::bind(&MY_UI::Controls::Button::Show_Hovered_Texture, this));
}
void MY_UI::Controls::Button::KeyUp(){// a space bar is the same as a leftmousedown
	OnKeyUp.Call(this);
}
void MY_UI::Controls::Button::SetFocus(bool focus){
	cWidgetBase::SetFocus(focus);
	if(Get_ShownImage() == 3) return ;// do nothing if the image is a selected image
	if(!focus) Show_UnPressed_Texture();
}
void MY_UI::Controls::Button::Draw_Clip(){
	MY_UI::Controls::Image::Draw_Clip();
	Text->Draw_Clip();
}
void MY_UI::Controls::Button::Draw_NoClip(){
	MY_UI::Controls::Image::Draw_NoClip();
	Text->Draw_NoClip();
}
void MY_UI::Controls::Button::LeftMouseButtonDown(){
	Show_Pressed_Texture();
	MY_UI::Controls::cWidgetBase::LeftMouseButtonDown();
}

void MY_UI::Controls::Button::MouseExited(){
	if(LastShowImage ==3) Show_Selected_Texture();
	else Show_UnPressed_Texture();
	MY_UI::Controls::cWidgetBase::MouseExited();
}
void MY_UI::Controls::Button::LeftMouseButtonUp(){// imporant to restore the button to its previous state before the mousedown
	if(LastShowImage == 2){// a hovered texture was showing previously.. special case
		Show_Hovered_Texture();
		if(ImageExists(3)){// there exists a selected image. show that on exit
			LastShowImage = 3;// make sure it doesnt change on the mouse exit
		} else {// no selected texture exists.. so standard unpressed
			LastShowImage =0;
		}
	} else { // no hovered texture.. switch to the unpressed texture
		Show_UnPressed_Texture();
		LastShowImage=0;// make sure it doesnt switch back to the pressed texture on exit
	}
	MY_UI::Controls::cWidgetBase::LeftMouseButtonUp();
}

MY_UI::Controls::cWidgetBase* MY_UI::Controls::Button::Hit(){ 
	if(Internal_Widget.Hidden) return 0;
	const Utilities::Rect& r = Internal_Widget.AbsolutePosition;
	if((New_MousePosx > r.left) & (New_MousePosx < r.left + r.width) & (New_MousePosy > r.top) & (New_MousePosy < r.top + r.height)){// only check within our bounds		
		return this;// no child was hit, but this was
	}
	return 0;
} 

void MY_UI::Controls::Button::Set_Pressed_Texture(Utilities::Texture& tex, bool takeownership){
	Add_Texture(tex, takeownership, 1);
}
bool MY_UI::Controls::Button::Set_Pressed_Texture(std::string filename){
	return Add_Texture(filename, 1);
}
void MY_UI::Controls::Button::Set_UnPressed_Texture(Utilities::Texture& tex, bool takeownership){
	Add_Texture(tex, takeownership, 0);
	Set_ShownImage(0);
}
bool MY_UI::Controls::Button::Set_UnPressed_Texture(std::string filename){
	if(Add_Texture(filename, 0)){
		Set_ShownImage(0);
		return true;
	}
	return false;
}
		
void MY_UI::Controls::Button::Set_Hovered_Texture(Utilities::Texture& tex, bool takeownership){
	Add_Texture(tex, takeownership, 2);
}
bool MY_UI::Controls::Button::Set_Hovered_Texture(std::string filename){
	return Add_Texture(filename, 2);
}
void MY_UI::Controls::Button::Set_Selected_Texture(Utilities::Texture& tex, bool takeownership){
	Add_Texture(tex, takeownership, 3);
}
bool MY_UI::Controls::Button::Set_Selected_Texture(std::string filename){
	return Add_Texture(filename, 3);
}

void MY_UI::Controls::Button::Show_Pressed_Texture(){
	if(ImageExists(1)){
		LastShowImage = Get_ShownImage();
		Set_ShownImage(1);
	}
}
void MY_UI::Controls::Button::Show_UnPressed_Texture(){
	if(ImageExists(0)){
		LastShowImage = Get_ShownImage();
		Set_ShownImage(0);
	}
}
void MY_UI::Controls::Button::Show_Hovered_Texture(){
	if(ImageExists(2)){
		LastShowImage = Get_ShownImage();
		Set_ShownImage(2);
	}
}
void MY_UI::Controls::Button::Show_Selected_Texture(){
	if(ImageExists(3)){
		LastShowImage = Get_ShownImage();
		Set_ShownImage(3);
	}
}
void MY_UI::Controls::Button::SetText(std::string text, int align){
	Text->SetText(text);
	Text->SetSizeToContents();
	Text->Align_Within_Parent(align);
}
std::string MY_UI::Controls::Button::GetText() const { return Text->GetText(); }

MY_UI::Controls::Tab_Button::Tab_Button(cWidgetBase* parent) : Button(parent) {
	auto skin = MY_UI::Internal::WidgetSkin;
	Set_Hovered_Texture(skin->Tab_Hovered);
	Set_UnPressed_Texture(skin->Tab_WithOut_Focus);
	Set_Pressed_Texture(skin->Tab_WithOut_Focus);
	Set_Selected_Texture(skin->Tab_With_Focus);
	Show_UnPressed_Texture();
	SetBorderDrawn(false);
}
void MY_UI::Controls::Tab_Button::SetText(std::string text, int align){
	MY_UI::Controls::Button::SetText(text, align);
	int y = GetSize().y;
	SetSizeToContents();
	int x = GetSize().x;
	SetSize(x + 12, y);// fugde needed for the x 
	Text->SetPos(6, 0);
}