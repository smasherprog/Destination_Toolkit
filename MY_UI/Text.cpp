#include "PCH.h"
#include "Text.h"
#include "cWidgetRenderer.h"
#include "cWidgetSkin.h"
#include "Image.h"

MY_UI::Controls::Text::Text(cWidgetBase* parent): cWidgetBase(parent) { 
	SetControlBounds(Utilities::Rect(0, 0, 50, 50));
	SetClientBounds(Utilities::Rect(0, 0, 50, 50));
	SetColor(Utilities::Color(255, 0, 0, 0));
	auto skin = Internal::WidgetSkin;
	SetHeight(skin->GetCharacterSize('A', GetFontSize()).y + 5);
}

void MY_UI::Controls::Text::SetSizeToContents(){
	auto skin = Internal::WidgetSkin;
	Utilities::Point p = skin->GetTextSize(Internal_Text.Text, Internal_Text.TextSize);
	SetSize(p.x, p.y);
}

void MY_UI::Controls::Text::Draw_Clip(){
	if(Internal_Text.Text.empty()) return;
	if(Internal_Widget.Selected !=0 )Internal_Widget.Selected->Draw_Clip();
	auto rend = Internal::WidgetRenderer;
	Utilities::Point p(Internal_Widget.AbsolutePosition.left, Internal_Widget.AbsolutePosition.top);
	auto skin = MY_UI::Internal::WidgetSkin;
	int left = Internal_Widget.AbsolutePosition.left;
	float sizescale =Internal_Text.TextSize/TEXT_HEIGHT;
	std::string& st = Internal_Text.Text;
	Utilities::UVs tempuvs;
	for(size_t i(0); i< st.size(); i++){	
		if(st[i] == '\n') {
			p.y += static_cast<int>(sizescale*skin->Keyboard['A'].height + (Internal_Text.TextSize*1.0f/4.0f));// new line.. move down 
			left = Internal_Widget.AbsolutePosition.left;// reset the left coord
			continue;// continue to actual text
		}
		Utilities::Texture& t = skin->Keyboard[st[i]];
		Utilities::Rect r(left, p.y, static_cast<int>(sizescale*t.width), static_cast<int>(sizescale*t.height));
		left += static_cast<int>(sizescale*t.width);
		Utilities::CopyUvs(tempuvs, t);
		rend->DrawTexturedRect_Clip(t, tempuvs, r, Internal_Widget.Color_Top_Left, Internal_Widget.Color_Top_Right, Internal_Widget.Color_Bottom_Left, Internal_Widget.Color_Bottom_Right);
		if(Internal_Text.Bold){
			r.left +=1;
			r.top +=1;
			rend->DrawTexturedRect_Clip(t, tempuvs, r, Internal_Widget.Color_Top_Left, Internal_Widget.Color_Top_Right, Internal_Widget.Color_Bottom_Left, Internal_Widget.Color_Bottom_Right);
		}
	}
}
void MY_UI::Controls::Text::Draw_NoClip(){
	if(Internal_Text.Text.empty()) return;
	if(Internal_Widget.Selected !=0 )Internal_Widget.Selected->Draw_NoClip();
	auto rend = Internal::WidgetRenderer;
	Utilities::Point p(Internal_Widget.AbsolutePosition.left, Internal_Widget.AbsolutePosition.top);
	auto skin = MY_UI::Internal::WidgetSkin;
	int left = Internal_Widget.AbsolutePosition.left;
	float sizescale =Internal_Text.TextSize/TEXT_HEIGHT;
	std::string& st = Internal_Text.Text;
	Utilities::UVs tempuvs;
	for(size_t i(0); i< st.size(); i++){	
		if(st[i] == '\n') {
			p.y += static_cast<int>(sizescale*skin->Keyboard['A'].height + (Internal_Text.TextSize*1.0f/4.0f));// new line.. move down 
			left = Internal_Widget.AbsolutePosition.left;// reset the left coord
			continue;// continue to actual text
		}
		Utilities::Texture& t = skin->Keyboard[st[i]];
		Utilities::Rect r(left, p.y, static_cast<int>(sizescale*t.width), static_cast<int>(sizescale*t.height));
		left += static_cast<int>(sizescale*t.width);
		Utilities::CopyUvs(tempuvs, t);
		rend->DrawTexturedRect_NoClip(t, tempuvs, r, Internal_Widget.Color_Top_Left, Internal_Widget.Color_Top_Right, Internal_Widget.Color_Bottom_Left, Internal_Widget.Color_Bottom_Right);
		if(Internal_Text.Bold){
			r.left +=1;
			r.top +=1;
			rend->DrawTexturedRect_NoClip(t, tempuvs, r, Internal_Widget.Color_Top_Left, Internal_Widget.Color_Top_Right, Internal_Widget.Color_Bottom_Left, Internal_Widget.Color_Bottom_Right);
		}
	}
}
void MY_UI::Controls::Text::SelectAll(){
	if(Internal_Widget.Selected == 0 ) Internal_Widget.Selected = new Controls::Image(this);
	Internal_Widget.Selected->SetSize(GetTextSize());
	Internal_Widget.Selected->SetPos(GetPos());
	Internal_Widget.Selected->SetHidden(false);
	OUTPUT_DEBUG_MSG("selected text"<<GetTextSize()<<GetAbsolutePos());
}
MY_UI::Utilities::Point MY_UI::Controls::Text::GetTextSize(){  return MY_UI::Internal::WidgetSkin->GetTextSize(Internal_Text.Text, Internal_Text.TextSize); }

void MY_UI::Controls::Text::WrapText(int xsize){// wraps the text to the xsize
	std::string& st = Internal_Text.Text;
	auto skin = MY_UI::Internal::WidgetSkin;
	float sizescale =Internal_Text.TextSize/TEXT_HEIGHT;
	int left =0;
	for(size_t i(0); i< st.size(); i++){	
		Utilities::Texture& t = skin->Keyboard[st[i]];
		int temp =  static_cast<int>(sizescale*t.width);
		if(left + temp >=xsize){
			st.insert(st.begin() + i, '\n');// add a new line
			left =0;// reset
		} else left +=temp;
	}

}