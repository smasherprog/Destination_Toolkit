#ifndef MY_UI_TOO_ROOT_H
#define MY_UI_TOO_ROOT_H
#include "Widget.h"

namespace MY_UI_Too{
	namespace Controls{
		class Root: public Widget{

		protected:

			IWidget* Focus_Holder;
			IWidget* Hovered_Widget;
			IWidget* Dragged_Widget;

		public:
			Root();
			virtual ~Root();

			virtual void Set_Control_Bounds(Utilities::Rect p) override;
			virtual void Set_Control_Size(Utilities::Point p) override;
			virtual void Set_Control_Pos(Utilities::Point p) override;
			virtual void Set_Client_Bounds(Utilities::Rect p) override { Set_Control_Bounds(p); }
			virtual void Set_Client_Size(Utilities::Point p) override { Set_Control_Size(p); }
			virtual void Set_Client_Pos(Utilities::Point p) override { Set_Control_Pos(p); }

			virtual void Mouse_Left_Down() override;
			virtual void Mouse_Left_Up() override;
			virtual void Mouse_Right_Down() override;
			virtual void Mouse_Right_Up() override;
			virtual void Mouse_Moved() override;
			virtual void Mouse_Wheel_Moved() override;

			virtual void Key_Down() override;
			virtual void Key_Up() override;

			virtual IWidget* Hit() override;
			virtual IWidget* Hit_And_SetFocus() override;

			virtual void Draw() override;
		};
	};
};


#endif