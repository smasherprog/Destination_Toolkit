#include "stdafx.h"
#include "Test_Helper.h"
#include "Text.h"
#include "Slider.h"
#include "DropDown.h"
#include "ColorPickerWindow.h"

int tester = 400;
float tester2 = 400;
int tester3 = 400;

void DropButtonPressed(MY_UI::Controls::cWidgetBase* widget){
	MY_UI::Controls::Button* button = static_cast<MY_UI::Controls::Button*>(widget);
	OUTPUT_DEBUG_MSG(button->Text->GetText()<<" was Selected from the drop down");

}
void AddItem(MY_UI::Controls::cWidgetBase* widget){
	MY_UI::Controls::DropDown* drop = static_cast<MY_UI::Controls::DropDown*>(widget->Internal_Widget.ExtraData);
	drop->Insert("Added Item")->OnLeftClicked.Add(DropButtonPressed, true);
}
void Values(){
	delete Dummy;
	Dummy = new MY_UI::Controls::cWidget_Dummy(root);
	Dummy->SetControlBounds(root->GetControlBounds());
	Dummy->SetClientBounds(root->GetClientBounds());


	MY_UI::Controls::DropDown* drop = new MY_UI::Controls::DropDown(Dummy);
	MY_UI::Controls::Button* dropbutton = drop->Insert("hey there");
	dropbutton->OnLeftClicked.Add(DropButtonPressed, true);

	dropbutton = drop->Insert("hey hi");
	dropbutton->OnLeftClicked.Add(DropButtonPressed, true);
	dropbutton = drop->Insert("Poppbp");
	dropbutton->OnLeftClicked.Add(DropButtonPressed, true);
	dropbutton = drop->Insert("see here");
	dropbutton->OnLeftClicked.Add(DropButtonPressed, true);
	dropbutton = drop->Insert("I know you");
	dropbutton->OnLeftClicked.Add(DropButtonPressed, true);
	drop->SetPos(300, 200);
	drop->SetSize(150, 25);

	MY_UI::Controls::Button* pbutton = new MY_UI::Controls::Button(Dummy);

	pbutton->SetText("Add Item", CENTER);
	pbutton->SetPopup("This adds an item to the list");
	pbutton->SetName("adder Button");
	pbutton->OnLeftClicked.Add(AddItem, true);
	pbutton->Internal_Widget.ExtraData = drop;// store this for later use
	pbutton->Align_To_Widget(drop, RIGHT | IN_LINE, 15);

	MY_UI::Controls::VerticalSlider<int>* vslider = new MY_UI::Controls::VerticalSlider<int>(Dummy);
	vslider->SetPos(290, 300);
	vslider->SetRange(200, 400);
	vslider->SetSliderLength(300);
	vslider->SetSliderValue(250);

	MY_UI::Controls::VerticalSlider_w_Label<int>* vsliderlab = new MY_UI::Controls::VerticalSlider_w_Label<int>(Dummy);
	vsliderlab->SetLabel("Link 1", TOP | CENTER, 5);
	vsliderlab->Align_To_Widget(vslider, RIGHT | IN_LINE, 30);
	vsliderlab->Widget->SetRange(200, 1000);
	vsliderlab->Widget->SetSliderLength(300);
	vsliderlab->Widget->SetLink(&tester);

	MY_UI::Controls::VerticalSlider_w_Label<float>* vsliderlabel = new MY_UI::Controls::VerticalSlider_w_Label<float>(Dummy);
	vsliderlabel->SetLabel("Link 2", TOP | CENTER, 5);	
	vsliderlabel->Align_To_Widget(vsliderlab, RIGHT | IN_LINE, 30);
	vsliderlabel->Widget->SetRange(400, 1000);
	vsliderlabel->Widget->SetSliderLength(200);
	vsliderlabel->Widget->SetLink(&tester2);

	MY_UI::Controls::VerticalSlider_w_Label<float>* hslider = new MY_UI::Controls::VerticalSlider_w_Label<float>(Dummy);
	hslider->SetLabel("Link 2", TOP | CENTER, 5);	
	hslider->Align_To_Widget(vsliderlabel, RIGHT | IN_LINE, 30);
	hslider->Widget->SetRange(200, 1000);
	hslider->Widget->SetSliderLength(300);
	hslider->Widget->SetLink(&tester2);


	MY_UI::Controls::HorizontalSlider_w_Label<int>* hsliderlabels = new MY_UI::Controls::HorizontalSlider_w_Label<int>(Dummy);
	hsliderlabels->SetLabel("Link 1", LEFT | IN_LINE, 5);
	hsliderlabels->Align_To_Widget(hslider, RIGHT | IN_LINE, 40);
	hsliderlabels->Widget->SetRange(200, 1000);
	hsliderlabels->Widget->SetSliderLength(300);
	hsliderlabels->Widget->SetLink(&tester);

	MY_UI::Controls::HorizontalSlider_w_Label<int>* hsliderlabel = new MY_UI::Controls::HorizontalSlider_w_Label<int>(Dummy);
	hsliderlabel->SetLabel("No Link", LEFT | IN_LINE, 5);
	hsliderlabel->Align_To_Widget(hsliderlabels, BOTTOM | IN_LINE, 40);
	hsliderlabel->Widget->SetRange(200, 1000);

	MY_UI::Controls::Numeric_TextBox<int>* tbox = new MY_UI::Controls::Numeric_TextBox<int>(Dummy);
	tbox->SetLength(65);
	tbox->SetMaxCharacters(4);
	tbox->SetRange(0, 500);
	tbox->SetLink(&tester3);
	tbox->Align_To_Widget(hsliderlabel, BOTTOM | IN_LINE, 20);

	MY_UI::Controls::Numeric_TextBox_w_Adjusters<int>* tbox2 = new MY_UI::Controls::Numeric_TextBox_w_Adjusters<int>(Dummy);
	tbox2->SetLength(65);
	tbox2->SetMaxCharacters(4);
	tbox2->SetRange(0, 500);
	tbox2->SetLink(&tester3);
	tbox2->Align_To_Widget(tbox, RIGHT | IN_LINE, 20);

	MY_UI::Controls::ColorPicker_w_Label* picker = new MY_UI::Controls::ColorPicker_w_Label(Dummy);
	picker->SetLabel("Pick Color ", LEFT | IN_LINE, 5);
	picker->Align_To_Widget(tbox, BOTTOM | IN_LINE, 40);


}