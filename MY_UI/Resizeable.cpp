#include "PCH.h"
#include "Resizable.h"
#include "Button.h"
#include "cWidgetSkin.h"

MY_UI::Controls::Resizable::Resizable(cWidgetBase* parent) : cWidgetBase(parent) {
	auto skin = MY_UI::Internal::WidgetSkin;
	Internal_Resizable.TopLeft = new Controls::Button(this);
	Internal_Resizable.TopLeft->SetSize(RESIZE_BORDER, RESIZE_BORDER);
	Internal_Resizable.TopLeft->OnHoverEnter.Add(std::bind(&MY_UI::Controls::Resizable::StartedResizing, this, std::placeholders::_1), this, true);
	Internal_Resizable.TopLeft->OnHoverExit.Add(std::bind(&MY_UI::Controls::Resizable::FinishedResizing, this, std::placeholders::_1), this, true);
	Internal_Resizable.TopLeft->SetId(Utilities::Tl_to_Br);
	Internal_Resizable.TopLeft->SetName("Top Left");
	Internal_Resizable.TopLeft->SetDraggable(true);

	Internal_Resizable.Top = new Button(this);
	Internal_Resizable.Top->OnHoverEnter.Add(std::bind( &MY_UI::Controls::Resizable::StartedResizing, this, std::placeholders::_1), this, true);
	Internal_Resizable.Top->OnHoverExit.Add(std::bind(&MY_UI::Controls::Resizable::FinishedResizing, this, std::placeholders::_1), this, true);
	Internal_Resizable.Top->SetPos(RESIZE_BORDER, 0);
	Internal_Resizable.Top->SetId(Utilities::U_to_D);
	Internal_Resizable.Top->SetName("Top");
	Internal_Resizable.Top->Set_Texture(skin->Horizontal_Slider);
	Internal_Resizable.Top->SetDraggable(true);

	Internal_Resizable.TopRight = new Button(this);
	Internal_Resizable.TopRight->SetSize(RESIZE_BORDER, RESIZE_BORDER);
	Internal_Resizable.TopRight->OnHoverEnter.Add(std::bind(&MY_UI::Controls::Resizable::StartedResizing, this, std::placeholders::_1), this, true);
	Internal_Resizable.TopRight->OnHoverExit.Add(std::bind(&MY_UI::Controls::Resizable::FinishedResizing, this, std::placeholders::_1), this, true);
	Internal_Resizable.TopRight->SetId(Utilities::Tr_to_Bl);
	Internal_Resizable.TopRight->SetName("Top Right");
	Internal_Resizable.TopRight->SetDraggable(true);

	Internal_Resizable.Right = new Button(this);
	Internal_Resizable.Right->OnHoverEnter.Add(std::bind(&MY_UI::Controls::Resizable::StartedResizing, this, std::placeholders::_1), this, true);
	Internal_Resizable.Right->OnHoverExit.Add(std::bind(&MY_UI::Controls::Resizable::FinishedResizing, this, std::placeholders::_1), this, true);
	Internal_Resizable.Right->SetId(Utilities::L_to_R);
	Internal_Resizable.Right->SetName("Right");
	Internal_Resizable.Right->Set_Texture(skin->Vertical_Slider);
	Internal_Resizable.Right->SetDraggable(true);

	Internal_Resizable.BottomRight = new Button(this);
	Internal_Resizable.BottomRight->SetSize(RESIZE_BORDER, RESIZE_BORDER);
	Internal_Resizable.BottomRight->OnHoverEnter.Add(std::bind(&MY_UI::Controls::Resizable::StartedResizing, this, std::placeholders::_1), this, true);
	Internal_Resizable.BottomRight->OnHoverExit.Add(std::bind(&MY_UI::Controls::Resizable::FinishedResizing, this, std::placeholders::_1), this, true);
	Internal_Resizable.BottomRight->SetId(Utilities::Tl_to_Br);
	Internal_Resizable.BottomRight->SetName("Bottom Right");
	Internal_Resizable.BottomRight->SetDraggable(true);

	Internal_Resizable.Bottom = new Button(this);
	Internal_Resizable.Bottom->OnHoverEnter.Add(std::bind(&MY_UI::Controls::Resizable::StartedResizing, this, std::placeholders::_1), this, true);
	Internal_Resizable.Bottom->OnHoverExit.Add(std::bind(&MY_UI::Controls::Resizable::FinishedResizing, this, std::placeholders::_1), this, true);
	Internal_Resizable.Bottom->SetId(Utilities::U_to_D);
	Internal_Resizable.Bottom->SetName("Bottom");
	Internal_Resizable.Bottom->Set_Texture(skin->Horizontal_Slider);
	Internal_Resizable.Bottom->SetDraggable(true);

	Internal_Resizable.BottomLeft = new Button(this);
	Internal_Resizable.BottomLeft->SetSize(RESIZE_BORDER, RESIZE_BORDER);
	Internal_Resizable.BottomLeft->OnHoverEnter.Add(std::bind(&MY_UI::Controls::Resizable::StartedResizing, this, std::placeholders::_1), this, true);
	Internal_Resizable.BottomLeft->OnHoverExit.Add(std::bind(&MY_UI::Controls::Resizable::FinishedResizing, this, std::placeholders::_1), this, true);
	Internal_Resizable.BottomLeft->SetId(Utilities::Tr_to_Bl);
	Internal_Resizable.BottomLeft->SetName("Bottom Left");
	Internal_Resizable.BottomLeft->SetDraggable(true);

	Internal_Resizable.Left = new Button(this);
	Internal_Resizable.Left->OnHoverEnter.Add(std::bind(&MY_UI::Controls::Resizable::StartedResizing, this, std::placeholders::_1), this, true);
	Internal_Resizable.Left->OnHoverExit.Add(std::bind(&MY_UI::Controls::Resizable::FinishedResizing, this, std::placeholders::_1), this, true);
	Internal_Resizable.Left->SetId(Utilities::L_to_R);
	Internal_Resizable.Left->SetName("Left");
	Internal_Resizable.Left->Set_Texture(skin->Vertical_Slider);
	Internal_Resizable.Left->SetDraggable(true);
	SetMinSize(RESIZE_BORDER*2, RESIZE_BORDER*2); 
	SetSize(100, 100);
}
void MY_UI::Controls::Resizable::SetSize(int x, int y){
	MY_UI::Controls::cWidgetBase::SetSize(x, y);
	Utilities::Point p = GetSize();
	x = p.x;
	y = p.y;
	Internal_Resizable.Top->SetSize(x - RESIZE_BORDER*2, RESIZE_BORDER);
	Internal_Resizable.TopRight->SetPos(x - RESIZE_BORDER, 0);

	Internal_Resizable.Right->SetPos(x - RESIZE_BORDER , RESIZE_BORDER);
	Internal_Resizable.Right->SetSize(RESIZE_BORDER, y -RESIZE_BORDER*2);

	Internal_Resizable.BottomRight->SetPos(x - RESIZE_BORDER , y - RESIZE_BORDER);

	Internal_Resizable.Bottom->SetPos(RESIZE_BORDER , y - RESIZE_BORDER);
	Internal_Resizable.Bottom->SetSize(x -RESIZE_BORDER*2, RESIZE_BORDER);

	Internal_Resizable.BottomLeft->SetPos(0, y - RESIZE_BORDER);
	Internal_Resizable.BottomLeft->SetSize(RESIZE_BORDER, RESIZE_BORDER);

	Internal_Resizable.Left->SetPos(0, RESIZE_BORDER);
	Internal_Resizable.Left->SetSize(RESIZE_BORDER, y-RESIZE_BORDER*2);

}


