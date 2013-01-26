#ifndef UI_CAMERA_H
#define UI_CAMERA_H
#include "../MY_UI/cWidgetBase.h"
#include "../MY_UI/ApplicationLayer.h"
class cDemoControl;

class UI_Camera : public MY_UI::Controls::cWidgetBase {
public:

	static UI_Camera* self;
	
	UI_Camera(cDemoControl* demo);
	virtual ~UI_Camera();
	void DraggingCamera();
	virtual void KeyDown();
	virtual void KeyUp();
	cDemoControl& DemoControl;

};

#endif