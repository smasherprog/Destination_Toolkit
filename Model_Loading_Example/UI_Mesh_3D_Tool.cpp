#include "stdafx.h"
#include "UI_Mesh_3D_Tool.h"
#include "../MY_UI/cWidgetUtilities.h"
#include "../MY_UI/Window.h"
#include "../MY_UI/DropDown.h"
#include "../MY_UI/Text.h"
#include "../MY_UI/TextBox.h"
#include "../MY_UI/ColorPickerWindow.h"
#include "../MY_UI/Objects_3d.h"
#include "../Graphics_Lib/Trans_Mesh.h"
#include "../MY_UI/ApplicationLayer.h"
#include "../Utilities/MyMath.h"

UI_Mesh_3D_Tool::UI_Mesh_3D_Tool(mat4* camera_view, mat4* camera_proj, std::function<Base_Mesh*(vec3& ray, vec3& origin)> f) : cWidgetBase(MY_UI::Internal::RootWidget->GetApplicationLayer()){// set the root widget as the parent
	SetControlBounds(MY_UI::Utilities::Rect(0, 0, MAX_WINDOWSIZE, MAX_WINDOWSIZE));
	SetClientBounds(MY_UI::Utilities::Rect(0, 0, MAX_WINDOWSIZE, MAX_WINDOWSIZE));
	SetDraggable(true);
	SetName("UI_WorldSelector");

	View= camera_view;
	Proj=camera_proj;
	HitChecker=f;

}
void UI_Mesh_3D_Tool::MouseMoved(){
	MY_UI::Utilities::Point p= MY_UI::Internal::RootWidget->GetSize();// the root widget is the size of the screen
	vec3 ray, origin;
	UnProject(vec2((float)New_MousePosx, (float)New_MousePosy), *View, *Proj, (float)p.x, (float)p.y, ray, origin);

	Graphics::Internal_Components::Trans->HighLight_Hit_Axis(origin, ray);

}
MY_UI::Controls::cWidgetBase* UI_Mesh_3D_Tool::Hit_And_SetFocus(){
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

MY_UI::Controls::cWidgetBase* UI_Mesh_3D_Tool::Hit(){
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
