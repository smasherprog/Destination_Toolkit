#ifndef CWIDGET_BUTTON_H
#define CWIDGET_BUTTON_H
#include <string>
#include "Image.h"

namespace MY_UI{

	namespace Controls{
		class cWidgetBase;
		
		class Button: public Image {
		public:
			

			Button(cWidgetBase* parent);
			virtual ~Button(){}
/*-----------------------------Derived Virutal Functions go here -----------------------------------------*/
			virtual void SetFocus(bool focus);		
			virtual void LeftMouseButtonDown();
			virtual void LeftMouseButtonUp();

			virtual void KeyUp();

			virtual cWidgetBase* Hit();
			virtual cWidgetBase* Hit_And_SetFocus(){ return Hit(); };

			virtual void Draw_Clip();
			virtual void Draw_NoClip();
		
			virtual void MouseExited();

/*-----------------------------Added Functions go here -----------------------------------------*/
			virtual void Show_Pressed_Texture();
			virtual void Show_UnPressed_Texture();
			virtual void Show_Hovered_Texture();
			virtual void Show_Selected_Texture();

			virtual void Set_Pressed_Texture(Utilities::Texture& tex, bool takeownership = false);
			virtual bool Set_Pressed_Texture(std::string filename);

			virtual void Set_UnPressed_Texture(Utilities::Texture& tex, bool takeownership = false);
			virtual bool Set_UnPressed_Texture(std::string filename);
		
			virtual void Set_Hovered_Texture(Utilities::Texture& tex, bool takeownership = false);
			virtual bool Set_Hovered_Texture(std::string filename);

			virtual void Set_Selected_Texture(Utilities::Texture& tex, bool takeownership = false);
			virtual bool Set_Selected_Texture(std::string filename);

			virtual void SetText(std::string text, int align = CENTER);
			virtual std::string GetText() const ;

			Controls::Text* Text; // this is so the user can access the text directly to modify it rather than me making a bunch of wrapper calls
			int LastShowImage;
		};
		typedef cWidget_w_Label<Button> Button_w_Label;

		class Tab_Button: public Button{
		public:

			Tab_Button(cWidgetBase* parent);
			virtual ~Tab_Button(){}

			virtual void SetText(std::string text, int align = CENTER);

		};
	};
};

#endif



