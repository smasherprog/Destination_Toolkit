#include "PCH.h"
#include "Root.h"
#include "IRenderer.h"
#include "IInput.h"

MY_UI_Too::Controls::Root::Root():Widget(nullptr) {
	Set_Name("Root");
}
MY_UI_Too::Controls::Root::~Root() {
	OUTPUT_DEBUG_MSG("Shuttng down Root");
}

void MY_UI_Too::Controls::Root::Set_Size(Utilities::Point p){
	_Internals.Size = p;
	Internal::Renderer->Set_Size(p);
}
void MY_UI_Too::Controls::Root::Set_Pos(Utilities::Point p){ //always 0, 0
	_Internals.Pos= Utilities::Point(0,0);
}

void MY_UI_Too::Controls::Root::Mouse_Left_Down(){
	Interfaces::IWidget* temp = Hit_And_SetFocus();
	if(temp != nullptr) temp->Mouse_Left_Down();// something was hit
	MY_UI_Too::Internal::Focus_Holder = temp;
}
void MY_UI_Too::Controls::Root::Mouse_Left_Up(){
	Interfaces::IWidget* temp = Hit();
	if(temp != nullptr && MY_UI_Too::Internal::Focus_Holder == temp) temp->Mouse_Left_Up();// make sure the same control completed a mousedown / up
	MY_UI_Too::Internal::Dragged_Widget = nullptr;
}
void MY_UI_Too::Controls::Root::Mouse_Left_DblClk() {
	Interfaces::IWidget* temp = Hit_And_SetFocus();
	if(temp != nullptr) temp->Mouse_Left_DblClk();// make sure the same control completed a mousedown / up
	MY_UI_Too::Internal::Dragged_Widget = nullptr;
	MY_UI_Too::Internal::Focus_Holder = temp;
}
void MY_UI_Too::Controls::Root::Mouse_Right_Down() {
	Interfaces::IWidget* temp = Hit_And_SetFocus();
	if(temp != nullptr) temp->Mouse_Right_Down();// something was hit
	MY_UI_Too::Internal::Focus_Holder = temp;
}
void MY_UI_Too::Controls::Root::Mouse_Right_Up() {
	Interfaces::IWidget* temp = Hit();
	if(temp != nullptr && MY_UI_Too::Internal::Focus_Holder == temp) temp->Mouse_Right_Up();// make sure the same control completed a mousedown / up
	MY_UI_Too::Internal::Dragged_Widget = nullptr;
}
void MY_UI_Too::Controls::Root::Mouse_Right_DblClk() {
	Interfaces::IWidget* temp = Hit_And_SetFocus();
	if(temp != nullptr) temp->Mouse_Right_DblClk();// make sure the same control completed a mousedown / up
	MY_UI_Too::Internal::Dragged_Widget = nullptr;
	MY_UI_Too::Internal::Focus_Holder = temp;
}

void MY_UI_Too::Controls::Root::Mouse_Moved() {
	if((MY_UI_Too::Internal::Dragged_Widget != nullptr) & (Mouse_RButton_Down | Mouse_LButton_Down)) return MY_UI_Too::Internal::Dragged_Widget->Mouse_Moved();

	Interfaces::IWidget* temp = Hit();
	if(temp != nullptr){// something was hit
		if(temp != MY_UI_Too::Internal::Hovered_Widget) {
			if(	MY_UI_Too::Internal::Hovered_Widget != nullptr) MY_UI_Too::Internal::Hovered_Widget->Mouse_Exited();// let the old hovered widget know the mouse left
			temp->Mouse_Entered(); // new control hit
		}
		MY_UI_Too::Internal::Hovered_Widget = temp;
		MY_UI_Too::Internal::Hovered_Widget->Mouse_Moved();
		if(Mouse_RButton_Down | Mouse_LButton_Down) MY_UI_Too::Internal::Dragged_Widget = MY_UI_Too::Internal::Hovered_Widget; 
	} else { // nothing was hit
		if(	MY_UI_Too::Internal::Hovered_Widget != nullptr) MY_UI_Too::Internal::Hovered_Widget->Mouse_Exited();// let the old hovered widget know the mouse left
		MY_UI_Too::Internal::Dragged_Widget= MY_UI_Too::Internal::Hovered_Widget = nullptr;
	}
}
void MY_UI_Too::Controls::Root::Mouse_Wheel_Moved() {
	if(MY_UI_Too::Internal::Focus_Holder != nullptr) MY_UI_Too::Internal::Focus_Holder->Mouse_Wheel_Moved();
}
void MY_UI_Too::Controls::Root::Key_Down() {
	if(MY_UI_Too::Internal::Focus_Holder!= nullptr) MY_UI_Too::Internal::Focus_Holder->Key_Down();
}
void MY_UI_Too::Controls::Root::Key_Up() {
	if(MY_UI_Too::Internal::Focus_Holder!= nullptr) MY_UI_Too::Internal::Focus_Holder->Key_Up();
}
void MY_UI_Too::Controls::Root::Draw(MY_UI_Too::Interfaces::ISkin* skin){
	//free all the widgets that were to be deleted 
	for(auto beg = MY_UI_Too::Internal::Widgets_ToBeDeleted.begin(); beg != Internal::Widgets_ToBeDeleted.end(); beg++) delete *beg;
	Internal::Widgets_ToBeDeleted.clear();

	Internal::Renderer->Begin();

	Internal::Renderer->StartNewBatch();
	for( auto rbeg = _Internals.Children.rbegin(); rbeg != _Internals.Children.rend(); rbeg++) (*rbeg)->Draw(Internal::UI_Skin);
	
	Internal::Renderer->Draw();

	Internal::Renderer->End();
	Delta_Mousex = 0;
	Delta_Mousey = 0;	
}

MY_UI_Too::Interfaces::IWidget* MY_UI_Too::Controls::Root::Hit() {
	MY_UI_Too::Interfaces::IWidget* temp = Widget::Hit();
	if(temp == this) return nullptr;// cant hit the root
	return temp;
}
MY_UI_Too::Interfaces::IWidget* MY_UI_Too::Controls::Root::Hit_And_SetFocus(){
	MY_UI_Too::Interfaces::IWidget* temp = Widget::Hit_And_SetFocus();
	if(temp == this) return nullptr;// cant hit the root
	if(temp != nullptr){ // something was hit
		temp->Set_Focus(true);
		if(MY_UI_Too::Internal::Focus_Holder != nullptr && temp != MY_UI_Too::Internal::Focus_Holder) MY_UI_Too::Internal::Focus_Holder->Set_Focus(false);
	}
	return temp;
}
