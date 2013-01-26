#include "PCH.h"
#include "Window.h"
#include "cWidgetRenderer.h"
#include "cWidgetSkin.h"
#include "Button.h"
#include "cWidgetEvents.h"
#include "Resizable.h"
#include "Image.h"
#include "Text.h"
/*-----------------------------Derived Virutal Functions go here -----------------------------------------*/


MY_UI::Controls::Titlebar::Titlebar(cWidgetBase* parent) : cWidgetBase(parent) {
	auto skin = MY_UI::Internal::WidgetSkin;


	// set the client portion of the window	
	Title = new Text(this);
	Title->SetPos(5, 2);
	Title->SetFontSize(20);
	SetTitle("Window Label");
	int height = max(Title->GetSize().top, static_cast<int>(skin->WindowBar_Without_Focus.height));
	int width = max(Title->GetSize().left, static_cast<int>(skin->WindowBar_Without_Focus.width));
	SetControlBounds(Utilities::Rect(0, 0, width, height) );
	SetClientBounds(Utilities::Rect(0, 0, width, height) );
	SetMinSize(width, height);

	// the code below might be a little verbose, but it allows greater flexibility in case I change the size of things or their positions
	Close_Button = new Button(this);
	Close_Button->ClearImages();
	Close_Button->Set_UnPressed_Texture(skin->Close_Window_Button);// take a copy of the UI skin for the texture
	Close_Button->Set_Pressed_Texture(skin->Close_Window_Button_Pressed);// take a copy of the UI skin for the texture
	Close_Button->SetSize(static_cast<int>(skin->Close_Window_Button.width), static_cast<int>(skin->Close_Window_Button.height));
	Utilities::Point po = GetSize();
	Close_Button->SetPopup("Close_Button");
	Close_Button->SetPos(po.x - static_cast<int>(skin->Close_Window_Button.width - 6.0f) , 4);
	Close_Button->SetName("Close_Button");
	Close_Button->SetImageBorderSize(2, 2);


	Maximize_Button = new Button(this);
	Maximize_Button->ClearImages();
	Maximize_Button->Set_UnPressed_Texture(skin->Maximize_Window_Button);// take a copy of the UI skin for the texture
	Maximize_Button->Set_Pressed_Texture(skin->Maximize_Window_Button_Pressed);// take a copy of the UI skin for the texture

	po = Close_Button->GetPos();
	Maximize_Button->SetPos(po.x - static_cast<int>(skin->Maximize_Window_Button.width - 4.0f ) , 4);
	Maximize_Button->SetSize(static_cast<int>(skin->Maximize_Window_Button.width), static_cast<int>(skin->Maximize_Window_Button.height));
	Maximize_Button->SetPopup("Maximize_Button");
	Maximize_Button->SetName("Maximize_Button");
	Maximize_Button->SetImageBorderSize(2, 2);


	Minimize_Button = new Button(this);
	Minimize_Button->ClearImages();
	Minimize_Button->Set_UnPressed_Texture(skin->Minimize_Window_Button);// take a copy of the UI skin for the texture
	Minimize_Button->Set_Pressed_Texture(skin->Minimize_Window_Button_Pressed);// take a copy of the UI skin for the texture
	
	po = Maximize_Button->GetPos();
	Minimize_Button->SetPos(po.x - static_cast<int>(skin->Minimize_Window_Button.width - 4.0f)  , 4);
	Minimize_Button->SetSize(static_cast<int>(skin->Minimize_Window_Button.width), static_cast<int>(skin->Minimize_Window_Button.height));
	Minimize_Button->SetPopup("Minimize_Button");
	Minimize_Button->SetName("Minimize_Button");
	Minimize_Button->SetImageBorderSize(2, 2);


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
void MY_UI::Controls::Titlebar::SetTitle(const std::string& title){
	Title->SetText(title);
	Title->SetSizeToContents();
}
MY_UI::Controls::cWidgetBase* MY_UI::Controls::Titlebar::Hit(){
	MY_UI::Controls::cWidgetBase* controlhit = MY_UI::Controls::cWidgetBase::Hit();
	if(controlhit == Title) return Bar;
	return controlhit;
}
MY_UI::Controls::cWidgetBase* MY_UI::Controls::Titlebar::Hit_And_SetFocus(){
	return MY_UI::Controls::Titlebar::Hit();
}

void MY_UI::Controls::Titlebar::SetSize(int x, int y){ 
	auto skin = MY_UI::Internal::WidgetSkin;
	MY_UI::Controls::cWidgetBase::SetSize(x, static_cast<int>(skin->WindowBar_With_Focus.height));
	Utilities::Point size = GetSize();

	Bar->SetSize(size.x, Title->GetSize().top); 

	Close_Button->SetPos(size.x + static_cast<int>(-skin->Close_Window_Button.width - 6.0f) , 4);
	Maximize_Button->Align_To_Widget(Close_Button, LEFT | IN_LINE, 2);
	Minimize_Button->Align_To_Widget(Maximize_Button, LEFT | IN_LINE, 2);
	Title->SetLength(Minimize_Button->GetPos().x -9);
}
void MY_UI::Controls::Titlebar::Draw_Clip(){
	Bar->Draw_NoClip();
	Utilities::Rect& clip = Title->Internal_Widget.AbsolutePosition;// this is the clip bounds of this window
	MY_UI::Internal::WidgetRenderer->StartClip(clip);
	Title->Draw_Clip();
	MY_UI::Internal::WidgetRenderer->EndClip();
	Close_Button->Draw_NoClip();
	Minimize_Button->Draw_NoClip();
	Maximize_Button->Draw_NoClip();
	
}




MY_UI::Controls::Window::Window(cWidgetBase* parent): cWidgetBase(parent) { 
	// manually set the starting size of the window

	SetControlBounds(Utilities::Rect(0, 0, 100, 100));

	TitleBar = new Titlebar(this);
	Utilities::Point titlebarsize = TitleBar->GetSize();
	SetClientBounds(Utilities::Rect(0, titlebarsize.y, 100, 100));

	TitleBar->Bar->OnLeftDrag.Add(std::bind(&MY_UI::Controls::Window::Dragging, this));
	TitleBar->Bar->OnLeftDoubleClick.Add(std::bind(&MY_UI::Controls::Window::Maximize, this));
	TitleBar->Close_Button->OnLeftClicked.Add(std::bind(&MY_UI::Controls::Window::Close, this));
	TitleBar->Maximize_Button->OnLeftClicked.Add(std::bind( &MY_UI::Controls::Window::Maximize, this));
	TitleBar->Minimize_Button->OnLeftClicked.Add(std::bind(&MY_UI::Controls::Window::Minimize, this));

	SimpleWindow = new Simple_Window(this);
	DeCouple(SimpleWindow);

	SetMinSize(titlebarsize.x, titlebarsize.y);
	SetSize(100, 100);
}
MY_UI::Controls::Window::~Window(){
	delete SimpleWindow;
}

void MY_UI::Controls::Window::SetFocus(bool focus){
	cWidgetBase::SetFocus(focus);

	if(focus){
		TitleBar->Bar->Show_Selected_Texture();
		SimpleWindow->Background->Set_ShownImage(1);
	} else {
		TitleBar->Bar->Show_UnPressed_Texture();
		SimpleWindow->Background->Set_ShownImage(0);
	}
}

void MY_UI::Controls::Window::SetPos(int x, int y){
	Internal_Widget.Children.push_back(SimpleWindow);
	MY_UI::Controls::cWidgetBase::SetPos(x, y);
	Internal_Widget.Children.pop_back();
}
void MY_UI::Controls::Window::SetSize(int x, int y){ 
	MY_UI::Controls::cWidgetBase::SetSize(x, y);
	Utilities::Point size = GetSize();

	Utilities::Point p = TitleBar->GetSize();
	TitleBar->SetSize(size.x, p.y);
	SimpleWindow->SetSize(size.x, size.y - p.y);
	MY_UI::Controls::cWidgetBase::ForceUpdate();
}

void MY_UI::Controls::Window::Draw_Clip(){
	if(Internal_Widget.Hidden) return;
	Utilities::Rect& clip = Internal_Widget.AbsolutePosition;// this is the clip bounds of this window
	MY_UI::Internal::WidgetRenderer->StartClip(clip);
	if(!Internal_Window.Minimized){
		SimpleWindow->Draw_Clip();
		for(int i((int)Internal_Widget.Children.size()-1); i >= 0 ; i--) {
			Internal_Widget.Children[i]->Draw_Clip(); 
		}
	} else TitleBar->Draw_Clip(); 
	MY_UI::Internal::WidgetRenderer->EndClip();
}
void MY_UI::Controls::Window::Draw_NoClip(){
	Draw_Clip();
}

/*-----------------------------Added Functions go here -----------------------------------------*/

void MY_UI::Controls::Window::SetTitle(const std::string& title){
	TitleBar->Title->SetText(title);
	TitleBar->Title->SetSizeToContents();
}
void MY_UI::Controls::Window::Close(){
	if(!Internal_Window.Closeable) return;
	DeCouple(this);// unattach this widget from a parent if there is any
	MY_UI::Internal::RootWidget->DelayedDelete(this);// add this to a delayed delete in case this close is called internally
	//Delaying the deletion of components is very important because it could be done by the user, or internally so I want the object to
	//exist through the entire loop in case any components need it

}

void MY_UI::Controls::Window::Restore(){
	SetPos(Internal_Window.Old_Pos.x, Internal_Window.Old_Pos.y);
	SetSize(Internal_Window.Old_Size.x, Internal_Window.Old_Size.y);
}
void MY_UI::Controls::Window::Minimize(){
	if(!Internal_Window.Minimizeable) return;
	auto skin = MY_UI::Internal::WidgetSkin;
	if(Internal_Window.Minimized){// the window is in a minimized state.. this function should restore the window
		TitleBar->Minimize_Button->ClearImages();
		TitleBar->Minimize_Button->Set_UnPressed_Texture(skin->Minimize_Window_Button);// take a copy of the UI skin for the texture
		TitleBar->Minimize_Button->Set_Pressed_Texture(skin->Minimize_Window_Button_Pressed);// take a copy of the UI skin for the texture
		Restore();
		Internal_Window.Minimized = false;
		
	}else {// window is being minimized.. save old settings so it can be restored
		if(Internal_Window.Maximized){ // if minimizing from a maximized state, make sure to set maximized to false and set the texture correctly
			TitleBar->Maximize_Button->ClearImages();
			TitleBar->Maximize_Button->Set_UnPressed_Texture(skin->Maximize_Window_Button);// take a copy of the UI skin for the texture
			TitleBar->Maximize_Button->Set_Pressed_Texture(skin->Maximize_Window_Button_Pressed);// take a copy of the UI skin for the texture

			Internal_Window.Maximized = false;
		} else {
			Internal_Window.Old_Size = GetSize();
			Internal_Window.Old_Pos = GetPos();
		}
		Utilities::Point p = Internal_Widget.Parent->GetSize();// get the parents size so the minimized window can be placed correctly
		Utilities::Point minsize = GetMinSize();
		SetPos(0, abs(p.y - minsize.y) );
		SetSize(minsize.x, minsize.y);	
		TitleBar->Minimize_Button->ClearImages();
		TitleBar->Minimize_Button->Set_UnPressed_Texture(skin->Restore_Window_Button);// take a copy of the UI skin for the texture
		TitleBar->Minimize_Button->Set_Pressed_Texture(skin->Restore_Window_Button_Pressed);// take a copy of the UI skin for the texture

		Internal_Window.Minimized = true;
	}
}
void MY_UI::Controls::Window::Maximize(){
	if(!Internal_Window.Maximizeable) return;
	auto skin = MY_UI::Internal::WidgetSkin;
	if(Internal_Window.Maximized){// the window is in a maximized state.. this function should restore the window
		TitleBar->Maximize_Button->ClearImages();
		TitleBar->Maximize_Button->Set_UnPressed_Texture(skin->Maximize_Window_Button);// take a copy of the UI skin for the texture
		TitleBar->Maximize_Button->Set_Pressed_Texture(skin->Maximize_Window_Button_Pressed);// take a copy of the UI skin for the texture

		Restore();
		Internal_Window.Maximized = false;
	}else {// window is being maximized.. save old settings so it can be restored	
		if(Internal_Window.Minimized){ // if maximizing from a minimized state, make sure to set minimized to false and set the texture correctly
			Internal_Window.Minimized = false;
			TitleBar->Minimize_Button->ClearImages();
			TitleBar->Minimize_Button->Set_UnPressed_Texture(skin->Minimize_Window_Button);// take a copy of the UI skin for the texture
			TitleBar->Minimize_Button->Set_Pressed_Texture(skin->Minimize_Window_Button_Pressed);// take a copy of the UI skin for the texture

		} else {
			Internal_Window.Old_Size = GetSize();
			Internal_Window.Old_Pos = GetPos();
		}
		SetPos(0, 0);
		Utilities::Point p = Internal_Widget.Parent->GetSize();// get the parents size so the maximized window can be sized correctly
	
		SetSize(p.x, p.y);	

		TitleBar->Maximize_Button->ClearImages();
		TitleBar->Maximize_Button->Set_UnPressed_Texture(skin->Restore_Window_Button);// take a copy of the UI skin for the texture
		TitleBar->Maximize_Button->Set_Pressed_Texture(skin->Restore_Window_Button_Pressed);// take a copy of the UI skin for the texture

		Internal_Window.Maximized = true;
	}
}
void MY_UI::Controls::Window::Dragging(){	
	Utilities::Point p = GetPos();
	p.x += New_MousePosx - Old_MousePosx;
	p.y += New_MousePosy - Old_MousePosy;
	SetPos(p.x, p.y);
}



///////////////////////////////////////////////////////////////////////////////////////SIMPLE Window

MY_UI::Controls::Simple_Window::Simple_Window(cWidgetBase* parent) : cWidgetBase(parent) {
	SetName("Simple Window");
	SetControlBounds(Utilities::Rect(0, 0, 100, 100));
	SetClientBounds(Utilities::Rect(0, 0, 100, 100));

	auto skin = MY_UI::Internal::WidgetSkin;
	// setting the controls to NULL means that they will not be inserted in the children array
	Background = new Image(this);
	Background->Add_Texture(skin->Window_Without_Focus);// take a copy of the UI skin for the texture
	Background->Add_Texture(skin->Window_With_Focus);// take a copy of the UI skin for the texture
	Background->Set_ShownImage(0);
	Background->SetName("Background");
	Background->SetImageBorderSize(1,1);
	Background->SetBorderDrawn(true);

	DeCouple(Background);
	SetDraggable(true);
	OnLeftDrag.Add(std::bind( &MY_UI::Controls::Simple_Window::Dragging, this));
	SetSize(100, 100);
}
MY_UI::Controls::Simple_Window::~Simple_Window(){
	delete Background;
}
void MY_UI::Controls::Simple_Window::SetFocus(bool focus){
	cWidgetBase::SetFocus(focus);
	if(focus) {
		Background->Set_ShownImage(1);
	}else {
		Background->Set_ShownImage(0);
	}
}
void MY_UI::Controls::Simple_Window::SetSize(int x, int y){
	MY_UI::Controls::cWidgetBase::SetSize(x, y);
	Background->SetSize(x, y);
	MY_UI::Controls::cWidgetBase::ForceUpdate();
}
void MY_UI::Controls::Simple_Window::SetPos(int x, int y){
	Internal_Widget.Children.push_back(Background);
	MY_UI::Controls::cWidgetBase::SetPos(x, y);
	Internal_Widget.Children.pop_back();
}
void MY_UI::Controls::Simple_Window::Draw_Clip(){
	if(Internal_Widget.Hidden) return;
	Utilities::Rect& clip = Internal_Widget.AbsolutePosition;// this is the clip bounds of this window
	MY_UI::Internal::WidgetRenderer->StartClip(clip);
	Background->Draw_Clip();
	for(int i((int)Internal_Widget.Children.size()-1); i >= 0 ; i--) {
		Internal_Widget.Children[i]->Draw_Clip(); 
	}
	MY_UI::Internal::WidgetRenderer->EndClip();
}
void MY_UI::Controls::Simple_Window::Draw_NoClip(){
	Draw_Clip();
}
void MY_UI::Controls::Simple_Window::Dragging(){	
	if(GetDraggable()){
		Utilities::Point p = GetPos();
		p.x += New_MousePosx - Old_MousePosx;
		p.y += New_MousePosy - Old_MousePosy;
		SetPos(p.x, p.y);
	}
}