#include "PCH.h"
#include "List_Box.h"
#include "Image.h"
#include "cWidgetSkin.h"
#include "Button.h"
#include "Text.h"
#include "Scrollable.h"


MY_UI::Controls::List::List(cWidgetBase* parent): cWidgetBase(parent){
	Selected =0;
	auto skin = MY_UI::Internal::WidgetSkin;
	SetControlBounds(Utilities::Rect(0, 0, 100, 100));
	Selectable = true;
	TextAlignment= CENTER;
	
	SetClientBounds(Utilities::Rect(0, 0, 100, 100));

}

void MY_UI::Controls::List::Update(){
	if(!Internal_Widget.Needs_Update) return;
	if(ItemList.empty()) return;
	int maxwidth = GetClientBounds().width;
	for(size_t i(1); i< ItemList.size(); i++){// get the largest width and set that as the width to keep things uniform
		maxwidth = max(ItemList[i]->GetLength(), maxwidth);
	}
	ItemList[0]->SetPos(0, 0);
	ItemList[0]->SetLength(maxwidth);
	ItemList[0]->Text->Align_Within_Parent(TextAlignment);
	for(size_t i(1); i< ItemList.size(); i++){
		if(ItemList[i]->GetFocus()) ItemList[i]->Show_Selected_Texture();
		ItemList[i]->SetLength(maxwidth);
		ItemList[i]->Text->Align_Within_Parent(TextAlignment);
		ItemList[i]->Align_To_Widget(ItemList[i-1], BOTTOM | IN_LINE);
	}
	Internal_Widget.Needs_Update=false;
}
void MY_UI::Controls::List::SetSelected(cWidgetBase* widget){

	if(!Selectable) return;// do not allow of selecting is turned off
	auto skin = MY_UI::Internal::WidgetSkin;
	if(Selected != 0) {	
		Selected->Set_Hovered_Texture(skin->WindowBar_With_Focus);
		Selected->Set_UnPressed_Texture(skin->White_Box);
		Selected->Set_Selected_Texture(skin->WindowBar_With_Focus);
		Selected->Show_UnPressed_Texture();
		if(Selected == widget){// unselect the button
			Selected=0;
			return;
		} 
	}
	Selected = static_cast<MY_UI::Controls::Button*>(widget);
	Selected->Set_Texture(skin->WindowBar_With_Focus);
	OnSelected.Call(this);
}
void MY_UI::Controls::List::DeCouple(cWidgetBase* widget){// called when a menu item is destroyed. . . .
	MY_UI::Controls::cWidgetBase::DeCouple(widget);
	for(size_t i(0); i< ItemList.size(); i++){
		if(ItemList[i] == widget) {
			ItemList.erase(ItemList.begin() + i);
		}
	}
	Internal_Widget.Needs_Update = true;
	Update();
}

MY_UI::Controls::Button* MY_UI::Controls::List::Insert(std::string text){
	auto skin = MY_UI::Internal::WidgetSkin;
	MY_UI::Controls::Button* b = new MY_UI::Controls::Button(this);
	b->Set_Hovered_Texture(skin->WindowBar_With_Focus);
	b->Set_UnPressed_Texture(skin->White_Box);
	b->Set_Selected_Texture(skin->WindowBar_With_Focus);
	b->SetImageBorderSize(2, 2);
	b->SetText(text, TextAlignment);
	b->Text->SetSizeToContents();
	b->SetSizeToContents();
	b->OnLeftClicked.Add(std::bind(&MY_UI::Controls::List::SetSelected, this, std::placeholders::_1), this, true);
	ItemList.push_back(b);
	Internal_Widget.Needs_Update = true;
	Update();
	return b;
}

bool MY_UI::Controls::List::Erase(std::string text){
	for(size_t i(0); i< ItemList.size(); i++){
		if(ItemList[i]->GetName() == text) {
			ItemList.erase(ItemList.begin() + i);// remove the item first because DeCouple will also call delete 
			delete ItemList[i];
			return true;
		}
	}
	return false;
}
bool MY_UI::Controls::List::Erase(MY_UI::Controls::Button* widget){
	for(size_t i(0); i< ItemList.size(); i++){
		if(ItemList[i] == widget) {
			ItemList.erase(ItemList.begin() + i);
			delete ItemList[i];
			SetSizeToContents();
			return true;
		}
	}
	return false;
}
void MY_UI::Controls::List::EraseAll(){
	for(size_t i(0); i< ItemList.size(); i++){
		for(auto j(Internal_Widget.Children.begin()); j !=Internal_Widget.Children.end(); j++) {
			if(*j == ItemList[i]){
				Internal_Widget.Children.erase(j);
				break;
			}
		}
		MY_UI::Internal::RootWidget->DelayedDelete(ItemList[i]);
	}
	ItemList.clear();
	Selected =0;
}





