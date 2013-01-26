#include "PCH.h"
#include "cWidgetBase.h"
#include "cWidgetRenderer.h"
#include "Window.h"
#include "Text.h"
#include "cWidgetSkin.h"
#include <iostream>
#include "Image.h"
#include <set>
#include "ApplicationLayer.h"
/*-----------------------------------------------------cWidgetBase-----------------------------------------------------------------*/
MY_UI::Controls::cWidgetBase::cWidgetBase(cWidgetBase* parent){ 
	Internal_Widget.Parent = parent; 
	if(parent !=0) {
		parent->Couple(this); 
		SetPos(0, 0);
	}
	MY_UI::Internal::AllWidgets.insert(this);
}	
MY_UI::Controls::cWidgetBase::~cWidgetBase(){
	OnDestroyed.Call(this);
	SAFE_DELETE(Internal_Widget.Selected);
	for(size_t i(0); i< Internal_Widget.Children.size(); i++){
		Internal_Widget.Children[i]->Internal_Widget.Parent =0;// set this so Decouple isnt called.
		SAFE_DELETE(Internal_Widget.Children[i]);
	}
	// make sure to remove references from a parent if there is a parent
	cWidgetBase* parent = Internal_Widget.Parent;
	if(parent !=0) parent->DeCouple(this);
	// make sure this widget removes all references to itself
	MY_UI::Internal::RootWidget->CleanLinks(this);
	MY_UI::Internal::AllWidgets.erase(this);
	Internal::RootWidget->WidgetHasChanged = true;
}

void MY_UI::Controls::cWidgetBase::SetFocus(bool focus){ 
	if(!focus){ 
		for(size_t i(0); i< Internal_Widget.Children.size(); i++){
			Internal_Widget.Children[i]->SetFocus(focus);
		}
	}
	bool prev = Internal_Widget.Focus;
	Internal_Widget.Focus = focus; 
	if(prev != focus) OnFocusChange.Call(this);
}

void MY_UI::Controls::cWidgetBase::SetHidden(bool hidden) { 
	for(size_t i(0); i< Internal_Widget.Children.size(); i++){
		Internal_Widget.Children[i]->SetHidden(hidden);
	}
	Internal_Widget.Hidden = hidden; 
	Internal::RootWidget->WidgetHasChanged = true;
}
// do a depth first search of a widget that is hit
MY_UI::Controls::cWidgetBase* MY_UI::Controls::cWidgetBase::Hit(){ 
	if(Internal_Widget.Hidden) return 0;
	for(size_t i(0); i< Internal_Widget.Children.size(); i++){
		MY_UI::Controls::cWidgetBase* temp = Internal_Widget.Children[i]->Hit();
		if(temp != 0) return temp;// return the hit child
	}
	const Utilities::Rect& r = Internal_Widget.AbsolutePosition;
	if((New_MousePosx > r.left) & (New_MousePosx < r.left + r.width) & (New_MousePosy > r.top) & (New_MousePosy < r.top + r.height)){// only check within our bounds		
		return this;// no child was hit, but this was
	}
	return 0;
} 
// this will search and set the widget hit to have focus, i.e. bring it to the front
MY_UI::Controls::cWidgetBase* MY_UI::Controls::cWidgetBase::Hit_And_SetFocus(){
	if(Internal_Widget.Hidden) return 0;
	std::vector<cWidgetBase*>& children = Internal_Widget.Children;
	for(unsigned int i(0); i < children.size(); i++){
		MY_UI::Controls::cWidgetBase* temp = children[i]->Hit_And_SetFocus();
		if( temp != 0) {
			MY_UI::Controls::cWidgetBase* otemp = children[i];
			if(i != 0){// if the focus has changed... tell the controls
				children[0]->SetFocus(false);// the widget at the beginning has lost focus.. let it know
				children[i]->SetFocus(true);
				for(int j = i; j != 0; j--){
					children[j] = children[j-1];
				}
				children[0]=otemp;

			} else if(!children[i]->GetFocus()){
				children[i]->SetFocus(true);
			}
			return temp;// return the hit child
		}
	}
	Utilities::Rect& r = Internal_Widget.AbsolutePosition;
	if((New_MousePosx > r.left) & (New_MousePosx < r.left + r.width) & (New_MousePosy > r.top) & (New_MousePosy < r.top + r.height)){// only check within our bounds	
		for(unsigned int i(0); i < children.size(); i++){
			children[i]->SetFocus(false);
		}
		return this;// no child was hit, but this was
	}
	return 0;
}

