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
			WindowCloseButton(IWidget* parent=nullptr);
			virtual ~WindowCloseButton(){}

			virtual void Draw(MY_UI_Too::Interfaces::ISkin* skin) override;

		};


		class WindowTitleBar : public Widget{
			WindowCloseButton* _CloseButton;
			bool _Closeable;
			MY_UI_Too::Interfaces::IWidget* _Child_Control;
			MY_UI_Too::Controls::Text* _Title;

		public:
			WindowTitleBar(IWidget* control, IWidget* parent=nullptr);
			virtual ~WindowTitleBar(){}

			virtual void Set_Closeable(bool closeable);
			virtual bool Get_Closeable() const{ return _Closeable;}

			virtual void Attach_Child(IWidget* child)override{_Child_Control->Attach_Child(child);}
			virtual void Detach_Child(IWidget* child)override{_Child_Control->Detach_Child(child);}

			virtual void Set_Size(Utilities::Point p) override;

			void Set_Title(std::string title);
			std::string Get_Title() const;

			virtual void Draw(MY_UI_Too::Interfaces::ISkin* skin) override;

		};

		class Window : public Widget{
		public:
			Window(IWidget* parent=nullptr);
			virtual ~Window(){}

			virtual void Draw(MY_UI_Too::Interfaces::ISkin* skin) override;
		};
	};
};


#endif