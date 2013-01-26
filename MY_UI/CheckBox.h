#ifndef CWIDGETCHECKBOX_H
#define CWIDGETCHECKBOX_H

#include "Button.h"

namespace MY_UI{

	namespace Controls{
		class cWidgetBase;

		class CheckBox: public  Button {
		public:
			Internal::cInternal_CheckBox Internal_CheckBox;

			CheckBox(cWidgetBase* parent);
			virtual ~CheckBox(){}
/*-----------------------------Derived Virutal Functions go here -----------------------------------------*/
			virtual void MouseEntered();
			virtual void MouseExited();

			virtual void Update();
/*-----------------------------Added Functions go here -----------------------------------------*/
			virtual void SetChecked(bool checked);
			virtual bool GetChecked();
			virtual void ToggleCheckBox();
			// Link checkbox takes a bool pointer and will keep the checkbox updated with the value of the bool and will update the bool when the checkbox is checked
			virtual void SetLink(bool* ptr);
			virtual bool* GetLink();


		};
		typedef cWidget_w_Label<CheckBox> CheckBox_w_Label;
	};
};

#endif