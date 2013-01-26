#include "PCH.h"
#include "Scrollable.h"
#include "Button.h"
#include "cWidgetSkin.h"
#include "Resizable.h"

MY_UI::Controls::ScrollBars::ScrollBars(cWidgetBase* parent) :cWidgetBase(parent) {
	auto skin = MY_UI::Internal::WidgetSkin;
	
	Internal_Scrollable.TR_Up_Button = new Button(this);
	Internal_Scrollable.TR_Up_Button->ClearImages();
	Internal_Scrollable.TR_Up_Button->Set_UnPressed_Texture(skin->Up_Arrow_Button);// take a copy of the UI skin for the texture
	Internal_Scrollable.TR_Up_Button->Set_Pressed_Texture(skin->Up_Arrow_Button_Pressed);
	Internal_Scrollable.TR_Up_Button->SetSize(static_cast<int>(skin->Up_Arrow_Button.width), static_cast<int>(skin->Up_Arrow_Button.height));
	Internal_Scrollable.TR_Up_Button->SetImageBorderSize(2, 2);

	Internal_Scrollable.BR_Down_Button = new Button(this);
	Internal_Scrollable.BR_Down_Button->ClearImages();
	Internal_Scrollable.BR_Down_Button->Set_UnPressed_Texture(skin->Down_Arrow_Button);// take a copy of the UI skin for the texture
	Internal_Scrollable.BR_Down_Button->Set_Pressed_Texture(skin->Down_Arrow_Button_Pressed);
	Internal_Scrollable.BR_Down_Button->SetSize(static_cast<int>(skin->Down_Arrow_Button.width), static_cast<int>(skin->Down_Arrow_Button.height));
	Internal_Scrollable.BR_Down_Button->SetImageBorderSize(2, 2);

	Internal_Scrollable.Right_Scroller = new Button(this);
	Internal_Scrollable.Right_Scroller->Set_Texture(skin->Vertical_Scrollbar);
	Internal_Scrollable.Right_Scroller->SetSize(static_cast<int>(skin->Vertical_Scrollbar.width), static_cast<int>(skin->Vertical_Scrollbar.height));
	Internal_Scrollable.Right_Scroller->SetImageBorderSize(2, 2);
	Internal_Scrollable.Right_Scroller->SetDraggable(true);

	Internal_Scrollable.Right_Scroller_Background = new Button(this);
	Internal_Scrollable.Right_Scroller_Background->Set_Texture(skin->Vertical_Scrollbar_Background);
	Internal_Scrollable.Right_Scroller_Background->SetSize(static_cast<int>(skin->Vertical_Scrollbar_Background.width), static_cast<int>(skin->Vertical_Scrollbar_Background.height));
	
	Internal_Scrollable.BR_Right_Button = new Button(this);
	Internal_Scrollable.BR_Right_Button->ClearImages();
	Internal_Scrollable.BR_Right_Button->Set_UnPressed_Texture(skin->Right_Arrow_Button);// take a copy of the UI skin for the texture
	Internal_Scrollable.BR_Right_Button->Set_Pressed_Texture(skin->Right_Arrow_Button_Pressed);
	Internal_Scrollable.BR_Right_Button->SetSize(static_cast<int>(skin->Right_Arrow_Button.width), static_cast<int>(skin->Right_Arrow_Button.height));
	Internal_Scrollable.BR_Right_Button->SetImageBorderSize(2, 2);

	Internal_Scrollable.BL_Left_Button = new Button(this);
	Internal_Scrollable.BL_Left_Button->ClearImages();
	Internal_Scrollable.BL_Left_Button->Set_UnPressed_Texture(skin->Left_Arrow_Button);// take a copy of the UI skin for the texture
	Internal_Scrollable.BL_Left_Button->Set_Pressed_Texture(skin->Left_Arrow_Button_Pressed);
	Internal_Scrollable.BL_Left_Button->SetSize(static_cast<int>(skin->Left_Arrow_Button.width), static_cast<int>(skin->Left_Arrow_Button.height));
	Internal_Scrollable.BL_Left_Button->SetImageBorderSize(2, 2);

	Internal_Scrollable.Bottom_Scroller = new Button(this);
	Internal_Scrollable.Bottom_Scroller->Set_Texture(skin->Horizontal_Scrollbar);
	Internal_Scrollable.Bottom_Scroller->SetSize(static_cast<int>(skin->Horizontal_Scrollbar.width), static_cast<int>(skin->Horizontal_Scrollbar.height));
	Internal_Scrollable.Bottom_Scroller->SetImageBorderSize(2, 2);
	Internal_Scrollable.Bottom_Scroller->SetDraggable(true);

	Internal_Scrollable.Bottom_Scroller_Background = new Button(this);
	Internal_Scrollable.Bottom_Scroller_Background->Set_Texture(skin->Horizontal_Scrollbar_Background);
	Internal_Scrollable.Bottom_Scroller_Background->SetSize(static_cast<int>(skin->Horizontal_Scrollbar_Background.width), static_cast<int>(skin->Horizontal_Scrollbar_Background.height));

	Internal_Scrollable.BR_Corner = new Button(this);
	Internal_Scrollable.BR_Corner->Set_Texture(skin->Grey_Box);
	Internal_Scrollable.BR_Corner->SetSize(static_cast<int>(skin->Grey_Box.width), static_cast<int>(skin->Grey_Box.height));
	Internal_Scrollable.BR_Corner->SetImageBorderSize(2, 2);

	SetControlBounds(Utilities::Rect(0, 0, 100, 100));
	SetClientBounds(Utilities::Rect(0, 0, 100, 100));
	SetSize(100, 100);
	SetPos(0, 0);
}
void MY_UI::Controls::ScrollBars::SetSize(int x, int y){
	MY_UI::Controls::cWidgetBase::SetSize(x, y);
	Update();
}

