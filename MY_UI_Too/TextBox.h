#ifndef MY_UI_TOO_TEXTBOX_H
#define MY_UI_TOO_TEXTBOX_H
#include "Widget.h"
#include <string>

namespace MY_UI_Too{

	namespace Controls{

		class TextBox : public Widget{
		public:
			TextBox(IWidget* parent) : Widget(parent) {}
			virtual ~TextBox(){}

		};
	};
};


#endif