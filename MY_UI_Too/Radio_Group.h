#ifndef MY_UI_TOO_RADIO_GROUP_H
#define MY_UI_TOO_RADIO_GROUP_H
#include "Widget.h"
#include "ITexture.h"

namespace MY_UI_Too{
	namespace Controls{
		class Button;

		class Radio_Button : public Widget{

			Utilities::UVs _Radio_Checked_Hovered,  _Radio_Checked, _Radio_UnChecked_Hovered, _Radio_UnChecked, _Selected_UVs;
			bool _Checked;
			std::function<void(Radio_Button*)> _Radio_Selected;
		public:
			Radio_Button(IWidget* parent, std::function<void(Radio_Button*)> func);
			virtual ~Radio_Button();

			virtual void Mouse_Entered()override;
			virtual void Mouse_Exited()override;

			virtual void Mouse_Left_Up()override;

			virtual void Set_Focus(bool focus) override;
			virtual void Draw() override;

			virtual void Select();
			virtual void UnSelect();
		};



		class Radio_Group : public Widget{
		protected:
			std::vector<Radio_Button*> _Buttons;
			void _Radio_Selected(Radio_Button* radio);
		public:
			Radio_Group(IWidget* parent);
			virtual ~Radio_Group() override{}

			virtual void Draw() override;

			virtual Radio_Button* Add();

			

		};
	};
};
#endif