#ifndef MY_UI_TOO_BUTTON_H
#define MY_UI_TOO_BUTTON_H
#include "Widget.h"
#include <string>
#include "ITexture.h"

namespace MY_UI_Too{

	namespace Controls{
		class Text;

		class Button : public Widget{

			Utilities::UVs _UVs_MouseDown, _UVs_MouseHovered, _UVs_MouseUp, _Selected_UVs;
			bool _Cust_MouseDown, _Cust_MouseHovered, _Cust_MouseUp;
		public:
			Button(IWidget* parent);
			virtual ~Button();

			virtual IWidget* Hit() override;
			virtual IWidget* Hit_And_SetFocus() override{return Hit();}
		
			virtual void Mouse_Entered()override;
			virtual void Mouse_Exited()override;
			virtual void Mouse_Left_Down()override;
			virtual void Mouse_Left_Up()override;

			virtual void Set_Focus(bool focus) override;
			virtual void Draw() override;

			virtual void Set_MouseDownImage(std::string file);
			virtual void Set_MouseUpImage(std::string file);
			virtual void Set_MouseHoveredImage(std::string file);

			virtual void Set_Text(std::string txt);
			//if you want to change the text, do it through this pointer.
			Text* Text;
		};
	};
};


#endif