#include "stdafx.h"
#include "Base_Mesh.h"
#include "MeshTypes.h"

Base_Mesh::~Base_Mesh(){ 
	Query.Destroy();
}
void Base_Mesh::DeInit(){

	Visible = true;
	FileName = Name="";
	Angles.identity();
	Rot.identity();
	MovementDelta.zero();
	Translation.identity();
	Scaling.identity();
	Rotation.identity();
	World.identity();
	Query.Destroy();
}
void Base_Mesh::Load(std::ifstream& stream){
	std::vector<char> name;
	unsigned int temp = 0;
	stream.read((char*)&temp, sizeof(unsigned int));
	name.resize(temp);
	stream.read(&name[0], temp);
	Name = std::string(&name[0], temp);
	stream.read((char*)&Translation[0], sizeof(mat4));
	stream.read((char*)&Scaling[0], sizeof(mat4));
	stream.read((char*)&Rotation[0], sizeof(mat4));
	stream.read((char*)&Rot[0], sizeof(quat));
	stream.read((char*)&Angles.heading, sizeof(euler));
}
void Base_Mesh::Save(std::ofstream& stream){
	std::string name = Name;
	unsigned int temp = (unsigned int)name.size()+1;
	stream.write((char*)&temp, sizeof(unsigned int));
	stream.write(name.c_str(), temp);
	stream.write((char*)&Translation[0], sizeof(mat4));
	stream.write((char*)&Scaling[0], sizeof(mat4));
	stream.write((char*)&Rotation[0], sizeof(mat4));
	stream.write((char*)&Rot[0], sizeof(quat));
	stream.write((char*)&Angles.heading, sizeof(euler));
}

