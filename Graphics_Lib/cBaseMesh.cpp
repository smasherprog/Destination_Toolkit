#include "stdafx.h"
#include "cBaseMesh.h"
#include "../Utilities/My_Timer.h"

cBaseMesh::~cBaseMesh(){ clear(); }
cBaseMesh::cBaseMesh(){ clear(); }

void cBaseMesh::clear(){
	Vertices.clear();	
	Lod1.Destroy();
	Lod2.Destroy();
	Lod3.Destroy();
	ObjectSpace_BV.clear();
	WorldSpace_BV.clear();
	Name.clear();
	FileName.clear();	
	Translation.identity();
	Scaling.identity();
	Rotation.identity();
	World.identity();
	Rot.identity();
	Angles.identity();
	MovementDelta.zero();
	InstanceChanged=false;
	LastQueryTime= My_Timer::tick_count::now();
	Visible=false;
	Query.Destroy();
}

void cBaseMesh::Save(ofstream& file){
	//Save(FileName);
	uint32_t si(static_cast<uint32_t>(FileName.size()));
	file.write(reinterpret_cast<char*>(&si), sizeof(uint32_t)); // num of characters in the std::string we are loading
	file.write(FileName.c_str(), si);// write
	si = static_cast<uint32_t>(Name.size());
	file.write(reinterpret_cast<char*>(&si), sizeof(uint32_t)); // num of characters in the std::string we are loading
	file.write(Name.c_str(), si);// read the name
	vec3 scal, pos, rot;
	GetScaling(scal.x, scal.y, scal.z);
	GetPosition(pos.x, pos.y, pos.z);
	GetRotation(rot.x, rot.y, rot.z);
	file.write(reinterpret_cast<char*>(&scal), sizeof(vec3)); // read the scaling
	file.write(reinterpret_cast<char*>(&rot), sizeof(vec3)); // read the  rotation
	file.write(reinterpret_cast<char*>(&pos), sizeof(vec3)); // read the translation
}
bool cBaseMesh::Load(ifstream& file){
	clear();
	char na[256];
	uint32_t si(0);
	file.read(reinterpret_cast<char*>(&si), sizeof(uint32_t)); // num of characters in the std::string we are loading
	file.read(na, si);// read the filename
	na[si] = 0;// insert the null character
	FileName = na;
	file.read(reinterpret_cast<char*>(&si), sizeof(uint32_t)); // num of characters in the std::string we are loading
	file.read(na, si);// read the name
	na[si] = 0;// insert the null character
	Name = na;
	vec3 scal, pos, rot;
	file.read(reinterpret_cast<char*>(&scal), sizeof(vec3)); // read the scaling
	file.read(reinterpret_cast<char*>(&rot), sizeof(vec3)); // read the  rotation
	file.read(reinterpret_cast<char*>(&pos), sizeof(vec3)); // read the translation
	std::string temp;
	uint32_t ty=GetType();

	if(Load(temp)){
		SetPosition(pos.x, pos.y, pos.z);
		SetScaling(scal.x, scal.y, scal.z);
		SetRotation(rot.x, rot.y, rot.z);
		Rot.SetupRotation(Angles);
		Rotation.setupfromquat(Rot);
		InitAABB();
		UpdateWorldMatrix();
		return true;
	}
	return false;
}
void cBaseMesh::InitAABB(){// this is in object space so you need to transfor the bv into world space to do any work
	if(Vertices.empty()) return ObjectSpace_BV.CreateDefault();// create a small bounding volume to test against
	size_t i(0);
	do{	ObjectSpace_BV.Add(Vertices[i]); } while(++i!=Vertices.size());
	ObjectSpace_BV.Init();
	WorldSpace_BV = ObjectSpace_BV;
}
