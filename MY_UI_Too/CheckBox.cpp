#include "PCH.h"
#include "CheckBox.h"
#include "IRenderer.h"
#include "ISkin.h"


MY_UI_Too::Controls::CheckBox::CheckBox(IWidget* parent): Widget(parent) {
	Set_Bounds(Utilities::Rect(0, 0, 16, 16));
	_Checked = false;
	_CheckBox_Checked_Hovered = Internal::UI_Skin->Get_CheckBox_Checked_Hovered();
	_CheckBox_Checked= Internal::UI_Skin->Get_CheckBox_Checked();
	_CheckBox_UnChecked_Hovered= Internal::UI_Skin->Get_CheckBox_UnChecked_Hovered();
	_Selected_UVs=_CheckBox_UnChecked= Internal::UI_Skin->Get_CheckBox_UnChecked();

}
MY_UI_Too::Controls::CheckBox::~CheckBox(){
}

void MY_UI_Too::Controls::CheckBox::Set_Focus(bool focus){
	MY_UI_Too::Controls::Widget::Set_Focus(focus);
	if(!focus) {
		if(_Checked) _Selected_UVs = _CheckBox_Checked;
		else _Selected_UVs = _CheckBox_UnChecked;
	}
}

void MY_UI_Too::Controls::CheckBox::Mouse_Left_Up(){
	MY_UI_Too::Controls::Widget::Mouse_Left_Up();
	if(_Checked) _Selected_UVs = _CheckBox_UnChecked_Hovered;
	else _Selected_UVs = _CheckBox_Checked_Hovered;
	_Checked = !_Checked;
}
void MY_UI_Too::Controls::CheckBox::Mouse_Entered(){
	MY_UI_Too::Controls::Widget::Mouse_Entered();
	if(_Checked) _Selected_UVs = _CheckBox_Checked_Hovered;
	else _Selected_UVs = _CheckBox_UnChecked_Hovered;
}
void MY_UI_Too::Controls::CheckBox::Mouse_Exited(){
	MY_UI_Too::Controls::Widget::Mouse_Exited();
	if(_Checked) _Selected_UVs = _CheckBox_Checked;
	else _Selected_UVs = _CheckBox_UnChecked;
}
void MY_UI_Too::Controls::CheckBox::Draw(){
	Utilities::Rect rect = Get_Bounds();
	rect.left = _Internals.Absolute_TL.x;
	rect.top = _Internals.Absolute_TL.y;
	Internal::Renderer->DrawTexturedRect_NoClip(Internal::UI_Skin->Get_Skin(), _Selected_UVs, rect);

}
