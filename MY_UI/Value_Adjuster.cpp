#include "PCH.h"
#include "Value_Adjuster.h"
#include "Image.h"
#include "cWidgetRenderer.h"
#include "cWidgetSkin.h"

MY_UI::Controls::Value_Adjuster::Value_Adjuster(cWidgetBase* parent): cWidgetBase(parent) {
	SetControlBounds(Utilities::Rect(0, 0, 100, 100));
	SetClientBounds(Utilities::Rect(0, 0, 100, 100));
	SetDraggable(true);
	SetName("Value_Adjuster");
	LastMovementCall=0;
	MaxChangesPerSecond=100;
	MY_UI::Internal::RootWidget->CheckForUpdate(this);
	
}
void MY_UI::Controls::Value_Adjuster::Draw_Clip(){
	auto skin = MY_UI::Internal::WidgetSkin; 
	Utilities::Point size = GetSize();
	Utilities::Point end(size.x, 0);
	Utilities::Point beg(size);
	beg /=2;
	Utilities::Rect& rect = Internal_Widget.AbsolutePosition;
	end.x += rect.left;
	end.y += rect.top;

	beg.x += rect.left;
	beg.y += rect.top;
	if(New_MousePosx < beg.x){
		end.x = rect.left;
	}
	MY_UI::Internal::WidgetRenderer->DrawLine_NoClip(skin->White_Box, beg, end, 4, MY_UI::Utilities::Red, MY_UI::Utilities::Red);
	end.y = size.y +  rect.top;
	MY_UI::Internal::WidgetRenderer->DrawLine_NoClip(skin->White_Box, beg, end, 4, MY_UI::Utilities::Red, MY_UI::Utilities::Red);
	end.x = New_MousePosx;
	end.y = New_MousePosy;
	MY_UI::Internal::WidgetRenderer->DrawLine_NoClip(skin->White_Box, beg, end, 4, MY_UI::Utilities::Green, MY_UI::Utilities::Green);
}
void MY_UI::Controls::Value_Adjuster::Draw_NoClip(){
	Draw_Clip();
}
void MY_UI::Controls::Value_Adjuster::Close(){
	DeCouple(this);// unattach this widget from a parent if there is any
	MY_UI::Internal::RootWidget->DelayedDelete(this);// add this to a delayed delete in case this close is called internally
	//Delaying the deletion of components is very important because it could be done by the user, or internally so I want the object to
	//exist through the entire loop in case any components need it

}
void MY_UI::Controls::Value_Adjuster::LeftMouseButtonUp(){
	MY_UI::Controls::cWidgetBase::LeftMouseButtonUp();
	Close();// delete this control on a up
}
void MY_UI::Controls::Value_Adjuster::SetFocus(bool focus){
	MY_UI::Controls::cWidgetBase::SetFocus(focus);
	if(!focus) Close();// delete this control on a lose focus
}
void MY_UI::Controls::Value_Adjuster::Update(){
	if(!Mouse_LButton_Down) Close();// watch for the release of the mouse button. Since this control is different it requires this type of watching
	float height = static_cast<float>(GetHeight());
	if(Old_MousePosx != New_MousePosx || Old_MousePosy != New_MousePosy) Internal::RootWidget->WidgetHasChanged= true;// notify that a change has occured
	float dif = static_cast<float>(New_MousePosy - Internal_Widget.AbsolutePosition.top - GetHeight()/2);
	dif = Utilities::Clamp<float>(dif, -height/2.0f, height/2.0f);
	dif/=height;
	dif *=2.0f;
	unsigned int timebetweencalls = static_cast<unsigned int>((abs(dif) -1.0f) *MaxChangesPerSecond);
	if(timebetweencalls + Utilities::GetTime() > LastMovementCall){
		if(dif < 0.0f ){// increment
			OnIncrease.Call(this);
		}else {
			OnDecrease.Call(this);
		}
		LastMovementCall = Utilities::GetTime();
	}				
}
