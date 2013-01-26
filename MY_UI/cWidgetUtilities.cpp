#include "PCH.h"
#include "cWidgetUtilities.h"
#include "cWidgetBase.h"
#include "cWidgetRenderer.h"
#include "cWidgetInternals.h"
#include "Mmsystem.h"

bool CapsLock = false;
bool ShiftDown =false;
bool CtrlDown =false;
bool NumLock = false;
bool Mouse_RButton_Down= false;
bool Mouse_LButton_Down = false;
bool Mouse_MButton_Down= false;
bool Key_Down = false;

int Old_MousePosx(0), Old_MousePosy(0), New_MousePosx(0), New_MousePosy(0), Delta_Mousex(0), Delta_Mousey(0);
int Mouse_Wheel_Delta(0);
int Current_Key=0;

unsigned int Current_Cursor = MY_UI::Utilities::Standard;

MY_UI::Utilities::Point MY_UI::Utilities::TranslateToRelativeSpace(MY_UI::Controls::cWidgetBase* widget, int x, int y){
	MY_UI::Controls::cWidgetBase* parent=0;
	if(widget->Internal_Widget.Parent != 0) parent = widget->Internal_Widget.Parent;
	else return MY_UI::Utilities::Point(x, y);
	return MY_UI::Utilities::Point(x - parent->Internal_Widget.Clientpos.left, y - parent->Internal_Widget.Clientpos.top);
}

void MY_UI::Utilities::Texture::clear() { 
	Tex=0; 
	width=height=4; 
	id=-1; 
	FileName.clear(); 
	u1=v1=0.0f;
	u2=v2=1.0f;
}
void MY_UI::Utilities::Font::clear() { 
	FontName = "Arial";
	float size =15;
	bold=dropshadow=false;
	FontData=0;
}
#ifdef _WIN32

	unsigned int MY_UI::Utilities::GetTime(){ return timeGetTime(); }
#else
	unsigned int MY_UI::Utilities::GetTime(){
		struct timeval timeVal;
		gettimeofday (& timeVal, NULL);
		return timeVal.tv_sec * 1000 + timeVal.tv_usec / 1000;
	}
