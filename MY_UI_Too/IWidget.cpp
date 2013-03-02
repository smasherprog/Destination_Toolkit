#include "PCH.h"
#include "IWidget.h"
#include "IRenderer.h"
#include "ISkin.h"

std::set<MY_UI_Too::Controls::IWidget*> MY_UI_Too::Internal::AllWidgets;
MY_UI_Too::Controls::IWidget* MY_UI_Too::Internal::Root_Widget=nullptr;
MY_UI_Too::IRenderer* MY_UI_Too::Internal::Renderer=nullptr;
MY_UI_Too::ISkin* MY_UI_Too::Internal::UI_Skin=nullptr;

MY_UI_Too::Controls::IWidget::IWidget(){
#if defined(DEBUG) || defined(_DEBUG)
	MY_UI_Too::Internal::AllWidgets.insert(this);
#endif
}
MY_UI_Too::Controls::IWidget::~IWidget(){
#if defined(DEBUG) || defined(_DEBUG)
	MY_UI_Too::Internal::AllWidgets.erase(this);
#endif
}

void MY_UI_Too::Safe_Delete(MY_UI_Too::Controls::IWidget*& widget) {
	if(widget == nullptr) return;
	auto found = Internal::AllWidgets.find(widget);
	if(found!=Internal::AllWidgets.end()) delete widget;// the destructor should take care of removing itself
	widget = nullptr;
}
void MY_UI_Too::Init(MY_UI_Too::IRenderer* renderer, MY_UI_Too::ISkin* skin, MY_UI_Too::Controls::IWidget* root, unsigned int screen_width, unsigned int screen_height){
	Internal::Renderer=renderer; 
	Internal::UI_Skin = skin; 
	Internal::Root_Widget = root;

	Internal::Renderer->Init();
	MY_UI_Too::Internal::Root_Widget->Set_Control_Bounds(MY_UI_Too::Utilities::Rect(0, 0, screen_width, screen_height));

	MY_UI_Too::Internal::UI_Skin->Init(Internal::Renderer);
	MY_UI_Too::Utilities::SetCursor(Utilities::Standard);
} 