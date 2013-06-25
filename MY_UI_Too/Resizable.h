#ifndef MY_UI_TOO_RESIZABLE_H
#define MY_UI_TOO_RESIZABLE_H
#include "Widget.h"
#include "Common.h"

namespace MY_UI_Too{

	namespace Controls{

		class Resizable : public Widget{
			MY_UI_Too::Interfaces::IWidget* _Child_Control;
			Resizing_States _Resize_States;
			void _Update_Mouse_Cursor();

		public:
			Resizable(IWidget* control, IWidget* parent=nullptr);
			virtual ~Resizable(){}

			virtual void Attach_Child(IWidget* child)override;
			virtual void Detach_Child(IWidget* child)override;

			virtual void Set_Size(Utilities::Point p)override;
			virtual void Set_Pos(Utilities::Point p)override;

			virtual void Mouse_Entered() override;
			virtual void Mouse_Left_Down() override;
			virtual void Mouse_Moved() override;

		};
	};
};


#endif