#ifndef CWIDGET_COLLAPSIBLE_LIST_H
#define CWIDGET_COLLAPSIBLE_LIST_H
#include "cWidgetBase.h"
#include <string>
#include "List_Box.h"

namespace MY_UI{

	namespace Controls{
		// forward delcaration
		class Image;
		class Button;
		class Text;
		class Image;
		class Resizable;

		class Titlebar_Min : public  cWidgetBase {
		public:
			Titlebar_Min(cWidgetBase* parent);
			virtual ~Titlebar_Min() {}

			virtual void SetTitle(const std::string& windowtitle);
			virtual void SetSize( int x, int y);
			virtual cWidgetBase* Hit();
			virtual cWidgetBase* Hit_And_SetFocus();
		

			Text* Title;
			Button  *Bar;
		};
		
		class Collapsible_List : public  cWidgetBase {
		public:

			Internal::cInternal_CollapsibleList Internal_CollapsibleList;
			Collapsible_List(cWidgetBase* parent);
			virtual ~Collapsible_List(){}

/*-----------------------------Derived Virutal Functions go here -----------------------------------------*/
			virtual void SetSize(int x, int y);
		
			virtual void Draw_Clip(); // this will attempt to batch draw calls for better preformance. Default is to Draw_NoClip though . . 
			virtual void Draw_NoClip();// this will draw the widget immediately. Sometimes, you need to draw stuff right now!

/*-----------------------------Added Functions go here -----------------------------------------*/
			virtual void FocusChanged();
			virtual void SetTitle(const std::string& windowtitle);

			// Close will delete the window and all its children. The SetHidden function will only HIDE the window so its not drawn, not actually destroy it
			// override any of the following functions to be blank in order to make them not work
			virtual void Minimize();
			virtual void Restore();
			virtual void Dragging();

			virtual MY_UI::Controls::Button* Insert(std::string text);
			// if there are multiple slections with the same text, the first one encountered with the string text will be removed. To be exact, remove with the pointer
			// Erase WILL DELETE THE MENU ITEM!!!!!!
			virtual bool Erase(std::string text);
			virtual bool Erase(MY_UI::Controls::Button* widget);


			Titlebar_Min *TitleBar;
			MY_UI::Controls::List_Box* List_Box;
		};
	};
};

#endif