void MY_UI::Controls::cWidgetBase::LeftMouseButtonDown(){

}
void MY_UI::Controls::cWidgetBase::RightMouseButtonDown(){ 

} 
void MY_UI::Controls::cWidgetBase::LeftMouseButtonUp(){
	if(Hit()){// check to see if the mouse is still over the control
		if(MY_UI::Internal::RootWidget->FocusHolder == this){// if this control has focus, it means it received a mousedown. This is necessary because a user can hold the mouse down and move it and release it above a different control
			unsigned int newtime = Utilities::GetTime();
			if(newtime - Internal_Widget.LastLeftClickTime < DOUBLE_CKICK_SPEED){
				OnLeftDoubleClick.Call(this);
				newtime = 0;
			} else {
				Internal_Widget.LastLeftClickTime = newtime;
				OnLeftClicked.Call(this);
			}
		}
	}
}
void MY_UI::Controls::cWidgetBase::RightMouseButtonUp(){
	if(Hit()){// check to see if the mouse is still over the control
		if(MY_UI::Internal::RootWidget->FocusHolder == this){// if this control has focus, it means it received a mousedown
			unsigned int newtime = Utilities::GetTime();
			if(newtime - Internal_Widget.LastRightClickTime < DOUBLE_CKICK_SPEED){
				OnRightDoubleClick.Call(this);
				newtime = 0;
			} else {
				Internal_Widget.LastLeftClickTime = newtime;
				OnRightClicked.Call(this);
			}
		} 
	}
}
void MY_UI::Controls::cWidgetBase::MouseWheelMoved(){
	OnMouseWheelMoved.Call(this);
}
void MY_UI::Controls::cWidgetBase::MouseMoved(){
	if(Mouse_LButton_Down){// the user is dragging the button
		OnLeftDrag.Call(this);
	} else if(Mouse_RButton_Down){	
		OnRightDrag.Call(this);
	}
}
void MY_UI::Controls::cWidgetBase::MouseEntered(){
	OnHoverEnter.Call(this);
}
void MY_UI::Controls::cWidgetBase::MouseExited(){
	OnHoverExit.Call(this);
}
void MY_UI::Controls::cWidgetBase::KeyUp(){
	OnKeyUp.Call(this);
}
void  MY_UI::Controls::cWidgetBase::KeyDown(){ 
	OnKeyDown.Call(this);
} 
void MY_UI::Controls::cWidgetBase::SetSelectable(bool selectedable){ 
	if(Internal_Widget.Selected !=0 ) delete Internal_Widget.Selected;
	Internal_Widget.Selected=0;
	if(!selectedable) return;
	Internal_Widget.Selected = new Controls::Image(this);
	//set some variables to get it started 
	Internal_Widget.Selected->SetHidden(false);// dont show it yet
	Internal_Widget.Selected->SetPos(0, 0);// set to 0 0
	Internal_Widget.Selected->SetSize(GetSize());
}
void MY_UI::Controls::cWidgetBase::SetColors(Utilities::Color tl, Utilities::Color tr, Utilities::Color bl, Utilities::Color br){
	Internal_Widget.Color_Top_Left = tl;
	Internal_Widget.Color_Bottom_Right = br;
	Internal_Widget.Color_Bottom_Left = bl;
	Internal_Widget.Color_Top_Right = tr;
	Internal::RootWidget->WidgetHasChanged = true;
}
void MY_UI::Controls::cWidgetBase::GetColors(Utilities::Color& tl, Utilities::Color& tr, Utilities::Color& bl, Utilities::Color& br){
	 tl = Internal_Widget.Color_Top_Left;
	 br = Internal_Widget.Color_Bottom_Right;
	 bl = Internal_Widget.Color_Bottom_Left;
	 tr = Internal_Widget.Color_Top_Right;
	 Internal::RootWidget->WidgetHasChanged = true;
}
void MY_UI::Controls::cWidgetBase::Align_Within_Parent(int align, int buffer){
	Internal_Widget.Alignment = align;
	Internal_Widget.Buffer = buffer;
	if(align == NO_ALIGNMENT) return;// no work to do, assign and get out
		

	// WORK TO DO>>>>>>>
	Controls::cWidgetBase* parent =Internal_Widget.Parent;
	Utilities::Rect parentrect;
	// first get the top left corner of the avaiable client area of the parent if there is a parent. If not, this will be (0, 0)
	if(parent != 0) parentrect = parent->GetClientBounds();
	
	Utilities::Point size = GetSize();
	Utilities::Point pos = GetPos();

	if(align & BOTTOM){// do an alignment to the bottom of the parent Client area
		if(align & CENTER) pos.x = parentrect.width/2 + parentrect.left-size.left/2;
		pos.y = parentrect.height - size.y - buffer;
		SetPos(pos.x, pos.y);
	} else if(align & TOP){// do an alignment to the top of the parent Client area
		if(align & CENTER) pos.x = parentrect.width/2 + parentrect.left - size.left/2;
		pos.y=0 + buffer;
		SetPos(pos.x, pos.y);

	}
	if(align & LEFT){// do an alignment to the bottom of the parent Client area
		if(align & CENTER) pos.y = parentrect.height/2 -size.top/2;
		pos.x = 0 + buffer;
		SetPos(pos.x, pos.y);
	} else if(align & RIGHT){// do an alignment to the bottom of the parent Client area
		if(align & CENTER) pos.y = parentrect.height/2 -size.top/2;
		pos.x = parentrect.width- size.x - buffer;
		SetPos(pos.x, pos.y);
	}
	if(align == CENTER){
		Utilities::Point centerofparent(parentrect.width/2, parentrect.height/2);
		Utilities::Point centerofcontrol(size.left/2, size.top/2);
		SetPos(centerofparent.left - centerofcontrol.left, centerofparent.top- centerofcontrol.top);
	} else if(align == FILL){
		Update();// update the control, it should know how to position itself
	}
	
}
unsigned int MY_UI::Controls::cWidgetBase::Get_Parent_Alignment() const { return Internal_Widget.Alignment; }

