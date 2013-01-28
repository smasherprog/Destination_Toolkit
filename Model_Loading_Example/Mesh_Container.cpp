#include "stdafx.h"
#include "Mesh_Container.h"
#include "../Graphics_Lib/Static_Mesh.h"

Mesh_Container::~Mesh_Container(){
	for(size_t i=0; i< Mesh.size(); i++) delete Mesh[i];
}

bool Mesh_Container::Load_Mesh(std::string file){
	Static_Mesh* m = new Static_Mesh();
	if(m->Load(file)){
		Mesh.push_back(m);
		return true;
	} else {
		delete m;
		return false;
	}
}
Base_Mesh* Mesh_Container::Check_Hit(vec3& ray, vec3& origin){
	for(size_t i=0; i< Mesh.size(); i++) {
		float f =Mesh[i]->Ray_Tri_Intersect(ray, origin);
		if(f!=INFINITY) return Mesh[i];
	}
	return 0;
}
void Mesh_Container::Draw(mat4& view, mat4& proj){
	for(size_t i=0; i< Mesh.size(); i++) Mesh[i]->Draw(view, proj);
}