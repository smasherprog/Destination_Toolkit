#include "PCH.h"
#include "Window.h"
#include "ISkin.h"
#include "IInput.h"
#include "Text.h"

#define WND_TITLE_HEIGHT 30
#define WND_BORDER 6

MY_UI_Too::Controls::Window::Window(MY_UI_Too::Interfaces::IWidget* parent) : Widget(parent){
	_Resize_States = Resizing_States::No_Resizing;

	_Title = new MY_UI_Too::Controls::Text(nullptr);
	_Internals.Children.push_back(_Title);
	_Title->Set_Parent(this);
	_Title->Set_Text("Window Title");
	_Title->Set_Font_Size(18);
	_Title->Set_Pos(Utilities::Point(10, 10));
	_Title->Set_Hitable(false);

	_Background = new MY_UI_Too::Controls::Widget(nullptr);
	_Internals.Children.push_back(_Background);
	_Background->Set_Parent(this);
	_Background->Set_Pos(Utilities::Point(WND_BORDER, WND_TITLE_HEIGHT));
	_Background->Set_Name("wnd back");

	Set_Draggable(true);
	Set_Size(Utilities::Point(250, 300));
}

void MY_UI_Too::Controls::Window::Set_Title(std::string title){
	_Title->Set_Text(title);
}
std::string MY_UI_Too::Controls::Window::Get_Title() const{
	return _Title->Get_Text();
}
void MY_UI_Too::Controls::Window::Add_Child(IWidget* child){
	_Background->Add_Child(child);
}
void MY_UI_Too::Controls::Window::Mouse_Left_Down(){
	MY_UI_Too::Controls::Widget::Mouse_Left_Down();
	Utilities::Point mouseloc(New_MousePosx, New_MousePosy);
	if(Utilities::Rect(_Internals.Absolute_TL.x , _Internals.Absolute_TL.y + WND_BORDER, WND_BORDER, _Internals.Size.y- (WND_BORDER*2)).Intersect(mouseloc)){//left center
		_Resize_States= Resizing_States::Resizing_L;
	} else if(Utilities::Rect(_Internals.Absolute_TL.x , _Internals.Absolute_TL.y, WND_BORDER, WND_BORDER).Intersect(mouseloc)){//top left corner
		_Resize_States= Resizing_States::Resizing_TL;
	} else if(Utilities::Rect(_Internals.Absolute_TL.x +WND_BORDER, _Internals.Absolute_TL.y,_Internals.Size.x- (WND_BORDER*2), WND_BORDER).Intersect(mouseloc)){//top center
		_Resize_States= Resizing_States::Resizing_T;
	} else if(Utilities::Rect(_Internals.Absolute_TL.x + _Internals.Size.x-WND_BORDER, _Internals.Absolute_TL.y,WND_BORDER, WND_BORDER).Intersect(mouseloc)){//top right corner
		_Resize_States= Resizing_States::Resizing_TR;
	} else if(Utilities::Rect(_Internals.Absolute_TL.x + _Internals.Size.x-WND_BORDER, _Internals.Absolute_TL.y + WND_BORDER, WND_BORDER, _Internals.Size.y- (WND_BORDER*2)).Intersect(mouseloc)){//right center
		_Resize_States= Resizing_States::Resizing_R;
	}else if(Utilities::Rect(_Internals.Absolute_TL.x + _Internals.Size.x-WND_BORDER, _Internals.Absolute_TL.y +_Internals.Size.y- WND_BORDER, WND_BORDER, WND_BORDER).Intersect(mouseloc)){//bottom right corner
		_Resize_States= Resizing_States::Resizing_BR;
	}else if(Utilities::Rect(_Internals.Absolute_TL.x +WND_BORDER, _Internals.Absolute_TL.y +_Internals.Size.y- WND_BORDER, _Internals.Size.x- (WND_BORDER*2), WND_BORDER).Intersect(mouseloc)){//bottom center
		_Resize_States= Resizing_States::Resizing_B;
	}else if(Utilities::Rect(_Internals.Absolute_TL.x, _Internals.Absolute_TL.y +_Internals.Size.y- WND_BORDER, WND_BORDER, WND_BORDER).Intersect(mouseloc)){//bottom left corner
		_Resize_States= Resizing_States::Resizing_BL;
	} else {
		_Resize_States= Resizing_States::No_Resizing;
	}
}


void MY_UI_Too::Controls::Window::Mouse_Entered(){
	MY_UI_Too::Controls::Widget::Mouse_Entered();
	_Update_Mouse_Cursor();
}

