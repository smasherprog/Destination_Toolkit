#ifndef MY_UI_CAMERA_H
#define MY_UI_CAMERA_H
#include "cWidgetBase.h"

// the camera class name is made with MY_UI_ appended to avoid name conflicts with other classes
// this clashows you how to implement a class to capture the proper input from mouse and keyboard to move the camera. This keeps the input code seperate from the other parts of the application
// use this class as a skeleton and fill in the code in the switch statement in the cpp file. 

// the camera class DOSE NOT HAVE A PARENT!!! This class is built to be used in the Application layer. You must allocate a new camera class and pass it to the application layer through SetCamera
// After that, the Application layer will take ownership and clean up the camera class and you do not need to
class MY_UI_Camera : public MY_UI::Controls::cWidgetBase {
public:

	MY_UI_Camera();
	virtual ~MY_UI_Camera();
	
	virtual void KeyDown();
	virtual void KeyUp();

};

#endif