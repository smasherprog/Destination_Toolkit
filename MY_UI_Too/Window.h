#ifndef MY_UI_TOO_WINDOW_H
#define MY_UI_TOO_WINDOW_H
#include "Widget.h"
#include <string>

namespace MY_UI_Too{

	namespace Controls{
		class Text;
		class Image;
		class Window : public Widget{
			MY_UI_Too::Controls::Text* _Title;
			MY_UI_Too::Controls::Image* _Background;
		public:
			Window(IWidget* parent);
			virtual ~Window();

			virtual void Set_Title(std::string title);
			virtual std::string Get_Title() const;

			virtual void Mouse_Moved() override;

			virtual void Add_Child(IWidget* child)override;
			virtual IWidget* Hit() override;
			virtual IWidget* Hit_And_SetFocus() override;
		};
	};
};


#endif