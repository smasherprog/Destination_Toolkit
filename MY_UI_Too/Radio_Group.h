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
		public:
			Radio_Button(IWidget* parent);
			virtual ~Radio_Button();

			virtual void Mouse_Entered()override;
			virtual void Mouse_Exited()override;

			virtual void Mouse_Left_Up()override;

			virtual void Set_Focus(bool focus) override;
			virtual void Draw() override;

		};



		class Radio_Group : public Widget{
		protected:
			std::vector<Button*> _Buttons;
		public:
			Radio_Group(Widget* parent);
			virtual ~Radio_Group() override;

			virtual void Mouse_Left_Down()override;
			virtual void Mouse_Left_Up()override;

			virtual void Draw() override;

			virtual Button* Insert(std::string txt);


		};
	};
};
#endif