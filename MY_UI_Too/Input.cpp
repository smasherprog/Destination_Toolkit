#include "Input.h"
#include "Utilities.h"
#include "IWidget.h"
#include "Common.h"

Cursor_Types Current_Cursor =Standard;

#ifdef _WIN32
	void MY_UI_Too::Utilities::SetCursor(Cursor_Types type) { ::SetCursor( LoadCursor(0, C_Types[type]) ); Current_Cursor = type;}
#else
	void MY_UI_Too::Utilities::SetCursor(unsigned int type) { static_assert("You must define your own function for non windows platforms"); }
#endif


#ifdef _WIN32

MY_UI_Too::Utilities::Input::~Input() { 
	SetCursor(Standard); 
} 
bool MY_UI_Too::Utilities::Input::ProcessMessage(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam){
	if(Internal::Root_Widget == nullptr) return false;
	int otherkey=0;
	switch( msg ){
		case WM_ACTIVATE:
			if( LOWORD(wParam) == WA_INACTIVE ) AppPaused = true;
			else AppPaused = false;
			break;
		case WM_LBUTTONUP:
			if(!Mouse_LButton_Down) return false; // windows screws up and sends multiple mouse events though there actually was only one...
			Mouse_LButton_Down= false;
			Internal::Root_Widget->Mouse_Left_Up();
			break;
		case WM_RBUTTONUP:
			if(!Mouse_RButton_Down) return false;// windows screws up and sends multiple mouse events though there actually was only one...
			Mouse_RButton_Down = false;
			Internal::Root_Widget->Mouse_Right_Up();
			break;
		case WM_LBUTTONDOWN:
			if(Mouse_LButton_Down) return false;// windows screws up and sends multiple mouse events though there actually was only one...
			Mouse_LButton_Down = true;
			Internal::Root_Widget->Mouse_Left_Down();
			break;
		case WM_RBUTTONDOWN:
			if(Mouse_RButton_Down) return false;// windows screws up and sends multiple mouse events though there actually was only one...
			Mouse_RButton_Down = true;
			Internal::Root_Widget->Mouse_Right_Down();
			break;
		case WM_MOUSEMOVE:	
			Old_MousePosx = New_MousePosx;
			Old_MousePosy = New_MousePosy;		
			New_MousePosx = LOWORD(lParam);
			New_MousePosy = HIWORD(lParam);
			Delta_Mousex = New_MousePosx - Old_MousePosx;
			Delta_Mousey = New_MousePosy - Old_MousePosy;
			Internal::Root_Widget->Mouse_Moved();
			break;
		case WM_MOUSEWHEEL:
			Mouse_Wheel_Delta = GET_WHEEL_DELTA_WPARAM(wParam)/120;// the mouse wheel gives increments of 120 for some odd reason
			Internal::Root_Widget->Mouse_Wheel_Moved();
			break;
		case WM_KEYDOWN:	
			if(wParam == VK_SHIFT) ShiftDown=true; 
			else if(wParam == VK_CONTROL) CtrlDown =true; 
			if(Key_Down) {
				if(Current_Key != static_cast<int>(wParam)){// two keys pressed simultatiously
					otherkey = Current_Key;
					Current_Key = static_cast<int>(wParam);
					
				}// else the keys are the same which means fire away
			} // else first time keydown.. 
			Key_Down= true;
			Current_Key = static_cast<int>(wParam);
			Internal::Root_Widget->Key_Down();
			break;
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
			Internal::Root_Widget->Key_Up();
			break;
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
					Internal::Root_Widget->Set_Size(Utilities::Point(LOWORD(lParam), HIWORD(lParam)));
				}else if( wParam == SIZE_RESTORED ){// Restoring from minimized state?
					if( Minimized ){
						AppPaused = false;
						Minimized = false;
						Internal::Root_Widget->Set_Size(Utilities::Point(LOWORD(lParam), HIWORD(lParam)));
					} else if( Maximized ){// Restoring from maximized state?
						AppPaused = false;
						Maximized = false;
						Internal::Root_Widget->Set_Size(Utilities::Point(LOWORD(lParam), HIWORD(lParam)));
					}else {// API call such as SetWindowPos or SwapChain->SetFullscreenState.
						Internal::Root_Widget->Set_Size(Utilities::Point(LOWORD(lParam), HIWORD(lParam)));
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
			Utilities::SetCursor(Standard);
			return false;
		default:	
			return false;
	};
	return false;
}
#endif