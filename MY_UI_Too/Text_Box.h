#ifndef MY_UI_TOO_TEXT_BOX_H
#define MY_UI_TOO_TEXT_BOX_H
#include "Widget.h"
#include <string>
#include "ITexture.h"


namespace MY_UI_Too{

	namespace Controls{
		class Text;

		class Text_Box: public Widget{

			Utilities::UVs _UVs_No_Focus, _UVs_Focus,  _Selected_UVs;
			

		public:
			Text_Box(IWidget* parent);
			virtual ~Text_Box();

			virtual IWidget* Hit() override;
			virtual IWidget* Hit_And_SetFocus() override{return Hit();}

			virtual void Mouse_Entered()override;
			virtual void Mouse_Exited()override;
			virtual void Mouse_Left_Down()override;
			virtual void Mouse_Left_Up()override;

			virtual void Set_Focus(bool focus) override;
			virtual void Draw() override;

			virtual void Key_Up() override;
			//if you want to change the text, do it through this pointer.
			Text* Text;
		};

	};
};


#endif