#include "PCH.h"
#include "CheckBox.h"
#include "ISkin.h"


MY_UI_Too::Controls::CheckBox::CheckBox(IWidget* parent): Widget(parent) {
	Set_Size(Utilities::Point(16, 16));
	_State = Widget_States::UnPressed;
	_Checked=false;
}

void MY_UI_Too::Controls::CheckBox::Draw(MY_UI_Too::Interfaces::ISkin* skin){
	skin->Draw_CheckBox(_State, _Checked, Utilities::Rect(_Internals.Absolute_TL.x , _Internals.Absolute_TL.y, _Internals.Size.x, _Internals.Size.y));
}
