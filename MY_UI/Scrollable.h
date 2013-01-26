#ifndef CWIDGET_SCROLLABLE_H
#define CWIDGET_SCROLLABLE_H

#include "cWidgetBase.h"

namespace MY_UI{
	namespace Controls{

		class ScrollBars: public cWidgetBase{
		public:
			Internal::cInternal_Scrollable Internal_Scrollable;

			ScrollBars(cWidgetBase* parent);
			virtual ~ScrollBars() {} 

			virtual cWidgetBase* Hit();
			virtual cWidgetBase* Hit_And_SetFocus();
			
			virtual void SetSize(int x, int y);
			virtual void Update();

			virtual bool AtTop() { return Internal_Scrollable.Position.y >= 0; }
			virtual bool AtBottom(){ return Internal_Scrollable.Position.y <= -Internal_Scrollable.Size.top + GetSize().top; }
			virtual bool AtLeft(){ return Internal_Scrollable.Position.x >= 0; }
			virtual bool AtRight(){ return Internal_Scrollable.Position.x -1 <= -Internal_Scrollable.Size.left + GetSize().left; }
			
		};

		class Scrollable: public cWidgetBase{
		public:

/*-----------------------------Derived Virutal Functions go here -----------------------------------------*/
			Scrollable(cWidgetBase* parent);
			virtual ~Scrollable() {} 

			virtual void SetSize(int x, int y);

			virtual cWidgetBase* Hit();
			virtual cWidgetBase* Hit_And_SetFocus();

			virtual void MouseWheelMoved_UpDown(){ Scrolling_Y(-Mouse_Wheel_Delta*10); }
			virtual void MouseWheelMoved_LeftRight(){ Scrolling_X(-Mouse_Wheel_Delta*10); }
			virtual void Update();
			virtual void Couple(cWidgetBase* widget);
			virtual void DeCouple(cWidgetBase* widget);

/*-----------------------------Added Functions go here -----------------------------------------*/
			virtual void Scrolling_X(){ Scrolling_X((New_MousePosx - Old_MousePosx)); } 
			virtual void Scrolling_X(int x);
			virtual void Scrolling_Y() { Scrolling_Y((New_MousePosy - Old_MousePosy)); } 
			virtual void Scrolling_Y(int y);

			virtual void Scroll_DownBy_One(){ Scrolling_Y(1); }
			virtual void Scroll_UpBy_One(){ Scrolling_Y(-1); }

			virtual void Scroll_RightBy_One(){ Scrolling_X(1); }
			virtual void Scroll_LeftBy_One(){ Scrolling_X(-1); }

			virtual void Scroll_DownBy_OnePage() { Scroll_YBy_Page(1); } 
			virtual void Scroll_UpBy_OnePage(){Scroll_YBy_Page(-1); } 

			virtual void Scroll_RightBy_OnePage(){Scroll_XBy_Page(1); } 
			virtual void Scroll_LeftBy_OnePage(){Scroll_XBy_Page(-1); } 
			virtual void Scroll_YBy_Page(int pages);
			virtual void Scroll_XBy_Page(int pages);

			virtual void Bottom_ScrollerBackGroundHit();
			virtual void Right_ScrollerBackGroundHit();

			virtual void SetScrollableSize(int x, int y) ;
			virtual MY_UI::Utilities::Point GetScrollableSize() const { return Scroll_Bars->Internal_Scrollable.Size; }

			virtual void SetScrollablePos(int x, int y);
			virtual MY_UI::Utilities::Point GetScrollablePos() const { return Scroll_Bars->Internal_Scrollable.Position; }

			virtual void Draw_Clip();
			virtual void Draw_NoClip();

			ScrollBars* Scroll_Bars;
			cWidgetBase* Container;
		};
	
	};
};
#endif