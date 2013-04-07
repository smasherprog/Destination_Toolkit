#ifndef MY_UI_TOO_CHECKBOX_H
#define MY_UI_TOO_CHECKBOX_H
#include "Widget.h"

namespace MY_UI_Too{
	namespace Controls{
		class CheckBox : public Widget{
			Widget_States _State;
			bool _Checked;
		public:
			CheckBox(IWidget* parent);
			virtual ~CheckBox(){}
		
			virtual void Draw(MY_UI_Too::Interfaces::ISkin* skin) override;

		};
	};
};

#endif