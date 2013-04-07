#include "PCH.h"
#include "FPS.h"
#include "Text.h"
#include <sstream>

MY_UI_Too::Controls::FPS::FPS(MY_UI_Too::Interfaces::IWidget* parent):Widget(parent), _Last_Second(0), _Current_Frame(0), _LastFps(0) {
	_Text = new MY_UI_Too::Controls::Text(this);
	Start();
}


void MY_UI_Too::Controls::FPS::Start(){
	_Start = std::chrono::high_resolution_clock::now();
}
unsigned int MY_UI_Too::Controls::FPS::Get_FPS(){
	return _LastFps;
}
void MY_UI_Too::Controls::FPS::Draw(MY_UI_Too::Interfaces::ISkin* skin){
	MY_UI_Too::Controls::Widget::Draw(skin);
	_Last_Second += std::chrono::duration_cast<milliseconds>(std::chrono::high_resolution_clock::now() - _Start).count();
	_Start = std::chrono::high_resolution_clock::now();
	if(_Last_Second>=1000){
		_Last_Second=0;
		_LastFps = _Current_Frame;
		_Current_Frame =0;
	} 
	_Current_Frame+=1;
	std::stringstream str;
	str<<_LastFps<<" FPS";
	_Text->Set_Text(str.str());
}