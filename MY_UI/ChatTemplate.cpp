#include "stdafx.h"
#include "Test_Helper.h"
#include "TextBox.h"
#include "Button.h"
#include "List_Box.h"

MY_UI::Controls::Text_Window* ChatWindow=0;
MY_UI::Controls::TextBox* ChatBox=0;

void AddtoWindow(MY_UI::Controls::cWidgetBase* btn){
	ChatWindow->Insert(ChatBox->GetText());
	ChatBox->SetText("");//clear the chat box
}

void ChatTemplate(){
	delete Dummy;
	Dummy = new MY_UI::Controls::cWidget_Dummy(root);
	Dummy->SetControlBounds(root->GetControlBounds());
	Dummy->SetClientBounds(root->GetClientBounds());
	
	ChatWindow = new MY_UI::Controls::Text_Window(Dummy);
	ChatWindow->SetPos(300, 200);
	ChatWindow->SetSize(200, 300);
	
	ChatBox = new MY_UI::Controls::TextBox(Dummy);
	ChatBox->SetLength(200);
	ChatBox->Align_To_Widget(ChatWindow, BOTTOM | IN_LINE, 5);
	ChatBox->OnReturnPressed.Add(AddtoWindow, true);
	ChatBox->SetMaxCharacters(250);

	MY_UI::Controls::Button* btn = new MY_UI::Controls::Button(Dummy);
	btn->Align_To_Widget(ChatBox, RIGHT | IN_LINE, 5);
	btn->SetText("Send");
	btn->OnLeftClicked.Add(AddtoWindow, true);

}