void MY_UI::Controls::cWidgetBase::SetSize(int x, int y){ 
	// make sure to enforce the smallest size
	x = max(x, Internal_Widget.MinSize.x);
	y = max(y, Internal_Widget.MinSize.y);
	Utilities::Point p = GetSize();
	Internal_Widget.Clientpos.height += y - p.y;
	Internal_Widget.Clientpos.width += x - p.x;
	Internal_Widget.AbsolutePosition.width= x; 
	Internal_Widget.AbsolutePosition.height= y; 
	Internal::RootWidget->WidgetHasChanged = true;
}

MY_UI::Utilities::Point MY_UI::Controls::cWidgetBase::GetSize() const{ 
	return MY_UI::Utilities::Point(Internal_Widget.AbsolutePosition.width, Internal_Widget.AbsolutePosition.height);
}
void MY_UI::Controls::cWidgetBase::SetControlBounds(Utilities::Rect r){
	Controls::cWidgetBase* parent =Internal_Widget.Parent;
	Utilities::Point parentpos;
	// first get the top left corner of the avaiable client area of the parent if there is a parent. If not, this will be (0, 0)
	if(parent != 0) parentpos = Utilities::Point(parent->Internal_Widget.Clientpos.left, parent->Internal_Widget.Clientpos.top);
	// convert to an absolute position
	r.left += parentpos.left;
	r.top += parentpos.top;
	Internal_Widget.AbsolutePosition = r;
}
MY_UI::Utilities::Rect MY_UI::Controls::cWidgetBase::GetControlBounds() const{
	Controls::cWidgetBase* parent =Internal_Widget.Parent;
	Utilities::Point parentpos;
	MY_UI::Utilities::Rect r = Internal_Widget.AbsolutePosition;
	// first get the top left corner of the avaiable client area of the parent if there is a parent. If not, this will be (0, 0)
	if(parent != 0) parentpos = Utilities::Point(parent->Internal_Widget.Clientpos.left, parent->Internal_Widget.Clientpos.top);
	// convert to a relative  position
	r.left -= parentpos.left;
	r.top -= parentpos.top;
	return r;
}
// this function will clamp the bounds to the absolutesize of the control
//VERY IMPORTANT>>>> SET THE CONTROL BOUNDS BEFORE SETING CLIENT BOUNDS BECAUSE THE CLIENT BOUNDS ARE CLAMPED TO THE CONTROL BOUNDS!!!!!
void MY_UI::Controls::cWidgetBase::SetClientBounds(Utilities::Rect r){
	// convert from relative space to absolute screen space
	r.left += Internal_Widget.AbsolutePosition.left;
	r.top += Internal_Widget.AbsolutePosition.top;
	// make sure that a valid rect is passed	
	// check the top left corner points
	

	if(r.left < Internal_Widget.AbsolutePosition.left) r.left = Internal_Widget.AbsolutePosition.left;
	if(r.top < Internal_Widget.AbsolutePosition.top) r.top = Internal_Widget.AbsolutePosition.top;
	if(r.left > Internal_Widget.AbsolutePosition.left + Internal_Widget.AbsolutePosition.width ) r.left = Internal_Widget.AbsolutePosition.left + Internal_Widget.AbsolutePosition.width;
	if(r.top > Internal_Widget.AbsolutePosition.top + Internal_Widget.AbsolutePosition.height) r.top = Internal_Widget.AbsolutePosition.top + Internal_Widget.AbsolutePosition.height;
	// check the bottom right corner points
	if(r.height + r.top > Internal_Widget.AbsolutePosition.height + Internal_Widget.AbsolutePosition.top) r.height = Internal_Widget.AbsolutePosition.height + Internal_Widget.AbsolutePosition.top - r.top;
	if(r.width + r.left > Internal_Widget.AbsolutePosition.width + Internal_Widget.AbsolutePosition.left) r.width = r.width - ((r.left + r.width) - Internal_Widget.AbsolutePosition.width + Internal_Widget.AbsolutePosition.left);
	if(r.height < 0) r.height = 0;
	if(r.width  < 0) r.width = 0;
	Internal_Widget.Clientpos = r;
}
MY_UI::Utilities::Rect MY_UI::Controls::cWidgetBase::GetClientBounds() const {
	// convert it to relative space
	MY_UI::Utilities::Rect rect(Internal_Widget.Clientpos);
	rect.left -= Internal_Widget.AbsolutePosition.left;
	rect.top -= Internal_Widget.AbsolutePosition.top;
	return rect;
}
void MY_UI::Controls::cWidgetBase::SetPos(int x, int y){
	// the positioning needs to take into account the avaiable area not only the top left corner of the control
	Controls::cWidgetBase* parent =Internal_Widget.Parent;
	Utilities::Point parentpos;
	// first get the top left corner of the avaiable client area of the parent if there is a parent. If not, this will be (0, 0)
	if(parent != 0) parentpos = Utilities::Point(parent->Internal_Widget.Clientpos.left, parent->Internal_Widget.Clientpos.top);

	// keep the old position before we update it to see how far the control has actually moved so the client area of THIS widget can be moved
	int offsetx(Internal_Widget.AbsolutePosition.left);
	int offsety(Internal_Widget.AbsolutePosition.top);
	// move the top left corner of the widget 
	Internal_Widget.AbsolutePosition.left = parentpos.left + x;
	Internal_Widget.AbsolutePosition.top = parentpos.top + y;
	
	// now, figure out how far the control actually moved
	offsetx = Internal_Widget.AbsolutePosition.left - offsetx;
	offsety = Internal_Widget.AbsolutePosition.top - offsety;
	// add that amount of movement ontop THIS widgets client area
	// if this widget is a button, the clientpos will always be equal to the absolute position, so this is wasted work in that case, but it is simple addition and subtraction
	// hardly anything that will cause a slow down
	Internal_Widget.Clientpos.left +=offsetx;
	Internal_Widget.Clientpos.top += offsety;
	// now make sure to let all the children know the control has moved
	for(size_t i(0); i< Internal_Widget.Children.size(); i++){
		Utilities::Point p = Internal_Widget.Children[i]->GetPos();
		p.left+=offsetx;
		p.top+=offsety;
		Internal_Widget.Children[i]->SetPos(p.left, p.top);
	}
	Internal::RootWidget->WidgetHasChanged = true;
}
MY_UI::Utilities::Point MY_UI::Controls::cWidgetBase::GetPos() const {
	// this retrives the top left position of the control, but relative to the parents position
	Controls::cWidgetBase* parent =Internal_Widget.Parent;
	Utilities::Point temp(Internal_Widget.AbsolutePosition.left, Internal_Widget.AbsolutePosition.top);
	if(parent != 0) {
		temp.left -= parent->Internal_Widget.Clientpos.left;
		temp.top -= parent->Internal_Widget.Clientpos.top;
	}
	return temp; 

}
// search through all the children to fit the size to be the tightest fit 
void MY_UI::Controls::cWidgetBase::SetSizeToContents(){
	if(Internal_Widget.Children.empty()) return;
	Utilities::Rect r(99999, 99999, -99999, -99999);
	for(size_t i(0); i< Internal_Widget.Children.size(); i++){
		Utilities::Rect rect = Internal_Widget.Children[i]->Internal_Widget.AbsolutePosition;
		r.left = min(rect.left, r.left);
		r.width = max(rect.left + rect.width, r.width);
		
		r.top = min(rect.top , r.top);
		r.height = max(rect.top + rect.height , r.height);

	}
	
	r.width = r.width -  r.left;
	r.height = r.height -  r.top;

	// keep the old position before we update it to see how far the control has actually moved so the client area of THIS widget can be moved
	int offsetx(Internal_Widget.AbsolutePosition.left);
	int offsety(Internal_Widget.AbsolutePosition.top);
	// move the top left corner of the widget 
	Internal_Widget.AbsolutePosition.left = r.left;
	Internal_Widget.AbsolutePosition.top = r.top;
	
	// now, figure out how far the control actually moved
	offsetx = Internal_Widget.AbsolutePosition.left - offsetx;
	offsety = Internal_Widget.AbsolutePosition.top - offsety;
	// add that amount of movement ontop THIS widgets client area
	// if this widget is a button, the clientpos will always be equal to the absolute position, so this is wasted work in that case, but it is simple addition and subtraction
	// hardly anything that will cause a slow down
	Internal_Widget.Clientpos.left +=offsetx;
	Internal_Widget.Clientpos.top += offsety;
	
	SetSize(r.width, r.height);
}
MY_UI::Utilities::Point MY_UI::Controls::cWidgetBase::GetSizeOfContents() const{
	if(Internal_Widget.Children.empty()) return Utilities::Point();
	Utilities::Rect r(99999, 99999, -99999, -99999);
	for(size_t i(0); i< Internal_Widget.Children.size(); i++){
		Utilities::Rect rect = Internal_Widget.Children[i]->Internal_Widget.AbsolutePosition;
		r.left = min(rect.left, r.left);
		r.width = max(rect.left + rect.width, r.width);
		
		r.top = min(rect.top , r.top);
		r.height = max(rect.top + rect.height , r.height);

	}
	r.width = r.width -  r.left;
	r.height = r.height -  r.top;
	return MY_UI::Utilities::Point(r.width, r.height);
}
MY_UI::Utilities::Rect MY_UI::Controls::cWidgetBase::GetLargestChildBounds() const{
	if(Internal_Widget.Children.empty()){
		Utilities::Point s = GetSize();
		return Utilities::Rect(0, 0, s.x, s.y);
	}
	Utilities::Rect r(99999, 99999, -99999, -99999);
	for(size_t i(0); i< Internal_Widget.Children.size(); i++){
		Utilities::Rect rect = Internal_Widget.Children[i]->Internal_Widget.AbsolutePosition;
		r.left = min(rect.left, r.left);
		r.width = max(rect.left + rect.width, r.width);
		
		r.top = min(rect.top , r.top);
		r.height = max(rect.top + rect.height , r.height);

	}
	int leftsub = min(r.left, Internal_Widget.Clientpos.left);// need the smallest
	int topsub = min(r.top, Internal_Widget.Clientpos.top);// need the smallest
	r.width = r.width -  leftsub;
	r.height = r.height -  topsub;
	r.left = r.top =0;
	return r;
}
void MY_UI::Controls::cWidgetBase::Align_To_Widget(cWidgetBase* widget, int align, int buffer){
	Internal_Widget.Alignment = align;
	Internal_Widget.Buffer = buffer;
	if(align == NO_ALIGNMENT) return;// no work to do, assign and get out
	Utilities::Rect widgetrect = widget->GetControlBounds();
	Utilities::Point size = GetSize();
	Utilities::Point pos = GetPos();


	if(align & BOTTOM){
		if(align & CENTER) pos.x = widgetrect.width/2 + widgetrect.left - size.left/2;
		else if(align & IN_LINE) pos.x = widgetrect.left;
		pos.y = widgetrect.top + widgetrect.height + buffer;
		SetPos(pos.x, pos.y);
	} else if(align & TOP){
		if(align & CENTER) pos.x = widgetrect.width/2 + widgetrect.left - size.left/2;
		else if(align & IN_LINE) pos.x = widgetrect.left;
		pos.y = widgetrect.top - size.y - buffer;
		SetPos(pos.x, pos.y);
	}
	if(align & LEFT){
		if(align & CENTER) pos.y = widgetrect.top + widgetrect.height/2 - size.top/2;
		else if(align & IN_LINE) pos.y = widgetrect.top;
		pos.x = widgetrect.left - size.x - buffer;
		SetPos(pos.x, pos.y);
	} else if(align & RIGHT){
		if(align & CENTER) pos.y = widgetrect.top + widgetrect.height/2 - size.top/2;
		else if(align & IN_LINE) pos.y = widgetrect.top;
		pos.x = widgetrect.left + widgetrect.width + buffer;
		SetPos(pos.x, pos.y);
	}
}

