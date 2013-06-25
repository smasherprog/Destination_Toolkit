#ifndef MY_UI_TOO_BUTTON_H
#define MY_UI_TOO_BUTTON_H
#include "Widget.h"
#include <string>

namespace MY_UI_Too{

	namespace Controls{
		class Text;
		class Button : public Widget{
		public:
			Button(IWidget* parent=nullptr);
			virtual ~Button(){}

			virtual IWidget* Hit() override;
			virtual IWidget* Hit_And_SetFocus() override{return Hit();}
	
			virtual void Draw(MY_UI_Too::Interfaces::ISkin* skin) override;

			virtual void Set_Text(std::string txt);
			//if you want to change the text, do it through this pointer.
			Text* Text;
		};
	};
};


#endif