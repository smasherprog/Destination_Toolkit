#include "stdafx.h"
#include "Mesh_Container_Presentation.h"
#include "../MY_UI/ApplicationLayer.h"
#include "../Graphics_Lib/Base_Mesh.h"
#include "../Graphics_Lib/BV_Mesh.h"
#include "../Graphics_Lib/Trans_Mesh.h"
#include "Mesh_UI.h"
#include "../assimp/include/assimp/cimport.h"
#include "../Graphics_Lib/Base_Camera.h"
#include "Mesh_Container_Service.h"

Mesh_Container_Presentation::Mesh_Container_Presentation(const Base_Camera* camera): cWidgetBase(MY_UI::Internal::RootWidget->GetApplicationLayer()){	
	Last_Hit=0;
	SetControlBounds(MY_UI::Utilities::Rect(0, 0, MAX_WINDOWSIZE, MAX_WINDOWSIZE));
	SetClientBounds(MY_UI::Utilities::Rect(0, 0, MAX_WINDOWSIZE, MAX_WINDOWSIZE));
	SetDraggable(true);
	SetName("Mesh_Container_Presentation");
	Mesh_Container = new Mesh_Container_Service();

	Camera = camera;
	MeshUI = new Mesh_UI();
	//pass the load function. The UI does not manage the lifetime, just acts as a middle man
	MeshUI->Set_AddMesh_Func(std::bind(&Mesh_Container_Service::Load_Mesh, Mesh_Container, std::placeholders::_1));//pass load mesh function
	aiString sz;
	aiGetExtensionList(&sz);// get the extensions that assimp is able to load
	MeshUI->SetFileExts(std::string(sz.C_Str()));

}
Mesh_Container_Presentation::~Mesh_Container_Presentation(){
	delete Mesh_Container;
}
void Mesh_Container_Presentation::Draw(float dt){
	for(size_t i=0; i< Mesh_Container->Mesh.size(); i++) {
		Base_Mesh* mesh = Mesh_Container->Mesh[i];
		mesh->Draw(Camera, dt);
		if(Last_Hit!=0){
			Mesh_Container->BV->SetPosition(mesh->Get_Center());
			Mesh_Container->BV->Draw(Camera);
			Mesh_Container->Trans->Draw(Camera);
		}
	}
}

