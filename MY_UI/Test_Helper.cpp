#include "stdafx.h"
#include "Test_Helper.h"
#include "cWidgetBase.h"
#include "cWidgetSkin.h"
#include "Collapsible_List.h"
#include "Button.h"

MY_UI::Controls::cWidget_Dummy* Dummy=0;
MY_UI::cWidgetRenderer* renderer=0;
MY_UI::cRootWidget* root=0;
MY_UI::cWidgetSkin* skin=0;

void F1_keyTest(){
	std::cout<<"F1_keyTest"<<std::endl;
}
void CTRL_R_keyTest(){
	std::cout<<"CTRL_R_keyTest"<<std::endl;
}
// just a dummy class to show how you can call functions from within a class
class Keypresser{
public:
	std::string name;
	void CTRL_Y_keyTest(){
		std::cout<<"CTRL_Y_keyTest"<<name<<std::endl;
	}
};

Keypresser Keypresser_temp0;
Keypresser Keypresser_temp1;

void InitDemo(){
	// take copies of the real pointer so there isnt so much TYPING!. ahhahhh
	renderer = MY_UI::Internal::WidgetRenderer;
	root =MY_UI::Internal::RootWidget;
	skin = MY_UI::Internal::WidgetSkin;

	MY_UI::Controls::Collapsible_List* collapsible = new MY_UI::Controls::Collapsible_List(root);	
	collapsible->SetSize(150, 500);
	collapsible->SetPos(5, 100);

	collapsible->SetTitle("Widget Examples");

	MY_UI::Controls::Button* button = collapsible->Insert("Buttons");
	button->SetPopup("Press this to see button examples");
	button->OnLeftClicked.Add(ButtonExamples);

	button = collapsible->Insert("Text");
	button->SetPopup("Press this to see text examples");
	button->OnLeftClicked.Add(TextExamples);

	button = collapsible->Insert("Window");
	button->SetPopup("Press this to see window examples");
	button->OnLeftClicked.Add(WindowExamples);

	button = collapsible->Insert("Values");
	button->SetPopup("Press this to see Value Adjustment examples");
	button->OnLeftClicked.Add(Values);

	button = collapsible->Insert("Scrollers");
	button->SetPopup("Press this to see Scroll Bar examples");
	button->OnLeftClicked.Add(Scrollers);

	button = collapsible->Insert("Tabs");
	button->SetPopup("Press this to see tabbed examples");
	button->OnLeftClicked.Add(Tabs);

	button = collapsible->Insert("Chat Template");
	button->SetPopup("Press this to see a Chat Template examples");
	button->OnLeftClicked.Add(ChatTemplate);
	
	button = collapsible->Insert("3d Example");
	button->SetPopup("Press this to see a 3d examples");
	button->OnLeftClicked.Add(Functions3D);
	Keypresser_temp0.name = "class 0";
	Keypresser_temp1.name = "class 1";
	
	MY_UI::BindToKey(KEY_F1, F1_keyTest);
	MY_UI::BindToKeys(KEY_CTRL, 'R', CTRL_R_keyTest);// all letters passed must be capital for windows because that is how windows relays a keypress
	MY_UI::BindToKeys(KEY_CTRL, 'Y', std::bind(&Keypresser::CTRL_Y_keyTest, &Keypresser_temp0));
	MY_UI::BindToKeys(KEY_CTRL, 'Q', std::bind(&Keypresser::CTRL_Y_keyTest, &Keypresser_temp1));


}
