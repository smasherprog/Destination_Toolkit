#include "stdafx.h"
#include "Test_Helper.h"
#include "Window.h"
#include "Button.h"
#include "Scrollable.h"
#include "Text.h"
#include "Resizable.h"
#include "Tab_Control.h"
#include "DropDown.h"
#include "Value_Adjuster.h"
#include "File_Selector.h"

void Create_NonResizableWindow(){
	MY_UI::Controls::Window* window = new MY_UI::Controls::Window(Dummy);
	window->SetPos((rand()% 900), (rand()% 500));
	window->SetSize((rand()% 100) + 300, (rand()% 100) + 150);
	window->SetTitle("Non Resizable Window");

	MY_UI::Controls::Text* text = new MY_UI::Controls::Text(window);
	text->SetPos(20, 60);
	text->SetText("This is a window that cannot be resized by the user ....");
}
void Create_SimpleWindow(){
	MY_UI::Controls::Simple_Window* window = new MY_UI::Controls::Simple_Window(Dummy);
	window->SetPos((rand()% 900), (rand()% 500));
	window->SetSize((rand()% 100) + 300, (rand()% 100) + 150);

	MY_UI::Controls::Text* text = new MY_UI::Controls::Text(window);
	text->SetPos(20, 60);
	text->SetText("This is a stupid simple window....");

	MY_UI::Controls::DropDown* drop = new MY_UI::Controls::DropDown(window);
	drop->SetPos(40, 90);
	drop->Insert("hey there");
	drop->Insert("see there");
	drop->Insert("see you");

}
void Create_ResizableWindow(){
	MY_UI::Controls::cWidget_w_Resizable<MY_UI::Controls::Window>* resizablewindow = new MY_UI::Controls::cWidget_w_Resizable<MY_UI::Controls::Window>(Dummy);
	resizablewindow->SetPos((rand()% 900), (rand()% 500));
	resizablewindow->SetSize((rand()% 100) + 300, (rand()% 100) + 150);
	resizablewindow->SetTitle("Resizable Window");

	MY_UI::Controls::Button* pbutton = new MY_UI::Controls::Button(resizablewindow);
	pbutton->SetText("Window", CENTER);
	pbutton->SetPos(0, 0);
	pbutton->SetPopup("button");

	MY_UI::Controls::Text* text = new MY_UI::Controls::Text(resizablewindow);
	text->SetPos(20, 60);
	text->SetText("This is a resizable window....");

}
void Create_ScrollableWindow(){
	MY_UI::Controls::cWidget_w_Resizable<MY_UI::Controls::Window>* resizablewindow = new MY_UI::Controls::cWidget_w_Resizable<MY_UI::Controls::Window>(Dummy);
	
	resizablewindow->SetPos((rand()% 900), (rand()% 500));
	resizablewindow->SetSize((rand()% 100) + 200, (rand()% 100) + 150);
	resizablewindow->SetTitle("Resizable Window");

	MY_UI::Controls::Scrollable* scroll = new MY_UI::Controls::Scrollable(resizablewindow);
	scroll->SetScrollableSize(400, 400);
	scroll->Align_Within_Parent(FILL);	
	
	MY_UI::Controls::Button* pbutton = new MY_UI::Controls::Button(scroll);
	pbutton->SetText("Window", CENTER);
	pbutton->SetPos(200, 300);

	MY_UI::Controls::Text* text = new MY_UI::Controls::Text(scroll);
	text->SetPos(5, 5);
	text->SetText("This is a resizable window with scrollbars inside !....");
}
void FileSelected(MY_UI::Controls::cWidgetBase* widget){
	MY_UI::Controls::File_Selector* f = static_cast<MY_UI::Controls::File_Selector*>(widget);
	OUTPUT_DEBUG_MSG("File Selected is: "<<f->GetSelectedFile());
	OUTPUT_DEBUG_MSG("Path to File Selected is: "<<f->GetPathToSelectedFile());
}
void Files(){
	MY_UI::Controls::File_Selector* f = new MY_UI::Controls::File_Selector(Dummy);
	f->SetPos(300, 300);
	f->OnSelected.Add(FileSelected, true);
	f->SetSearchFilter("*.exe;*.dll");
}