MY_UI::Controls::cWidgetBase* MY_UI::Controls::ScrollBars::Hit(){
	MY_UI::Controls::cWidgetBase* controlhit = MY_UI::Controls::cWidgetBase::Hit();
	if(controlhit == this) return 0;
	return controlhit;
}
MY_UI::Controls::cWidgetBase* MY_UI::Controls::ScrollBars::Hit_And_SetFocus(){
	MY_UI::Controls::cWidgetBase* controlhit = MY_UI::Controls::cWidgetBase::Hit();
	if(controlhit == this) return 0;
	return controlhit;
}
void MY_UI::Controls::ScrollBars::Update(){
	auto skin = MY_UI::Internal::WidgetSkin;
	Utilities::Point p = GetSize();

	/////////////////////////////Vertical Scrollbar first
	// if the amount showing is greater than the area of the scrollers, hide the scrollers
	Internal_Scrollable.BR_Corner->SetPos(p.x - static_cast<int>(skin->Down_Arrow_Button.height), p.y - static_cast<int>(skin->Down_Arrow_Button.height));

	if(p.y >= Internal_Scrollable.Size.top ) {
		Internal_Scrollable.TR_Up_Button->SetHidden(true);
		Internal_Scrollable.BR_Down_Button->SetHidden(true);
		Internal_Scrollable.Right_Scroller_Background->SetHidden(true);
		Internal_Scrollable.Right_Scroller->SetHidden(true);

	} else {//otherwise, do not hide the controls and place correctly
		Internal_Scrollable.TR_Up_Button->SetHidden(false);
		Internal_Scrollable.BR_Down_Button->SetHidden(false);
		Internal_Scrollable.Right_Scroller_Background->SetHidden(false);
		Internal_Scrollable.Right_Scroller->SetHidden(false);

		int topright_x_pos = p.x - static_cast<int>(skin->Down_Arrow_Button.height);
		Internal_Scrollable.TR_Up_Button->SetPos(topright_x_pos, 0);
		Internal_Scrollable.BR_Down_Button->SetPos(topright_x_pos, p.y - static_cast<int>(skin->Down_Arrow_Button.height)*2);
		Internal_Scrollable.Right_Scroller_Background->SetPos(topright_x_pos, static_cast<int>(skin->Down_Arrow_Button.height));
		Internal_Scrollable.Right_Scroller_Background->SetSize(static_cast<int>(skin->Vertical_Scrollbar_Background.width), p.y  - static_cast<int>(skin->Down_Arrow_Button.height*3));
		
		
		float topofscrollablearea = static_cast<float>(Internal_Scrollable.Size.top - p.y);
		float total_avaiable_scrolbarsize = static_cast<float>(p.y) - skin->Up_Arrow_Button.height*3.0f;

		float ysize = total_avaiable_scrolbarsize * static_cast<float>(p.y) / static_cast<float>(Internal_Scrollable.Size.top);
		ysize = max(ysize, 5.0f);// set the min size to 5
		Internal_Scrollable.Right_Scroller->SetSize(static_cast<int>(skin->Vertical_Scrollbar.width), static_cast<int>(ysize));

		float tbratio;
		if(Internal_Scrollable.Position.top ==0) tbratio =0;
		else  tbratio =  (abs(static_cast<float>(Internal_Scrollable.Position.top)) / static_cast<float>(Internal_Scrollable.Size.top) )* total_avaiable_scrolbarsize;
		Internal_Scrollable.Right_Scroller->SetPos(topright_x_pos, static_cast<int>(tbratio +skin->Up_Arrow_Button.height) );
		
	}


	////////////////horizontal scroller info

	if(p.x>= Internal_Scrollable.Size.left) {// no need for the scrollers,
		Internal_Scrollable.BR_Right_Button->SetHidden(true);
		Internal_Scrollable.BL_Left_Button->SetHidden(true);
		Internal_Scrollable.Bottom_Scroller->SetHidden(true);
		Internal_Scrollable.Bottom_Scroller_Background->SetHidden(true);
		
	} else {
		Internal_Scrollable.BR_Right_Button->SetHidden(false);
		Internal_Scrollable.BL_Left_Button->SetHidden(false);
		Internal_Scrollable.Bottom_Scroller->SetHidden(false);
		Internal_Scrollable.Bottom_Scroller_Background->SetHidden(false);
	
		int top_y_pos = p.y - static_cast<int>(skin->Right_Arrow_Button.height);
		Internal_Scrollable.BR_Right_Button->SetPos(p.x - static_cast<int>(skin->Up_Arrow_Button.width*2.0f), top_y_pos);
		Internal_Scrollable.BL_Left_Button->SetPos(0, top_y_pos);

		Internal_Scrollable.Bottom_Scroller_Background->SetPos(static_cast<int>(skin->Right_Arrow_Button.width) , top_y_pos);
		Internal_Scrollable.Bottom_Scroller_Background->SetSize( p.x  - static_cast<int>(skin->Down_Arrow_Button.width*3.0f), static_cast<int>(skin->Horizontal_Scrollbar_Background.height));
	
		float topofscrollablearea = static_cast<float>(Internal_Scrollable.Size.left - p.x);
		float total_avaiable_scrolbarsize = static_cast<float>(p.x) - skin->Up_Arrow_Button.width*3.0f;

		float ysize = total_avaiable_scrolbarsize * static_cast<float>(p.x) / static_cast<float>(Internal_Scrollable.Size.left);
		ysize = max(ysize, 5.0f);// set the min size to 5
		Internal_Scrollable.Bottom_Scroller->SetSize(static_cast<int>(ysize), static_cast<int>(skin->Horizontal_Scrollbar_Background.height));
		
		float tbratio;
		if(Internal_Scrollable.Position.left ==0) tbratio =0;
		else  tbratio =  abs(static_cast<float>(Internal_Scrollable.Position.left)) / static_cast<float>(Internal_Scrollable.Size.left) * total_avaiable_scrolbarsize;
		Internal_Scrollable.Bottom_Scroller->SetPos(static_cast<int>(tbratio +skin->Up_Arrow_Button.width), top_y_pos);
	
	}
	// if at least one is showing, drawn the bottom right corner peice
	if(!Internal_Scrollable.BL_Left_Button->GetHidden() | !Internal_Scrollable.Right_Scroller_Background->GetHidden()) {
		Internal_Scrollable.BR_Corner->SetHidden(false);
	} else {// none are showing.. hide 
		Internal_Scrollable.BR_Corner->SetHidden(true);
	}
}



