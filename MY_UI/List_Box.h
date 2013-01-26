#ifndef LISTBOX_H
#define LISTBOX_H
#include "cWidgetBase.h"
#include "cWidgetEvents.h"
#include "Scrollable.h"
#include <vector>
#include <string>

namespace MY_UI{

	namespace Controls{
		class Veritcal_ScrollBar;
		class Button;
		class Image;

		class List: public cWidgetBase {
		public:

			List(MY_UI::Controls::cWidgetBase* parent);
			virtual ~List() {} 
			
			virtual void Update();
			virtual void DeCouple(cWidgetBase* widget);

			virtual void SetSelected(cWidgetBase* widget);
			virtual MY_UI::Controls::Button* GetSelected(){ return Selected; }
			virtual MY_UI::Controls::Button* Insert(std::string text);
			// if there are multiple slections with the same text, the first one encountered with the string text will be removed. To be exact, remove with the pointer
			// Erase WILL DELETE THE MENU ITEM!!!!!!
			virtual bool Erase(std::string text);
			virtual bool Erase(MY_UI::Controls::Button* widget);
			virtual void EraseAll();// erase all the items

			virtual void SetSelectable(bool selectable) { Selectable = selectable; }
			virtual bool GetSelectable() const { return Selectable; } 

			virtual void SetTextAlignment(int alignment = CENTER){ TextAlignment = alignment; }
			virtual int GetTextAlignment() const { return TextAlignment; }

			std::vector<Button*> ItemList;
			MY_UI::Controls::Button* Selected;
			MY_UI::Internal::cWidgetCaller OnSelected;
			bool Selectable;
			int TextAlignment;

		};
		class List_Box: public List {
		public:

			List_Box(MY_UI::Controls::cWidgetBase* parent);
			virtual ~List_Box();

			virtual void SetSize(int x, int y);
			virtual void SetPos(int x, int y);

			virtual void Draw_Clip();
			virtual void Draw_NoClip();
			
			MY_UI::Controls::Image* BackGround;

		};

		class List_Box_wScrollbars : public Scrollable {
		public:

			List_Box_wScrollbars(cWidgetBase* parent);
			virtual ~List_Box_wScrollbars() ;

			virtual void Draw_Clip();
			virtual void Draw_NoClip();

			virtual void SetSize(int x, int y);
			virtual void SetPos(int x, int y);

			virtual MY_UI::Controls::Button* Insert(std::string text){ Internal_Widget.Needs_Update= true; auto b = List->Insert(text); List->SetSizeToContents(); return b;}
			List* List;
			MY_UI::Controls::Image* BackGround;

		};
		class Text_Window : public List_Box_wScrollbars {
		public:
					
			Text_Window(cWidgetBase* parent);
			virtual ~Text_Window() {}

			virtual MY_UI::Controls::Button* Insert(std::string text);

		};
	};

};


#endif