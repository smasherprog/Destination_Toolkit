#ifndef MY_UI_TOO_TEST_H
#define MY_UI_TOO_TEST_H
#include "IWidget.h"
#include <vector>

extern std::vector<MY_UI_Too::Interfaces::IWidget*> widgets;
inline void ClearWidgets(){
	for(auto beg =widgets.begin(); beg!=widgets.end(); beg++) delete *beg;
	widgets.clear();
}
void Init_Text();
#endif