#include "PCH.h"
#include "ISkin.h"
#include "IRenderer.h"
#include "Radio_Group.h"
#include "Button.h"


MY_UI_Too::Controls::Radio_Button::Radio_Button(IWidget* parent, std::function<void(Radio_Button*)> func): Widget(parent) {
	Set_Bounds(Utilities::Rect(0, 0, 16, 16));
	_Checked = false;
	_Radio_Checked_Hovered = Internal::UI_Skin->Get_Radio_Checked_Hovered();
	_Radio_Checked= Internal::UI_Skin->Get_Radio_Checked();
	_Radio_UnChecked_Hovered= Internal::UI_Skin->Get_Radio_UnChecked_Hovered();
	_Selected_UVs=_Radio_UnChecked= Internal::UI_Skin->Get_Radio_UnChecked();
	_Radio_Selected= func;
}
MY_UI_Too::Controls::Radio_Button::~Radio_Button(){
}

void MY_UI_Too::Controls::Radio_Button::Set_Focus(bool focus){
	MY_UI_Too::Controls::Widget::Set_Focus(focus);
	if(!focus) {
		if(_Checked) _Selected_UVs = _Radio_Checked;
		else _Selected_UVs = _Radio_UnChecked;
	}
}

void MY_UI_Too::Controls::Radio_Button::Mouse_Left_Up(){
	MY_UI_Too::Controls::Widget::Mouse_Left_Up();
	if(_Checked)  _Selected_UVs = _Radio_UnChecked_Hovered;
	else _Selected_UVs = _Radio_Checked_Hovered;
	_Checked = !_Checked;
	if(_Checked)_Radio_Selected(this);
}
void MY_UI_Too::Controls::Radio_Button::Select(){
	if(_Selected_UVs==_Radio_Checked_Hovered || _Selected_UVs==_Radio_UnChecked_Hovered) _Selected_UVs = _Radio_Checked_Hovered;
	else _Selected_UVs = _Radio_Checked;
	_Checked=true;
}
void MY_UI_Too::Controls::Radio_Button::UnSelect(){
	if(_Selected_UVs==_Radio_Checked_Hovered || _Selected_UVs==_Radio_UnChecked_Hovered) _Selected_UVs = _Radio_UnChecked_Hovered;
	else _Selected_UVs = _Radio_UnChecked;
	_Checked=false;
}
void MY_UI_Too::Controls::Radio_Button::Mouse_Entered(){
	MY_UI_Too::Controls::Widget::Mouse_Entered();
	if(_Checked) _Selected_UVs = _Radio_Checked_Hovered;
	else _Selected_UVs = _Radio_UnChecked_Hovered;
}
void MY_UI_Too::Controls::Radio_Button::Mouse_Exited(){
	MY_UI_Too::Controls::Widget::Mouse_Exited();
	if(_Checked) _Selected_UVs = _Radio_Checked;
	else _Selected_UVs = _Radio_UnChecked;
}
void MY_UI_Too::Controls::Radio_Button::Draw(){
	Utilities::Rect rect = Get_Bounds();
	rect.left = _Internals.Absolute_TL.x;
	rect.top = _Internals.Absolute_TL.y;
	Internal::Renderer->DrawTexturedRect_NoClip(Internal::UI_Skin->Get_Skin(), _Selected_UVs, rect);

}



MY_UI_Too::Controls::Radio_Group::Radio_Group(IWidget* parent) : Widget(parent){

}

void MY_UI_Too::Controls::Radio_Group::Draw() {
	for(auto beg=_Buttons.begin(); beg!= _Buttons.end(); beg++) (*beg)->Draw();
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