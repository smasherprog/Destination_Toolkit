#include "stdafx.h"
#include "Test_Helper.h"
#include "Tab_Control.h"
#include "Text.h"
#include "Button.h"
#include "Window.h"

void Tabs(){
	delete Dummy;
	Dummy = new MY_UI::Controls::cWidget_Dummy(root);
	Dummy->SetControlBounds(root->GetControlBounds());
	Dummy->SetClientBounds(root->GetClientBounds());

	MY_UI::Controls::Tab_Control* tab = new MY_UI::Controls::Tab_Control(Dummy);
	tab->SetSize(300, 300);
	tab->SetPos(300, 300);
	MY_UI::Controls::Simple_Window* window = tab->Insert("tab");

	MY_UI::Controls::Text* txt = new MY_UI::Controls::Text(window);
	txt->SetPos(10, 10);
	txt->SetText(" I want to see this GOO!!");
	txt->SetSizeToContents();

	MY_UI::Controls::Button* b = new MY_UI::Controls::Button(window);
	b->Align_To_Widget(txt, BOTTOM | RIGHT, 10);
	b->SetText("st btn");

	window = tab->Insert("control ");

	txt = new MY_UI::Controls::Text(window);
	txt->SetPos(100, 10);
	txt->SetText(" in working order !!");
	txt->SetSizeToContents();

	b = new MY_UI::Controls::Button(window);
	b->Align_To_Widget(txt, BOTTOM | RIGHT, 10);
	b->SetText("see btn");

	window = tab->Insert("otro tab ");

	txt = new MY_UI::Controls::Text(window);
	txt->SetPos(30, 100);
	txt->SetText(" DISSPAERING ACT! !!");
	txt->SetSizeToContents();

	b = new MY_UI::Controls::Button(window);
	b->Align_To_Widget(txt, BOTTOM | RIGHT, 10);
	b->SetText("Not normal");


}