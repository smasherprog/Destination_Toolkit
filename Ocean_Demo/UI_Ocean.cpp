#include "stdafx.h"
#include "UI_Ocean.h"
#include "Ocean.h"
#include "../Utilities/MyMath.h"
#include "../MY_UI/Window.h"
#include "../MY_UI/Slider.h"
#include "../MY_UI/CheckBox.h"
#include "../MY_UI/DropDown.h"
#include "../MY_UI/Button.h"

void UI_Ocean::Init(Ocean* Ocean_Demo){

	//set up the UI controls
	
	window = new MY_UI::Controls::Window(MY_UI::Internal::RootWidget);
	window->SetPos(300, 150);
	window->SetSize(300, 400);
	window->SetTitle("Ocean Settings");
	window->SetCloseable(false);// dont allow the window to close

	MY_UI::Controls::HorizontalSlider_w_Label<float>* vsliderlab = new MY_UI::Controls::HorizontalSlider_w_Label<float>(window);
	vsliderlab->Widget->SetRange(3.0f, 21.0f);
	vsliderlab->Widget->SetIncrement(.1f);
	vsliderlab->Widget->SetSliderLength(100);
	vsliderlab->Widget->SetLink(&Ocean_Demo->Wind);
	vsliderlab->SetLabel("Wind Speed", LEFT | IN_LINE, 5);
	vsliderlab->SetPos(5, 5);

	MY_UI::Controls::HorizontalSlider_w_Label<float>* vsliderlab1 = new MY_UI::Controls::HorizontalSlider_w_Label<float>(window);
	vsliderlab1->Widget->SetRange(.3f, 3.0f);
	vsliderlab1->Widget->SetIncrement(.1f);
	vsliderlab1->Widget->SetSliderLength(100);
	vsliderlab1->Widget->SetLink(&Ocean_Demo->Omega);
	vsliderlab1->SetLabel("Inv Wave Age", LEFT | IN_LINE, 5);
	vsliderlab1->Align_To_Widget(vsliderlab, BOTTOM | IN_LINE, 5);

	MY_UI::Controls::HorizontalSlider_w_Label<float>* vsliderlab2 = new MY_UI::Controls::HorizontalSlider_w_Label<float>(window);
	vsliderlab2->Widget->SetRange(.01f, 10.0f);
	vsliderlab2->Widget->SetIncrement(.1f);
	vsliderlab2->Widget->SetSliderLength(100);
	vsliderlab2->Widget->SetLink(&Ocean_Demo->A);
	vsliderlab2->SetLabel("Amplitude", LEFT | IN_LINE, 5);
	vsliderlab2->Align_To_Widget(vsliderlab1, BOTTOM | IN_LINE, 5);

	MY_UI::Controls::CheckBox_w_Label* checkbox = new MY_UI::Controls::CheckBox_w_Label(window);
	checkbox->Widget->SetLink(&Ocean_Demo->DrawWireFrame);
	checkbox->SetLabel("Wire Frame", LEFT | IN_LINE, 5);
	checkbox->Align_To_Widget(vsliderlab2, BOTTOM | IN_LINE, 5);

	MY_UI::Controls::DropDown* dropdown = new MY_UI::Controls::DropDown(window);
	dropdown->Insert("Calm Seas")->OnLeftClicked.Add(std::bind(&Ocean::SetCalmSeas, Ocean_Demo));
	dropdown->Insert("Normal Seas")->OnLeftClicked.Add(std::bind(&Ocean::SetNormalSeas, Ocean_Demo));
	dropdown->Insert("Rough Seas")->OnLeftClicked.Add(std::bind(&Ocean::SetRoughSeas, Ocean_Demo));
	dropdown->SetSizeToContents();
	dropdown->Align_To_Widget(checkbox, BOTTOM | IN_LINE, 5);

	MY_UI::Controls::HorizontalSlider_w_Label<float>* vsliderlab4 = new MY_UI::Controls::HorizontalSlider_w_Label<float>(window);
	vsliderlab4->Widget->SetRange(10, 200.0f);
	vsliderlab4->Widget->SetIncrement(1.0f);
	vsliderlab4->Widget->SetSliderLength(100);
	vsliderlab4->Widget->SetLink(&Ocean_Demo->TesselationFactor);
	vsliderlab4->SetLabel("Tesselation Factor", LEFT | IN_LINE, 5);
	vsliderlab4->Align_To_Widget(dropdown, BOTTOM | IN_LINE, 5);

	MY_UI::Controls::Button* b = new MY_UI::Controls::Button(window);
	b->SetText("Reload Shader");
	b->OnLeftClicked.Add(std::bind(&Ocean::ReloadShaders, Ocean_Demo));
	b->Align_To_Widget(vsliderlab4, BOTTOM | IN_LINE, 5);
}
