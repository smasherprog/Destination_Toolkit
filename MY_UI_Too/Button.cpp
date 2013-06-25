#include "PCH.h"
#include "Button.h"
#include "ISkin.h"
#include "Text.h"
#include "IInput.h"

MY_UI_Too::Controls::Button::Button(IWidget* parent): Widget(parent) {
	Set_Size(Utilities::Point(100, 30));

	Set_Name("Button");
	Text=new MY_UI_Too::Controls::Text(this);
	Text->Set_Font_Size(20);
	Text->Set_Text("Button");
	Text->Align_Center();
}


void MY_UI_Too::Controls::Button::Set_Text(std::string txt){
	Text->Set_Text(txt);
	Set_Size(Text->Get_Size() + Utilities::Point(6, 6));// extra padding
	Text->Align_Center();
}

void MY_UI_Too::Controls::Button::Draw(MY_UI_Too::Interfaces::ISkin* skin){
	skin->Draw_Button(_Internals.State, Utilities::Rect(_Internals.Absolute_TL.x , _Internals.Absolute_TL.y, _Internals.Size.x, _Internals.Size.y));
	Text->Draw(skin);
}
MY_UI_Too::Interfaces::IWidget* MY_UI_Too::Controls::Button::Hit(){
	Utilities::Rect rect(_Internals.Absolute_TL.left, _Internals.Absolute_TL.top, _Internals.Size.x, _Internals.Size.y);
	if(rect.Intersect(Utilities::Point(New_MousePosx, New_MousePosy))) return this;
	return nullptr;
}
