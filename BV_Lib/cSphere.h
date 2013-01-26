#ifndef CSPHERE_H
#define CSPHERE_H
#include "../Utilities/MyMath.h"

class cSphere{
public:

	vec3 Center;
	float Radius;
	
	cSphere(): Center(vec3(0.0f, 0.0f, 0.0f)), Radius(1.0f) {}
	cSphere(const cSphere& object): Center(object.Center), Radius(object.Radius) {}
	cSphere(vec3 center, float radius= 1.0f) : Center(center), Radius(radius) {}
	
	void SetRadius(const float r){ Radius =r; }

	void clear(){ Center.zero(); Radius =0.0f ;}
	void MoveTo(const vec3& newpos){ Center = newpos; }
	void MoveByOffset(const vec3& offset) { Center += offset; }
	//bool Insertsect(cAABB& A) const;
	float RayIntersect(const vec3& rayOrg, const vec3& rayDir);

	//bool OutSideOf(cAABB& other) const;

	//bool InsidePlane(const Plane & plane) const;

	//sphere - plane intersect dynamic test with two moving objects
	bool SweepIntersect(const vec3& prevpos, const Plane& plane, vec3& hitpos, float& timeofhit);

	// two different ways to do a static sphere test
	bool Insertsect(const cSphere& A) const{ return LengthSq(A.Center , Center) < (Radius * Radius + A.Radius*A.Radius); }
	bool Insertsect(const vec3& pos, float radius) const{ return LengthSq(Center , pos) < (Radius * Radius + radius*radius);}

};




#endif