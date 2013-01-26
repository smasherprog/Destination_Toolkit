#include "stdafx.h"
#include "Test_Helper.h"
#include "Scrollable.h"
#include "Button.h"
#include "Slider.h"
#include "List_Box.h"

MY_UI::Controls::List_Box_wScrollbars* list_wscrollers = 0;

void Addtolistbox(MY_UI::Controls::cWidgetBase* btn){
	list_wscrollers->Insert("hey there");
}

void Scrollers(){
	delete Dummy;
	Dummy = new MY_UI::Controls::cWidget_Dummy(root);
	Dummy->SetControlBounds(root->GetControlBounds());
	Dummy->SetClientBounds(root->GetClientBounds());

	MY_UI::Controls::Scrollable* scroller = new MY_UI::Controls::Scrollable(Dummy);
	scroller->SetPos(300, 200);
	scroller->SetSize(300, 300);

	MY_UI::Controls::Button* pbutton = new MY_UI::Controls::Button(scroller);
	pbutton->SetText("Button", CENTER);
	pbutton->SetPopup("The button changes when the mouse enters and leaves the Button");
	pbutton->SetName("Standard Button");
	pbutton->SetPos(20, 30);

	MY_UI::Controls::VerticalSlider<int>* vslider = new MY_UI::Controls::VerticalSlider<int>(scroller);
	vslider->SetPos(100, 40);
	vslider->SetMin(0);
	vslider->SetMax(400);
	vslider->SetSliderLength(200);

	MY_UI::Controls::Text* ptext = new MY_UI::Controls::Text(scroller);
	ptext->SetText("ABCDEFGHIJKLMNOPQRSTUVWXYZ ");
	ptext->SetFontSize(30);
	ptext->SetSizeToContents();// this is necessary to ensure the correct size is calculated from the text above
	ptext->SetPos(140, 250);

	pbutton = new MY_UI::Controls::Button(scroller);
	pbutton->SetText("Button", CENTER);
	pbutton->SetPopup("See mee!! ");
	pbutton->SetName("Standard Button");
	pbutton->Align_To_Widget(ptext, BOTTOM | CENTER, 10);

	pbutton = new MY_UI::Controls::Button(scroller);
	pbutton->SetText("Button", CENTER);
	pbutton->SetPopup("See mee!! ");
	pbutton->SetName("Standard Button");
	pbutton->SetPos(750, 800);

	MY_UI::Controls::List_Box* list = new MY_UI::Controls::List_Box(Dummy);
	list->SetPos(630, 200);
	list->SetSize(100, 300);
	list->Insert("one ");
	list->Insert("two ");

	

	list_wscrollers = new MY_UI::Controls::List_Box_wScrollbars(Dummy);
	list_wscrollers->SetPos(750, 200);
	list_wscrollers->SetSize(100, 200);
	list_wscrollers->Insert("one ");
	list_wscrollers->Insert("two ");
	list_wscrollers->Insert("too ");
	list_wscrollers->Insert("uhh ohh ");
	list_wscrollers->Insert("three ");
	list_wscrollers->Insert("four ");
	list_wscrollers->Insert("two ");

	list_wscrollers->Insert("ryhm ");
	list_wscrollers->Insert("see  ");
	list_wscrollers->Insert("mee 3");
	list_wscrollers->Insert("mee23 ");
	list_wscrollers->Insert("mee 23");
	list_wscrollers->Insert("mee ");
	list_wscrollers->Insert("mee 3");
	list_wscrollers->Insert("mee ");
	list_wscrollers->Insert("mee 2");
	list_wscrollers->Insert("mee ");
	list_wscrollers->Insert("mee123 ");
	list_wscrollers->Insert("mee ");
	list_wscrollers->Insert("mee ");
	list_wscrollers->Insert("mee ");
	list_wscrollers->Insert("mee1 ");

	MY_UI::Controls::Button* btn = new MY_UI::Controls::Button(Dummy);
	btn->SetText("Add Item");
	btn->Align_To_Widget(list_wscrollers, TOP | CENTER, 10);
	btn->OnLeftClicked.Add(Addtolistbox, true);
}