#include "PCH.h"
#include "FPS.h"
#include "Text.h"
#include <sstream>

MY_UI_Too::Controls::FPS::FPS(MY_UI_Too::Interfaces::IWidget* parent):Widget(parent){
	_Text = new MY_UI_Too::Controls::Text(this);
	Start();
	_Timer.Advance();
	std::stringstream str;
	str<<Get_FPS()<<" FPS";
	_Text->Set_Text(str.str());
	Set_Size(_Text->Get_Size() + Utilities::Point(10, 0));
}

void MY_UI_Too::Controls::FPS::Draw(MY_UI_Too::Interfaces::ISkin* skin){
	MY_UI_Too::Controls::Widget::Draw(skin);
	_Timer.Advance();
	std::stringstream str;
	str<<Get_FPS()<<" FPS";
	_Text->Set_Text(str.str());
	Set_Size(_Text->Get_Size());
}