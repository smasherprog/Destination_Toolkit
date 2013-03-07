#ifndef MY_UI_TOO_BUTTON_H
#define MY_UI_TOO_BUTTON_H
#include "Widget.h"
#include <string>

namespace MY_UI_Too{

	namespace Controls{
		class Button : public Widget{
			unsigned int _State;
		public:
			Button(IWidget* parent);
			virtual ~Button();

			virtual void Mouse_Entered()override;
			virtual void Mouse_Exited()override;
			virtual void Mouse_Left_Down()override;
			virtual void Mouse_Left_Up()override;

			virtual void Set_Focus(bool focus) override;

			virtual void Draw() override;
		};
	};
};


#endif