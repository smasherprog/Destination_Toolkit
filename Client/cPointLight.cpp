#include "stdafx.h"
#include "cPointLight.h"

void cPointLight::Save(ofstream& file){
	file.write(reinterpret_cast<char*>(&Center), sizeof(vec3));
	file.write(reinterpret_cast<char*>(&Radius), sizeof(float));
	file.write(reinterpret_cast<char*>(&Attenuation), sizeof(vec4));
	file.write(reinterpret_cast<char*>(&Diffuse), sizeof(vec4));
	file.write(reinterpret_cast<char*>(&SpecularColor), sizeof(vec4));
	uint8_t t(static_cast<uint8_t>(On));
	file.write(reinterpret_cast<char*>(&t), sizeof(uint8_t));
	uint32_t si(static_cast<uint32_t>(Name.size()));
	file.write(reinterpret_cast<char*>(&si), sizeof(uint32_t));
	file.write(Name.c_str(), si);
}

bool cPointLight::Load(ifstream& file){
	file.read(reinterpret_cast<char*>(&Center), sizeof(vec3));
	file.read(reinterpret_cast<char*>(&Radius), sizeof(float));
	file.read(reinterpret_cast<char*>(&Attenuation), sizeof(vec4));
	file.read(reinterpret_cast<char*>(&Diffuse), sizeof(vec4));
	file.read(reinterpret_cast<char*>(&SpecularColor), sizeof(vec4));
	uint8_t t(0);
	file.read(reinterpret_cast<char*>(&t), sizeof(uint8_t));
	On = t != 0;
	uint32_t si(0);
	file.read(reinterpret_cast<char*>(&si), sizeof(uint32_t));
	char temp[255];
	file.read(temp, si);
	temp[si]=0;// null char
	Name=temp;
	return true;
}
