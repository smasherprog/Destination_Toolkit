#ifndef MY_UI_TOO_IINPUT_H
#define MY_UI_TOO_IINPUT_H
#include "IWidget.h"
#include "Common.h"

extern bool Mouse_RButton_Down, Mouse_LButton_Down, Mouse_MButton_Down;
extern bool CapsLock, ShiftDown, CtrlDown, NumLock, Key_Down;

// old_mouse pos is the position of the mouse cursor on the last loop. So, you can subtract the Old-New to get the delta to move things on screen
extern int Old_MousePosx, Old_MousePosy, New_MousePosx, New_MousePosy, Delta_Mousex, Delta_Mousey;
extern int Mouse_Wheel_Delta; // mouse wheel is always a delta
extern char Current_Key;


// the IInput's contract is to clean up the entire library by calling DeInit on the library
namespace MY_UI_Too{
	namespace Interfaces{
		class IInput{
		protected:
			Cursor_Types _Current_Cursor;
		public:

			IInput() { _Current_Cursor =Standard; }
			virtual ~IInput(){ MY_UI_Too::DeInit(); }
			virtual void SetCursor(Cursor_Types type)=0;
			virtual Cursor_Types GetCursor()=0;

		};
	};
};
#endif