MY_UI::Controls::List_Box::List_Box(MY_UI::Controls::cWidgetBase* parent): List(parent){

	auto skin = MY_UI::Internal::WidgetSkin;
	BackGround = new MY_UI::Controls::Image(this);
	BackGround->Set_Texture(skin->ListBox);
	BackGround->SetImageBorderSize(2, 2);

	SetClientBounds(Utilities::Rect(4, 6, 92, 90));
	SetSize(static_cast<int>(skin->ListBox.width), static_cast<int>(skin->ListBox.height));
	DeCouple(BackGround);
}
MY_UI::Controls::List_Box::~List_Box(){
	delete BackGround;
}
void MY_UI::Controls::List_Box::SetPos(int x, int y){
	Internal_Widget.Children.push_back(BackGround);
	MY_UI::Controls::cWidgetBase::SetPos(x, y);
	Internal_Widget.Children.pop_back();
}
void MY_UI::Controls::List_Box::SetSize(int x, int y){
	MY_UI::Controls::cWidgetBase::SetSize(x, y);
	BackGround->SetSize(x, y);	
	Internal_Widget.Needs_Update = true;
	Update();
}
void MY_UI::Controls::List_Box::Draw_Clip(){
	BackGround->Draw_Clip();
	List::Draw_Clip();
}
void MY_UI::Controls::List_Box::Draw_NoClip(){
	BackGround->Draw_NoClip();
	List::Draw_NoClip();
}


/////////////////////////////////////////////////////////////////////////////////////     List_Box_wScrollbars
MY_UI::Controls::List_Box_wScrollbars::List_Box_wScrollbars(cWidgetBase* parent): Scrollable(parent) {
	List = new MY_UI::Controls::List(this);
	auto skin = MY_UI::Internal::WidgetSkin;
	BackGround = new MY_UI::Controls::Image(this);
	BackGround->Set_Texture(skin->ListBox);
	BackGround->SetImageBorderSize(2, 2);
	BackGround->SetPos(-2, -2);
	DeCouple(BackGround);// dont want the background selectable
}
MY_UI::Controls::List_Box_wScrollbars::~List_Box_wScrollbars(){
	delete BackGround;
}
void MY_UI::Controls::List_Box_wScrollbars::SetSize(int x, int y){
	Scrollable::SetSize(x-4, y-4);
	auto skin = MY_UI::Internal::WidgetSkin;
	List->SetLength(x - static_cast<int>(skin->Right_Arrow_Button.width)-4);
	BackGround->SetSize(x, y);
}
void MY_UI::Controls::List_Box_wScrollbars::SetPos(int x, int y){
	Internal_Widget.Children.push_back(BackGround);
	MY_UI::Controls::cWidgetBase::SetPos(x, y);
	Internal_Widget.Children.pop_back();
}
void MY_UI::Controls::List_Box_wScrollbars::Draw_Clip(){
	BackGround->Draw_NoClip();
	Scrollable::Draw_Clip();
	
}
void MY_UI::Controls::List_Box_wScrollbars::Draw_NoClip(){
	BackGround->Draw_NoClip();
	Scrollable::Draw_NoClip();
}
MY_UI::Controls::Text_Window::Text_Window(cWidgetBase* parent): List_Box_wScrollbars(parent) {
	List->SetSelectable(false);
	List->SetTextAlignment(LEFT);
}

MY_UI::Controls::Button* MY_UI::Controls::Text_Window::Insert(std::string text){
	
	auto skin = MY_UI::Internal::WidgetSkin;
	MY_UI::Controls::Button* b=  List->Insert(text);
	b->ClearImages();// make sure to clear them all!!
	b->Set_Texture(skin->White_Box);	
	b->Text->WrapText(List->GetLength());
	b->Text->SetSizeToContents();
	b->SetSizeToContents();
	List->Internal_Widget.Needs_Update = true;
	List->Update();
	Internal_Widget.Needs_Update= true;  
	List->SetSizeToContents();
	return b;
}
