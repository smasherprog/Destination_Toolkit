#ifndef COBJECTWINDOW_H
#define COBJECTWINDOW_H
#include "../MY_UI\Window.h"

namespace MY_UI{
	namespace Controls{
		class DropDown;

	};
};
class cDemoControl;

class cObjectWindow : public MY_UI::Controls::Window{
public:

	static cObjectWindow* self;

	cObjectWindow(cWidgetBase* parent, cDemoControl* demo);
	virtual ~cObjectWindow();
	cDemoControl& DemoControl;

	void LoadObject( );
	void ObjectSelection(MY_UI::Controls::cWidgetBase* pControl );
	void TextureChanged(MY_UI::Controls::cWidgetBase* pControl );
	MY_UI::Controls::DropDown* SelectType;
};

#endif