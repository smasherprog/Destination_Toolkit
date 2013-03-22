#include "PCH.h"
#include "Window.h"
#include "ISkin.h"
#include "Image.h"
#include "IInput.h"

MY_UI_Too::Controls::Window::Window(MY_UI_Too::Interfaces::IWidget* parent) : Widget(parent) {
	MY_UI_Too::Controls::Image* img =  new MY_UI_Too::Controls::Image(nullptr);
	_Internals.Children.push_back(img);
	img->Set_Parent(this);
	img->Set_Texture(MY_UI_Too::Internal::UI_Skin->Get_Skin(), false);
	img->Set_UVs(MY_UI_Too::Internal::UI_Skin->Get_Window_Top());
	img->Set_Size(Utilities::Point(360, 35));
	img->Set_Draggable(true);
	img->On_Mouse_Moved.Connect(&On_Mouse_Moved, std::bind(&MY_UI_Too::Controls::Window::Mouse_Moved, this));
	img->Set_Name("Window_top");
	img->Set_Pos(Utilities::Point(0, 0));

	_Background =  new MY_UI_Too::Controls::Image(nullptr);
	_Internals.Children.push_back(_Background);
	_Background->Set_Parent(this);
	_Background->Set_Texture(MY_UI_Too::Internal::UI_Skin->Get_Skin(), false);
	_Background->Set_UVs(MY_UI_Too::Internal::UI_Skin->Get_Window_Bottom());
	_Background->Set_Size(Utilities::Point(360, 225));
	_Background->Set_Pos(Utilities::Point(0, 35));
	_Background->Set_Name("Window_bottom");
}

MY_UI_Too::Controls::Window::~Window(){
}
void MY_UI_Too::Controls::Window::Mouse_Moved(){
	MY_UI_Too::Controls::Widget::Mouse_Moved();
	if(Mouse_LButton_Down) Set_Pos_ByOffset(Utilities::Point(Delta_Mousex, Delta_Mousey));
}

void MY_UI_Too::Controls::Window::Add_Child(IWidget* child){
	MY_UI_Too::Controls::Widget::Add_Child(child);
	child->Set_Absolute_Pos(Get_Absolute_Pos() + Utilities::Point(5, 35));
}
MY_UI_Too::Interfaces::IWidget* MY_UI_Too::Controls::Window::Hit() {
	for(auto &x : _Internals.Children){
		MY_UI_Too::Interfaces::IWidget* hitcontrol = x->Hit();
		if(hitcontrol != nullptr && hitcontrol != _Background) return hitcontrol;
	}
	Utilities::Rect rect(_Internals.Absolute_TL.left, _Internals.Absolute_TL.top, _Internals.Size.x, _Internals.Size.y);
	if(rect.Intersect(Utilities::Point(New_MousePosx, New_MousePosy))) return this;
	return nullptr;
}
MY_UI_Too::Interfaces::IWidget* MY_UI_Too::Controls::Window::Hit_And_SetFocus(){
	for(auto beg = _Internals.Children.begin(); beg != _Internals.Children.end(); beg++){
		MY_UI_Too::Interfaces::IWidget* hitcontrol = (*beg)->Hit_And_SetFocus();
		if(hitcontrol != nullptr && hitcontrol != _Background) {
			if(beg != _Internals.Children.begin()){// dont rearrange if the control hit is already at the beginning
				MY_UI_Too::Interfaces::IWidget* temp = (*beg);
				_Internals.Children.erase(beg);// remove from the array
				_Internals.Children.insert(_Internals.Children.begin() +2, temp);// insert at the beginning
			}
			return hitcontrol;
		}
	}
	Utilities::Rect rect(_Internals.Absolute_TL.left, _Internals.Absolute_TL.top, _Internals.Size.x, _Internals.Size.y);
	if(rect.Intersect(Utilities::Point(New_MousePosx, New_MousePosy))) return this;
	return nullptr;
}