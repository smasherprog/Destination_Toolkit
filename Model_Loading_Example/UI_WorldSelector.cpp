#include "stdafx.h"
#include "UI_WorldSelector.h"
#include "../MY_UI/cWidgetUtilities.h"



UI_WorldSelector::UI_WorldSelector() : cWidgetBase(MY_UI::GetApplicationLayer()){// set the application layer as the parent
	SetControlBounds(MY_UI::Utilities::Rect(0, 0, MAX_WINDOWSIZE, MAX_WINDOWSIZE));
	SetClientBounds(MY_UI::Utilities::Rect(0, 0, MAX_WINDOWSIZE, MAX_WINDOWSIZE));
	SetDraggable(true);
	SetName("UI_WorldSelector");
}

void UI_WorldSelector::MouseMoved(){

}
void UI_WorldSelector::KeyDown(){

}

void UI_WorldSelector::KeyUp(){
}

