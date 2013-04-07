#include "PCH.h"
#include "Collapsible_List.h"
#include "ISkin.h"
#include "Image.h"
#include "IInput.h"
#include "Text.h"

MY_UI_Too::Controls::Collapsible_List::Collapsible_List(MY_UI_Too::Interfaces::IWidget* parent) : Widget(parent) {

	_Background =  new MY_UI_Too::Controls::Image(nullptr);
	_Internals.Children.push_back(_Background);

	_Title = new MY_UI_Too::Controls::Text(nullptr);
	_Internals.Children.push_back(_Title);
	_Title->Set_Parent(this);
	_Title->Set_Text("Window Title");
	_Title->Set_Font_Size(18);
	_Title->Set_Pos(Utilities::Point(10, 15));
	_Title->Set_Hitable(false);

	//Set_Size(Utilities::Point(_Background->Get_Size().x, _Background->Get_Size().y + INTERNAL_WND_TITLE_SIZE));
}

MY_UI_Too::Controls::Collapsible_List::~Collapsible_List(){

}
void MY_UI_Too::Controls::Collapsible_List::Mouse_Moved(){
	MY_UI_Too::Controls::Widget::Mouse_Moved();
	if(Mouse_LButton_Down) Set_Pos_ByOffset(Utilities::Point(Delta_Mousex, Delta_Mousey));
}
void MY_UI_Too::Controls::Collapsible_List::Set_Size(Utilities::Point size) {
	MY_UI_Too::Controls::Widget::Set_Size(size);
	/*
	int parentcenter_x = Get_Size().x/2 + INTERNAL_COLLAPSIBLE_BORDER/2;
	int top_y = INTERNAL_WND_TITLE_SIZE;

	for(auto beg = _Internals.Children.begin()+INTERNAL_WND_CONTROLS; beg != _Internals.Children.end(); beg++){
		(*beg)->Set_Size_To_Contents();
		Utilities::Point size = (*beg)->Get_Size();
		(*beg)->Set_Size(Get_Size() - Utilities::Point(INTERNAL_COLLAPSIBLE_BORDER, INTERNAL_COLLAPSIBLE_BORDER));
		(*beg)->Set_Pos(Utilities::Point(parentcenter_x - (size.x/2), top_y));
		top_y += (*beg)->Get_Size().y;
	}
	*/
}
void MY_UI_Too::Controls::Collapsible_List::Add_Child(IWidget* child){
	MY_UI_Too::Controls::Widget::Add_Child(child);// call the lower level after
	child->Set_Absolute_Pos(Get_Absolute_Pos());
	Set_Size(Get_Size());// force all the objects to reorient.. this is a cheat!
}
void MY_UI_Too::Controls::Collapsible_List::Remove_Child(IWidget* child){
	MY_UI_Too::Controls::Widget::Remove_Child(child);
	Set_Size(Get_Size());// force all the objects to reorient.. this is a cheat!
}
