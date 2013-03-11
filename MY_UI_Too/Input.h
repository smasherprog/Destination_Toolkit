#ifndef MY_UI_TOO_INPUT_H
#define MY_UI_TOO_INPUT_H
#include "IInput.h"
#include "Common.h"

extern bool Mouse_RButton_Down, Mouse_LButton_Down, Mouse_MButton_Down;
extern bool CapsLock, ShiftDown, CtrlDown, NumLock, Key_Down;

// old_mouse pos is the position of the mouse cursor on the last loop. So, you can subtract the Old-New to get the delta to move things on screen
extern int Old_MousePosx, Old_MousePosy, New_MousePosx, New_MousePosy, Delta_Mousex, Delta_Mousey;
extern int Mouse_Wheel_Delta; // mouse wheel is always a delta
extern Cursor_Types Current_Cursor;
extern int Current_Key;



#ifdef _WIN32

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
#include <windows.h>

namespace MY_UI_Too{
	namespace Utilities{
		void SetCursor(Cursor_Types type);
		inline Cursor_Types GetCursor(){ return Current_Cursor; }

		class Input : public Interfaces::IInput{
			// make sure there are no copies made!
			Input(const Input& rhs);
			Input& operator=(const Input& rhs);
			bool AppPaused, Minimized, Maximized, Resizing;

		public:

			bool ProcessMessage(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
			Input(): AppPaused(false), Minimized(false), Maximized(false), Resizing(false){  }
			virtual ~Input() override;

		};
	};
};

#endif


#endif