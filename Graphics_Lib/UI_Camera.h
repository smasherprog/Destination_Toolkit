#ifndef UI_CAMERA_H
#define UI_CAMERA_H
#include "../MY_UI/cWidgetBase.h"
#include "../Graphics_Lib/Base_Camera.h"

class UI_Camera : public MY_UI::Controls::cWidgetBase {
	UI_Camera();// not allowed.. must specify a camera
public:

	static int Self_Counter;
	// this class will call delete on the camera passed to it
	UI_Camera(Base_Camera* camera);
	virtual ~UI_Camera() override;
	virtual void MouseMoved() override;
	virtual void KeyDown() override;
	virtual void KeyUp() override;

	Base_Camera* Camera;

};

#endif