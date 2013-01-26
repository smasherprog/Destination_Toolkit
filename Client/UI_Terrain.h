#ifndef UI_TERRAIN_H
#define UI_TERRAIN_H
#include "../MY_UI/Window.h"
#include "../MY_UI/ApplicationLayer.h"
class cDemoControl;

class UI_Terrain : public MY_UI::Controls::cWidgetBase {
public:

	UI_Terrain(MY_UI::Controls::cWidgetBase* parent, cDemoControl* demo);
	virtual ~UI_Terrain();
	cDemoControl& DemoControl;

	virtual void MouseMoved();
	virtual void LeftMouseButtonDown();
	void PaintOcean();
	void ToggleEditing();
	bool Editing;
};

class Terrain_Window : public MY_UI::Controls::Window{
public:

	static Terrain_Window* self;

	Terrain_Window(MY_UI::Controls::cWidgetBase* parent, cDemoControl* demo);
	virtual ~Terrain_Window();
	cDemoControl& DemoControl;

	void ToggleEditing();
	UI_Terrain* UI_TerrainCatcher;

};

#endif