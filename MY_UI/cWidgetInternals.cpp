#include "PCH.h"
#include "cWidgetInternals.h"
#include "cWidgetBase.h"
#include "assert.h"
#include "cWidgetRenderer.h"
#include "cWidgetSkin.h"
#include "Image.h"
#include <iostream>

MY_UI::cWidgetRenderer* MY_UI::Internal::WidgetRenderer=0;
MY_UI::cWidgetSkin* MY_UI::Internal::WidgetSkin=0;
MY_UI::cRootWidget* MY_UI::Internal::RootWidget=0;
std::set<MY_UI::Controls::cWidgetBase*> MY_UI::Internal::AllWidgets;

//this function helps ensure there are no double creations, and sets our internal pointers and calls init on the objects in the correct order
bool MY_UI::Init(cWidgetRenderer* renderer, cWidgetSkin* skin, cRootWidget* root){	

	Internal::WidgetRenderer= renderer;
	Internal::WidgetSkin=skin;
	Internal::RootWidget=root;	
	assert(Internal::WidgetRenderer!=0);
	assert(Internal::WidgetSkin!=0);
	assert(Internal::RootWidget!=0);
	// if either of these two functions fail, there is no point in trying to continue. The program is not going to run, there is no gracefull way to make it work
	bool b = Internal::WidgetRenderer->Init();
	assert(b);
	b = Internal::WidgetSkin->Init();
	assert(b);
	// always the last to init!!!!
	b = Internal::RootWidget->Init();
	assert(b);
	Internal::AllWidgets.erase(Internal::RootWidget);// the root widget should not be inside because its memory is managed outside 
	return true;
}
void MY_UI::DeInit(){
	// the order of deletion is important . . . .
	SAFE_DELETE(MY_UI::Internal::RootWidget);
	SAFE_DELETE(MY_UI::Internal::WidgetSkin);
	SAFE_DELETE(MY_UI::Internal::WidgetRenderer);
	for (MY_UI::Controls::cWidgetBase* i: MY_UI::Internal::AllWidgets)
	{
		std::cout << i->GetName() << std::endl;
	}
	assert2(MY_UI::Internal::AllWidgets.size()==0, L"Some widgets were not destroyed. Its time to check your code to make sure that the parent/child relationships were created properly");
}
MY_UI::Controls::ApplicationLayer* MY_UI::GetApplicationLayer(){ return MY_UI::Internal::RootWidget->GetApplicationLayer(); }
void MY_UI::UnBindKey(unsigned char key){ assert(Internal::RootWidget!= 0); MY_UI::Internal::RootWidget->UnBindKey(key); }
void MY_UI::UnBindKeys(unsigned char key1, unsigned char key2){ assert(Internal::RootWidget!= 0); MY_UI::Internal::RootWidget->UnBindKeys(key1, key2);}
void MY_UI::BindToKey(unsigned char key, std::function<void ()> functiontocall){assert(Internal::RootWidget!= 0); MY_UI::Internal::RootWidget->BindToKey(key, functiontocall); }
void MY_UI::BindToKeys(unsigned char key1, unsigned char key2 , std::function<void ()> functiontocall){ assert(Internal::RootWidget!= 0); MY_UI::Internal::RootWidget->BindToKeys(key1, key2, functiontocall); }
