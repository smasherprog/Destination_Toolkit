#include "PCH.h"
#include "Collapsible_List.h"
#include "cWidgetRenderer.h"
#include "cWidgetSkin.h"
#include "List_Box.h"
#include "Button.h"
#include "cWidgetEvents.h"
#include "Resizable.h"
#include "Image.h"
#include "Text.h"


MY_UI::Controls::Titlebar_Min::Titlebar_Min(cWidgetBase* parent) : cWidgetBase(parent) {
	auto skin = MY_UI::Internal::WidgetSkin;

	Title = new Text(this);
	Title->SetPos(5, 2);
	Title->SetFontSize(20);
	SetTitle("Window Label");

	int height = max(Title->GetSize().top, static_cast<int>(skin->WindowBar_Without_Focus.height));
	int width = max(Title->GetSize().left, static_cast<int>(skin->WindowBar_Without_Focus.width));
	SetControlBounds(Utilities::Rect(0, 0, width, height) );
	SetClientBounds(Utilities::Rect(0, 0, width, height) );
	SetMinSize(width, height);
	// set the client portion of the window	

	Bar = new Button(this);
	Bar->ClearImages();
	Bar->Set_UnPressed_Texture(skin->WindowBar_Without_Focus);// take a copy of the UI skin for the texture
	Bar->Set_Selected_Texture(skin->WindowBar_With_Focus);// take a copy of the UI skin for the texture
	Bar->Set_Hovered_Texture(skin->WindowBar_With_Focus);
	
	Bar->SetSize(static_cast<int>(skin->WindowBar_Without_Focus.width), static_cast<int>(skin->WindowBar_Without_Focus.height));
	Bar->SetName("Bar");
	Bar->SetDraggable(true);
	Bar->SetSize(static_cast<int>(skin->WindowBar_Without_Focus.width), Title->GetSize().top); 
	Bar->SetImageBorderSize(1, 1);

	SetSize(width, height);
}
void MY_UI::Controls::Titlebar_Min::SetTitle(const std::string& title){
	Title->SetText(title);
	Title->SetSizeToContents();
}
MY_UI::Controls::cWidgetBase* MY_UI::Controls::Titlebar_Min::Hit(){
	MY_UI::Controls::cWidgetBase* controlhit = MY_UI::Controls::cWidgetBase::Hit();
	if(controlhit == Title) return Bar;
	return controlhit;
}
MY_UI::Controls::cWidgetBase* MY_UI::Controls::Titlebar_Min::Hit_And_SetFocus(){
	return MY_UI::Controls::Titlebar_Min::Hit();
}

void MY_UI::Controls::Titlebar_Min::SetSize(int x, int y){ 
	MY_UI::Controls::cWidgetBase::SetSize(x, y);
	Utilities::Point size = GetSize();
	auto skin = MY_UI::Internal::WidgetSkin;
	Bar->SetSize(static_cast<int>(skin->WindowBar_Without_Focus.width), Title->GetSize().top); 

	Title->Align_Within_Parent( TOP | CENTER);
}


/*-----------------------------Derived Virutal Functions go here -----------------------------------------*/


MY_UI::Controls::Collapsible_List::Collapsible_List(cWidgetBase* parent): cWidgetBase(parent) { 
	// manually set the starting size of the window

	TitleBar = new Titlebar_Min(this);
	Utilities::Point titlebarsize = TitleBar->GetSize();

	SetControlBounds(Utilities::Rect(0, 0, titlebarsize.x, 100));
	TitleBar->SetDraggable(true);

	TitleBar->Bar->OnLeftDrag.Add(std::bind(&MY_UI::Controls::Collapsible_List::Dragging, this));
	TitleBar->Bar->OnLeftClicked.Add(std::bind(&MY_UI::Controls::Collapsible_List::Minimize, this));
	
	SetMinSize(titlebarsize.x, titlebarsize.y);

	SetClientBounds(Utilities::Rect(0, titlebarsize.y, titlebarsize.x, 100));
	List_Box = new MY_UI::Controls::List_Box(this);
	auto skin = MY_UI::Internal::WidgetSkin;
	// setting the controls to NULL means that they will not be inserted in the children array

	SetSize(titlebarsize.x, 100);
	OnFocusChange.Add(std::bind(&MY_UI::Controls::Collapsible_List::FocusChanged, this));
}
void MY_UI::Controls::Collapsible_List::FocusChanged(){
	if(GetFocus()) TitleBar->Bar->Show_Selected_Texture();
	else  TitleBar->Bar->Show_UnPressed_Texture();
}
void MY_UI::Controls::Collapsible_List::SetSize(int x, int y){ 
	MY_UI::Controls::cWidgetBase::SetSize(x, y);
	Utilities::Point size = GetSize();
	TitleBar->SetSize(size.x, TitleBar->GetMinSize().y);
	Utilities::Point p = TitleBar->GetSize();
	size.y -= p.y;
	List_Box->SetSize(size.x, size.y);
}

void MY_UI::Controls::Collapsible_List::Draw_Clip(){
	if(!Internal_CollapsibleList.Minimized) MY_UI::Controls::cWidgetBase::Draw_Clip();
	else TitleBar->Draw_Clip(); 	
}
void MY_UI::Controls::Collapsible_List::Draw_NoClip(){
	if(!Internal_CollapsibleList.Minimized) MY_UI::Controls::cWidgetBase::Draw_NoClip();
	else TitleBar->Draw_NoClip(); 	
}
/*-----------------------------Added Functions go here -----------------------------------------*/

void MY_UI::Controls::Collapsible_List::SetTitle(const std::string& title){
	TitleBar->Title->SetText(title);
	TitleBar->Title->SetSizeToContents();
}

void MY_UI::Controls::Collapsible_List::Restore(){
	SetPos(Internal_CollapsibleList.Old_Pos.x, Internal_CollapsibleList.Old_Pos.y);
	SetSize(Internal_CollapsibleList.Old_Size.x, Internal_CollapsibleList.Old_Size.y);
}
void MY_UI::Controls::Collapsible_List::Minimize(){
	auto skin = MY_UI::Internal::WidgetSkin;
	if(Internal_CollapsibleList.Minimized){// the window is in a minimized state.. this function should restore the window
		Restore();
		Internal_CollapsibleList.Minimized = false;
		
	}else {// window is being minimized.. save old settings so it can be restored
		Internal_CollapsibleList.Old_Size = GetSize();
		Internal_CollapsibleList.Old_Pos = GetPos();
		
		Utilities::Point p = Internal_Widget.Parent->GetSize();// get the parents size so the minimized window can be placed correctly

		SetPos(0, abs(p.y - GetMinSize().y) );
		Internal_CollapsibleList.Minimized = true;
	}
}
void MY_UI::Controls::Collapsible_List::Dragging(){	
	Utilities::Point p = GetPos();
	p.x += New_MousePosx - Old_MousePosx;
	p.y += New_MousePosy - Old_MousePosy;
	SetPos(p.x, p.y);
}
MY_UI::Controls::Button* MY_UI::Controls::Collapsible_List::Insert(std::string text){ return List_Box->Insert(text); } 
bool MY_UI::Controls::Collapsible_List::Erase(std::string text){ return List_Box->Erase(text); }
bool MY_UI::Controls::Collapsible_List::Erase(MY_UI::Controls::Button* widget){ return List_Box->Erase(widget); }