#ifndef MY_UI_TOO_WINDOW_H
#define MY_UI_TOO_WINDOW_H
#include "Widget.h"
#include <string>
#include "Common.h"

namespace MY_UI_Too{

	namespace Controls{
		class Text;
		class Window : public Widget{
			MY_UI_Too::Controls::Text* _Title;
			MY_UI_Too::Controls::Widget* _Background;
			Resizing_States _Resize_States;
			void _Update_Mouse_Cursor();
		public:
			Window(IWidget* parent);
			virtual ~Window(){}

			virtual void Set_Title(std::string title);
			virtual std::string Get_Title() const;

			virtual void Set_Size(Utilities::Point p) override;

			virtual void Add_Child(IWidget* child)override;
			
			virtual void Mouse_Entered() override;
			virtual void Mouse_Left_Down() override;
			virtual void Mouse_Moved() override;
			virtual void Draw(MY_UI_Too::Interfaces::ISkin* skin) override;
		};
	};
};


#endif