void Create_Window_w_tabs(){

	MY_UI::Controls::cWidget_w_Resizable<MY_UI::Controls::Window>* resizablewindow = new MY_UI::Controls::cWidget_w_Resizable<MY_UI::Controls::Window>(Dummy);
	resizablewindow->SetPos((rand()% 900), (rand()% 500));
	resizablewindow->SetSize((rand()% 100) + 300, (rand()% 100) + 150);
	resizablewindow->SetTitle("Resizable Window");

	MY_UI::Controls::Tab_Control* tab = new MY_UI::Controls::Tab_Control(resizablewindow);
	tab->Align_Within_Parent(FILL);
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

	window = tab->Insert("tab 3");

	MY_UI::Controls::Button* pbutton = new MY_UI::Controls::Button(window);
	pbutton->SetText("Window", CENTER);
	pbutton->SetPos(0, 0);
	pbutton->SetPopup("button");

	MY_UI::Controls::Text* text = new MY_UI::Controls::Text(window);
	text->SetPos(20, 60);
	text->SetText("This is a resizable window....");
}
void Create_Window_w_tabs_w_scrollbar(){
	
	MY_UI::Controls::cWidget_w_Resizable<MY_UI::Controls::Window>* resizablewindow = new MY_UI::Controls::cWidget_w_Resizable<MY_UI::Controls::Window>(Dummy);
	resizablewindow->SetPos((rand()% 900), (rand()% 500));
	resizablewindow->SetSize((rand()% 100) + 300, (rand()% 100) + 150);
	resizablewindow->SetTitle("Resizable Window");

	MY_UI::Controls::Tab_Control* tab = new MY_UI::Controls::Tab_Control(resizablewindow);
	tab->Align_Within_Parent(FILL);
	MY_UI::Controls::Simple_Window* window = tab->Insert("tab");

	MY_UI::Controls::Scrollable* scr =new  MY_UI::Controls::Scrollable(window);
	scr->Align_Within_Parent(FILL);

	MY_UI::Controls::Text* txt = new MY_UI::Controls::Text(scr);
	txt->SetPos(400, 400);
	txt->SetText(" I want to see this GOO!!");
	txt->SetSizeToContents();

	MY_UI::Controls::Button* b = new MY_UI::Controls::Button(scr);
	b->Align_To_Widget(txt, BOTTOM | RIGHT, 10);
	b->SetText("st btn");

}
void WindowExamples(){
	delete Dummy;
	Dummy = new MY_UI::Controls::cWidget_Dummy(root);
	Dummy->SetControlBounds(root->GetControlBounds());
	Dummy->SetClientBounds(root->GetClientBounds());

	MY_UI::Controls::Button* pbutton = new MY_UI::Controls::Button(Dummy);
	
	pbutton->SetText("Window", CENTER);
	pbutton->SetPos(300, 250);
	pbutton->SetPopup("Create a Non Resizable window");
	pbutton->OnLeftClicked.Add(Create_NonResizableWindow);

	MY_UI::Controls::Button* button = new MY_UI::Controls::Button(Dummy);
	
	button->SetText("Window", CENTER);
	button->SetPopup("Create a Resizable window");
	button->OnLeftClicked.Add(Create_ResizableWindow);
	button->Align_To_Widget(pbutton, RIGHT | IN_LINE, 20);

	pbutton = new MY_UI::Controls::Button(Dummy);
	pbutton->SetText("Window", CENTER);
	pbutton->SetPopup("Create a window with scrollbars");
	pbutton->OnLeftClicked.Add(Create_ScrollableWindow);
	pbutton->Align_To_Widget(button, RIGHT | IN_LINE, 20);

	button = new MY_UI::Controls::Button(Dummy);
	button->SetText("Window", CENTER);
	button->SetPopup("Create a Simple window");
	button->OnLeftClicked.Add(Create_SimpleWindow);
	button->Align_To_Widget(pbutton, RIGHT | IN_LINE, 20);

	pbutton = new MY_UI::Controls::Button(Dummy);
	pbutton->SetText("Window", CENTER);
	pbutton->SetPopup("Create a window with tabs inside");
	pbutton->OnLeftClicked.Add(Create_Window_w_tabs);
	pbutton->Align_To_Widget(button, RIGHT | IN_LINE, 20);

	button = new MY_UI::Controls::Button(Dummy);
	button->SetText("Window", CENTER);
	button->SetPopup("Create a window with tabs and scrollbars inside the tabs");
	button->OnLeftClicked.Add(Create_Window_w_tabs_w_scrollbar);
	button->Align_To_Widget(pbutton, RIGHT | IN_LINE, 20);
	
	pbutton = new MY_UI::Controls::Button(Dummy);
	pbutton->SetText("Files", CENTER);
	pbutton->SetPopup("Create file Selector");
	pbutton->OnLeftClicked.Add(Files);
	pbutton->Align_To_Widget(button, BOTTOM | IN_LINE, 20);

}