#include "PCH.h"
#include "RadioButton.h"
#include "cWidgetRenderer.h"
#include "cWidgetSkin.h"
#include "Text.h"
#include "cWidgetEvents.h"

MY_UI::Controls::RadioButton::RadioButton(cWidgetBase* parent): Image(parent) { 	
	auto skin = Internal::WidgetSkin;
	Image::Set_Texture(skin->Radio_Button);
	SetSize(static_cast<int>(skin->Radio_Button.width), static_cast<int>(skin->Radio_Button.height));
	OnLeftClicked.Add(std::bind(&MY_UI::Controls::RadioButton::ToggleRadioBox, this));
}

void MY_UI::Controls::RadioButton::MouseEntered(){
	MY_UI::Controls::cWidgetBase::MouseEntered();
	auto skin = Internal::WidgetSkin;
	if(GetSelected()) Image::Set_Texture(skin->Radio_Button_Selected_Hovered);
	else Image::Set_Texture(skin->Radio_Button_Hovered);
}
void MY_UI::Controls::RadioButton::MouseExited(){
	MY_UI::Controls::cWidgetBase::MouseExited();
	auto skin = Internal::WidgetSkin;
	if(GetSelected()) Image::Set_Texture(skin->Radio_Button_Selected);
	else Image::Set_Texture(skin->Radio_Button);
}

void MY_UI::Controls::RadioButton::SetSelected(bool checked){
	auto skin = Internal::WidgetSkin;
	if(checked){
		if(Internal_CheckBox.Users_Checked) *Internal_CheckBox.Users_Checked = true;
		Image::Set_Texture(skin->Radio_Button_Selected);
	}else {
		if(Internal_CheckBox.Users_Checked) *Internal_CheckBox.Users_Checked = false;
		Image::Set_Texture(skin->Radio_Button);
	}
	Internal_CheckBox.Checked = checked;
	
}
bool MY_UI::Controls::RadioButton::GetSelected(){
	return Internal_CheckBox.Checked;
}
void MY_UI::Controls::RadioButton::ToggleRadioBox(){
	auto skin = Internal::WidgetSkin;
	if(Internal_CheckBox.Checked) {
		if(Internal_CheckBox.Users_Checked) *Internal_CheckBox.Users_Checked = false;
		Image::Set_Texture(skin->Radio_Button);
		Internal_CheckBox.Checked = false;
	}else {
		if(Internal_CheckBox.Users_Checked) *Internal_CheckBox.Users_Checked = true;
		Image::Set_Texture(skin->Radio_Button_Selected);
		Internal_CheckBox.Checked = true;
	}
}
void MY_UI::Controls::RadioButton::Update(){
	if(Internal_CheckBox.Users_Checked){
		Internal_CheckBox.Checked = *Internal_CheckBox.Users_Checked;
		auto skin = Internal::WidgetSkin;
		if(Internal_CheckBox.Checked) Image::Set_Texture(skin->Radio_Button_Selected);
		else Image::Set_Texture(skin->Radio_Button);
	}
}

MY_UI::Controls::RadioButtonContainer::RadioButtonContainer(cWidgetBase* parent): cWidgetBase(parent) { 	
	SetName("Radio Button Container");
}
MY_UI::Controls::RadioButton_w_Label* MY_UI::Controls::RadioButtonContainer::Insert(std::string txt, unsigned int id){
	MY_UI::Controls::RadioButton_w_Label* but = new MY_UI::Controls::RadioButton_w_Label(this);
	but->Widget->SetName(txt);
	but->SetLabel(txt, Internal_RadioButtonContainer.LabelAlignment, 5);
	if(Internal_Widget.Children.size() == 1) but->SetPos(0, 0);
	else but->Align_To_Widget( *(Internal_Widget.Children.end()-2), Internal_RadioButtonContainer.ButtonAlignment, 5);
	but->Widget->OnLeftClicked.Add(std::bind(&MY_UI::Controls::RadioButtonContainer::SetSelected, this, std::placeholders::_1), this, true);
	but->Widget->SetId(id);
	SetSizeToContents();
	return but;
}

bool MY_UI::Controls::RadioButtonContainer::Erase(MY_UI::Controls::RadioButton_w_Label* radio){
	for(auto i=Internal_Widget.Children.begin(); i !=Internal_Widget.Children.end(); i++){
		if(*i== radio){
			(*i)->Internal_Widget.Parent =0;// set this to null so it doesnt call decouple on this
			delete *i;// free the memory
			if(i +1 == Internal_Widget.Children.end()) {// this is the end of the list
				Internal_Widget.Children.erase(i);
				SetSizeToContents();
				return true;
			} else if(i ==  Internal_Widget.Children.begin()){
				auto j = Internal_Widget.Children.erase(i);
				(*j)->SetPos(0, 0);
				auto prev = j;
				j+=1;// advance one
				for( ; j !=Internal_Widget.Children.end(); j++){
					(*j)->Align_To_Widget(*prev, Internal_RadioButtonContainer.ButtonAlignment, 5);
					prev+=1;
				}
				SetSizeToContents();
				return true;
			} else {// i is not the end, or the beginning
				auto j = Internal_Widget.Children.erase(i);
				j-=1;// back up one
				if(j == Internal_Widget.Children.begin()) j+=1;// at the beginning, advance to the next in the list
				// otherwise, this is not at the beginning so placement can begin
				auto prev = j;
				for( ; j !=Internal_Widget.Children.end(); j++){
					(*j)->Align_To_Widget(*prev, Internal_RadioButtonContainer.ButtonAlignment, 5);
					prev+=1;
				}
				SetSizeToContents();
				return true;
			}

		}
	}
	return false;
}
void MY_UI::Controls::RadioButtonContainer::clear(){
	for(auto i=Internal_Widget.Children.begin(); i !=Internal_Widget.Children.end(); i++){
		(*i)->Internal_Widget.Parent =0;// set this to null so it doesnt call decouple on this
		delete *i;// free the memory
	}
	Internal_Widget.Children.clear();
	SetSizeToContents();
}
void MY_UI::Controls::RadioButtonContainer::SetSelected(cWidgetBase* widget){
	for(auto i=Internal_Widget.Children.begin(); i !=Internal_Widget.Children.end(); i++){
		MY_UI::Controls::RadioButton_w_Label* but = static_cast<MY_UI::Controls::RadioButton_w_Label*>(*i);
		but->Widget->SetSelected(false);
	}
	MY_UI::Controls::RadioButton* radio = static_cast<MY_UI::Controls::RadioButton*>(widget);
	radio->SetSelected(true);
	Selected = radio;
	OnSelected.Call(this);
}