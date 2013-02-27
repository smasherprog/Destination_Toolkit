#include "PCH.h"
#include "IWidget.h"

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