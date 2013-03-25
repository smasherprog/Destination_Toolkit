#ifndef MY_UI_TOO_WINDOW_H
#define MY_UI_TOO_WINDOW_H
#include "Widget.h"
#include <string>

namespace MY_UI_Too{

	namespace Controls{
		class Text;
		class Window : public Widget{
			MY_UI_Too::Controls::Text* _Title;
		public:
			Window(IWidget* parent);
			virtual ~Window();

			virtual void Set_Title(std::string title);
			virtual std::string Get_Title() const;

			virtual void Mouse_Moved() override;

			virtual void Add_Child(IWidget* child)override;

			virtual IWidget* Hit_And_SetFocus() override;
		};
	};
};


#endif