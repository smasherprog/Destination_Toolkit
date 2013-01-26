#ifndef CTABCONTROL_H
#define CTABCONTROL_H
#include "cWidgetBase.h"

namespace MY_UI{

	namespace Controls{
		class Simple_Window;
		class Tab_Button;

		class Tab_Control: public  cWidgetBase {
		public:

			Tab_Control(cWidgetBase* parent);
			virtual ~Tab_Control(){}


			virtual void SetSize(int x, int y);
	
			virtual void Update();
			virtual void SwitchPage(MY_UI::Controls::cWidgetBase* widget);
			virtual Simple_Window* Insert(std::string text);
			// if there are multiple selections with the same text, the first one encountered with the string text will be removed. To be exact, remove with the pointer
			// Erase WILL DELETE THE MENU ITEM!!!!!!
			virtual bool Erase(std::string text);
			virtual bool Erase(MY_UI::Controls::Simple_Window* widget);

			std::vector<Simple_Window*> Pages;
			std::vector<Tab_Button*> Tabs;

			int CurrentPage;
		};
		

		

	};
};

#endif