void MY_UI::Controls::cWidgetBase::Draw_Clip(){
	if(Internal_Widget.Hidden) return;
	for(int i((int)Internal_Widget.Children.size()-1); i >= 0 ; i--) {
		Internal_Widget.Children[i]->Draw_Clip(); 
	}
}

void MY_UI::Controls::cWidgetBase::Draw_NoClip(){
	if(Internal_Widget.Hidden) return;
	for(int i((int)Internal_Widget.Children.size()-1); i >= 0 ; i--) {
		Internal_Widget.Children[i]->Draw_NoClip(); 
	}
}
void MY_UI::Controls::cWidgetBase::Update(){ for(size_t i(0); i< Internal_Widget.Children.size(); i++) Internal_Widget.Children[i]->Update(); } 
void MY_UI::Controls::cWidgetBase::ForceUpdate(){ 
	for(size_t i(0); i< Internal_Widget.Children.size(); i++){
		Internal_Widget.Children[i]->Internal_Widget.Needs_Update =true;
		Internal_Widget.Children[i]->Update(); 
	}
} 

// this is called by a CHILD when it is being destroyed so the parent can preform any necessary actions before removal
void MY_UI::Controls::cWidgetBase::DeCouple(cWidgetBase* child){
	for(auto i(Internal_Widget.Children.begin()); i !=Internal_Widget.Children.end(); i++) {
		if(*i== child){
			if(i == Internal_Widget.Children.begin()){
				bool focus = (*i)->GetFocus();
				Internal_Widget.Children.erase(i);
				if((focus) &  !Internal_Widget.Children.empty()) {
					(*Internal_Widget.Children.begin())->SetFocus(true);
				}
			} else Internal_Widget.Children.erase(i);		
			break;
		}
	}
}
void MY_UI::Controls::cWidgetBase::Couple(cWidgetBase* child){
	Internal_Widget.Children.push_back(child);
}
void MY_UI::Controls::cWidgetBase::clear(){
	for(size_t i(0); i< Internal_Widget.Children.size(); i++){
		// if you want to delete the children RIGHT NOW, then delete them yourself!!
		MY_UI::Internal::RootWidget->DelayedDelete(Internal_Widget.Children[i]);// this is a safe delete. It ensures that the widget will continue to exsist till the begining of the next UI loop.
	}
}
/*-----------------------------------------------------cRootWidget-----------------------------------------------------------------*/


