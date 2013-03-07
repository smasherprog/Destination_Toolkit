#include "PCH.h"
#include "Root.h"
#include "IRenderer.h"
#include "ISkin.h"
#include "Input.h"
#include "IFont_Factory.h"

MY_UI_Too::Controls::Root::Root(): Focus_Holder(nullptr),Hovered_Widget(nullptr), Dragged_Widget(nullptr), Widget(nullptr) {

}
MY_UI_Too::Controls::Root::~Root() {
	OUTPUT_DEBUG_MSG("Shuttng down Root");
	SAFE_DELETE(MY_UI_Too::Internal::UI_Skin); 
	SAFE_DELETE(MY_UI_Too::Internal::Renderer);
	SAFE_DELETE(MY_UI_Too::Internal::Font_Factory);
	MY_UI_Too::Internal::Root_Widget = nullptr;
}

void MY_UI_Too::Controls::Root::Set_Control_Bounds(Utilities::Rect p){
	Set_Control_Pos(Utilities::Point(p.left, p.top));
	Set_Control_Size(Utilities::Point(p.width, p.height));
}
void MY_UI_Too::Controls::Root::Set_Control_Size(Utilities::Point p){
	_Internals.Absolute_Control_Area.width = _Internals.Relative_Client_Area.width =p.left;
	_Internals.Absolute_Control_Area.top = _Internals.Relative_Client_Area.top = p.top;
	Internal::Renderer->Set_Size(p);
}
void MY_UI_Too::Controls::Root::Set_Control_Pos(Utilities::Point p){ //always 0, 0
	_Internals.Absolute_Control_Area.left = _Internals.Relative_Client_Area.top =0;
	_Internals.Absolute_Control_Area.top = _Internals.Relative_Client_Area.left = 0;
}

void MY_UI_Too::Controls::Root::Mouse_Left_Down(){
	Interfaces::IWidget* temp = Hit_And_SetFocus();
	if(temp != nullptr) temp->Mouse_Left_Down();// something was hit
	Focus_Holder = temp;
}
void MY_UI_Too::Controls::Root::Mouse_Left_Up(){
	Interfaces::IWidget* temp = Hit();
	if(temp != nullptr && Focus_Holder == temp) temp->Mouse_Left_Up();// make sure the same control completed a mousedown / up
	Dragged_Widget = nullptr;
}
void MY_UI_Too::Controls::Root::Mouse_Right_Down() {
	Interfaces::IWidget* temp = Hit_And_SetFocus();
	if(temp != nullptr) temp->Mouse_Right_Down();// something was hit
	Focus_Holder = temp;
}
void MY_UI_Too::Controls::Root::Mouse_Right_Up() {
	Interfaces::IWidget* temp = Hit();
	if(temp != nullptr && Focus_Holder == temp) temp->Mouse_Right_Up();// make sure the same control completed a mousedown / up
	Dragged_Widget = nullptr;
}
void MY_UI_Too::Controls::Root::Mouse_Moved() {
	//dragging the control
	if(Dragged_Widget != nullptr && (Mouse_LButton_Down | Mouse_RButton_Down)){
		return Dragged_Widget->Mouse_Moved();
	}

	Interfaces::IWidget* temp = Hit();
	if(temp != nullptr){// something was hit
		if(temp != Hovered_Widget) {
			temp->Mouse_Entered(); // new control hit
			if(	Hovered_Widget != nullptr) Hovered_Widget->Mouse_Exited();// let the old hovered widget know the mouse left
		}
		Hovered_Widget = temp;
	} else { // nothing was hit
		if(	Hovered_Widget != nullptr) Hovered_Widget->Mouse_Exited();// let the old hovered widget know the mouse left
		Hovered_Widget = nullptr;
	}
}
void MY_UI_Too::Controls::Root::Mouse_Wheel_Moved() {
	if(Focus_Holder != nullptr) Focus_Holder->Mouse_Wheel_Moved();
}
void MY_UI_Too::Controls::Root::Key_Down() {

}
void MY_UI_Too::Controls::Root::Key_Up() {

}
void MY_UI_Too::Controls::Root::Draw(){
	Internal::Renderer->Begin();

	Internal::Renderer->StartNewBatch();
	for(int i((int)_Internals.Children.size()-1); i >= 0 ; i--) {
		_Internals.Children[i]->Draw();
	}

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
		if(Focus_Holder != nullptr && temp != Focus_Holder) Focus_Holder->Set_Focus(false);
	}
	return temp;
}