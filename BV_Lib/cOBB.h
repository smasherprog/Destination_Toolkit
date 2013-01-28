#ifndef COBB_H 
#define COBB_H
#include "../Utilities/MyMath.h"
#include "cAABB.h"

class cOBB: public cAABB{
public:

	vec3 Size;
	cOBB();
	cOBB(const vec3& size);

	void Init();// call this after a change is made so that the internals can be updated
	void SetSize(const vec3& size);
	bool Intersect(const cOBB &b) const;
	float Intersect(const vec3& ray, const vec3& origin) const;
	bool Intersect(const vec3 &point) const;
	
	void GetContactPoint(const vec3 &raydir, const vec3& rayorigin, vec3& outraydir, vec3& outrayorigin);
	void GetCorners(vec3* arr)  const; // the array is assumed to always have 8 slots

	quat Rot;
	mat4 MatRot, World, InvWorld;
};

#endif