MY_UI::cRootWidget::cRootWidget(): FocusHolder(0), HoveredWidget(0), AppLayer(0), cWidgetBase(0), DraggedWidget(0), HoveredText(0), TimeOverControl(0), Caret(0) {
	LastCaretFlash = 0;
	Internal_Widget.Parent =0;// make sure to set this so children wont try and speed by us!!
	WidgetHasChanged=true;
	
}
MY_UI::cRootWidget::~cRootWidget(){ // root widget will delete all the ui components
	DeleteAllWidgets();
	// the caret, hovered text and app layer will exist until the end of the cWidgetBase Destructor ...
	Caret->Internal_Widget.Parent=0;
	Internal_Widget.Children.push_back(Caret);
	if(HoveredText){
		HoveredText->Internal_Widget.Parent=0;
		Internal_Widget.Children.push_back(HoveredText);
	}
	if(AppLayer){
		AppLayer->Internal_Widget.Parent=0;
		Internal_Widget.Children.push_back(AppLayer);
	}
}
bool MY_UI::cRootWidget::Init(){
	Caret = new MY_UI::Controls::Image(NULL);
	Caret->Set_Texture(MY_UI::Internal::WidgetSkin->White_Box);
	Caret->SetColor(MY_UI::Utilities::Black);
	Caret->SetSize(2, 19);
	Caret->SetHidden(true);// hide this!!
	AppLayer = new Controls::ApplicationLayer();//
	return true;
}
MY_UI::Controls::cWidgetBase* MY_UI::cRootWidget::Hit(){
	for(size_t i(0); i< Internal_Widget.Children.size(); i++){
		MY_UI::Controls::cWidgetBase* temp = Internal_Widget.Children[i]->Hit();
		if(temp != 0) return temp;// return the hit child
	}
	return 0;
}
MY_UI::Controls::cWidgetBase* MY_UI::cRootWidget::Hit_And_SetFocus(){
	std::vector<cWidgetBase*>& children = Internal_Widget.Children;
	for(unsigned int i(0); i < children.size(); i++){
		MY_UI::Controls::cWidgetBase* temp = children[i]->Hit_And_SetFocus();
		if( temp != 0) {
			MY_UI::Controls::cWidgetBase* otemp = children[i];
			if(i != 0){// if the focus has changed... tell the controls
				children[0]->SetFocus(false);// the widget at the beginning has lost focus.. let it know
				children[i]->SetFocus(true);
				for(int j = i; j != 0; j--){
					children[j] = children[j-1];
				}
				children[0]=otemp;

			} else if(!children[i]->GetFocus()){
				children[i]->SetFocus(true);
			}
			return temp;// return the hit child
		}
	}
	return 0;
}
bool  MY_UI::cRootWidget::MouseButtonDown(void (MY_UI::Controls::cWidgetBase::*func)()){
	MY_UI::Controls::cWidgetBase* controlhit = Hit_And_SetFocus();

	if(controlhit == 0) {
		AllLoseFocus();// we compleltly missed evertything, make sure all controls have NO FOCUS
		controlhit = AppLayer->Hit_And_SetFocus();// if the UI layer was missed, check the other layers, which could be a camera, terrain editor, key catcher.. etc	

	} 
	if(controlhit != 0){// something was hit.. run mouse down on it
		(controlhit->*func)();// run the mousedown on the control
	}
	FocusHolder=controlhit;
	return FocusHolder != 0;
}

