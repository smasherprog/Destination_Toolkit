#include "PCH.h"
#include "ISkin.h"
#include "Radio_Group.h"
#include "Button.h"


MY_UI_Too::Controls::Radio_Button::Radio_Button(IWidget* parent, std::function<void(Radio_Button*)> func): Widget(parent) {
	Set_Size(Utilities::Point(16, 16));
	_Checked = false;
	_Radio_Selected= func;
}

void MY_UI_Too::Controls::Radio_Button::Mouse_Left_Up(){
	MY_UI_Too::Controls::Widget::Mouse_Left_Up();
	_Checked = !_Checked;
	if(_Checked)_Radio_Selected(this);
}
void MY_UI_Too::Controls::Radio_Button::Select(){
	_Checked=true;
}
void MY_UI_Too::Controls::Radio_Button::UnSelect(){
	_Checked=false;
}

void MY_UI_Too::Controls::Radio_Button::Draw(MY_UI_Too::Interfaces::ISkin* skin){
	skin->Draw_Radio(_Internals.State, _Checked, Utilities::Rect(_Internals.Absolute_TL.x , _Internals.Absolute_TL.y, _Internals.Size.x, _Internals.Size.y));
}



MY_UI_Too::Controls::Radio_Group::Radio_Group(IWidget* parent) : Widget(parent){

}

void MY_UI_Too::Controls::Radio_Group::Draw(MY_UI_Too::Interfaces::ISkin* skin) {
	for(auto beg=_Buttons.begin(); beg!= _Buttons.end(); beg++) (*beg)->Draw(skin);
}

MY_UI_Too::Controls::Radio_Button* MY_UI_Too::Controls::Radio_Group::Add(){
	MY_UI_Too::Controls::Radio_Button* b = new MY_UI_Too::Controls::Radio_Button(this, std::bind(&MY_UI_Too::Controls::Radio_Group::_Radio_Selected, this, std::placeholders::_1));
	if(_Buttons.size()>=1)  b->Align_BottomLeft(0, _Buttons.back());
	_Buttons.push_back(b);
	return b;
}

void MY_UI_Too::Controls::Radio_Group::_Radio_Selected(Radio_Button* radio){
	for(auto beg=_Buttons.begin(); beg!= _Buttons.end(); beg++){
		if(*beg != radio) (*beg)->UnSelect();
		else (*beg)->Select();
	}
}