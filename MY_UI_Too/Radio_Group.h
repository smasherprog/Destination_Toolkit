#ifndef MY_UI_TOO_RADIO_GROUP_H
#define MY_UI_TOO_RADIO_GROUP_H
#include "Widget.h"

namespace MY_UI_Too{
	namespace Controls{
		class Button;

		class Radio_Button : public Widget{

			bool _Checked;
			std::function<void(Radio_Button*)> _Radio_Selected;
		public:
			Radio_Button(IWidget* parent, std::function<void(Radio_Button*)> func);
			virtual ~Radio_Button(){}

			virtual void Mouse_Left_Up()override;

			virtual void Draw(MY_UI_Too::Interfaces::ISkin* skin) override;

			virtual void Select();
			virtual void UnSelect();
		};



		class Radio_Group : public Widget{
		protected:
			std::vector<Radio_Button*> _Buttons;
			void _Radio_Selected(Radio_Button* radio);
		public:
			Radio_Group(IWidget* parent=nullptr);
			virtual ~Radio_Group() override{}

			virtual void Draw(MY_UI_Too::Interfaces::ISkin* skin) override;

			virtual Radio_Button* Add();

			

		};
	};
};
#endif