bool   MY_UI::cRootWidget::MouseButtonUp(void (MY_UI::Controls::cWidgetBase::*func)()){
	cWidgetBase* controlhit= Hit();

	if(controlhit == 0) controlhit = AppLayer->Hit();// if the UI layer was missed, check the other layers, which could be a camera, terrain editor, key catcher.. etc	
	if(controlhit != 0){
		if(controlhit != DraggedWidget){// dont run mouse up of the control was just being dragged
			(controlhit->*func)();// run the mouseup on the control
		}
	}

	/*
	if((FocusHolder != 0) & (FocusHolder != controlhit)){
		FocusHolder->SetFocus(false);
		FocusHolder->SetFocus(true);
	}
	*/
	if(!Mouse_LButton_Down) DraggedWidget = 0;// make sure to set this, just in case
	return FocusHolder != 0;
}

bool  MY_UI::cRootWidget::MouseMoved(bool extraparam){

	if(DraggedWidget != 0 ){// dragging something
		DraggedWidget->MouseMoved();	
		return true;// get out, nothing else can be hit 
		
	} 

	cWidgetBase* controlhit = Hit();
	if(controlhit == 0 ) controlhit = AppLayer->Hit();

	// if the control just hit was not the current focus holder, make sure to tell it to lose focus	
	// run the mouseexited first, then mouseentered
	if((HoveredWidget != controlhit) & (HoveredWidget != 0 )){// this means there is a new control as the hovered widget, 
		HoveredWidget->MouseExited();
	} 
	if(controlhit != 0){
		if(HoveredWidget != controlhit){// new control is being hovered over... 
			controlhit->MouseEntered();
			delete HoveredText;
			HoveredText =0;
			TimeOverControl = Utilities::GetTime();
		}

		if((Mouse_LButton_Down | Mouse_RButton_Down) & (controlhit == FocusHolder)){// dragging
			if(controlhit->GetDraggable()) DraggedWidget = controlhit;
			else DraggedWidget =0;
		} else DraggedWidget =0;
		controlhit->MouseMoved();
	} else {// no control is being hovered over.. make sure to set everything to a good state
		Utilities::SetCursor(Utilities::Standard); // this is needed because sometimes the cursor skips and I miss a mouse leaving a control
		delete HoveredText;
		HoveredText =0;
		TimeOverControl=0;
	}
	HoveredWidget=controlhit;
	return HoveredWidget != 0;
}
bool MY_UI::cRootWidget::MouseWheelMoved(bool extraparam){
	if(FocusHolder != 0) FocusHolder->MouseWheelMoved();
	return FocusHolder != 0;
}

