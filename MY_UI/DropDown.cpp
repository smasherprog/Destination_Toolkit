#include "PCH.h"
#include "DropDown.h"
#include "Button.h"
#include "cWidgetSkin.h"
#include "Text.h"
#include "Image.h"

MY_UI::Controls::DropDown::DropDown(cWidgetBase* parent): cWidgetBase(parent), Selected(0) {
	auto skin = MY_UI::Internal::WidgetSkin;
	// placeholder is a backup in case there are no objects to draw
	Internal_DropDown.PlaceHolder = new MY_UI::Controls::Button(this);
	Internal_DropDown.PlaceHolder->Set_UnPressed_Texture(skin->White_Box);
	Internal_DropDown.PlaceHolder->Set_Hovered_Texture(skin->WindowBar_With_Focus);
	Internal_DropDown.PlaceHolder->SetText("Empty ", CENTER);
	Internal_DropDown.PlaceHolder->SetImageBorderSize(2, 2);
	DeCouple(Internal_DropDown.PlaceHolder);

	Utilities::Point p =Internal_DropDown.PlaceHolder->GetSize();
	Internal_DropDown.Background = new MY_UI::Controls::Image(0);
	Internal_DropDown.Background->Set_Texture(skin->TextBox);
	Internal_DropDown.Background->SetSize(p.x+4, p.y+4);
	Internal_DropDown.Background->Internal_Widget.Parent = this;
	Internal_DropDown.Background->SetPos(0, 0);
	Internal_DropDown.Background->SetImageBorderSize(2, 2);

	SetControlBounds(Utilities::Rect(0, 0, p.x + 4, p.y + 2));
	SetClientBounds(Utilities::Rect(2, 2, p.x + 6, p.y + 4));	

	Internal_DropDown.DownArrow = new MY_UI::Controls::Button(this);
	Internal_DropDown.DownArrow->Set_UnPressed_Texture(skin->Down_Arrow_Button);
	Internal_DropDown.DownArrow->SetSize(static_cast<int>(skin->Down_Arrow_Button.width), static_cast<int>(skin->Down_Arrow_Button.height));
	Internal_DropDown.DownArrow->SetPos(p.x - static_cast<int>(skin->Down_Arrow_Button.width) - 3, 3);
	Internal_DropDown.DownArrow->OnLeftClicked.Add(std::bind(&MY_UI::Controls::DropDown::ToggleOpened, this));
	Internal_DropDown.DownArrow->SetImageBorderSize(2, 2);
	SetName("Drop Down");
}
MY_UI::Controls::DropDown::~DropDown(){
	delete Internal_DropDown.PlaceHolder;
	delete Internal_DropDown.Background;
}
void MY_UI::Controls::DropDown::CloseMenu(){
	Internal_DropDown.Opened = false;
	if(Selected !=0) Selected->SetPos(0, 0);
}
void MY_UI::Controls::DropDown::ToggleOpened(){
	if(Internal_DropDown.Opened) {// close the control
		CloseMenu();
	} else {// open the control
		Internal_DropDown.Opened = Internal_Widget.Needs_Update= true;
		Update();
	}
}
void MY_UI::Controls::DropDown::SetSize(int x, int y){
	auto skin = MY_UI::Internal::WidgetSkin;
	MY_UI::Controls::cWidgetBase::SetSize(x, y);
	Internal_DropDown.DownArrow->SetPos(x - static_cast<int>(skin->Down_Arrow_Button.width) - 3, 3);
	Internal_DropDown.Background->SetSize(x+4, y+2);
	Internal_Widget.Needs_Update=true;
	Update();
}
void MY_UI::Controls::DropDown::SetPos(int x, int y){
	Internal_Widget.Children.push_back(Internal_DropDown.Background);
	MY_UI::Controls::cWidgetBase::SetPos(x, y);
	Internal_Widget.Children.pop_back();
}
void MY_UI::Controls::DropDown::MouseWheelMoved(){
	if(Internal_DropDown.Opened) return;
	if(List.empty()) return;// nothing to do, at the end of the list
	size_t index=-1;
	for(size_t i(0); i< List.size(); i++){
		if(Selected == List[i]){
			index=i;
			break;
		}
	}
	if(index == -1) return;
	if(Mouse_Wheel_Delta > 0 ){
		index = Utilities::Clamp<size_t>(index -1, 0, List.size() -1);
	} else if(Mouse_Wheel_Delta < 0 ){
		index = Utilities::Clamp<size_t>(index +1, 0, List.size() -1);
	}
	Selected = List[index];
	if(Selected != 0) Selected->SetPos(0, 0);
}

