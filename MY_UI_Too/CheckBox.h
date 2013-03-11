#ifndef MY_UI_TOO_CHECKBOX_H
#define MY_UI_TOO_CHECKBOX_H
#include "Widget.h"
#include "ITexture.h"

namespace MY_UI_Too{
	namespace Controls{
		class CheckBox : public Widget{

			Utilities::UVs _CheckBox_Checked_Hovered,  _CheckBox_Checked, _CheckBox_UnChecked_Hovered, _CheckBox_UnChecked, _Selected_UVs;
			bool _Checked;
		public:
			CheckBox(IWidget* parent);
			virtual ~CheckBox();
		
			virtual void Mouse_Entered()override;
			virtual void Mouse_Exited()override;
	
			virtual void Mouse_Left_Up()override;

			virtual void Set_Focus(bool focus) override;
			virtual void Draw() override;

		};
	};
};

#endif