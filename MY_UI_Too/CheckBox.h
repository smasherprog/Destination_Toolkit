#ifndef MY_UI_TOO_CHECKBOX_H
#define MY_UI_TOO_CHECKBOX_H
#include "Widget.h"

namespace MY_UI_Too{
	namespace Controls{
		class CheckBox : public Widget{
			Widget_States _State;
			bool _Checked;
		public:
			CheckBox(IWidget* parent=nullptr);
			virtual ~CheckBox(){}

			virtual void Set_Checked(bool c){_Checked=c;}
			virtual bool Get_Checked() const{ return _Checked;}
			virtual void Mouse_Left_Up() override;
			virtual void Draw(MY_UI_Too::Interfaces::ISkin* skin) override;

		};
	};
};

#endif