void MY_UI::Controls::DropDown::LeftMouseButtonUp(){
	ToggleOpened();
	MY_UI::Controls::cWidgetBase::LeftMouseButtonUp();
}
void MY_UI::Controls::DropDown::SetFocus(bool focus){
	cWidgetBase::SetFocus(focus);
	
	if(!focus){// if this is loosing focus
		CloseMenu();
	}
}
MY_UI::Controls::cWidgetBase* MY_UI::Controls::DropDown::Hit_And_SetFocus(){
	return MY_UI::Controls::DropDown::Hit();
}
MY_UI::Controls::cWidgetBase* MY_UI::Controls::DropDown::Hit(){
	const Utilities::Rect& r = Internal_Widget.AbsolutePosition;
	if(!Internal_DropDown.Opened){ // the control is closed
		if((New_MousePosx > r.left) & (New_MousePosx < r.left + r.width) & (New_MousePosy > r.top) & (New_MousePosy < r.top + r.height)){// only check within our bounds		
			return this;
		}
		return 0;
	} // otherwise the control is open, check all children
	return MY_UI::Controls::cWidgetBase::Hit();
}

void MY_UI::Controls::DropDown::KeyUp(){
	if(Internal_DropDown.Opened) return;
	if(List.empty()) return;// nothing to do, at the end of the list
	size_t index=-1;
	for(size_t i(0); i< List.size(); i++){
		if(Selected == List[i]){
			index=i;
			break;
		}
	}
	if(index == -1) return;
	if(Current_Key == KEY_UP){
		index = Utilities::Clamp<size_t>(index -1, 0, List.size() -1);
	} else if(Current_Key == KEY_DOWN){
		index = Utilities::Clamp<size_t>(index +1, 0, List.size() -1);
	}
	Selected = List[index];
	if(Selected != 0) Selected->SetPos(0, 0);
}

void MY_UI::Controls::DropDown::Update(){
	if((!Internal_Widget.Needs_Update) | List.empty()) return;
	Utilities::Point size =GetSize();
	List[0]->SetPos(0, 0);
	List[0]->SetSize(size.x, size.y);
	List[0]->Text->Align_Within_Parent(CENTER);
	for(size_t i(1); i< List.size(); i++){
		if(List[i]->GetFocus()) List[i]->Show_Selected_Texture();
		List[i]->Align_To_Widget(List[i-1], BOTTOM | IN_LINE);
		List[i]->SetSize(size.x, size.y);
		List[i]->Text->Align_Within_Parent(CENTER);
	}
	if((Selected !=0) & (!Internal_DropDown.Opened)){
		Selected->SetPos(0, 0);
	}
	Internal_Widget.Needs_Update=false;
}
void MY_UI::Controls::DropDown::SetSelected(cWidgetBase* but){
	Selected = static_cast<MY_UI::Controls::Button*>(but);
	if(Internal_DropDown.Link != 0 ) *Internal_DropDown.Link = Selected->GetId();
	if(Internal_DropDown.Opened) {// close the control
		if(!Selected->Hit()) Selected->Show_UnPressed_Texture();// prevent the hovered texture from sticking if the drop down closes
		ToggleOpened();
	} else {// not opened, user setting externally
		Internal_Widget.Needs_Update= true;
		Update();
	}		
	OnLeftClicked.Call(this);
}

