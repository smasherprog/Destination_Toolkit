#ifndef MY_UI_TOO_TEXT_H
#define MY_UI_TOO_TEXT_H

#include "Widget.h"

namespace MY_UI_Too{
	namespace Controls{
		class Text: public Widget{

			Text(IWidget* widget);
			virtual ~Text(){}

			static bool Init_FontLibrary();
			static void DeInit_FontLibrary();

		};
	};
};

#endif