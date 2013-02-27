#include "PCH.h"
#include "Widget.h"

MY_UI_Too::Controls::Widget::Widget(IWidget* parent){
	if(parent != nullptr) {
		parent->Add(this);// add this as a child
		Set_Client_Pos(Utilities::Point(0, 0));
	}
}
MY_UI_Too::Controls::Widget::~Widget(){
	On_Destructor.Call();
	for(auto beg = _Internals.Children.begin(); beg!= _Internals.Children.end(); beg++){
		(*beg)->Set_Parent(nullptr);
		delete *beg;
	}
}

void MY_UI_Too::Controls::Widget::Set_Control_Bounds(Utilities::Rect p){
	Set_Control_Pos(Utilities::Point(p.left, p.top));
	Set_Control_Size(Utilities::Point(p.width, p.height));
}
MY_UI_Too::Utilities::Rect MY_UI_Too::Controls::Widget::Get_Control_Bounds(){
	return _Internals.Absolute_Control_Area;
}
void MY_UI_Too::Controls::Widget::Set_Control_Size(Utilities::Point p){
	assert(p.left >= _Internals.Relative_Client_Area.width);
	assert(p.top >= _Internals.Relative_Client_Area.height);
	_Internals.Absolute_Control_Area.width=p.left;
	_Internals.Absolute_Control_Area.height = p.top;
}
MY_UI_Too::Utilities::Point MY_UI_Too::Controls::Widget::Get_Control_Size(){
	return Utilities::Point(_Internals.Relative_Client_Area.width, _Internals.Relative_Client_Area.height);
}
void MY_UI_Too::Controls::Widget::Set_Control_Pos(Utilities::Point p){
	_Internals.Relative_Client_Area.left +=p.left - _Internals.Absolute_Control_Area.left;
	_Internals.Relative_Client_Area.top +=p.top - _Internals.Absolute_Control_Area.top;
	_Internals.Absolute_Control_Area.left = p.left;
	_Internals.Absolute_Control_Area.top = p.top;

}
MY_UI_Too::Utilities::Point MY_UI_Too::Controls::Widget::Get_Control_Pos(){
	return Utilities::Point(_Internals.Absolute_Control_Area.left, _Internals.Absolute_Control_Area.top);
}

void MY_UI_Too::Controls::Widget::Set_Client_Bounds(Utilities::Rect p){
	Set_Client_Pos(Utilities::Point(p.left, p.top));
	Set_Client_Size(Utilities::Point(p.width, p.height));
}
MY_UI_Too::Utilities::Rect MY_UI_Too::Controls::Widget::Get_Client_Bounds(){
	return _Internals.Relative_Client_Area;
}
void MY_UI_Too::Controls::Widget::Set_Client_Size(Utilities::Point p){
	assert(p.left <= _Internals.Absolute_Control_Area.width);
	assert(p.top <= _Internals.Absolute_Control_Area.height);
	_Internals.Relative_Client_Area.width=p.left;
	_Internals.Relative_Client_Area.height = p.top;
}
MY_UI_Too::Utilities::Point MY_UI_Too::Controls::Widget::Get_Client_Size(){
	return Utilities::Point(_Internals.Relative_Client_Area.width, _Internals.Relative_Client_Area.height);
}
void MY_UI_Too::Controls::Widget::Set_Client_Pos(Utilities::Point p){
	assert(p.left >=0);
	assert(p.top >=0);
	_Internals.Absolute_Control_Area.left += p.left - _Internals.Relative_Client_Area.left;
	_Internals.Absolute_Control_Area.top += p.top - _Internals.Relative_Client_Area.top;
	_Internals.Relative_Client_Area.left =p.left;
	_Internals.Relative_Client_Area.top =p.top;

}
MY_UI_Too::Utilities::Point MY_UI_Too::Controls::Widget::Get_Client_Pos(){
	return Utilities::Point(_Internals.Relative_Client_Area.left, _Internals.Relative_Client_Area.top);
}



void MY_UI_Too::Controls::Widget::Mouse_Left_Down(){
	On_Mouse_Left_Down.Call();
}
void MY_UI_Too::Controls::Widget::Mouse_Left_Up(){
	On_Mouse_Left_Up.Call();
}
void MY_UI_Too::Controls::Widget::Mouse_Right_Down(){
	On_Mouse_Right_Down.Call();
}
void MY_UI_Too::Controls::Widget::Mouse_Right_Up(){
	On_Mouse_Right_Up.Call();
}
void MY_UI_Too::Controls::Widget::Mouse_Moved(){
	On_Mouse_Moved.Call();
}
void MY_UI_Too::Controls::Widget::Mouse_Entered(){
	On_Mouse_Entered.Call();
}
void MY_UI_Too::Controls::Widget::Mouse_Exited(){
	On_Mouse_Exited.Call();
}
void MY_UI_Too::Controls::Widget::Mouse_Wheel_Moved(){
	On_Mouse_Wheel_Moved.Call();
}
void MY_UI_Too::Controls::Widget::Key_Down(){
	On_Key_Down.Call();
}
void MY_UI_Too::Controls::Widget::Key_Up(){
	On_Key_Up.Call();
}



void MY_UI_Too::Controls::Widget::Add(IWidget* child){
	_Internals.Children.push_back(child);
	child->Set_Parent(this);
}
void MY_UI_Too::Controls::Widget::Remove(IWidget* child){
	for(auto i= _Internals.Children.begin(); i != _Internals.Children.end(); i++){
		if( (*i) == child) {
			child->Set_Parent(nullptr);//make sure to clear this
			_Internals.Children.erase(i);
			return;
		}
	}
}

MY_UI_Too::Controls::IWidget* MY_UI_Too::Controls::Widget::Hit() {
	for(auto &x : _Internals.Children){
		MY_UI_Too::Controls::IWidget* hitcontrol = x->Hit();
		if(hitcontrol != nullptr) return hitcontrol;
	}
	if(_Internals.Absolute_Control_Area.Within(Utilities::Point(New_MousePosx, New_MousePosy))) return this;
	return nullptr;
}
MY_UI_Too::Controls::IWidget* MY_UI_Too::Controls::Widget::Hit_And_SetFocus(){
	for(auto beg = _Internals.Children.begin(); beg != _Internals.Children.end(); beg++){
		MY_UI_Too::Controls::IWidget* hitcontrol = (*beg)->Hit_And_SetFocus();
		if(hitcontrol != nullptr) {
			if(beg != _Internals.Children.begin()){// dont rearrange if the control hit is already at the beginning
				MY_UI_Too::Controls::IWidget* temp = (*beg);
				_Internals.Children.erase(beg);// remove from the array
				_Internals.Children.insert(_Internals.Children.begin(), temp);// insert at the beginning
			}
			return hitcontrol;
		}
	}
	if(_Internals.Absolute_Control_Area.Within(Utilities::Point(New_MousePosx, New_MousePosy))) return this;
	return nullptr;
}