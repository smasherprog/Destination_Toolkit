#include "PCH.h"
#include "Widget.h"
#include "Input.h"

#define ALIGNINGMACRO \
	Utilities::Point dimensions; \
	Utilities::Point topleft; \
	if(widget==nullptr){ \
	dimensions = Get_Parent()->Get_Size();\
	} else {\
	dimensions = widget->Get_Size();\
	topleft=widget->Get_Pos();\
	}\
	Utilities::Point newpos;\

void MY_UI_Too::Controls::Widget::Set_Pos_ByOffset(Utilities::Point p){
	for(auto beg = _Internals.Children.begin(); beg!= _Internals.Children.end(); beg++){
		(*beg)->Set_Pos_ByOffset(p);
	}
	_Internals.Absolute_TL+=p;
}
MY_UI_Too::Controls::Widget::Widget(IWidget* parent){
	if(parent != nullptr) {
		parent->Add(this);// add this as a child
		Set_Pos(Utilities::Point(0, 0));
	}
}
MY_UI_Too::Controls::Widget::~Widget(){
	On_Destructor.Call();
	for(auto beg = _Internals.Children.begin(); beg!= _Internals.Children.end(); beg++){
		(*beg)->Set_Parent(nullptr);
		delete *beg;
	}
}

void MY_UI_Too::Controls::Widget::Set_Bounds(Utilities::Rect p){
	Set_Pos(Utilities::Point(p.left, p.top));
	Set_Size(Utilities::Point(p.width, p.height));
}
MY_UI_Too::Utilities::Rect MY_UI_Too::Controls::Widget::Get_Bounds() const{
	return _Internals.Rect;
}
void MY_UI_Too::Controls::Widget::Set_Size(Utilities::Point p){
	_Internals.Rect.width=p.left;
	_Internals.Rect.height = p.top;
}
MY_UI_Too::Utilities::Point MY_UI_Too::Controls::Widget::Get_Size() const{
	return Utilities::Point(_Internals.Rect.width, _Internals.Rect.height);
}
void MY_UI_Too::Controls::Widget::Set_Pos(Utilities::Point p){
	int offsetx = p.left - _Internals.Rect.left;
	int offsety = p.top - _Internals.Rect.top;
	_Internals.Rect.left = p.left;
	_Internals.Rect.top = p.top;	
	//now move all the children
	p.x=offsetx;
	p.y=offsety;
	Set_Pos_ByOffset(p);//move all the children
}
MY_UI_Too::Utilities::Point MY_UI_Too::Controls::Widget::Get_Pos() const{
	return Utilities::Point(_Internals.Rect.left, _Internals.Rect.top);
}

void MY_UI_Too::Controls::Widget::Align_TopLeft(int padding, MY_UI_Too::Interfaces::IWidget* widget){
	ALIGNINGMACRO;
	newpos.x = topleft.left;
	if(widget==nullptr) {//align within the parent
		Set_Pos(Utilities::Point(padding, padding));
	} else {
		newpos.y = topleft.top - padding - Get_Size().y;
		Set_Pos(newpos);
	}
}
void MY_UI_Too::Controls::Widget::Align_TopCenter(int padding, IWidget* widget) {
	ALIGNINGMACRO;
	newpos.x = topleft.x+(dimensions.x/2 - Get_Size().x/2);
	if(widget==nullptr) {//align within the parent
		Set_Pos(Utilities::Point(newpos.x , padding));
	}
	else {
		newpos.y = topleft.top - padding - Get_Size().y;
		Set_Pos(newpos);
	}
}
void MY_UI_Too::Controls::Widget::Align_TopRight(int padding, IWidget* widget){
	ALIGNINGMACRO;
	newpos.x = topleft.left + dimensions.x - Get_Size().x;
	if(widget==nullptr) {//align within the parent
		Set_Pos(Utilities::Point(newpos.x, padding));
	}
	else {
		newpos.y = topleft.top - padding - Get_Size().y;
		Set_Pos(newpos);
	}
}

void MY_UI_Too::Controls::Widget::Align_BottomLeft(int padding, IWidget* widget){
	ALIGNINGMACRO;
	newpos.x = topleft.left;
	if(widget==nullptr) {//align within the parent
		Set_Pos(Utilities::Point(padding, topleft.y + dimensions.y - Get_Size().y-padding));
	}
	else {
		newpos.y = topleft.top + dimensions.y +padding;
		Set_Pos(newpos);
	}
}
void MY_UI_Too::Controls::Widget::Align_BottomCenter(int padding, IWidget* widget){
	ALIGNINGMACRO;
	newpos.x = topleft.x+(dimensions.x/2 - Get_Size().x/2);
	if(widget==nullptr) {//align within the parent
		Set_Pos(Utilities::Point(newpos.x+padding, topleft.y + dimensions.y - Get_Size().y-padding));
	}
	else {
		newpos.y = topleft.top + dimensions.y +padding;
		Set_Pos(newpos);
	}
}
void MY_UI_Too::Controls::Widget::Align_BottomRight(int padding, IWidget* widget){
	ALIGNINGMACRO;
	newpos.x = topleft.left + dimensions.x - Get_Size().x;
	if(widget==nullptr) {//align within the parent
		Set_Pos(Utilities::Point(newpos.x-padding, topleft.y + dimensions.y - Get_Size().y-padding));
	}
	else {
		newpos.y = topleft.top + dimensions.y +padding;
		Set_Pos(newpos);
	}
}