///////////////////////////////////////////////////////////////////Scrollable control


MY_UI::Controls::Scrollable::Scrollable(cWidgetBase* parent) :cWidgetBase(parent) {

	Container = new MY_UI::Controls::cWidgetBase(0);
	// manual coupling
	MY_UI::Controls::cWidgetBase::Couple(Container);
	Container->Internal_Widget.Parent = this;

	// manual coupling
	Scroll_Bars = new ScrollBars(0);
	MY_UI::Controls::cWidgetBase::Couple(Scroll_Bars); 
	Scroll_Bars->Internal_Widget.Parent = this;
	Scroll_Bars->SetPos(0, 0);
	Scroll_Bars->SetSize(100, 100);
	SetScrollableSize(200, 200);
	Container->SetControlBounds(Utilities::Rect(0, 0, 9999, 9999));// the control bounds should always be big enough to hold all items
	Container->SetClientBounds(Utilities::Rect(0, 0, 9999, 9999));// the client bounds should always be big enough to hold all items

	Scroll_Bars->Internal_Scrollable.Right_Scroller->OnLeftDrag.Add(std::bind((void (MY_UI::Controls::Scrollable::*)())&MY_UI::Controls::Scrollable::Scrolling_Y, this));
	Scroll_Bars->Internal_Scrollable.Right_Scroller->OnMouseWheelMoved.Add(std::bind( &MY_UI::Controls::Scrollable::MouseWheelMoved_UpDown, this));
	Scroll_Bars->Internal_Scrollable.Bottom_Scroller->OnLeftDrag.Add(std::bind((void (MY_UI::Controls::Scrollable::*)())&MY_UI::Controls::Scrollable::Scrolling_X, this));
	Scroll_Bars->Internal_Scrollable.Bottom_Scroller->OnMouseWheelMoved.Add(std::bind( &MY_UI::Controls::Scrollable::MouseWheelMoved_LeftRight, this));
	Scroll_Bars->Internal_Scrollable.BL_Left_Button->OnLeftClicked.Add(std::bind( &MY_UI::Controls::Scrollable::Scroll_LeftBy_One, this));
	Scroll_Bars->Internal_Scrollable.BL_Left_Button->OnMouseWheelMoved.Add(std::bind( &MY_UI::Controls::Scrollable::MouseWheelMoved_LeftRight, this));
	Scroll_Bars->Internal_Scrollable.BR_Right_Button->OnLeftClicked.Add(std::bind(&MY_UI::Controls::Scrollable::Scroll_RightBy_One, this));
	Scroll_Bars->Internal_Scrollable.BR_Right_Button->OnMouseWheelMoved.Add(std::bind( &MY_UI::Controls::Scrollable::MouseWheelMoved_LeftRight, this));

	Scroll_Bars->Internal_Scrollable.TR_Up_Button->OnLeftClicked.Add(std::bind(&MY_UI::Controls::Scrollable::Scroll_UpBy_One, this));
	Scroll_Bars->Internal_Scrollable.TR_Up_Button->OnMouseWheelMoved.Add(std::bind( &MY_UI::Controls::Scrollable::MouseWheelMoved, this));
	Scroll_Bars->Internal_Scrollable.BR_Down_Button->OnLeftClicked.Add(std::bind( &MY_UI::Controls::Scrollable::Scroll_DownBy_One, this));
	Scroll_Bars->Internal_Scrollable.BR_Down_Button->OnMouseWheelMoved.Add(std::bind( &MY_UI::Controls::Scrollable::MouseWheelMoved, this));
	Scroll_Bars->Internal_Scrollable.Bottom_Scroller_Background->OnLeftClicked.Add(std::bind(&MY_UI::Controls::Scrollable::Bottom_ScrollerBackGroundHit, this));
	Scroll_Bars->Internal_Scrollable.Bottom_Scroller_Background->OnMouseWheelMoved.Add(std::bind( &MY_UI::Controls::Scrollable::MouseWheelMoved_LeftRight, this));
	Scroll_Bars->Internal_Scrollable.Right_Scroller_Background->OnLeftClicked.Add(std::bind( &MY_UI::Controls::Scrollable::Right_ScrollerBackGroundHit, this));
	Scroll_Bars->Internal_Scrollable.Right_Scroller_Background->OnMouseWheelMoved.Add(std::bind( &MY_UI::Controls::Scrollable::MouseWheelMoved_UpDown, this));

	SetControlBounds(Utilities::Rect(0, 0, 100, 100));
	SetClientBounds(Utilities::Rect(0, 0, 100, 100));
	SetPos(0, 0);
	SetSize(100, 100);
	MY_UI::Internal::RootWidget->CheckForUpdate(this);
}
void MY_UI::Controls::Scrollable::Couple(cWidgetBase* widget){
	Container->Couple(widget);
	Internal_Widget.Needs_Update = true;
}
void MY_UI::Controls::Scrollable::DeCouple(cWidgetBase* widget){
	if((widget != Container) | (widget != Scroll_Bars)){
		Container->DeCouple(widget);
	} else {
		MY_UI::Controls::cWidgetBase::DeCouple(widget);
		Internal_Widget.Needs_Update = true;
	}
}
void MY_UI::Controls::Scrollable::Bottom_ScrollerBackGroundHit(){
	if(New_MousePosx < Scroll_Bars->Internal_Scrollable.Bottom_Scroller->Internal_Widget.AbsolutePosition.left){
		Scroll_LeftBy_OnePage();
	} else Scroll_RightBy_OnePage();
}
void MY_UI::Controls::Scrollable::Right_ScrollerBackGroundHit(){
	if(New_MousePosy < Scroll_Bars->Internal_Scrollable.Right_Scroller->Internal_Widget.AbsolutePosition.top){
		Scroll_UpBy_OnePage();
	} else Scroll_DownBy_OnePage();
}
void MY_UI::Controls::Scrollable::Draw_Clip(){
	if(Internal_Widget.Hidden) return;
	Utilities::Rect& clip = Internal_Widget.AbsolutePosition;// this is the clip bounds of this window
	MY_UI::Internal::WidgetRenderer->StartClip(clip);
	Container->Draw_Clip();
	Scroll_Bars->Draw_Clip();
	MY_UI::Internal::WidgetRenderer->EndClip();
}
void MY_UI::Controls::Scrollable::Draw_NoClip(){
	Draw_Clip();// this control must be sure to clip the children
}
void MY_UI::Controls::Scrollable::SetSize(int x, int y){
	MY_UI::Controls::cWidgetBase::SetSize(x, y);
	Scroll_Bars->SetSize(x, y);
	Internal_Widget.Needs_Update =true;
}


