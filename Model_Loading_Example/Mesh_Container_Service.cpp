#include "stdafx.h"
#include "Mesh_Container_Service.h"
#include "../Graphics_Lib/Static_Mesh.h"
#include "../Graphics_Lib/Trans_Mesh.h"
#include "../Graphics_Lib/BV_Mesh.h"

Mesh_Container_Service::Mesh_Container_Service(){
	BV = new BV_Mesh();
	BV->Init();
	Trans = new Trans_Mesh();
	Trans->Init();
}


Mesh_Container_Service::~Mesh_Container_Service(){
	for(size_t i=0; i< Mesh.size(); i++) delete Mesh[i];
	delete BV;
	delete Trans;
}

bool Mesh_Container_Service::Load_Mesh(std::string file){
	Static_Mesh* m = new Static_Mesh();
	if(m->Load(file)){
		Mesh.push_back(m);
		return true;
	} else {
		delete m;
		return false;
	}
}
Base_Mesh* Mesh_Container_Service::Check_Hit(vec3& ray, vec3& origin){

	for(size_t i=0; i< Mesh.size(); i++) {
		float f =Mesh[i]->Ray_Tri_Intersect( origin,ray);
		if(f!=INFINITY) return Mesh[i];
	}
	return 0;
}
