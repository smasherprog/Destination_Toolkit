#ifndef UI_CAMERA_H
#define UI_CAMERA_H
#include "../MY_UI/cWidgetBase.h"
#include "../MY_UI/ApplicationLayer.h"

class UI_WorldSelector : public MY_UI::Controls::cWidgetBase {
public:

	UI_WorldSelector();
	virtual ~UI_WorldSelector(){}
	virtual void MouseMoved();
	virtual void KeyDown();
	virtual void KeyUp();

};

#endif