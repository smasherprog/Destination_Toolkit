#ifndef TEST_HELPER_H
#define TEST_HELPER_H

namespace MY_UI {
	namespace Controls {
		class cWidget_Dummy;
	}
	class cWidgetRenderer;
	class cRootWidget;
	class cWidgetSkin;
}
extern MY_UI::Controls::cWidget_Dummy* Dummy;// this is used to hold the temporary objects being displayed.

// the three below are here solely for the purpose of ease of access.
extern	MY_UI::cWidgetRenderer* renderer;
extern	MY_UI::cRootWidget* root;
extern	MY_UI::cWidgetSkin* skin;
extern bool Drawing3dFunctions;

void InitDemo();

void ButtonExamples();
void TextExamples();
void WindowExamples();
void Values();
void Scrollers();
void Tabs();
void ChatTemplate();

//functions for the 3d part
void Functions3D();
void Render3D();
bool InitScene();
void Cleanup();
void InitBox();

#endif