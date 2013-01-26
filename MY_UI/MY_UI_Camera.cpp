#include "PCH.h"
#include "MY_UI_Camera.h"
#include "cWidgetUtilities.h"


MY_UI_Camera::~MY_UI_Camera(){

}
MY_UI_Camera::MY_UI_Camera() : cWidgetBase(0){
	// the set the size of this control to be big enough to capture all inputs
	SetControlBounds(MY_UI::Utilities::Rect(0, 0, 2000, 2000));
	SetClientBounds(MY_UI::Utilities::Rect(0, 0, 2000, 2000));
	// the next line should be in the contructor of the derived class so the correct function is called when the mouse is dragged
	//OnRightDrag.Add(this, &MY_UI_Camera::DraggingCamera); 

	SetDraggable(true);
	SetName("Camera");
}

//void MY_UI_Camera::DraggingCamera(){
	// this is what my actual implementation looks like.. I pass the deltamouse screen space to my camera class which rebuilds the view matrix
	//Camera.Pitch(Delta_Mousey);
	//Camera.Heading(Delta_Mousex);
//}
void MY_UI_Camera::KeyDown(){// no need to call the lower version, this  has no need for that functionality
	switch(Current_Key){
	case(KEY_Q): // strafing left
		
		break;
	case(KEY_E):  // strafing right
		
		break;
	case(KEY_W): 
	case(KEY_UP): // forward 

		break;
	case(KEY_S): 
	case(KEY_DOWN): // backwards

		break;
	case(KEY_A): 
	case(KEY_LEFT): // left

		break;
	case(KEY_D): 
	case(KEY_RIGHT): // right..

		break;
	case(KEY_SPACE): // jumpinggg
		break;
	};

}

void MY_UI_Camera::KeyUp(){
	switch(Current_Key){
	case(KEY_Q): // strafing left

		break;
	case(KEY_E):  // strafing right

		break;
	case(KEY_W): 
	case(KEY_UP): // forward 

		break;
	case(KEY_S): 
	case(KEY_DOWN): // backwards

		break;
	case(KEY_A): 
	case(KEY_LEFT): // left

		break;
	case(KEY_D): 
	case(KEY_RIGHT): // right..

		break;
	case(KEY_SPACE): // jumpinggg
		break;
	};
}

