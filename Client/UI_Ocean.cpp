#include "stdafx.h"
#include "UI_Ocean.h"
#include "../MY_UI/Text.h"
#include "cOcean.h"
#include "../Graphics_Lib/cCamera.h"
#include "../MY_UI/ApplicationLayer.h"
#include "../MY_UI\CheckBox.h"
#include "../My_UI\TextBox.h"
#include "../Common_Game_Objects_Lib/cBaseNetwork.h"
#include "cDemoControl.h"

UI_Ocean::UI_Ocean( MY_UI::Controls::cWidgetBase* parent, cDemoControl* demo): MY_UI::Controls::cWidgetBase(parent), DemoControl(*demo) {
	SetControlBounds(MY_UI::Utilities::Rect(0, 0, 2000, 2000));
	SetClientBounds(MY_UI::Utilities::Rect(0, 0, 2000, 2000));
	Editing=false;
	SetDraggable(true);

	SetHidden(true);// hide on construction
	SetName("UI_Ocean");
}
UI_Ocean::~UI_Ocean() {
	DemoControl.Ocean.Editing=false;
}
vec3 GetPos(cDemoControl& DemoControl){
	float mousex = (float)New_MousePosx;
	float mousey = (float)New_MousePosy;
	MY_UI::Utilities::Point p = MY_UI::Internal::RootWidget->GetSize();
	vec2 screensize((float)p.x, (float)p.y);
	float vx = (+2.0f*mousex/screensize.x - 1.0f)/Camera.Proj._11;// Compute picking ray in view space.
	float vy = (-2.0f*mousey/screensize.y + 1.0f)/Camera.Proj._22;
	mat4 inverseV(Camera.InvView);
	vec3 rayOrigin(0.0f, 0.0f, 0.0f);
	vec3 rayDir(vx, vy, 1.0f);
	rayOrigin*=inverseV;// this function actually translated the point
	TransformNormal(rayDir, inverseV);// this function does no translate the vector, it only changes the direction into the new coordinate system
	if(rayDir.y >=-0.05f) return vec3(DemoControl.Ocean.EditorPos.x, DemoControl.Ocean.EditorPos.y, DemoControl.Ocean.EditorPos.z);// return the old pos because the ray is pointing above the horizon

	float scaling = - rayOrigin.y/rayDir.y;
	rayDir*=scaling;
	rayDir+=rayOrigin;
	rayDir = floor(rayDir*vec3(1.0f/64.0f,0.0f , 1.0f/64.0f)) * vec3(64.0f, 64.0f, 64.0f);
	return rayDir;
}
void UI_Ocean::LeftMouseButtonDown(){
	if(Editing){	
		PaintOcean();
	}
}
void UI_Ocean::MouseMoved(){
	if(Editing){
		if(Mouse_LButton_Down) return PaintOcean();
		vec3 t = ::GetPos(DemoControl);// get the function above this one
		DemoControl.Ocean.EditorPos = vec4(t.x, t.y, t.z, 0);

	}
}

void UI_Ocean::PaintOcean(){
	
		vec3 t = ::GetPos(DemoControl);// get the function above this one
		DemoControl.Ocean.EditorPos = vec4(t.x, t.y, t.z, 0);
		if(!DemoControl.Ocean.GridExists(t.x,t.z)){// if the grid doesnt already exist.. try to make it
			DemoControl.ReliableSendTo(Network::GS_REQ_OCEAN_GRID, vec2(t.x, t.z));
		}
	
}
void UI_Ocean::ToggleEditing(){
	Editing = !Editing;// Toggle!
	DemoControl.Ocean.Editing=Editing;
	SetHidden(!Editing);
}

Ocean_Window* Ocean_Window::self=0;
Ocean_Window::Ocean_Window(MY_UI::Controls::cWidgetBase* parent, cDemoControl* demo): MY_UI::Controls::Window(parent), DemoControl(*demo) {
	assert(self==0);
	UI_OceanCatcher = new UI_Ocean(MY_UI::Internal::RootWidget->GetApplicationLayer(), demo);
	SetSize(200, 200);
	SetTitle("Ocean editor");
	MY_UI::Controls::CheckBox_w_Label* Editing = new MY_UI::Controls::CheckBox_w_Label(this);
	Editing->SetPos(5,5);
	Editing->SetLabel("Editing on/off", RIGHT, 5);
	Editing->Widget->OnLeftClicked.Add(std::bind(&Ocean_Window::ToggleEditing, this));	
	MY_UI::Controls::cWidget_w_Label<MY_UI::Controls::Numeric_TextBox_w_Adjusters<float>>* lod0 = new MY_UI::Controls::cWidget_w_Label<MY_UI::Controls::Numeric_TextBox_w_Adjusters<float>>(this);
	lod0->Widget->SetLink(&DemoControl.Ocean.LodDist0);
	lod0->SetLabel("lod 0", RIGHT | IN_LINE, 5);
	lod0->Align_To_Widget(Editing, BOTTOM, 5);
	MY_UI::Controls::CheckBox_w_Label* wireframe = new MY_UI::Controls::CheckBox_w_Label(this);
	wireframe->Widget->SetLink(&DemoControl.Ocean.DrawWireFrame);
	wireframe->SetLabel("Wireframe", RIGHT | IN_LINE, 5);
	wireframe->Align_To_Widget(lod0, BOTTOM, 5);
	SetName("Ocean_Window");
}
void Ocean_Window::ToggleEditing(){ 
	UI_OceanCatcher->ToggleEditing(); 
}
Ocean_Window::~Ocean_Window(){
	delete UI_OceanCatcher;// the desctructor will remove itself from the applicat
	self =0;
}