void MY_UI::Controls::DropDown::Draw_Clip(){
	Draw_NoClip();// drop downs should never be clipped
}
void MY_UI::Controls::DropDown::Draw_NoClip(){
	if(Internal_Widget.Hidden) return;
	if(Internal_DropDown.Opened){
		for(size_t i(0); i < Internal_Widget.Children.size() ; i++) {
			Internal_Widget.Children[i]->Draw_NoClip(); 
		}
		Internal_DropDown.DownArrow->Draw_NoClip();
	} else {		
		Internal_DropDown.Background->Draw_NoClip();
		if(Selected != 0) Selected->Draw_NoClip();
		else {
			Internal_DropDown.PlaceHolder->SetPos(0, 0);
			Internal_DropDown.PlaceHolder->Draw_NoClip();	
		}
		Internal_DropDown.DownArrow->Draw_NoClip();	
	}
}
	
MY_UI::Controls::Button* MY_UI::Controls::DropDown::Insert(std::string text, unsigned int id){
	auto skin = MY_UI::Internal::WidgetSkin;
	MY_UI::Controls::Button* b = new MY_UI::Controls::Button(this);
	b->SetId(id);
	b->Set_Hovered_Texture(skin->WindowBar_With_Focus);
	b->Set_UnPressed_Texture(skin->White_Box);
	b->SetImageBorderSize(2, 2);
	b->Text->SetText(text);
	b->Text->SetSizeToContents();
	Utilities::Point p = GetSize();
	b->SetSize(p.left, p.top);
	b->SetClientBounds(Utilities::Rect(0, 0, p.left, p.top));
	b->Text->Align_Within_Parent(CENTER);
	b->OnLeftClicked.Add(std::bind(&MY_UI::Controls::DropDown::SetSelected, this, std::placeholders::_1), this, true);
	b->OnKeyUp.Add(std::bind(&MY_UI::Controls::DropDown::KeyUp, this), this);
	b->OnMouseWheelMoved.Add(std::bind(&MY_UI::Controls::DropDown::MouseWheelMoved, this), this);
	List.push_back(b);
	Internal_Widget.Needs_Update = true;
	if(Selected == 0) Selected = b;
	Update();
	return b;
}
bool MY_UI::Controls::DropDown::Erase(std::string text){
	for(size_t i(0); i< List.size(); i++){
		if(List[i]->GetName() == text) {
			List.erase(List.begin() + i);// remove the item first because DeCouple will also call delete 
			delete List[i];
			return true;
		}
	}
	return false;
}
bool MY_UI::Controls::DropDown::Erase(MY_UI::Controls::Button* widget){
	for(size_t i(0); i< List.size(); i++){
		if(List[i] == widget) {
			List.erase(List.begin() + i);
			delete List[i];
			return true;
		}
	}
	return false;
}
void MY_UI::Controls::DropDown::SetSizeToContents(){
	auto skin = MY_UI::Internal::WidgetSkin;
	int height =0;
	int width=0;
	for(size_t i=0; i< Internal_Widget.Children.size(); i++){
		MY_UI::Utilities::Point p = Internal_Widget.Children[i]->GetSize();
		height = max(p.y, height);
		width = max(p.x, width);
	}
	SetSize(width + static_cast<int>(skin->Down_Arrow_Button.width) + 2, height);
}

void MY_UI::Controls::DropDown::DeCouple(cWidgetBase* widget){// called when a menu item is destroyed. . . .
	MY_UI::Controls::cWidgetBase::DeCouple(widget);
	for(size_t i(0); i< List.size(); i++){
		if(List[i] == widget) {
			auto itr = List.erase(List.begin() + i);
			if(widget == Selected) {// the selected widget is being removed, try to set the next to be selected
				if(itr == List.end()){// at the end of the list, try going back
					if(!List.empty()){// there is at least one to select from
						Selected = *(itr -1);
					} else Selected =0;
				} else Selected = *(itr -1);
			}
		}
	}

	Internal_Widget.Needs_Update = true;
	Update();
}