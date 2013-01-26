#include "PCH.h"
#include "Tab_Control.h"
#include "Window.h"
#include "Button.h"
#include "cWidgetSkin.h"
#include "Text.h"

MY_UI::Controls::Tab_Control::Tab_Control(cWidgetBase* parent) :cWidgetBase(parent) {
	SetControlBounds(Utilities::Rect(0, 0, 100, 100));
	auto skin = MY_UI::Internal::WidgetSkin;
	SetClientBounds(Utilities::Rect(0, static_cast<int>(skin->Tab_Hovered.height), 100, 100 - static_cast<int>(skin->Tab_Hovered.height)));
	CurrentPage=-1;
}
MY_UI::Controls::Simple_Window* MY_UI::Controls::Tab_Control::Insert(std::string text){
	MY_UI::Controls::Tab_Button* b = new MY_UI::Controls::Tab_Button(this);
	b->SetText(text);
	
	MY_UI::Controls::Simple_Window* p = new MY_UI::Controls::Simple_Window(this);
	auto skin = MY_UI::Internal::WidgetSkin;
	p->SetDraggable(false);// do not allow moveing the window
	if(!Tabs.empty()){
		b->Align_To_Widget(Tabs.back(), RIGHT | IN_LINE, 2);
	} else b->SetPos(0, static_cast<int>(-skin->Tab_Hovered.height));

	p->Align_To_Widget(b, BOTTOM, -2);
	b->OnLeftClicked.Add(std::bind(&MY_UI::Controls::Tab_Control::SwitchPage, this, std::placeholders::_1), this, true);
	auto parent = Internal_Widget.Parent;
	Utilities::Point size;
	if(Internal_Widget.Alignment == FILL){
		if(parent != 0 ){
			Utilities::Rect r = parent->GetClientBounds();	
			size.x = r.width;
			size.y = r.height;
		}			
	} else size = GetSize();
	p->SetSize(size.x, size.y - b->GetSize().y);
	Tabs.push_back(b);
	Pages.push_back(p);
	if(CurrentPage == -1){// make sure to set up the first page correctly.
		CurrentPage = 0;
		Pages[CurrentPage]->SetFocus(true);
		Pages[CurrentPage]->SetHidden(false);// unhide this window
		b->Show_Selected_Texture();
	} else {
		b->Show_UnPressed_Texture();
		p->SetHidden(true);// hide the active page
	}
	return p;
}

bool MY_UI::Controls::Tab_Control::Erase(std::string text){
	int index =-1;
	for(size_t i(0); i <Tabs.size(); i++){
		if(Tabs[i]->GetText() == text){// found the index of the tab
			index = static_cast<int>(i);
			break;
		}
	}	
	if(index == -1) return false;// could not find it
	delete Tabs[index];
	Tabs.erase(Tabs.begin() + index);
	delete Pages[index];
	Pages.erase(Pages.begin() + index);
	if(Tabs.empty()){
		CurrentPage =-1;
		return true;
	}
	CurrentPage = Utilities::Clamp<int>(CurrentPage, 0, static_cast<int>(Tabs.size()));// ensure proper value
	SwitchPage( *(Tabs.begin() + CurrentPage) );// swtich pages
	return true;
}
bool MY_UI::Controls::Tab_Control::Erase(MY_UI::Controls::Simple_Window* widget){
	int index =-1;
	for(size_t i(0); i <Pages.size(); i++){
		if(Pages[i] == widget){// found the index of the page
			index = static_cast<int>(i);
			break;
		}
	}	
	if(index == -1) return false;// could not find it
	delete Tabs[index];
	Tabs.erase(Tabs.begin() + index);
	delete Pages[index];
	Pages.erase(Pages.begin() + index);
	if(Tabs.empty()){
		CurrentPage =-1;
		return true;
	}
	CurrentPage = Utilities::Clamp<int>(CurrentPage, 0, static_cast<int>(Tabs.size()));// ensure proper value
	SwitchPage( *(Tabs.begin() + CurrentPage) );// swtich pages
	return true;
}
void MY_UI::Controls::Tab_Control::SwitchPage(MY_UI::Controls::cWidgetBase* widget){
	auto skin = MY_UI::Internal::WidgetSkin;
	size_t found =-1;
	for(size_t i(0); i <Tabs.size(); i++){
		if(widget == Tabs[i]){// found the index of the tab
			found = i;
			break;
		}
	}	
	if(found != CurrentPage) {
		if(CurrentPage != -1) {
			Tabs[CurrentPage]->Show_UnPressed_Texture();
			Pages[CurrentPage]->SetHidden(true);// hide the active page
		}
	}
	CurrentPage = static_cast<int>(found);
	Pages[CurrentPage]->SetHidden(false);// unhide this window
	Pages[CurrentPage]->SetFocus(true);

}

void MY_UI::Controls::Tab_Control::SetSize(int x, int y){
	MY_UI::Controls::cWidgetBase::SetSize(x, y);
	if(Tabs.empty()) return;
	Utilities::Point size = GetSize();
	size.y -= Tabs[0]->GetSize().y;
	for(size_t i(0); i <Pages.size(); i++){
		Pages[i]->SetSize(size.x, size.y);
	}
}
void MY_UI::Controls::Tab_Control::Update(){
	if(!Internal_Widget.Needs_Update) return; 
	auto parent = Internal_Widget.Parent;
	if(Internal_Widget.Alignment == FILL){
		if(parent != 0 ){
			Utilities::Rect r = parent->GetClientBounds();	
			SetSize(r.width, r.height);
		}			
	} 
	ForceUpdate();// force all the children to update themselves
}