MY_UI::Controls::cWidgetBase* MY_UI::Controls::Scrollable::Hit(){
	if(Internal_Widget.Hidden) return 0;
	const Utilities::Rect& rect = Internal_Widget.AbsolutePosition;

	if((New_MousePosx > rect.left) & (New_MousePosx < rect.left + rect.width) & (New_MousePosy > rect.top) & (New_MousePosy < rect.top + rect.height)){// only check within our bounds		

		MY_UI::Controls::cWidgetBase* ctrnhit = Scroll_Bars->Hit();
		if(ctrnhit != 0) return ctrnhit;	
		std::vector<cWidgetBase*>& children = Container->Internal_Widget.Children;// make sure to only check children that are within the control bounds
		for(size_t i(0); i< children.size(); i++){
			const Utilities::Rect& childrect = children[i]->Internal_Widget.AbsolutePosition;
			Utilities::Point tl(childrect.left, childrect.top);
			Utilities::Point tr(childrect.left + childrect.width, childrect.top);
			Utilities::Point bl(childrect.left, childrect.top + childrect.height);
			Utilities::Point br(childrect.left + childrect.width, childrect.top + childrect.height);
			// if all the points are not within the cliprect, dont draw it
			bool brin = rect.Within(br);
			bool trin = rect.Within(tr);

			bool blin = rect.Within(bl);
			bool tlin = rect.Within(tl);


			ctrnhit = children[i]->Hit();
			if(ctrnhit != 0) return ctrnhit;
			
		}
		// normally, this would be returned, but the scroll control should never be returned, only the scrollbars if they are hit.
		//return this;
	}
	return 0;
}
MY_UI::Controls::cWidgetBase* MY_UI::Controls::Scrollable::Hit_And_SetFocus(){
	if(Internal_Widget.Hidden) return 0;
	const Utilities::Rect& rect = Internal_Widget.AbsolutePosition;
	if((New_MousePosx > rect.left) & (New_MousePosx < rect.left + rect.width) & (New_MousePosy > rect.top) & (New_MousePosy < rect.top + rect.height)){// only check within our bounds		
		MY_UI::Controls::cWidgetBase* ctrnhit = Scroll_Bars->Hit();
		if(ctrnhit != 0) return ctrnhit;	
		std::vector<cWidgetBase*>& children = Container->Internal_Widget.Children;// make sure to only check children that are within the control bounds
		for(size_t i(0); i< children.size(); i++){
			const Utilities::Rect& childrect = children[i]->Internal_Widget.AbsolutePosition;


			Utilities::Point tl(childrect.left, childrect.top);
			Utilities::Point tr(childrect.left + childrect.width, childrect.top);
			Utilities::Point bl(childrect.left, childrect.top + childrect.height);
			Utilities::Point br(childrect.left + childrect.width, childrect.top + childrect.height);
			// if all the points are not within the cliprect, dont draw it
			bool brin = rect.Within(br);
			bool trin = rect.Within(tr);

			bool blin = rect.Within(bl);
			bool tlin = rect.Within(tl);
			ctrnhit = children[i]->Hit();
			if(ctrnhit != 0) {// since this is rewriting the hit and set focus.. need to make sure the correct control gains focus
				MY_UI::Controls::cWidgetBase* otemp = children[i];
				if(i != 0){// if the focus has changed... tell the controls
					children[0]->SetFocus(false);// the widget at the beginning has lost focus.. let it know
					children[i]->SetFocus(true);
					for(int j = (int)i; j != 0; j--){
						children[j] = children[j-1];
					}
					children[0]=otemp;
				} else if(!children[i]->GetFocus()){
					children[i]->SetFocus(true);
				}
				return ctrnhit;// return the hit child
			}
		}
		// normally, this would be returned, but the scroll control should never be returned, only the scrollbars if they are hit.
		//return this;
	}
	return 0;
}
void MY_UI::Controls::Scrollable::Update(){
	if(Internal_Widget.Needs_Update){
		auto skin = MY_UI::Internal::WidgetSkin;
		if(Internal_Widget.Alignment == FILL){
			auto parent = Internal_Widget.Parent;
			if(parent){
				Utilities::Rect r = parent->GetClientBounds();	
				SetPos(0, 0);		
				Utilities::Point psize = GetSize();
				SetSize(r.width, r.height);	
				psize -= GetSize();
				// the scroll control must be adjusted if the control is at the end of its bounds..
				if(Scroll_Bars->AtBottom()) Scrolling_Y(psize.y);
				if(Scroll_Bars->AtRight()) Scrolling_X(psize.x);	
	
			}			
		} 		
		
		Utilities::Rect rec = Container->GetLargestChildBounds();		
		SetScrollableSize(rec.width + static_cast<int>(skin->Down_Arrow_Button.width), rec.height + static_cast<int>(skin->Down_Arrow_Button.height));
		Internal_Widget.Needs_Update=false;
	}
}

