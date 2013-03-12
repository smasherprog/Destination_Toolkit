#include "Text.h"
#include "Common.h"
#include "UnitTest.h"

void Text_Test(){

	MY_UI_Too::Internal::Root_Widget->RemoveAll_Children();// delete all children
	std::string textexample="";
	char start = START_CHAR;
	while(start<=END_CHAR){
		textexample +=start;
		start+=1;
	}


	MY_UI_Too::Controls::Text* text1 = new MY_UI_Too::Controls::Text(MY_UI_Too::Internal::Root_Widget);
	text1->Set_Text(textexample);
	text1->Set_Font_Size(20);
	text1->Set_Pos(MY_UI_Too::Utilities::Point(30, 100));

	MY_UI_Too::Controls::Text* text2 = new MY_UI_Too::Controls::Text(MY_UI_Too::Internal::Root_Widget);
	text2->Set_Font("Arial");
	text2->Set_Text(textexample);
	text2->Set_Font_Size(20);
	text2->Set_Pos(MY_UI_Too::Utilities::Point(30, 140));

	MY_UI_Too::Controls::Text* text3 = new MY_UI_Too::Controls::Text(MY_UI_Too::Internal::Root_Widget);
	text3->Set_Font("Courier New");
	text3->Set_Text(textexample);
	text3->Set_Font_Size(45);
	text3->Set_Bold(true);	
	text3->Set_Color(MY_UI_Too::Utilities::Blue);
	text3->Set_Pos(MY_UI_Too::Utilities::Point(30, 50));
}