MY_UI::Controls::cWidgetBase* MY_UI::Controls::Resizable::Hit(){
	MY_UI::Controls::cWidgetBase* controlhit = MY_UI::Controls::cWidgetBase::Hit();
	if(controlhit == this) return 0;
	return controlhit;
}
MY_UI::Controls::cWidgetBase* MY_UI::Controls::Resizable::Hit_And_SetFocus(){
	MY_UI::Controls::cWidgetBase* controlhit = MY_UI::Controls::cWidgetBase::Hit_And_SetFocus();
	if(controlhit == this) return 0;
	return controlhit;
}
void MY_UI::Controls::Resizable::Resizing(cWidgetBase* widget){
	unsigned int t = widget->GetId();	
	Utilities::Point size = GetSize();
	Utilities::Point pos = GetPos();

	int deltax = New_MousePosx - Old_MousePosx;
	int deltay = New_MousePosy - Old_MousePosy;
	switch(t){	
	case(Utilities::Tr_to_Bl):// this is top right to bottom left
		if(widget == Internal_Resizable.BottomLeft){// this means the bottom control is being draggged!!!
			SetPos(pos.x+ deltax, pos.y);
			SetSize(size.x - deltax, size.y + deltay);
		} else {
			SetPos(pos.x, pos.y + deltay);
			SetSize(size.x + deltax, size.y - deltay);// otherwise, its the top right .. which doesnt need a new pos
		}
		break;		
	case(Utilities::U_to_D):// this is top to bottom 
		if(widget == Internal_Resizable.Top){// this means the top control is being draggged!!!
			SetPos(pos.x, pos.y + deltay);
			SetSize(size.x, size.y - deltay);
		} else SetSize(size.x, size.y + deltay);// otherwise, its the bottom .. which doesnt need a new pos
			
		break;	
	case(Utilities::Tl_to_Br):// this is top left to bottom right
		if(widget == Internal_Resizable.TopLeft){// this means the top left control is being draggged!!!
			SetPos(pos.x + deltax, pos.y + deltay);
			SetSize(size.x - deltax, size.y - deltay);
		} else SetSize(size.x + deltax, size.y + deltay);// otherwise, its the bottom right.. which doesnt need a new pos
			
		break;
	case(Utilities::L_to_R):// this is left to right
		if(widget == Internal_Resizable.Left){// this means the left control is being draggged!!!
			SetPos(pos.x + deltax, pos.y);
			SetSize(size.x - deltax, size.y);
		} else SetSize(size.x + deltax, size.y);// otherwise, its the bottom .. which doesnt need a new pos
		break;
	};
}
void MY_UI::Controls::Resizable::StartedResizing(cWidgetBase* widget){
	Utilities::SetCursor(widget->GetId());
}
void MY_UI::Controls::Resizable::FinishedResizing(cWidgetBase* widget){
	Utilities::SetCursor(Utilities::Standard);
}