#include "PCH.h"
#include "Utilities.h"
#include "Widget.h"
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

unsigned int Current_Cursor = MY_UI_Too::Utilities::Standard;

void MY_UI_Too::Utilities::Font::clear() { 
	FontName = "Arial";
	float size =15;
	bold=dropshadow=false;
	FontData=0;
}
#ifdef _WIN32

	unsigned int MY_UI_Too::Utilities::GetTime(){ return timeGetTime(); }
#else
	unsigned int MY_UI::Utilities::GetTime(){
		struct timeval timeVal;
		gettimeofday (& timeVal, NULL);
		return timeVal.tv_sec * 1000 + timeVal.tv_usec / 1000;
	}
#endif
MY_UI_Too::Utilities::Input::~Input() { 
	SetCursor(Standard); 
	SAFE_DELETE(Root);// this will cause the rest of the library to be destroyed as well
} 
bool MY_UI_Too::Utilities::Input::ProcessMessage(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam){
	int otherkey=0;
	switch( msg ){
		case WM_ACTIVATE:
			if( LOWORD(wParam) == WA_INACTIVE ) AppPaused = true;
			else AppPaused = false;
			break;
		case WM_LBUTTONUP:
			if(!Mouse_LButton_Down) return false; // windows screws up and sends multiple mouse events though there actually was only one...
			Mouse_LButton_Down= false;
			Root->Mouse_Left_Up();
			break;
		case WM_RBUTTONUP:
			if(!Mouse_RButton_Down) return false;// windows screws up and sends multiple mouse events though there actually was only one...
			Mouse_RButton_Down = false;
			Root->Mouse_Right_Up();
			break;
		case WM_LBUTTONDOWN:
			if(Mouse_LButton_Down) return false;// windows screws up and sends multiple mouse events though there actually was only one...
			Mouse_LButton_Down = true;
			Root->Mouse_Left_Down();
			break;
		case WM_RBUTTONDOWN:
			if(Mouse_RButton_Down) return false;// windows screws up and sends multiple mouse events though there actually was only one...
			Mouse_RButton_Down = true;
			Root->Mouse_Right_Down();
			break;
		case WM_MOUSEMOVE:	
			Old_MousePosx = New_MousePosx;
			Old_MousePosy = New_MousePosy;		
			New_MousePosx = LOWORD(lParam);
			New_MousePosy = HIWORD(lParam);
			Delta_Mousex = New_MousePosx - Old_MousePosx;
			Delta_Mousey = New_MousePosy - Old_MousePosy;
			Root->Mouse_Moved();
			break;
		case WM_MOUSEWHEEL:
			Mouse_Wheel_Delta = GET_WHEEL_DELTA_WPARAM(wParam)/120;// the mouse wheel gives increments of 120 for some odd reason
			Root->Mouse_Wheel_Moved();
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
			Root->Key_Down();
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
			Root->Key_Up();
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
					Root->Set_Control_Size(Utilities::Point(LOWORD(lParam), HIWORD(lParam)));
				}else if( wParam == SIZE_RESTORED ){// Restoring from minimized state?
					if( Minimized ){
						AppPaused = false;
						Minimized = false;
						Root->Set_Control_Size(Utilities::Point(LOWORD(lParam), HIWORD(lParam)));
					} else if( Maximized ){// Restoring from maximized state?
						AppPaused = false;
						Maximized = false;
						Root->Set_Control_Size(Utilities::Point(LOWORD(lParam), HIWORD(lParam)));
					}else {// API call such as SetWindowPos or SwapChain->SetFullscreenState.
						Root->Set_Control_Size(Utilities::Point(LOWORD(lParam), HIWORD(lParam)));
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
		default:	
			return true;
	};
	return false;
}
