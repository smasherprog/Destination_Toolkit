#ifndef ColorPickerWINDOW
#define ColorPickerWINDOW
#include "cWidgetBase.h"
#include "TextBox.h"

namespace MY_UI{

	namespace Controls{
		class Image;
		class Text;

		typedef cWidget_w_Label<ColoredRect> ColoredRect_w_Label;

		class ColorPickerWindow: public cWidgetBase {
		public:

			Internal::cInternal_ColorPicker Internal_ColorPicker;
			ColorPickerWindow(cWidgetBase* parent);

			virtual ~ColorPickerWindow();

/*-----------------------------Derived Virutal Functions go here -----------------------------------------*/
			virtual void SetFocus(bool focus);
			virtual void SetSize(int x, int y);
			virtual void SetPos(int x, int y);

			virtual cWidgetBase* Hit();
			virtual cWidgetBase* Hit_And_SetFocus();
		
			virtual void Draw_Clip(); // this will attempt to batch draw calls for better preformance. Default is to Draw_NoClip though . . 
			virtual void Draw_NoClip();// this will draw the widget immediately. Sometimes, you need to draw stuff right now!
			virtual void Update();

/*-----------------------------Added Functions go here -----------------------------------------*/

			virtual void SetLink(Utilities::Color* color) { Internal_ColorPicker.Users_Color = color; }
			virtual Utilities::Color* GetLink() const { return  Internal_ColorPicker.Users_Color; }
			virtual void SetPickedColor(Utilities::Color color);
			virtual void Dragging();
			virtual void Close();
			virtual void DraggingArrow();
			virtual void DraggingCircle();
			virtual void MoveCircle();
			virtual void MoveToStrip();
			virtual void ReverseColorApply();
			virtual void SetSliderFromArrows();

			Controls::cWidget_w_Label<Numeric_TextBox<unsigned int>> *RedBox, *GreenBox, *BlueBox;
			
		};	
		
		class ColorPicker: public cWidgetBase {
		public:

			
			ColorPicker(cWidgetBase* parent);

			virtual ~ColorPicker(){}
/*-----------------------------Derived Virutal Functions go here -----------------------------------------*/
	
			virtual void SetSize(int x, int y);
/*-----------------------------Added Functions go here -----------------------------------------*/
			virtual void SetLink(Utilities::Color* color) ;// this will only apply a color when one is selected. It wouldnt make sense to do it activly. This is a passive control
			virtual void SetLink(float* red, float* green, float* blue, float* alpha);// this will only apply a color when one is selected. It wouldnt make sense to do it activly. This is a passive control
	
			virtual Utilities::Color* GetLink() const { return  Users_Color; }// this will only apply a color when one is selected. It wouldnt make sense to do it activly. This is a passive control

			virtual void OpenColorWindow();	
			virtual void SetColor();
			virtual void ColorPickerDestroyed();

			MY_UI::Controls::ColorPickerWindow* ColorPickerWindow; 
			MY_UI::Controls::ColoredRect* ColorPalet;

			MY_UI::Internal::cWidgetCaller OnColorSelected;
			MY_UI::Utilities::Color* Users_Color;
			float* Users_Red, *Users_Green, *Users_Blue, *Users_Alpha;
		};
		typedef cWidget_w_Label<ColorPicker> ColorPicker_w_Label;
	};
};

#endif