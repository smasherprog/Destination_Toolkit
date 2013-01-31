#include "stdafx.h"
#include "UI_WorldSelector.h"
#include "../MY_UI/cWidgetUtilities.h"
#include "../MY_UI/Window.h"
#include "../MY_UI/DropDown.h"
#include "../MY_UI/Text.h"
#include "../MY_UI/TextBox.h"
#include "../MY_UI/ColorPickerWindow.h"
#include "../MY_UI/Objects_3d.h"
#include "../Graphics_Lib/Base_Mesh.h"

#include "../MY_UI/ApplicationLayer.h"
#include "../Utilities/MyMath.h"

UI_WorldSelector::UI_WorldSelector(mat4* camera_view, mat4* camera_proj, std::function<Base_Mesh*(vec3& ray, vec3& origin)> f) : cWidgetBase(MY_UI::Internal::RootWidget){// set the root widget as the parent
	SetControlBounds(MY_UI::Utilities::Rect(0, 0, MAX_WINDOWSIZE, MAX_WINDOWSIZE));
	SetClientBounds(MY_UI::Utilities::Rect(0, 0, MAX_WINDOWSIZE, MAX_WINDOWSIZE));
	SetDraggable(true);
	SetName("UI_WorldSelector");
	OnLeftClicked.Add(std::bind(&UI_WorldSelector::OpenWindow, this));

	Trans_Checkbox=0;
	ObjectTranslation=0;
	ObjectRotator=0;
	ObjectScaler=0;
	Selected=Last_Selected=0;
	View= camera_view;
	Proj=camera_proj;
	HitChecker=f;

}
void UI_WorldSelector::OpenWindow(){
	OUTPUT_DEBUG_MSG("Trying to open new window");
	// get what the target is
	MY_UI::Utilities::Point p= MY_UI::Internal::RootWidget->GetSize();// the root widget is the size of the screen
	vec3 ray, origin;
	UnProject(vec2((float)New_MousePosx, (float)New_MousePosy), *View, *Proj, (float)p.x, (float)p.y, ray, origin);
	//unproject will take the mouse coords in screen space and return a normalize ray and origin
	Selected = HitChecker(ray, origin);

	if(Selected==0) return;
	// something is selected, only open the window if its NOT the same as what was last selected
	if(Selected == Last_Selected) return;
	Last_Selected = Selected;
	MY_UI::Safe_Delete(Window);// make sure to delete the window which will delete all of its children as well. Then recreate it
	Window = new MY_UI::Controls::Simple_Window(this);
	Window->SetPos((rand()% 900), (rand()% 500));
	Window->SetSize(325, 200);
	Window->OnDestroyed.Add([this]() { Window=0; } );// make sure to clean the link on destruction. . .  Lambdas are cool!
	mat4* m =0;
	Selected->GetScaling(&m);

	MY_UI::Controls::cWidget_w_Label<MY_UI::Controls::Vector3_Adjuster<float>>* scaling = new MY_UI::Controls::cWidget_w_Label<MY_UI::Controls::Vector3_Adjuster<float>>(Window);
	scaling->SetLabel("Scaling", TOP | CENTER, 5);
	scaling->Widget->Set_Box_Length(65);
	scaling->Widget->Set_Box_Alignment(BOTTOM);
	scaling->Widget->Set_Box_Increment(.01f);
	scaling->Widget->Set_Box_Links(&m->_11, &m->_22, &m->_33);
	scaling->SetPos(5, 5);// important to setpos after the alignment is set

	MY_UI::Controls::Button* pbutton = new MY_UI::Controls::Button(Window);
	pbutton->SetText("Scaler", CENTER);
	pbutton->OnLeftClicked.Add(std::bind(&UI_WorldSelector::CreateScaler, this));
	pbutton->Align_To_Widget(scaling, BOTTOM | CENTER, 5);

	Selected->GetTranslation(&m);
	MY_UI::Controls::cWidget_w_Label<MY_UI::Controls::Vector3_Adjuster<float>>* position = new MY_UI::Controls::cWidget_w_Label<MY_UI::Controls::Vector3_Adjuster<float>>(Window);
	position->SetLabel("Position", TOP | CENTER, 5);
	position->Widget->Set_Box_Length(65);
	position->Widget->Set_Box_Alignment(BOTTOM);
	position->Widget->Set_Box_Increment(.01f);
	position->Widget->Set_Box_Range(-200.0f, 200.0f);
	position->Widget->Set_Box_Links(&m->_41, &m->_42, &m->_43);
	position->Align_To_Widget(scaling, RIGHT | IN_LINE, 10);

	pbutton = new MY_UI::Controls::Button(Window);
	pbutton->SetText("Translator", CENTER);

	pbutton->OnLeftClicked.Add(std::bind(&UI_WorldSelector::CreateTranslator, this));
	pbutton->Align_To_Widget(position, BOTTOM | CENTER, 5);

	Trans_Checkbox = new MY_UI::Controls::cWidget_w_Label<MY_UI::Controls::CheckBox>(Window);
	Trans_Checkbox->SetLabel("World Space", TOP | CENTER, 5);
	Trans_Checkbox->Align_To_Widget(pbutton, BOTTOM | IN_LINE, 5);
	quat* q=0;
	Selected->GetRotation(&q);
	scaling = new MY_UI::Controls::cWidget_w_Label<MY_UI::Controls::Vector3_Adjuster<float>>(Window);
	scaling->SetLabel("Rotation", TOP | CENTER, 5);
	scaling->Widget->Set_Box_Length(65);
	scaling->Widget->Set_Box_Alignment(BOTTOM);
	scaling->Widget->Set_Box_Increment(.01f);
	scaling->Widget->Set_Box_Links(&q->x, &q->y, &q->z);
	scaling->Widget->Set_Box_Range(-999.0f, 999.0f);
	scaling->Align_To_Widget(position, RIGHT | IN_LINE, 10);

	pbutton = new MY_UI::Controls::Button(Window);
	pbutton->SetText("Rotator", CENTER);

	pbutton->OnLeftClicked.Add(std::bind(&UI_WorldSelector::CreateRotator, this));
	pbutton->Align_To_Widget(scaling, BOTTOM | CENTER, 5);

}
void UI_WorldSelector::MouseMoved(){
	cWidgetBase::MouseMoved();
}
MY_UI::Controls::cWidgetBase* UI_WorldSelector::Hit_And_SetFocus(){
	cWidgetBase* b = cWidgetBase::Hit_And_SetFocus();// see if the window controls were hit
	if(b!=0 && b!= this) return b;// the windows are above the world, so return if its hit
		MY_UI::Utilities::Point p= MY_UI::Internal::RootWidget->GetSize();// the root widget is the size of the screen
	vec3 ray, origin;
	UnProject(vec2((float)New_MousePosx, (float)New_MousePosy), *View, *Proj, (float)p.x, (float)p.y, ray, origin);
	//unproject will take the mouse coords in screen space and return a normalize ray and origin
	Base_Mesh* temp = HitChecker(ray, origin);
	if(temp != 0) return this;
	return 0;
}

