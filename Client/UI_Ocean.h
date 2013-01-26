#ifndef UI_OCEAN_H
#define UI_OCEAN_H
#include "../MY_UI/cWidgetBase.h"
#include "../MY_UI\Window.h"
#include "../MY_UI/ApplicationLayer.h"
class cDemoControl;

class UI_Ocean : public MY_UI::Controls::cWidgetBase {
public:

	UI_Ocean(MY_UI::Controls::cWidgetBase* parent, cDemoControl* demo);
	virtual ~UI_Ocean();
	cDemoControl& DemoControl;

	virtual void MouseMoved();
	virtual void LeftMouseButtonDown();
	void PaintOcean();
	void ToggleEditing();
	bool Editing;
};

class Ocean_Window : public MY_UI::Controls::Window{
public:

	static Ocean_Window* self;

	Ocean_Window(MY_UI::Controls::cWidgetBase* parent, cDemoControl* demo);
	virtual ~Ocean_Window();
	cDemoControl& DemoControl;

	void ToggleEditing();
	UI_Ocean* UI_OceanCatcher;

};

#endif