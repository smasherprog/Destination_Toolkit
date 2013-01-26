#ifndef CWORLDOBJECT_H
#define CWORLDOBJECT_H

#include <fstream>
#include <vector>
#include "../Utilities/MyMath.h"
#include "cWorldDefines.h"

// a class that all objects should derive from if they wish to be sent and received over a network
class cSerialization{
public:

	cSerialization() {};
	virtual ~cSerialization() {};

	virtual void clear()=0;

	virtual void Construct(char*& buffer)=0;
	virtual void ConstructForVisual(char*& buffer)=0;

	virtual void Deconstruct(char*& buffer) const =0;
	virtual void DeconstructForVisual(char*& buffer)const =0;

	virtual size_t DeconstructForVisualSize()const=0;
	virtual size_t DeconstructSize() const=0;

};

class cWorldObject : public cSerialization{
public:
	uint16_t Gid;
	float Speed, Range;
	char Name[NAMESIZE];
	uint8_t NameSize;
	vec3 Position;
	uint16_t InnateEffects[MAXOBJECTEFFECTS], Stats[NUMBEROFSTATS], Skills[NUMBEROFSKILLS];
	uint8_t Type;
	uint16_t VEffect, VID, Weight;// VEffects is for visual appearance, not anything the item does, VID is for additional visual info

	cWorldObject(){ cWorldObject::clear(); }
	virtual ~cWorldObject() {}

	virtual void clear();

	virtual void Construct(char*& buffer);
	virtual void ConstructForVisual(char*& buffer) { cWorldObject::Construct(buffer); } 

	virtual void Deconstruct(char*& buffer) const;
	virtual void DeconstructForVisual(char*& buffer)const{ cWorldObject::Deconstruct(buffer); }

	virtual size_t DeconstructForVisualSize() const { return cWorldObject::DeconstructSize(); }// same thing
	virtual size_t DeconstructSize() const;

	virtual void SetPosition(const float&x, const float& y, const float& z){ Position.x = x; Position.y =y; Position.z =z; }
	virtual void GetPosition(float&x, float& y, float& z){  x=Position.x; y=Position.y; z=Position.z; }

	virtual void SetName(char* name, uint8_t size);
	virtual void SetName(std::string name);
	virtual std::string GetName();// null terminating name
	
};

#endif