MY_UI::Controls::cWidgetBase* UI_WorldSelector::Hit(){
	cWidgetBase* b = cWidgetBase::Hit();// see if the window controls were hit
	if(b!=0 && b!= this) return b;// the windows are above the world, so return if its hit
	MY_UI::Utilities::Point p= MY_UI::Internal::RootWidget->GetSize();// the root widget is the size of the screen
	vec3 ray, origin;
	UnProject(vec2((float)New_MousePosx, (float)New_MousePosy), *View, *Proj, (float)p.x, (float)p.y, ray, origin);
	//unproject will take the mouse coords in screen space and return a normalize ray and origin
	Base_Mesh* temp = HitChecker(ray, origin);
	if(temp != 0) return this;
	return 0;
}

void UI_WorldSelector::ClearControls(){
	MY_UI::Safe_Delete(ObjectTranslation);
	MY_UI::Safe_Delete(ObjectRotator);
	MY_UI::Safe_Delete(ObjectScaler);
	Trans_Checkbox->Widget->SetLink(&ObjectTranslation->WorldSpaceEditing);
}

void UI_WorldSelector::CreateRotator(){
	ClearControls();
	Trans_Checkbox->Widget->SetLink(0);// make sure to clear this
	ObjectRotator = new MY_UI::Controls::Rotator(Window);
	mat4* s(0), *t(0), *r(0);
	Selected->GetScaling(&s);
	Selected->GetRotation(&r);
	Selected->GetTranslation(&t);
	quat* q=0;
	Selected->GetRotation(&q);
	ObjectRotator->Set_Links(s, r, q, t, Proj, View, Selected->Get_Max_x_Size(),Selected->Get_Max_y_Size(), Selected->Get_Max_z_Size());
}
void UI_WorldSelector::CreateTranslator(){
	ClearControls();
	ObjectTranslation = new MY_UI::Controls::Translator(Window);
	// i can pass the size of the cube in as 1.0f because that is the untransformed dimensions of the cube!
	mat4* s(0), *t(0), *r(0);
	Selected->GetScaling(&s);
	Selected->GetRotation(&r);
	Selected->GetTranslation(&t);
	ObjectTranslation->Set_Links(s,r, t, Proj, View, Selected->Get_Max_x_Size(),Selected->Get_Max_y_Size(), Selected->Get_Max_z_Size());
	Trans_Checkbox->Widget->SetLink(&ObjectTranslation->WorldSpaceEditing);
}
void UI_WorldSelector::CreateScaler(){
	ClearControls();
	Trans_Checkbox->Widget->SetLink(0);// make sure to clear this
	ObjectScaler = new MY_UI::Controls::Scaler(Window);
	// i can pass the size of the cube in as 1.0f because that is the untransformed dimensions of the cube!
	mat4* s(0), *t(0), *r(0);
	Selected->GetScaling(&s);
	Selected->GetRotation(&r);
	Selected->GetTranslation(&t);
	ObjectScaler->Set_Links(s,r, t, Proj, View, Selected->Get_Max_x_Size(),Selected->Get_Max_y_Size(), Selected->Get_Max_z_Size());
}