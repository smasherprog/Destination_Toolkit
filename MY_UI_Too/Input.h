#ifndef MY_UI_TOO_INPUT_H
#define MY_UI_TOO_INPUT_H
#include "IInput.h"

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
#include <windows.h>

namespace MY_UI_Too{
	namespace Utilities{
		
		class Input : public Interfaces::IInput{
			// make sure there are no copies made!
			Input(const Input& rhs);
			Input& operator=(const Input& rhs);
			bool AppPaused, Minimized, Maximized, Resizing;
			virtual void SetCursor(Cursor_Types type);
			virtual Cursor_Types GetCursor(){ return _Current_Cursor; }

		public:

			bool ProcessMessage(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
			Input(): AppPaused(false), Minimized(false), Maximized(false), Resizing(false){  }
			virtual ~Input() override;

		};
	};
};

#endif

