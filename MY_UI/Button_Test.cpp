#include "stdafx.h"
#include "Test_Helper.h"
#include "Button.h"
#include "Text.h"
#include "CheckBox.h"
#include "RadioButton.h"
#include "Resizable.h"
#include "cWidgetSkin.h"


void ButtonPressed(MY_UI::Controls::cWidgetBase* widget){
	MY_UI::Controls::Button* button = static_cast<MY_UI::Controls::Button*>(widget);
	OUTPUT_DEBUG_MSG(widget->GetName()<<" was pressed");

}
void CheckPressed(MY_UI::Controls::cWidgetBase* widget){
	MY_UI::Controls::CheckBox* checkbox = static_cast<MY_UI::Controls::CheckBox*>(widget);
	if(checkbox->GetChecked()) {
		OUTPUT_DEBUG_MSG(widget->GetName()<<" was checked");
	}else {
		OUTPUT_DEBUG_MSG(widget->GetName()<<" was unchecked");
	}
}
void radioPressed(MY_UI::Controls::cWidgetBase* widget){
	MY_UI::Controls::RadioButton* radio = static_cast<MY_UI::Controls::RadioButton*>(widget);
	if(radio->GetSelected()) {
		OUTPUT_DEBUG_MSG(widget->GetName()<<" was selected");
	}else {
		OUTPUT_DEBUG_MSG(widget->GetName()<<" was unselected");
	}
}
class Button_Hotkey: public MY_UI::Controls::Button{
public:
	Button_Hotkey(MY_UI::Controls::cWidgetBase* parent) : Button(parent){
		MY_UI::BindToKeys(KEY_CTRL, 'A', std::bind(&Button_Hotkey::Hotkey, this));// bind the keys
	}
	~Button_Hotkey(){ MY_UI::UnBindKeys(KEY_CTRL, 'A'); }// unbind the keys !!
	void Hotkey(){
		ButtonPressed(this);
		OUTPUT_DEBUG_MSG("Hotkey macro called");
	}
};

void ButtonExamples(){
	delete Dummy;
	Dummy = new MY_UI::Controls::cWidget_Dummy(root);
	Dummy->SetControlBounds(root->GetControlBounds());
	Dummy->SetClientBounds(root->GetClientBounds());

	MY_UI::Controls::Button* pbutton = new MY_UI::Controls::Button(Dummy);

	pbutton->SetText("Button", CENTER);
	pbutton->SetPopup("The button changes when the mouse enters and leaves the Button");
	pbutton->SetName("Hovered Button");
	pbutton->OnLeftClicked.Add(ButtonPressed, true);
	pbutton->Set_Hovered_Texture(skin->WindowBar_Without_Focus);
	pbutton->SetImageBorderSize(2, 2);
	pbutton->SetPos(300, 260);
	
	MY_UI::Controls::Button* button = new MY_UI::Controls::Button(Dummy);
	if(!button->Set_Texture("..\\Assets\\blank_button.tif")) OUTPUT_DEBUG_MSG("couldnt load the texture");
	button->SetSize(75, 26);
	button->SetText("Button", CENTER);
	button->Align_To_Widget(pbutton, IN_LINE | BOTTOM, 10);
	button->SetPopup("Image Button");
	button->SetName("Image Button");
	button->OnLeftClicked.Add(ButtonPressed, true);

	pbutton = new MY_UI::Controls::Button(Dummy);
	pbutton->SetText("Button", CENTER);
	pbutton->Align_To_Widget(button, BOTTOM | CENTER, 10);
	pbutton->SetPopup("Standard Button");
	pbutton->SetName("Standard Button");
	pbutton->OnLeftClicked.Add(ButtonPressed, true);

	Button_Hotkey* hotkeybutton = new Button_Hotkey(Dummy);
	hotkeybutton->SetText("HotKey ", CENTER);
	hotkeybutton->Align_To_Widget(pbutton, BOTTOM | CENTER, 10);
	hotkeybutton->SetPopup("Press Ctrl A to run the hotkey bound to this button");
	hotkeybutton->SetName("hotkey button");
	hotkeybutton->OnLeftClicked.Add(ButtonPressed, true);

	MY_UI::Controls::CheckBox_w_Label* checkboxwlabel = new MY_UI::Controls::CheckBox_w_Label(Dummy);
	checkboxwlabel->SetLabel("CheckBox with label", IN_LINE | LEFT, 4);
	checkboxwlabel->Align_To_Widget(hotkeybutton, IN_LINE | BOTTOM, 10);
	checkboxwlabel->SetPopup("Checkbox with a label, aligned to the button above it");
	checkboxwlabel->Widget->SetName("Checkbox with a Label");
	checkboxwlabel->Widget->OnLeftClicked.Add(CheckPressed, true);

	MY_UI::Controls::CheckBox* checkbox = new MY_UI::Controls::CheckBox(Dummy);
	checkbox->Align_To_Widget(checkboxwlabel, IN_LINE | BOTTOM, 10);
	checkbox->SetName("No label checkbox");
	checkbox->OnLeftClicked.Add(CheckPressed, true);

	MY_UI::Controls::RadioButton* radiobutton = new MY_UI::Controls::RadioButton(Dummy);
	radiobutton->Align_To_Widget(pbutton, IN_LINE | RIGHT, 100);

	MY_UI::Controls::RadioButton_w_Label* radiobuttonwlabel = new MY_UI::Controls::RadioButton_w_Label(Dummy);
	radiobuttonwlabel->SetLabel("RadioButton with label", IN_LINE | LEFT);
	radiobuttonwlabel->Align_To_Widget(radiobutton, IN_LINE | BOTTOM, 10);
	radiobuttonwlabel->SetPopup("RadioButton with a label");

	MY_UI::Controls::RadioButtonContainer* radios = new MY_UI::Controls::RadioButtonContainer(Dummy);
	
	MY_UI::Controls::RadioButton_w_Label* but1 = radios->Insert("radio 1");
	but1->Widget->SetName("radio 1");
	but1->Widget->OnLeftClicked.Add(radioPressed, true);

	but1 = radios->Insert("radio 2");
	but1->Widget->SetName("radio 2");
	but1->Widget->OnLeftClicked.Add(radioPressed, true);

	but1 = radios->Insert("radio 3");
	but1->Widget->SetName("radio 3");
	but1->Widget->OnLeftClicked.Add(radioPressed, true);

	radios->Align_To_Widget(radiobuttonwlabel, BOTTOM | CENTER, 20);


}