void Mesh_Container_Presentation::MouseMoved(){
	return;
	MY_UI::Utilities::Point p= MY_UI::Internal::RootWidget->GetSize();// the root widget is the size of the screen
	vec3 ray, origin;
	UnProject(vec2((float)New_MousePosx, (float)New_MousePosy), Camera->View, Camera->Proj, (float)p.x, (float)p.y, ray, origin);
	if(Mouse_LButton_Down){// the user is dragging the control

		vec3 ray1, origin1;// the normalized ray in world space
		auto root = MY_UI::Internal::RootWidget;
		MY_UI::Utilities::Point screensize = root->GetSize();
		vec2 screens(static_cast<float>(screensize.x), static_cast<float>(screensize.y));
		vec2 mousepos(static_cast<float>(New_MousePosx), static_cast<float>(New_MousePosy));

		UnProject(mousepos, Camera->View, Camera->Proj, screens.x, screens.y, ray1, origin1);

		mat4 world;
		world.setTranslation(Mesh_Container->Trans->GetPosition());

		vec3 origin2(Mesh_Container->Trans->GetPosition());
		vec3 ray2(Mesh_Container->Trans->HitAxis);
		TransformNormal(ray2, world);

		vec2 intersectiondist = RayRayIntersect(ray1, origin1, ray2, origin2);
		if(intersectiondist.y ==INFINITY) intersectiondist.y =0;// dont get all crazy now!
		vec3 ray = ray2 *intersectiondist.y;// scale the ray
		ray += origin2;// now we have the new point 

		vec2 oldmousepos(static_cast<float>(Old_MousePosx), static_cast<float>(Old_MousePosy));
		UnProject(oldmousepos, Camera->View, Camera->Proj, screens.x, screens.y, ray1, origin1);

		// figure out where our last point was
		intersectiondist = RayRayIntersect(ray1, origin1, ray2, origin2);
		if(intersectiondist.y ==INFINITY)intersectiondist.y =0;// dont get all crazy now!
		vec3 oldray = ray2 *intersectiondist.y;// scale the ray
		oldray += origin2;// now we have the old point 
		ray -= oldray;// ray is now the delta amount of movement... add this onto the objects position to get correct translation

		Mesh_Container->Trans->Translate_By_Offset(ray*Mesh_Container->Trans->HitAxis);

	}
}
void Mesh_Container_Presentation::LeftMouseButtonUp(){
	MY_UI::Utilities::Point p= MY_UI::Internal::RootWidget->GetSize();// the root widget is the size of the screen
	vec3 ray, origin;
	UnProject(vec2((float)New_MousePosx, (float)New_MousePosy), Camera->View, Camera->Proj, (float)p.x, (float)p.y, ray, origin);
	Base_Mesh* mesh=  Mesh_Container->Check_Hit(ray, origin);
	if(mesh!=NULL) {
		if(Last_Hit != mesh){
			mesh->On_Translate_By_Offset.clear();//clear any connections to the trans
			Mesh_Container->Trans->On_Translate_By_Offset.clear();
			Mesh_Container->Trans->SetPosition(mesh->Get_Center());// set up the translation for the new mesh
			Mesh_Container->BV->SetScaling(vec3(mesh->Get_Max_x_Size(), mesh->Get_Max_y_Size(), mesh->Get_Max_z_Size()));
			Mesh_Container->BV->SetPosition(mesh->Get_Center());

			mesh->On_Translate_By_Offset.Connect(&Mesh_Container->Trans->On_Translate_By_Offset, std::bind(&Base_Mesh::Translate_By_Offset, Mesh_Container->Trans, std::placeholders::_1));
			Mesh_Container->Trans->On_Translate_By_Offset.Connect(&mesh->On_Translate_By_Offset, std::bind(&Base_Mesh::Translate_By_Offset, mesh, std::placeholders::_1));
			Last_Hit=mesh;
		}
		return;
	}
	Last_Hit=0;
}
MY_UI::Controls::cWidgetBase* Mesh_Container_Presentation::Hit(){
	return 0;
	MY_UI::Utilities::Point p= MY_UI::Internal::RootWidget->GetSize();// the root widget is the size of the screen
	vec3 ray, origin;
	UnProject(vec2((float)New_MousePosx, (float)New_MousePosy), Camera->View, Camera->Proj, (float)p.x, (float)p.y, ray, origin);

	float dist = Mesh_Container->Trans->HighLight_Hit_Axis(origin, ray);
	if(dist!=INFINITY){
		return this;
	}
	Base_Mesh* mesh=  Mesh_Container->Check_Hit(ray, origin);
	if(mesh!=NULL) {
		return this;
	}
	return 0;
}
MY_UI::Controls::cWidgetBase* Mesh_Container_Presentation::Hit_And_SetFocus() {
	return 0;
	MY_UI::Utilities::Point p= MY_UI::Internal::RootWidget->GetSize();// the root widget is the size of the screen
	vec3 ray, origin;
	UnProject(vec2((float)New_MousePosx, (float)New_MousePosy), Camera->View, Camera->Proj, (float)p.x, (float)p.y, ray, origin);

	float dist = Mesh_Container->Trans->HighLight_Hit_Axis(origin, ray);
	if(dist!=INFINITY){
		return this;
	}
	if(Last_Hit != NULL){
		Last_Hit->On_SetPosition.clear();//clear any connections to the trans
		Last_Hit->On_Translate_By_Offset.clear();//clear any connections to the trans
		Mesh_Container->Trans->On_Translate_By_Offset.clear();
		Last_Hit=0;
	}
	
	Base_Mesh* mesh=  Mesh_Container->Check_Hit(ray, origin);
	if(mesh!=NULL) {
		return this;
	}
	return 0;
}