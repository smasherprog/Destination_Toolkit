#ifndef CGRAPGHICS_WINDOW_H
#define CGRAPGHICS_WINDOW_H
#include "../MY_UI\Window.h"

class cDemoControl;
class cGraphicsWindow : public MY_UI::Controls::Window{
public:
	// there can only be one window every open of this, so it can manage itself
	static cGraphicsWindow* self;// pointer to self	

	cGraphicsWindow(cWidgetBase* parent, cDemoControl& DemoControl);
	virtual ~cGraphicsWindow();

	void ReloadShaders();
};

#endif