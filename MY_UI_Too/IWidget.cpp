#include "PCH.h"
#include "IWidget.h"
#include "IRenderer.h"
#include "ISkin.h"
#include "IFont_Factory.h"
#include "Input.h"

std::set<MY_UI_Too::Interfaces::IWidget*> MY_UI_Too::Internal::AllWidgets;
MY_UI_Too::Interfaces::IWidget* MY_UI_Too::Internal::Root_Widget=nullptr;
MY_UI_Too::Interfaces::IRenderer* MY_UI_Too::Internal::Renderer=nullptr;
MY_UI_Too::Interfaces::ISkin* MY_UI_Too::Internal::UI_Skin=nullptr;
MY_UI_Too::Interfaces::IFont_Factory* MY_UI_Too::Internal::Font_Factory=nullptr;

MY_UI_Too::Interfaces::IWidget::IWidget(){
#if defined(DEBUG) || defined(_DEBUG)
	MY_UI_Too::Internal::AllWidgets.insert(this);
#endif
}
MY_UI_Too::Interfaces::IWidget::~IWidget(){
#if defined(DEBUG) || defined(_DEBUG)
	MY_UI_Too::Internal::AllWidgets.erase(this);
#endif
}

void MY_UI_Too::Safe_Delete(MY_UI_Too::Interfaces::IWidget*& widget) {
	if(widget == nullptr) return;
	auto found = Internal::AllWidgets.find(widget);
	if(found!=Internal::AllWidgets.end()) delete widget;// the destructor should take care of removing itself
	widget = nullptr;
}
void MY_UI_Too::Init(MY_UI_Too::Interfaces::IRenderer* renderer, MY_UI_Too::Interfaces::ISkin* skin, MY_UI_Too::Interfaces::IFont_Factory* font_factory, MY_UI_Too::Interfaces::IWidget* root, unsigned int screen_width, unsigned int screen_height, unsigned int skinsize){
	Internal::Renderer=renderer; 
	Internal::UI_Skin = skin; 
	Internal::Root_Widget = root;

	Internal::Renderer->Init();
	Internal::Root_Widget->Set_Bounds(MY_UI_Too::Utilities::Rect(0, 0, screen_width, screen_height));

	Internal::UI_Skin->Init(Internal::Renderer, skinsize);
	Utilities::SetCursor(Standard);

	Internal::Font_Factory = font_factory;
	Internal::Font_Factory->Init();
	Internal::Font_Factory->Get_Font();// to build the default font

} 
void MY_UI_Too::DeInit(){
	//object destruction is always opposite of the order it was created
	SAFE_DELETE(Internal::Root_Widget);
	
	SAFE_DELETE(Internal::UI_Skin);
	SAFE_DELETE(Internal::Renderer);

	Utilities::SetCursor(Standard);//just in case
	SAFE_DELETE(Internal::Font_Factory);

}
