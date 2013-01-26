#include "stdafx.h"
#include "Test_Helper.h"
#include "Text.h"
#include "TextBox.h"

void TextExamples(){
	delete Dummy;
	Dummy = new MY_UI::Controls::cWidget_Dummy(root);
	Dummy->SetControlBounds(root->GetControlBounds());
	Dummy->SetClientBounds(root->GetClientBounds());

	MY_UI::Controls::Text* ptext = new MY_UI::Controls::Text(Dummy);
	ptext->SetPos(300, 300);
	ptext->SetFontSize(20);
	ptext->SetText("Text example");
	ptext->SetSizeToContents();// this is necessary to ensure the correct size is calculated from the text above

	MY_UI::Controls::Text* text = new MY_UI::Controls::Text(Dummy);
	text->SetText("Color example with BOTTOM | IN_LINE    alignment");
	text->SetFontSize(20);
	text->SetSizeToContents();// this is necessary to ensure the correct size is calculated from the text above
	text->SetColor(MY_UI::Utilities::Color(255, 0, 255, 0));
	text->Align_To_Widget(ptext, BOTTOM | IN_LINE);



	ptext = new MY_UI::Controls::Text(Dummy);
	ptext->SetText("ZYXWVUTSRQPONMLKJIHGFEDCBA ");
	ptext->SetBold(true);
	ptext->SetFontSize(30);
	ptext->SetSizeToContents();// this is necessary to ensure the correct size is calculated from the text above
	ptext->Align_To_Widget(text, BOTTOM | CENTER);

	text = new MY_UI::Controls::Text(Dummy);
	text->SetText("zyxwvutsrqpomnlkjihgfedcba ~ ! # $ % ^ \n  & * ( ) _ + { } | : < > ? 1 2 3 4 5 6 7 8 9 0 - = [ ] ; , . / ");
	text->SetBold(false);
	text->SetFontSize(30);
	text->SetSizeToContents();// this is necessary to ensure the correct size is calculated from the text above
	text->Align_To_Widget(ptext, BOTTOM | IN_LINE);

	MY_UI::Controls::TextBox* textbox = new MY_UI::Controls::TextBox(Dummy);
	textbox->Align_To_Widget(text, BOTTOM | CENTER);
	textbox->SetLength(150);
	textbox->SetSelectable(true);

	MY_UI::Controls::TextBox_w_Label* ptextbox = new MY_UI::Controls::TextBox_w_Label(Dummy);
	ptextbox->Widget->SetLength(70);
	ptextbox->SetLabel("TextBox w Label", LEFT | IN_LINE, 5);
	ptextbox->Align_To_Widget(textbox, BOTTOM | CENTER, 5);

	MY_UI::Controls::TextBox_w_Label* ptextbox1 = new MY_UI::Controls::TextBox_w_Label(Dummy);
	ptextbox1->Widget->SetLength(70);
	ptextbox1->Widget->Set_PasswordBox(true);
	ptextbox1->SetLabel("Password Box", LEFT | IN_LINE, 5);
	ptextbox1->Align_To_Widget(ptextbox, BOTTOM | CENTER, 5);
	ptextbox1->SetPopup("Password box");

	ptext = new MY_UI::Controls::Text(Dummy);
	ptext->SetText("Color example with BOTTOM | CENTER    alignment");
	ptext->SetFontSize(20);
	ptext->SetSizeToContents();// this is necessary to ensure the correct size is calculated from the text above
	ptext->SetColor(MY_UI::Utilities::Color(255, 255, 0, 0));
	ptext->Align_To_Widget(ptextbox1, BOTTOM | CENTER, 7);
	

}