#include "PCH.h"
#include "Collapsible_List.h"
#include "ISkin.h"
#include "IInput.h"
#include "Text.h"

#define WND_TITLE_HEIGHT 24
#define WND_BORDER 6

MY_UI_Too::Controls::Collapsible_List::Collapsible_List(MY_UI_Too::Interfaces::IWidget* parent) : Widget(parent) {

	Set_Name("Collapsible_List");

	_Title = new MY_UI_Too::Controls::Text(nullptr);
	_Internals.Children.push_back(_Title);
	_Title->Set_Parent(this);
	_Title->Set_Text("Window Title");
	_Title->Set_Font_Size(18);
	_Title->Set_Pos(Utilities::Point(10, 4));
	_Title->Set_Hitable(false);
	_Title->Set_Name("Window Title");

	_Background = new MY_UI_Too::Controls::Widget(nullptr);
	_Internals.Children.push_back(_Background);
	_Background->Set_Parent(this);
	_Background->Set_Pos(Utilities::Point(WND_BORDER, WND_TITLE_HEIGHT));
	_Background->Set_Name("Window BackGround");

	Set_Draggable(true);
	Set_Size(Utilities::Point(150, 300));
}

MY_UI_Too::Controls::Collapsible_List::~Collapsible_List(){

}
void MY_UI_Too::Controls::Collapsible_List::Set_Title(std::string title){
	_Title->Set_Text(title);
}
std::string MY_UI_Too::Controls::Collapsible_List::Get_Title() const{
	return _Title->Get_Text();
}
void MY_UI_Too::Controls::Collapsible_List::Mouse_Moved(){
	if(Mouse_LButton_Down && _Internals.Draggable) Set_Pos_ByOffset(Utilities::Point(Delta_Mousex, Delta_Mousey));
	_Update_Mouse_Cursor();
	On_Mouse_Moved.Call();
}
void MY_UI_Too::Controls::Collapsible_List::Mouse_Entered(){
	MY_UI_Too::Controls::Widget::Mouse_Entered();
	_Update_Mouse_Cursor();
}

void MY_UI_Too::Controls::Collapsible_List::Set_Size(Utilities::Point size) {
	MY_UI_Too::Controls::Widget::Set_Size(size);
	Reorient_Children();
}
void MY_UI_Too::Controls::Collapsible_List::Draw(MY_UI_Too::Interfaces::ISkin* skin) {
	skin->Draw_Window(_Internals.State, Utilities::Rect(_Internals.Absolute_TL.x , _Internals.Absolute_TL.y, _Internals.Size.x, _Internals.Size.y));
	for( auto beg = _Internals.Children.begin(); beg != _Internals.Children.end(); beg++) (*beg)->Draw(skin);
}
void MY_UI_Too::Controls::Collapsible_List::Add_Child(IWidget* child){
	_Background->Add_Child(child);
	Reorient_Children();
}
void MY_UI_Too::Controls::Collapsible_List::Remove_Child(IWidget* child){
	MY_UI_Too::Controls::Widget::Remove_Child(child);
	Reorient_Children();
}
void MY_UI_Too::Controls::Collapsible_List::_Update_Mouse_Cursor(){
	Utilities::Point mouseloc(New_MousePosx, New_MousePosy);
	if(Utilities::Rect(_Internals.Absolute_TL.x+ WND_BORDER, _Internals.Absolute_TL.y + WND_BORDER, _Internals.Size.x- (WND_BORDER*2), WND_TITLE_HEIGHT- WND_BORDER).Intersect(mouseloc)){
		MY_UI_Too::Internal::Input->SetCursor(Cursor_Types::Mover);
	} else {
		MY_UI_Too::Internal::Input->SetCursor(Cursor_Types::Standard);
	}
}
void MY_UI_Too::Controls::Collapsible_List::Reorient_Children(){

}




MY_UI_Too::Controls::Collapsible_Category::Collapsible_Category(Collapsible_List* parent) : Widget(parent) {

	Set_Name("Collapsible_Category");

	_Title = new MY_UI_Too::Controls::Text(nullptr);
	_Internals.Children.push_back(_Title);
	_Title->Set_Parent(this);
	_Title->Set_Text("Window Title");
	_Title->Set_Font_Size(18);
	_Title->Set_Pos(Utilities::Point(10, 4));
	_Title->Set_Hitable(false);
	_Title->Set_Name("Window Title");

	_Background = new MY_UI_Too::Controls::Widget(nullptr);
	_Internals.Children.push_back(_Background);
	_Background->Set_Parent(this);
	_Background->Set_Pos(Utilities::Point(WND_BORDER, WND_TITLE_HEIGHT));
	_Background->Set_Name("Window BackGround");

	Set_Draggable(true);

	Set_Size(Utilities::Point(150, 300));
}

MY_UI_Too::Controls::Collapsible_Category::~Collapsible_Category(){

}
void MY_UI_Too::Controls::Collapsible_Category::Set_Title(std::string title){
	_Title->Set_Text(title);
}
std::string MY_UI_Too::Controls::Collapsible_Category::Get_Title() const{
	return _Title->Get_Text();
}
void MY_UI_Too::Controls::Collapsible_Category::Mouse_Moved(){
	if(Mouse_LButton_Down && _Internals.Draggable) Set_Pos_ByOffset(Utilities::Point(Delta_Mousex, Delta_Mousey));
	_Update_Mouse_Cursor();
	On_Mouse_Moved.Call();
}
void MY_UI_Too::Controls::Collapsible_Category::Mouse_Entered(){
	MY_UI_Too::Controls::Widget::Mouse_Entered();
	_Update_Mouse_Cursor();
}

void MY_UI_Too::Controls::Collapsible_Category::Set_Size(Utilities::Point size) {
	MY_UI_Too::Controls::Widget::Set_Size(size);
	Reorient_Children();
}
void MY_UI_Too::Controls::Collapsible_Category::Draw(MY_UI_Too::Interfaces::ISkin* skin) {
	skin->Draw_Window(_Internals.State, Utilities::Rect(_Internals.Absolute_TL.x , _Internals.Absolute_TL.y, _Internals.Size.x, _Internals.Size.y));
	for( auto beg = _Internals.Children.begin(); beg != _Internals.Children.end(); beg++) (*beg)->Draw(skin);
}
void MY_UI_Too::Controls::Collapsible_Category::Add_Child(IWidget* child){
	_Background->Add_Child(child);
	Reorient_Children();
}
void MY_UI_Too::Controls::Collapsible_Category::Remove_Child(IWidget* child){
	MY_UI_Too::Controls::Widget::Remove_Child(child);
	Reorient_Children();
}
void MY_UI_Too::Controls::Collapsible_Category::_Update_Mouse_Cursor(){
	Utilities::Point mouseloc(New_MousePosx, New_MousePosy);
	if(Utilities::Rect(_Internals.Absolute_TL.x+ WND_BORDER, _Internals.Absolute_TL.y + WND_BORDER, _Internals.Size.x- (WND_BORDER*2), WND_TITLE_HEIGHT- WND_BORDER).Intersect(mouseloc)){
		MY_UI_Too::Internal::Input->SetCursor(Cursor_Types::Mover);
	} else {
		MY_UI_Too::Internal::Input->SetCursor(Cursor_Types::Standard);
	}
}
void MY_UI_Too::Controls::Collapsible_Category::Reorient_Children(){
	
}


