#ifndef UI_LOGINCONTROL_H
#define UI_LOGINCONTROL_H
#include "../MY_UI/cWidgetBase.h"
#include "../MY_UI/ApplicationLayer.h"

class cLoginControl;
class UI_LoginControl : public MY_UI::Controls::cWidgetBase {
public:

	static UI_LoginControl* self;

	UI_LoginControl(MY_UI::Controls::cWidgetBase* parent, cLoginControl* logincontrol);
	virtual ~UI_LoginControl(){self=0;}
	virtual void KeyUp();
	cLoginControl& LoginControl;

};

#endif