void MY_UI::Controls::Scrollable::Scrolling_X(int x){	
	if(GetSize().x >= Scroll_Bars->Internal_Scrollable.Size.x) return;
	Utilities::Point beforepos = GetScrollablePos();
	//OUTPUT_DEBUG_MSG("beforepos"<<beforepos);
	auto skin = MY_UI::Internal::WidgetSkin;
	float ratio = static_cast<float>(Scroll_Bars->Internal_Scrollable.Size.x)/(static_cast<float>(Internal_Widget.Clientpos.width) - skin->Up_Arrow_Button.width*3.0f);
	SetScrollablePos(beforepos.x - static_cast<int>(static_cast<float>(x) * ratio), beforepos.y);
	Utilities::Point afterpos = GetScrollablePos();
	//OUTPUT_DEBUG_MSG("afterpos"<<afterpos<<"  static_cast<int>(static_cast<float>(x) * ratio) "<<static_cast<int>(static_cast<float>(x) * ratio));
	x = afterpos.x -beforepos.x;
	beforepos = Container->GetPos();
	Container->SetPos(beforepos.x + x, beforepos.y);
}
void MY_UI::Controls::Scrollable::Scrolling_Y(int y){

	if(GetSize().y>= Scroll_Bars->Internal_Scrollable.Size.y) return;
	Utilities::Point beforepos = GetScrollablePos();
	auto skin = MY_UI::Internal::WidgetSkin;
	float ratio = static_cast<float>(Scroll_Bars->Internal_Scrollable.Size.y)/(static_cast<float>(Internal_Widget.Clientpos.height) - skin->Up_Arrow_Button.height*3.0f);
	SetScrollablePos(beforepos.x, beforepos.y - static_cast<int>(static_cast<float>(y) * ratio));
	Utilities::Point afterpos = GetScrollablePos();
	y = afterpos.y-beforepos.y;
	beforepos = Container->GetPos();
	Container->SetPos(beforepos.x, beforepos.y + y);

}
void MY_UI::Controls::Scrollable::Scroll_YBy_Page(int pages){ 
	auto skin = MY_UI::Internal::WidgetSkin;
	float invratio = static_cast<float>(Internal_Widget.Clientpos.height - skin->Up_Arrow_Button.height*3.0f)/static_cast<float>(Scroll_Bars->Internal_Scrollable.Size.y);
	int pixelspage = static_cast<int>(invratio * static_cast<float>(GetSize().y * pages));
	Scrolling_Y(pixelspage);
}
void MY_UI::Controls::Scrollable::Scroll_XBy_Page(int pages){ 
	auto skin = MY_UI::Internal::WidgetSkin;
	float invratio = static_cast<float>(Internal_Widget.Clientpos.width - skin->Up_Arrow_Button.width*3.0f)/static_cast<float>(Scroll_Bars->Internal_Scrollable.Size.x);
	int pixelspage = static_cast<int>(invratio * static_cast<float>(GetSize().x * pages));
	Scrolling_X(pixelspage);
}
void  MY_UI::Controls::Scrollable::SetScrollableSize(int x, int y) { 
	auto skin = MY_UI::Internal::WidgetSkin;
	Utilities::Point temp = Scroll_Bars->Internal_Scrollable.Size;
	Scroll_Bars->Internal_Scrollable.Size = MY_UI::Utilities::Point(x, y); 
	temp -=Scroll_Bars->Internal_Scrollable.Size;
	Scroll_Bars->Update();
} 

void  MY_UI::Controls::Scrollable::SetScrollablePos(int x, int y){  
	Utilities::Point size = Scroll_Bars->GetSize();
	x = Utilities::Clamp<int>(x, -Scroll_Bars->Internal_Scrollable.Size.left + size.left, 0);
	y = Utilities::Clamp<int>(y, -Scroll_Bars->Internal_Scrollable.Size.top + size.top, 0);
	Scroll_Bars->Internal_Scrollable.Position = MY_UI::Utilities::Point(x, y); 
	Scroll_Bars->Update();
} 