bool  MY_UI::cRootWidget::Key(void (MY_UI::Controls::cWidgetBase::*func)(), int key1, int key2){
	std::function<void ()>& f = KeyMap[key1][key2];
	if(f) { f(); return true; }

	if(FocusHolder !=0) {
		(FocusHolder->*func)();
		return true;
	}
	return false;
}
void MY_UI::cRootWidget::SetSize(int x, int y){ // store the size and let the renderer know we are changing resolutions
	cWidgetBase::SetSize(x, y);
	Internal::WidgetRenderer->OnResize(x, y); 
}

void MY_UI::cRootWidget::Draw_NoClip(){
	
	Internal::WidgetRenderer->Begin();
	Internal::WidgetRenderer->StartClip(GetControlBounds());
	// the drawing is backwards because I draw everything back to front to preserve the correct layering
	Update();
	// check to see if a tooltip should pop up ....
	if(HoveredWidget !=0 ){
		if( HoveredText == 0){
			if(HoveredWidget->Internal_Widget.Popup.size() > 0){// dont draw a tooltip if there is nothing to pop up
				unsigned int time =Utilities::GetTime();
				
				if(time - TimeOverControl > POPUPTIME){
					HoveredText = new MY_UI::Controls::ColoredRect_w_Label(NULL);
					HoveredText->SetLabel(HoveredWidget->Internal_Widget.Popup, CENTER);	
					MY_UI::Utilities::Point p = HoveredText->Label->GetSize();
					HoveredText->Widget->SetSize(p.x +5 , p.y +5);
					HoveredText->SetPos(New_MousePosx + 15, New_MousePosy);
					
				}
			}
		} else if((New_MousePosx != Old_MousePosx) | (New_MousePosy != Old_MousePosy)  ){// move the tooltip if the mouse moved. This causes WidgetHasChanged to be true, which causes everything to be redrawn
			HoveredText->SetPos(New_MousePosx + 15, New_MousePosy);	
		}
	} 

	if(WidgetHasChanged) {
		Internal::WidgetRenderer->StartNewBatch();
		for(int i((int)Internal_Widget.Children.size()-1); i >= 0 ; i--) {
			Internal_Widget.Children[i]->Draw_NoClip(); 
		}
		if(HoveredText){// draw the tooltip here	
			HoveredText->Draw_NoClip();
		}
	}
	MY_UI::Internal::WidgetRenderer->DrawBufferd();
	Internal::WidgetRenderer->EndClip();
	Internal::WidgetRenderer->End();
	WidgetHasChanged=false;// seed this for the next loop. If no controls make a change, draw the buffered stuff. No need to redo work
	// these have tpo be zero'd out each loop. This is the delta for the last frame and I cant put it into the cInput class because that only receives messages when something has changed. So, if you are dragging something and stop, the Input will think you are still dragging
	//because it wont receive a message to stop, it just stops receiving messages
	Delta_Mousex = 0;
	Delta_Mousey = 0;
}