#endif
MY_UI::Utilities::cInput::~cInput() { 
	SetCursor(Standard); 
	MY_UI::DeInit(); // this will shut down the library on its destruction.. . . . 
} 
bool MY_UI::Utilities::cInput::ProcessMessage(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam){
	if(MY_UI::Internal::RootWidget==0) return false;
	int otherkey=0;
	switch( msg ){
		case WM_ACTIVATE:
			if( LOWORD(wParam) == WA_INACTIVE ) AppPaused = true;
			else AppPaused = false;
		case WM_LBUTTONUP:
			if(!Mouse_LButton_Down) return false; // windows screws up and sends multiple mouse events though there actually was only one...
			Mouse_LButton_Down= false;
			return MY_UI::Internal::RootWidget->MouseButtonUp(&MY_UI::Controls::cWidgetBase::LeftMouseButtonUp);
		case WM_RBUTTONUP:
			if(!Mouse_RButton_Down) return false;// windows screws up and sends multiple mouse events though there actually was only one...
			Mouse_RButton_Down = false;
			return MY_UI::Internal::RootWidget->MouseButtonUp(&MY_UI::Controls::cWidgetBase::RightMouseButtonUp);
		case WM_LBUTTONDOWN:
			if(Mouse_LButton_Down) return false;// windows screws up and sends multiple mouse events though there actually was only one...
			Mouse_LButton_Down = true;
			return MY_UI::Internal::RootWidget->MouseButtonDown(&MY_UI::Controls::cWidgetBase::LeftMouseButtonDown);
		case WM_RBUTTONDOWN:
			if(Mouse_RButton_Down) return false;// windows screws up and sends multiple mouse events though there actually was only one...
			Mouse_RButton_Down = true;
			return MY_UI::Internal::RootWidget->MouseButtonDown(&MY_UI::Controls::cWidgetBase::RightMouseButtonDown);
		case WM_MOUSEMOVE:	
			Old_MousePosx = New_MousePosx;
			Old_MousePosy = New_MousePosy;		
			New_MousePosx = LOWORD(lParam);
			New_MousePosy = HIWORD(lParam);
			Delta_Mousex = New_MousePosx - Old_MousePosx;
			Delta_Mousey = New_MousePosy - Old_MousePosy;
			return MY_UI::Internal::RootWidget->MouseMoved(true);

		case WM_MOUSEWHEEL:
			Mouse_Wheel_Delta = GET_WHEEL_DELTA_WPARAM(wParam)/120;// the mouse wheel gives increments of 120 for some odd reason
			return MY_UI::Internal::RootWidget->MouseWheelMoved(true);
		case WM_KEYDOWN:	
			if(wParam == VK_SHIFT) ShiftDown=true; 
			else if(wParam == VK_CONTROL) CtrlDown =true; 
			if(Key_Down) {
				if(Current_Key != static_cast<int>(wParam)){// two keys pressed simultatiously
					otherkey = Current_Key;
					Current_Key = static_cast<int>(wParam);
					return MY_UI::Internal::RootWidget->Key(&MY_UI::Controls::cWidgetBase::KeyDown, otherkey, Current_Key);
				}// else the keys are the same which means fire away
			} // else first time keydown.. 
			Key_Down= true;
			Current_Key = static_cast<int>(wParam);
			return MY_UI::Internal::RootWidget->Key(&MY_UI::Controls::cWidgetBase::KeyDown, 0, Current_Key);
		case WM_KEYUP:
			if(wParam == VK_SHIFT) ShiftDown=false;
			else if(wParam == VK_CAPITAL) {// capslock is enabled per key up/down, like capslock
				if(CapsLock) CapsLock =false; 
				else CapsLock =true; 
			}
			else if(wParam == VK_CONTROL) CtrlDown =false; 
			else if(wParam == VK_NUMLOCK) { // numlock is enabled per key up/down, like capslock
				if(NumLock) NumLock =false; 
				else NumLock =true; 
			}
			Current_Key = static_cast<int>(wParam);
			Key_Down = false;
			return MY_UI::Internal::RootWidget->Key(&MY_UI::Controls::cWidgetBase::KeyUp, 0, 0);
		// Resize window
		case WM_SIZE:// WM_SIZE is sent when the user resizes the window.  		
				if( wParam == SIZE_MINIMIZED ){
					AppPaused = true;
					Minimized = true;
					Maximized = false;
				}else if( wParam == SIZE_MAXIMIZED ){
					AppPaused = false;
					Minimized = false;
					Maximized = true;
					MY_UI::Internal::RootWidget->SetSize(LOWORD(lParam), HIWORD(lParam));
				}else if( wParam == SIZE_RESTORED ){// Restoring from minimized state?
					if( Minimized ){
						AppPaused = false;
						Minimized = false;
						MY_UI::Internal::RootWidget->SetSize(LOWORD(lParam), HIWORD(lParam));
					} else if( Maximized ){// Restoring from maximized state?
						AppPaused = false;
						Maximized = false;
						MY_UI::Internal::RootWidget->SetSize(LOWORD(lParam), HIWORD(lParam));
					}else {// API call such as SetWindowPos or SwapChain->SetFullscreenState.
						MY_UI::Internal::RootWidget->SetSize(LOWORD(lParam), HIWORD(lParam));
					}
				}
				return false;
		case WM_ENTERSIZEMOVE:// WM_EXITSIZEMOVE is sent when the user grabs the resize bars.
			AppPaused = true;
			Resizing  = true;
			return false;
		case WM_EXITSIZEMOVE: // WM_EXITSIZEMOVE is sent when the user releases the resize bars. Here we reset everything based on the new window dimensions.
			AppPaused = false;
			Resizing  = false;
			return false;
		case WM_MOUSELEAVE:
			Utilities::SetCursor(Utilities::Standard);
			return false;
	};
	return false;
}
