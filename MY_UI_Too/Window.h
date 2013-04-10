#ifndef MY_UI_TOO_WINDOW_H
#define MY_UI_TOO_WINDOW_H
#include "Widget.h"
#include <string>
#include "Common.h"

namespace MY_UI_Too{

	namespace Controls{
		class Text;

		class WindowCloseButton : public Widget{
		public:
			WindowCloseButton(IWidget* parent);
			virtual ~WindowCloseButton(){}

			virtual IWidget* Hit() override;
			virtual IWidget* Hit_And_SetFocus() override{return Hit();}

			virtual void Draw(MY_UI_Too::Interfaces::ISkin* skin) override;

		};


		class Window : public Widget{
			MY_UI_Too::Controls::Text* _Title;
			MY_UI_Too::Controls::Widget* _Background;
			WindowCloseButton* _CloseButton;
			Resizing_States _Resize_States;
			bool _Closeable;

			void _Update_Mouse_Cursor();

		public:
			Window(IWidget* parent);
			virtual ~Window(){}

			virtual void Set_Closeable(bool closeable);
			virtual bool Get_Closeable() const{ return _Closeable;}

			void Set_Title(std::string title);
			std::string Get_Title() const;

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