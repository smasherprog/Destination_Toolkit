#include "stdafx.h"
#include "UI_Camera.h"
#include "../MY_UI/cWidgetUtilities.h"
#include "../MY_UI/ApplicationLayer.h"

int UI_Camera::Self_Counter=0;

UI_Camera::~UI_Camera(){
	Self_Counter-=1;
	MY_UI::GetApplicationLayer()->SetCamera(0);
	delete Camera;
}
UI_Camera::UI_Camera(Base_Camera* camera) : cWidgetBase(0){
	Self_Counter+=1;
	assert(Self_Counter==1);
	Camera = camera;
	SetControlBounds(MY_UI::Utilities::Rect(0, 0, MAX_WINDOWSIZE, MAX_WINDOWSIZE));
	SetClientBounds(MY_UI::Utilities::Rect(0, 0, MAX_WINDOWSIZE, MAX_WINDOWSIZE));
	SetDraggable(true);
	SetName("UI_Camera");
	MY_UI::GetApplicationLayer()->SetCamera(this);
}
void UI_Camera::MouseMoved(){
	if(Mouse_RButton_Down){	// only converned with the right mouse look
		Camera->Pitch(Screen_y_Pixels_To_Radians(Delta_Mousey, Camera->Screen_Fov, Camera->Screen_Width));
		Camera->Heading(Screen_x_Pixels_To_Radians(Delta_Mousex, Camera->Screen_Fov, Camera->Screen_Width, Camera->Screen_Aspect));
	}
}

void UI_Camera::KeyDown(){// no need to call the lower version, this  has no need for that functionality

	switch(Current_Key){
	case(KEY_Q):
		Camera->Strafe(Base_Camera::Movement_Types::Negative);
		break;
	case(KEY_E): 
		Camera->Strafe(Base_Camera::Movement_Types::Positive);
		break;
	case(KEY_W): 
	case(KEY_UP): 
		Camera->Forward(Base_Camera::Movement_Types::Positive);
		break;
	case(KEY_S): 
	case(KEY_DOWN):
		Camera->Forward(Base_Camera::Movement_Types::Negative);
		break;
	case(KEY_A): 
	case(KEY_LEFT): 
		if(Mouse_RButton_Down) Camera->Strafe(Base_Camera::Movement_Types::Negative);
		else Camera->Rotate(Base_Camera::Movement_Types::Negative);
		break;
	case(KEY_D): 
	case(KEY_RIGHT): 
		if(Mouse_RButton_Down) Camera->Strafe(Base_Camera::Movement_Types::Positive);
		else Camera->Rotate(Base_Camera::Movement_Types::Positive);
		break;
	};
}

void UI_Camera::KeyUp(){
	switch(Current_Key){
	case(KEY_Q): 
	case(KEY_E): 
		Camera->Strafe(Base_Camera::Movement_Types::Zero);
		break;
	case(KEY_W): 
	case(KEY_UP): 
	case(KEY_S): 
	case(KEY_DOWN):
		Camera->Forward(Base_Camera::Movement_Types::Zero);
		break;
	case(KEY_A): 
	case(KEY_LEFT): 
	case(KEY_D): 
	case(KEY_RIGHT): 
		Camera->Strafe(Base_Camera::Movement_Types::Zero);
		Camera->Rotate(Base_Camera::Movement_Types::Zero);
		break;
	};
}

