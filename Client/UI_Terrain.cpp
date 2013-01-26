#include "stdafx.h"
#include "UI_Terrain.h"
#include "../MY_UI/Text.h"
#include "../Graphics_Lib/cCamera.h"
#include "../MY_UI/ApplicationLayer.h"
#include "../MY_UI\CheckBox.h"
#include "../My_UI\TextBox.h"
#include "cDemoControl.h"
#include "../Common_Game_Objects_Lib/cBaseNetwork.h"

UI_Terrain::UI_Terrain( MY_UI::Controls::cWidgetBase* parent, cDemoControl* demo): MY_UI::Controls::cWidgetBase(parent), DemoControl(*demo) {
	SetControlBounds(MY_UI::Utilities::Rect(0, 0, 2000, 2000));
	SetClientBounds(MY_UI::Utilities::Rect(0, 0, 2000, 2000));
	Editing=false;
	SetDraggable(true);

	SetHidden(true);// hide on construction
	SetName("UI_Terrain");
}
UI_Terrain::~UI_Terrain() {
	DemoControl.Terr.Editing=false;
}

vec3 GetPos1(cDemoControl& DemoControl){
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
	// now rayorigin is the starting pos, and raydir is the normalize ray in world space

	if(rayDir.y >=-0.05f) return vec3(DemoControl.Terr.EditorPos.x, DemoControl.Terr.EditorPos.y, DemoControl.Terr.EditorPos.z);// return the old pos because the ray is pointing above the horizon
	return DemoControl.Terr.GetPosOfIntersection(rayOrigin, rayDir);
}
void UI_Terrain::LeftMouseButtonDown(){
	if(Editing){	
		PaintOcean();
	}
}
void UI_Terrain::MouseMoved(){
	if(Editing){
		if(Mouse_LButton_Down) return PaintOcean();
		vec3 t = GetPos1(DemoControl);// get the function above this one
		DemoControl.Terr.EditorPos = vec4(t.x, t.y, t.z, 0);
	}
}

void UI_Terrain::PaintOcean(){

		vec3 t = GetPos1(DemoControl);// get the function above this one
		DemoControl.Terr.EditorPos = vec4(t.x, t.y, t.z, 0);
		
			cDataPacket pac(sizeof(float)*2);
			char* temp = pac.Data;
			pac.SetID(Network::GS_REQ_OCEAN_GRID);

			*reinterpret_cast<float*>(temp) = t.x;
			temp+=sizeof(float);
			*reinterpret_cast<float*>(temp) = t.z;
			//Renderer.DemoControl.Network.ReliableSendTo(pac);
		

}
void UI_Terrain::ToggleEditing(){
	Editing = !Editing;// Toggle!
	DemoControl.Terr.Editing=Editing;
	SetHidden(!Editing);
}

Terrain_Window* Terrain_Window::self=0;
Terrain_Window::Terrain_Window(MY_UI::Controls::cWidgetBase* parent, cDemoControl* demo): MY_UI::Controls::Window(parent), DemoControl(*demo) {
	assert(self==0);

	UI_TerrainCatcher = new UI_Terrain(MY_UI::GetApplicationLayer(), demo);

	SetSize(200, 200);
	SetTitle("Terrain editor");
	MY_UI::Controls::CheckBox_w_Label* Editing = new MY_UI::Controls::CheckBox_w_Label(this);
	Editing->SetPos(5,5);
	Editing->SetLabel("Editing on/off", RIGHT, 5);
	Editing->Widget->OnLeftClicked.Add(std::bind(&Terrain_Window::ToggleEditing, this));	
	MY_UI::Controls::cWidget_w_Label<MY_UI::Controls::Numeric_TextBox_w_Adjusters<float>>* lod0 = new MY_UI::Controls::cWidget_w_Label<MY_UI::Controls::Numeric_TextBox_w_Adjusters<float>>(this);
	lod0->Widget->SetLink(&DemoControl.Ocean.LodDist0);
	lod0->SetLabel("lod 0", RIGHT | IN_LINE, 5);
	lod0->Align_To_Widget(Editing, BOTTOM, 5);
	MY_UI::Controls::CheckBox_w_Label* wireframe = new MY_UI::Controls::CheckBox_w_Label(this);
	wireframe->Widget->SetLink(&DemoControl.Terr.DrawWireFrame);
	wireframe->SetLabel("Wireframe", RIGHT | IN_LINE, 5);
	wireframe->Align_To_Widget(lod0, BOTTOM, 5);
	SetName("Ocean_Window");
	OUTPUT_DEBUG_MSG("created terrain window");
}
void Terrain_Window::ToggleEditing(){ 
	UI_TerrainCatcher->ToggleEditing(); 
}
Terrain_Window::~Terrain_Window(){
	MY_UI::Safe_Delete(UI_TerrainCatcher);// the desctructor will remove itself from the applicat
	self =0;
}