void MY_UI::cRootWidget::Update(){
	for(auto i = DelayedDeleteBuffer.begin(); i != DelayedDeleteBuffer.end(); i++) MY_UI::Safe_Delete(*i);
	DelayedDeleteBuffer.clear();
	for(auto i = WidgetsUpdateBuffer.begin(); i != WidgetsUpdateBuffer.end(); i++) (*i)->Update();
}
void MY_UI::cRootWidget::DeleteAllWidgets(){
	for(auto i(DelayedDeleteBuffer.begin()); i != DelayedDeleteBuffer.end(); i++) {
		(*i)->Internal_Widget.Parent =0;
		MY_UI::Safe_Delete(*i);
	}
	DelayedDeleteBuffer.clear();
	WidgetsUpdateBuffer.clear();
	WidgetUpdateSet.clear();
	FocusHolder=0;
	HoveredWidget=0;
	DraggedWidget=0;
	HoveredText=0;

}
void MY_UI::cRootWidget::CheckForUpdate(cWidgetBase* widget, bool check){
	if(check){
		if(WidgetUpdateSet.insert(widget).second){
			WidgetsUpdateBuffer.push_back(widget);
		}
	} else {
		if(!WidgetUpdateSet.empty()) WidgetUpdateSet.erase(widget);// the empty check is needed for some reason, otherwise there are run time errors
		for(size_t i = 0; i < WidgetsUpdateBuffer.size(); i++){
			if(WidgetsUpdateBuffer[i] == widget){
				WidgetsUpdateBuffer.erase(WidgetsUpdateBuffer.begin() + i);
				break;
			}
		}
	}
}
void MY_UI::cRootWidget::AllLoseFocus(){
	for(size_t i =0; i< Internal_Widget.Children.size(); i++)  Internal_Widget.Children[i]->SetFocus(false);
}

void MY_UI::cRootWidget::CleanLinks(cWidgetBase* widget){
	if(widget == FocusHolder) FocusHolder=0;
	if(widget == HoveredWidget) HoveredWidget=0;
	if(widget == DraggedWidget) DraggedWidget=0;
	if(widget == HoveredText) HoveredText=0;
	CheckForUpdate(widget, false);// remove the widget 

}
void MY_UI::cRootWidget::DelayedDelete(cWidgetBase* widget){
	DelayedDeleteBuffer.push_back(widget);
}