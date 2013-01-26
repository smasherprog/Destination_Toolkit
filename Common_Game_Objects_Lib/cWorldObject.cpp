#include "cWorldObject.h"

void cWorldObject::clear(){ 
	Gid=NO_GID;
	Speed=Range=6.0f;
	memcpy(Name, "No_Name", 8); 
	NameSize =8;
	Position.zero();
	memset(InnateEffects, 0, sizeof(InnateEffects));
	memset(Stats, 0, sizeof(Stats));
	memset(Skills, 0, sizeof(Skills));
	Type = MaxBaseType;// this is an invalid type
	VEffect=VID=Weight=0;
}
void cWorldObject::SetName(char* name, uint8_t size){
	if(size >= NAMESIZE-1) size = NAMESIZE-1;
	memcpy(Name, name, size);
	NameSize = size;
	Name[NameSize]=0;
}
void cWorldObject::SetName(std::string name){
	if(name.size() >= NAMESIZE-1) name.resize(NAMESIZE-1);
	memcpy(Name, name.c_str(), name.size());
	NameSize = (unsigned char)name.size();
	Name[NameSize]=0;
}
std::string cWorldObject::GetName(){
	return std::string(Name, NameSize);
}

void cWorldObject::Construct(char*& buffer){
	Gid=*reinterpret_cast<uint16_t*>(buffer);
	buffer+=sizeof(uint16_t);
	Speed = *reinterpret_cast<float*>(buffer);
	buffer+=sizeof(float);
	Range = *reinterpret_cast<float*>(buffer);
	buffer+=sizeof(float);
	NameSize = *buffer++;
	if(NameSize > NAMESIZE) NameSize = NAMESIZE-1;
	if(NameSize < 2) {
		memcpy(Name, "No_Name", 8); 
		NameSize =8;
		Name[8]=0;
	} else {
		memcpy(Name, buffer, NameSize);
		buffer+=NameSize;
		Name[NameSize]=0;
	}
	Position = *reinterpret_cast<vec3*>(buffer);
	buffer+=sizeof(vec3);
	memcpy(InnateEffects, buffer, sizeof(InnateEffects));
	buffer+=sizeof(InnateEffects);
	memcpy(Stats, buffer, sizeof(Stats));
	buffer+=sizeof(Stats);
	memcpy(Skills, buffer, sizeof(Skills));
	buffer+=sizeof(Skills);
	Type = *buffer++;
	VEffect = *reinterpret_cast<uint16_t*>(buffer);
	buffer+=sizeof(uint16_t);
	VID = *reinterpret_cast<uint16_t*>(buffer);
	buffer+=sizeof(uint16_t);
	Weight=*reinterpret_cast<uint16_t*>(buffer);
	buffer+=sizeof(uint16_t);
}

void cWorldObject::Deconstruct(char*& buffer) const{
	*reinterpret_cast<uint16_t*>(buffer)=Gid;
	buffer+=sizeof(uint16_t);
	*reinterpret_cast<float*>(buffer)=Speed;
	buffer+=sizeof(float);
	*reinterpret_cast<float*>(buffer)=Range;
	buffer+=sizeof(float);
	*buffer++= NameSize;
	memcpy(buffer, Name,  NameSize);
	buffer+=NameSize;
	*reinterpret_cast<vec3*>(buffer)=Position;
	buffer+=sizeof(vec3);
	memcpy( buffer, InnateEffects,sizeof(InnateEffects));
	buffer+=sizeof(InnateEffects);
	memcpy( buffer, Stats, sizeof(Stats));
	buffer+=sizeof(Stats);
	memcpy( buffer, Skills, sizeof(Skills));
	buffer+=sizeof(Skills);
	*buffer++=Type;
	*reinterpret_cast<uint16_t*>(buffer)=VEffect;
	buffer+=sizeof(uint16_t);
	*reinterpret_cast<uint16_t*>(buffer)=VID;
	buffer+=sizeof(uint16_t);
	*reinterpret_cast<uint16_t*>(buffer)=Weight;
	buffer+=sizeof(uint16_t);
}

size_t cWorldObject::DeconstructSize() const {

	size_t size = sizeof(Gid);
	size += sizeof(Speed);
	size += sizeof(Range);
	size +=  sizeof(Position);
	size += NameSize + sizeof(NameSize);//size of the name, plus the size of the sive of thename
	size += sizeof(InnateEffects);
	size += sizeof(Stats);
	size += sizeof(Skills);
	size += sizeof(Type);
	size += sizeof(VEffect);
	size += sizeof(VID);
	size += sizeof(Weight);	

	return size;

}