void MY_UI_Too::Controls::Widget::Align_LeftTop(int padding, IWidget* widget){
	ALIGNINGMACRO;
	if(widget==nullptr) {//align within the parent
		Set_Pos(Utilities::Point(padding, padding));
	}
	else {
		newpos.x = topleft.left - padding - Get_Size().y;
		newpos.y = topleft.top;
		Set_Pos(newpos);
	}
}
void MY_UI_Too::Controls::Widget::Align_LeftCenter(int padding, IWidget* widget){
	ALIGNINGMACRO;
	newpos.y = topleft.y+(dimensions.y/2 - Get_Size().y/2);
	if(widget==nullptr) {//align within the parent
		Set_Pos(Utilities::Point(padding, newpos.y + padding));
	}
	else {
		newpos.x = topleft.left - padding - Get_Size().y;
		Set_Pos(newpos);
	}
}
void MY_UI_Too::Controls::Widget::Align_LeftBottom(int padding, IWidget* widget){
	ALIGNINGMACRO;
	if(widget==nullptr) {//align within the parent
		Set_Pos(Utilities::Point(padding, topleft.y + dimensions.y - Get_Size().y-padding));
	}
	else {
		newpos.x = topleft.left;
		newpos.y = topleft.top + dimensions.y + Get_Size().y+padding;
		Set_Pos(newpos);
	}
}

void MY_UI_Too::Controls::Widget::Align_RightTop(int padding, IWidget* widget){
	ALIGNINGMACRO;
	newpos.x = topleft.left + dimensions.x - Get_Size().x;
	if(widget==nullptr) {//align within the parent
		Set_Pos(Utilities::Point(newpos.x-padding, padding));
	}
	else {
		newpos.y = topleft.top + padding + Get_Size().y;
		Set_Pos(newpos);
	}
}
void MY_UI_Too::Controls::Widget::Align_RightCenter(int padding, IWidget* widget){
	ALIGNINGMACRO;
	newpos.y = topleft.y+(dimensions.y/2 - Get_Size().y/2);
	if(widget==nullptr) {//align within the parent
		newpos.x = topleft.left + dimensions.x - Get_Size().x;
		Set_Pos(Utilities::Point(newpos.x-padding, newpos.y+padding));
	}
	else {
		newpos.x = topleft.left + dimensions.x + Get_Size().x +padding;
		Set_Pos(newpos);
	}
}
void MY_UI_Too::Controls::Widget::Align_RightBottom(int padding, IWidget* widget){
	ALIGNINGMACRO;
	newpos.y = topleft.y + dimensions.y - Get_Size().y+padding;
	if(widget==nullptr) {//align within the parent
		newpos.x = topleft.left + dimensions.x - Get_Size().x-padding;
		Set_Pos(newpos);
	}
	else {
		newpos.x = topleft.left + dimensions.x + Get_Size().x+padding;
		Set_Pos(newpos);
	}
}

void MY_UI_Too::Controls::Widget::Align_TR(int padding, IWidget* widget) {
	ALIGNINGMACRO;
	newpos.y = topleft.y - dimensions.y-padding;
	newpos.x = topleft.x + dimensions.x + padding;
	Set_Pos(newpos);
}
void MY_UI_Too::Controls::Widget::Align_TL(int padding, IWidget* widget){
	ALIGNINGMACRO;
	newpos.y = topleft.y - dimensions.y - padding;
	newpos.x = topleft.x - dimensions.x - padding;
	Set_Pos(newpos);
}
void MY_UI_Too::Controls::Widget::Align_BL(int padding, IWidget* widget) {
	ALIGNINGMACRO;
	newpos.y = topleft.y + dimensions.y + padding;
	newpos.x = topleft.x - Get_Size().x - padding;
	Set_Pos(newpos);
}
void MY_UI_Too::Controls::Widget::Align_BR(int padding, IWidget* widget) {
	ALIGNINGMACRO;
	newpos.y = topleft.y + dimensions.y + padding;
	newpos.x = topleft.x + dimensions.x + padding;
	Set_Pos(newpos);
}
void MY_UI_Too::Controls::Widget::Align_Center() {
	IWidget* parent = Get_Parent();
	assert(parent!=nullptr);
	Utilities::Point newpos, dimensions(parent->Get_Size());
	newpos.y = (dimensions.y/2 - Get_Size().y/2);
	newpos.x = (dimensions.x/2 - Get_Size().x/2);
	Set_Pos(newpos);
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

MY_UI_Too::Interfaces::IWidget* MY_UI_Too::Controls::Widget::Hit() {
	for(auto &x : _Internals.Children){
		MY_UI_Too::Interfaces::IWidget* hitcontrol = x->Hit();
		if(hitcontrol != nullptr) return hitcontrol;
	}
	if(_Internals.Rect.Intersect(Utilities::Point(New_MousePosx, New_MousePosy))) return this;
	return nullptr;
}
MY_UI_Too::Interfaces::IWidget* MY_UI_Too::Controls::Widget::Hit_And_SetFocus(){
	for(auto beg = _Internals.Children.begin(); beg != _Internals.Children.end(); beg++){
		MY_UI_Too::Interfaces::IWidget* hitcontrol = (*beg)->Hit_And_SetFocus();
		if(hitcontrol != nullptr) {
			if(beg != _Internals.Children.begin()){// dont rearrange if the control hit is already at the beginning
				MY_UI_Too::Interfaces::IWidget* temp = (*beg);
				_Internals.Children.erase(beg);// remove from the array
				_Internals.Children.insert(_Internals.Children.begin(), temp);// insert at the beginning
			}
			return hitcontrol;
		}
	}
	if(_Internals.Rect.Intersect(Utilities::Point(New_MousePosx, New_MousePosy))) return this;
	return nullptr;
}