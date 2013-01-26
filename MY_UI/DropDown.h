#ifndef CWIDGETDROPDOWN_H
#define CWIDGETDROPDOWN_H
#include "cWidgetBase.h"
#include "cWidgetEvents.h"

namespace MY_UI{

	namespace Controls{

		class DropDown: public cWidgetBase {
		public:

			Internal::cInternal_DropDown Internal_DropDown;
	
			DropDown(cWidgetBase* parent);
			virtual ~DropDown();

			virtual cWidgetBase* Hit_And_SetFocus();
			virtual cWidgetBase* Hit();
			virtual void SetSize(int x,int y);
			virtual void SetPos(int x, int y);
			virtual void MouseWheelMoved();
			virtual void LeftMouseButtonUp();
			virtual void SetFocus(bool s);
	
			virtual void KeyUp();

			virtual void Update();
			virtual void Draw_Clip();
			virtual void Draw_NoClip();
			virtual void DeCouple(cWidgetBase* widget);
			virtual void SetSizeToContents();

			virtual void ToggleOpened();
			virtual void CloseMenu();
			virtual void SetSelected(cWidgetBase* but);
			virtual MY_UI::Controls::Button* GetSelected() const { return Selected; }
			virtual MY_UI::Controls::Button* Insert(std::string text, unsigned int id =0);
			// if there are multiple selections with the same text, the first one encountered with the string text will be removed. To be exact, remove with the pointer
			// Erase WILL DELETE THE MENU ITEM!!!!!!
			virtual bool Erase(std::string text);
			virtual bool Erase(MY_UI::Controls::Button* widget);
			// this is a passive Link, meanig the drop down will not update the drop down from the ptr, but only update the ptr when an item is selected. I.e. it is One way, not two way
			virtual void SetLink(unsigned int* ptr) { Internal_DropDown.Link = ptr; }
			virtual unsigned int* GetLink() const { return Internal_DropDown.Link; }

			Button* Selected;
			std::vector<Button*> List;
			
		};
	};

};


#endif