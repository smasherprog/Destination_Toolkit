#include "stdafx.h"
#include "cFunction.h"


cFunction::cFunction(){
	memset(this, 0, sizeof(cFunction));
}
cFunction::cFunction(const cFunction &func){
	operator=(func);
}
cFunction& cFunction::operator =(const cFunction &rhs){
	memcpy(this, &rhs, sizeof(cFunction));
	return *this;
}
cFunction::cFunction(ifstream& stream){
	Construct(stream);
}
cFunction::cFunction(char*& buffer){
	Construct(buffer);
}
void cFunction::Construct(char*& buffer){
	Function=*reinterpret_cast<uint32_t*>(buffer);
	buffer+=sizeof(uint32_t);
	Radius=*reinterpret_cast<float*>(buffer);
	buffer+=sizeof(float);
	uint8_t temp=*reinterpret_cast<uint8_t*>(buffer);
	buffer+=sizeof(uint8_t);
	if(temp==1){
		Preserve=true;
	} else {
		Preserve=false;
	}
	Duration=*reinterpret_cast<double*>(buffer);
	buffer+=sizeof(double);
	Position=*reinterpret_cast<vec3*>(buffer);
	buffer+=sizeof(vec3);
	//BV.Construct(buffer);
}
void cFunction::Construct(ifstream& stream){
	uint8_t temp;
	stream>>Function>>Radius>>temp;
	if(temp==1){
		Preserve=true;
	} else {
		Preserve=false;
	}
	stream>>Duration>>Position.x>>Position.y>>Position.z;
	//BV.Construct(stream);
}
void cFunction::Deconstruct(char*& buffer) const{
	*reinterpret_cast<uint32_t*>(buffer)=Function;
	buffer+=sizeof(uint32_t);
	*reinterpret_cast<float*>(buffer)=Radius;
	buffer+=sizeof(float);
	uint8_t temp;
	if(Preserve){
		temp=1; 
	} else {
		temp=0;
	}
	*reinterpret_cast<uint8_t*>(buffer)=temp;
	buffer+=sizeof(uint8_t);
	*reinterpret_cast<double*>(buffer)=Duration;
	buffer+=sizeof(double);
	*reinterpret_cast<vec3*>(buffer)=Position;
	buffer+=sizeof(vec3);
	//BV.Deconstruct(buffer);
}
void cFunction::Deconstruct(ofstream& stream) const {
	uint8_t temp;
	if(Preserve){
		temp=1; 
	} else {
		temp=0;
	}
	stream<<Function<<endl<<Radius<<endl<<temp<<endl<<Duration<<endl<<Position.x<<endl<<Position.y<<endl<<Position.z<<endl;
	//BV.Deconstruct(stream);
}
uint32_t cFunction::DeconstructSize(){
	return sizeof(uint32_t) + sizeof(float) + sizeof(uint8_t) + sizeof(double) + sizeof(vec3);//+ cAABB::DeconstructSize();
}