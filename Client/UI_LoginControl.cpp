#include "stdafx.h"
#include "UI_LoginControl.h"
#include "../MY_UI/cWidgetUtilities.h"
#include "cLoginControl.h"
#include "Global_GameData.h"

UI_LoginControl* UI_LoginControl::self;

UI_LoginControl::UI_LoginControl(MY_UI::Controls::cWidgetBase* parent, cLoginControl* logincontrol) : MY_UI::Controls::cWidgetBase(parent), LoginControl(*logincontrol) {
	assert(self ==0);
	SetControlBounds(MY_UI::Utilities::Rect(0, 0, 2000, 2000));
	SetClientBounds(MY_UI::Utilities::Rect(0, 0, 2000, 2000));
	SetName("UI_LoginControl");
}
void UI_LoginControl::KeyUp() {
	if(Current_Key == KEY_ESCAPE){
		LoginControl.Focus = ShutDownAll;
	}
}