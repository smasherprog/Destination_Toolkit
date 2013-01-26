#include "PCH.h"
#include "CheckBox.h"
#include "cWidgetRenderer.h"
#include "cWidgetSkin.h"
#include "Text.h"
#include "Image.h"

MY_UI::Controls::CheckBox::CheckBox(cWidgetBase* parent): Button(parent) { 	
	auto skin = Internal::WidgetSkin;
	Set_Texture(skin->CheckBox_UnChecked);
	SetSize(static_cast<int>(skin->CheckBox_UnChecked.width), static_cast<int>(skin->CheckBox_UnChecked.height));
	OnLeftClicked.Add(std::bind(&MY_UI::Controls::CheckBox::ToggleCheckBox, this));
	SetBorderDrawn(false);
}
void MY_UI::Controls::CheckBox::MouseEntered(){
	MY_UI::Controls::cWidgetBase::MouseEntered();
	auto skin = Internal::WidgetSkin;
	if(GetChecked()) Image::Set_Texture(skin->CheckBox_Checked_Hovered);
	else Image::Set_Texture(skin->CheckBox_UnChecked_Hovered);
}
void MY_UI::Controls::CheckBox::MouseExited(){
	MY_UI::Controls::cWidgetBase::MouseExited();
	auto skin = Internal::WidgetSkin;
	if(GetChecked()) Image::Set_Texture(skin->CheckBox_Checked);
	else Image::Set_Texture(skin->CheckBox_UnChecked);
}
void MY_UI::Controls::CheckBox::SetChecked(bool checked){
	auto skin = Internal::WidgetSkin;
	if(checked){
		if(Internal_CheckBox.Users_Checked) *Internal_CheckBox.Users_Checked = true;
		Image::Set_Texture(skin->CheckBox_Checked);
	}else {
		if(Internal_CheckBox.Users_Checked) *Internal_CheckBox.Users_Checked = false;
		Image::Set_Texture(skin->CheckBox_UnChecked);
	}
	Internal_CheckBox.Checked = checked;
}
bool MY_UI::Controls::CheckBox::GetChecked(){
	return Internal_CheckBox.Checked;
}
void MY_UI::Controls::CheckBox::ToggleCheckBox(){
	auto skin = Internal::WidgetSkin;
	if(Internal_CheckBox.Checked) {
		if(Internal_CheckBox.Users_Checked) *Internal_CheckBox.Users_Checked = false;
		Image::Set_Texture(skin->CheckBox_UnChecked);
		Internal_CheckBox.Checked = false;
	}else {
		if(Internal_CheckBox.Users_Checked) *Internal_CheckBox.Users_Checked = true;
		Image::Set_Texture(skin->CheckBox_Checked);
		Internal_CheckBox.Checked = true;
	}
}
bool* MY_UI::Controls::CheckBox::GetLink(){ return Internal_CheckBox.Users_Checked; }
void MY_UI::Controls::CheckBox::SetLink(bool* ptr){ Internal_CheckBox.Users_Checked = ptr; MY_UI::Internal::RootWidget->CheckForUpdate(this); }
void MY_UI::Controls::CheckBox::Update(){
	if(Internal_CheckBox.Users_Checked){
		Internal_CheckBox.Checked = *Internal_CheckBox.Users_Checked;
		auto skin = Internal::WidgetSkin;
		if(Internal_CheckBox.Checked) Image::Set_Texture(skin->CheckBox_Checked);
		else Image::Set_Texture(skin->CheckBox_UnChecked);
	}
}


