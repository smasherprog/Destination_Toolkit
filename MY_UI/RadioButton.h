#ifndef CWIDGETRADIOBUTTON_H
#define CWIDGETRADIOBUTTON_H
#include "Image.h"

namespace MY_UI{

	namespace Controls{

		class Text;

		class RadioButton: public  Image {
		public:
			Internal::cInternal_CheckBox Internal_CheckBox;

			RadioButton(cWidgetBase* parent);
			virtual ~RadioButton(){}
/*-----------------------------Derived Virutal Functions go here -----------------------------------------*/
			virtual void MouseEntered();
			virtual void MouseExited();

			virtual void Update();
/*-----------------------------Added Functions go here -----------------------------------------*/
			virtual void SetSelected(bool selected);
			virtual bool GetSelected();
			virtual void ToggleRadioBox();

		};
		typedef cWidget_w_Label<RadioButton> RadioButton_w_Label;

		class RadioButtonContainer: public cWidgetBase{
		public:

			Internal::cInternal_RadioButtonContainer Internal_RadioButtonContainer;

			RadioButtonContainer(cWidgetBase* parent);
			virtual ~RadioButtonContainer(){}

			virtual void SetRadioButtonAlignment(int alignment, int labelalignment) { Internal_RadioButtonContainer.ButtonAlignment = alignment; }
			virtual int GetRadioButtonAlignment() { return Internal_RadioButtonContainer.ButtonAlignment; }
			virtual int GetLabelAlignment() { return Internal_RadioButtonContainer.LabelAlignment; }

			virtual RadioButton_w_Label* Insert(std::string txt, unsigned int id =0);
			virtual bool Erase(RadioButton_w_Label* radio);
			virtual void SetSelected(cWidgetBase* widget);

			virtual void clear();// deletes all buttons
			MY_UI::Controls::RadioButton* Selected;

			MY_UI::Internal::cWidgetCaller OnSelected;
		};
		

	};
};

#endif