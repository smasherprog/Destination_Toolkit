#include "PCH.h"
#include "Window.h"
#include "ISkin.h"
#include "IInput.h"
#include "Text.h"
#include "Button.h"

#define WND_TITLE_HEIGHT 24
#define WND_BORDER 6
#define WND_HEIGHT 200
#define WND_WIDTH 350

MY_UI_Too::Controls::Window::Window(MY_UI_Too::Interfaces::IWidget* parent) : Widget(parent){
	Set_Name("Window");
	Set_Size(Utilities::Point(WND_WIDTH, WND_HEIGHT));
}
void MY_UI_Too::Controls::Window::Draw(MY_UI_Too::Interfaces::ISkin* skin) {
	skin->Draw_Window_BackGround(_Internals.State, Utilities::Rect(_Internals.Absolute_TL.x , _Internals.Absolute_TL.y, _Internals.Size.x, _Internals.Size.y));
	for( auto beg = _Internals.Children.begin(); beg != _Internals.Children.end(); beg++) (*beg)->Draw(skin);
}

MY_UI_Too::Controls::WindowCloseButton::WindowCloseButton(IWidget* parent): Widget(parent) {
	Set_Name("Window CloseButton");
	Set_Size(Utilities::Point(17, 17));
}
void MY_UI_Too::Controls::WindowCloseButton::Draw(MY_UI_Too::Interfaces::ISkin* skin){
	skin->Draw_WindowCloseButton(_Internals.State, Utilities::Rect(_Internals.Absolute_TL.x , _Internals.Absolute_TL.y, _Internals.Size.x, _Internals.Size.y));
}

MY_UI_Too::Controls::WindowTitleBar::WindowTitleBar(MY_UI_Too::Interfaces::IWidget* control, MY_UI_Too::Interfaces::IWidget* parent) : Widget(parent), _Child_Control(control){
	_Child_Control->Detach_From_Parent();
	MY_UI_Too::Controls::Widget::Attach_Child(_Child_Control);
	Set_Draggable(true);
	Set_Name("Window TitleBar");
	MY_UI_Too::Controls::Widget::Set_Size(Utilities::Point(0, WND_TITLE_HEIGHT)+ _Child_Control->Get_Size());
	_Child_Control->Set_Absolute_Pos(Get_Absolute_Pos() + Utilities::Point(0, WND_TITLE_HEIGHT));

	_Title = new MY_UI_Too::Controls::Text(this);
	_Title->Set_Font_Size(18);
	_Title->Set_Pos(Utilities::Point(10, 4));
	_Title->Set_Hitable(false);
	_Title->Set_Name("Window Title Text");
	Set_Title("Window Title");

	_CloseButton = new WindowCloseButton(this);
	_CloseButton->On_Mouse_Left_Up.Connect(&On_Mouse_Left_Up, std::bind(&MY_UI_Too::Controls::Widget::Delete_This, this));

	Set_Size(Utilities::Point(0, WND_TITLE_HEIGHT)+ _Child_Control->Get_Size());
}
void MY_UI_Too::Controls::WindowTitleBar::Set_Title(std::string title){
	_Title->Set_Text(title);
}
std::string MY_UI_Too::Controls::WindowTitleBar::Get_Title() const{
	return _Title->Get_Text();
}
void MY_UI_Too::Controls::WindowTitleBar::Set_Size(Utilities::Point p){
	MY_UI_Too::Controls::Widget::Set_Size(p);
	_CloseButton->Set_Pos(Utilities::Point(Get_Size().x - _CloseButton->Get_Size().x -4, 4));
	_Child_Control->Set_Size(Utilities::Point(0, -WND_TITLE_HEIGHT) + p);
}

void MY_UI_Too::Controls::WindowTitleBar::Set_Closeable(bool closeable){
	if(!closeable) {
		SAFE_DELETE(_CloseButton);
	} else {
		_CloseButton = new WindowCloseButton(nullptr);
		_Internals.Children.push_back(_CloseButton);
		_CloseButton->Set_Parent(this);
		_CloseButton->On_Mouse_Left_Up.Connect(&On_Mouse_Left_Up, std::bind(&MY_UI_Too::Controls::Widget::Delete_This, this));
	}
}
void MY_UI_Too::Controls::WindowTitleBar::Draw(MY_UI_Too::Interfaces::ISkin* skin) {
	skin->Draw_Window_TitleBar(_Internals.State, Utilities::Rect(_Internals.Absolute_TL.x , _Internals.Absolute_TL.y, _Internals.Size.x, _Internals.Size.y));
	for( auto beg = _Internals.Children.begin(); beg != _Internals.Children.end(); beg++) (*beg)->Draw(skin);
}