void MY_UI_Too::Controls::Window::Mouse_Moved(){
	if(Mouse_LButton_Down){
		if(_Resize_States== Resizing_States::Resizing_L){//left center
			Set_Size(Get_Size() + Utilities::Point(-Delta_Mousex, 0));
			Set_Pos_ByOffset(Utilities::Point(Delta_Mousex, 0));
		} else if(_Resize_States== Resizing_States::Resizing_TL){//top left corner
			Set_Size(Get_Size() + Utilities::Point(-Delta_Mousex,- Delta_Mousey));
			Set_Pos_ByOffset(Utilities::Point(Delta_Mousex, Delta_Mousey));
		} else if(_Resize_States== Resizing_States::Resizing_T){//top center
			Set_Size(Get_Size() + Utilities::Point(0, -Delta_Mousey));
			Set_Pos_ByOffset(Utilities::Point(0, Delta_Mousey));
		} else if(_Resize_States== Resizing_States::Resizing_TR){//top right corner
			Set_Size(Get_Size() + Utilities::Point(Delta_Mousex, -Delta_Mousey));
			Set_Pos_ByOffset(Utilities::Point(0, Delta_Mousey));
		} else if(_Resize_States== Resizing_States::Resizing_R){//right center
			Set_Size(Get_Size() + Utilities::Point(Delta_Mousex, 0));
			
		}else if(_Resize_States== Resizing_States::Resizing_BR){//bottom right corner
			Set_Size(Get_Size() + Utilities::Point(Delta_Mousex, Delta_Mousey));
			
		}else if(_Resize_States== Resizing_States::Resizing_B){//bottom center
			Set_Size(Get_Size() + Utilities::Point(0, Delta_Mousey));
	
		}else if(_Resize_States== Resizing_States::Resizing_BL){//bottom left corner
			Set_Size(Get_Size() + Utilities::Point(-Delta_Mousex, Delta_Mousey));
			Set_Pos_ByOffset(Utilities::Point(Delta_Mousex, 0));
		}else {//this is titlebar
			Set_Pos_ByOffset(Utilities::Point(Delta_Mousex, Delta_Mousey));

		}
	} else {
		_Update_Mouse_Cursor();
	}
	On_Mouse_Moved.Call();
}
void MY_UI_Too::Controls::Window::Draw(MY_UI_Too::Interfaces::ISkin* skin) {
	skin->Draw_Window(_Internals.State, Utilities::Rect(_Internals.Absolute_TL.x , _Internals.Absolute_TL.y, _Internals.Size.x, _Internals.Size.y));
	for( auto beg = _Internals.Children.begin(); beg != _Internals.Children.end(); beg++) (*beg)->Draw(skin);
}
void MY_UI_Too::Controls::Window::Set_Size(Utilities::Point p){
	MY_UI_Too::Controls::Widget::Set_Size(p);
	_Background->Set_Size(p - Utilities::Point(WND_BORDER*2, WND_BORDER+WND_TITLE_HEIGHT));
}
void MY_UI_Too::Controls::Window::_Update_Mouse_Cursor(){
	Utilities::Point mouseloc(New_MousePosx, New_MousePosy);
	if(Utilities::Rect(_Internals.Absolute_TL.x , _Internals.Absolute_TL.y + WND_BORDER, WND_BORDER, _Internals.Size.y- (WND_BORDER*2)).Intersect(mouseloc)){//left center
		MY_UI_Too::Internal::Input->SetCursor(Cursor_Types::L_to_R);
	} else if(Utilities::Rect(_Internals.Absolute_TL.x , _Internals.Absolute_TL.y, WND_BORDER, WND_BORDER).Intersect(mouseloc)){//top left corner
		MY_UI_Too::Internal::Input->SetCursor(Cursor_Types::Tl_to_Br);
	} else if(Utilities::Rect(_Internals.Absolute_TL.x +WND_BORDER, _Internals.Absolute_TL.y,_Internals.Size.x- (WND_BORDER*2), WND_BORDER).Intersect(mouseloc)){//top center
		MY_UI_Too::Internal::Input->SetCursor(Cursor_Types::U_to_D);
	} else if(Utilities::Rect(_Internals.Absolute_TL.x + _Internals.Size.x-WND_BORDER, _Internals.Absolute_TL.y,WND_BORDER, WND_BORDER).Intersect(mouseloc)){//top right corner
		MY_UI_Too::Internal::Input->SetCursor(Cursor_Types::Tr_to_Bl);
	} else if(Utilities::Rect(_Internals.Absolute_TL.x + _Internals.Size.x-WND_BORDER, _Internals.Absolute_TL.y + WND_BORDER, WND_BORDER, _Internals.Size.y- (WND_BORDER*2)).Intersect(mouseloc)){//right center
		MY_UI_Too::Internal::Input->SetCursor(Cursor_Types::L_to_R);
	}else if(Utilities::Rect(_Internals.Absolute_TL.x + _Internals.Size.x-WND_BORDER, _Internals.Absolute_TL.y +_Internals.Size.y- WND_BORDER, WND_BORDER, WND_BORDER).Intersect(mouseloc)){//bottom right corner
		MY_UI_Too::Internal::Input->SetCursor(Cursor_Types::Tl_to_Br);
	}else if(Utilities::Rect(_Internals.Absolute_TL.x +WND_BORDER, _Internals.Absolute_TL.y +_Internals.Size.y- WND_BORDER, _Internals.Size.x- (WND_BORDER*2), WND_BORDER).Intersect(mouseloc)){//bottom center
		MY_UI_Too::Internal::Input->SetCursor(Cursor_Types::U_to_D);
	}else if(Utilities::Rect(_Internals.Absolute_TL.x, _Internals.Absolute_TL.y +_Internals.Size.y- WND_BORDER, WND_BORDER, WND_BORDER).Intersect(mouseloc)){//bottom left corner
		MY_UI_Too::Internal::Input->SetCursor(Cursor_Types::Tr_to_Bl);
	} else if(Utilities::Rect(_Internals.Absolute_TL.x+ WND_BORDER, _Internals.Absolute_TL.y + WND_BORDER, _Internals.Size.x- WND_BORDER, _Internals.Size.y- WND_BORDER).Intersect(mouseloc)){
		MY_UI_Too::Internal::Input->SetCursor(Cursor_Types::Mover);
	} else {
		MY_UI_Too::Internal::Input->SetCursor(Cursor_Types::Standard);
	}
}