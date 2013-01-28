#ifndef cAABB_H
#define cAABB_H
#include "cSphere.h"
#include "../Utilities/MyMath.h"

class cAABB: public cSphere{
public:

	vec3 Min, Max;

	cAABB();
	cAABB(const cAABB& object);
	cAABB(vec3* min, vec3* max);
	cAABB(vec3& min, vec3& max);
	cAABB(const cSphere& shp);

	void Centroid(vec3* p) const{ *p=Min+Max; *p*=.5f;}
	void HalfDiag(vec3* p) const{ *p=Max-Min; *p*=.5f; }
	void CreateDefault(){ clear(); Min = vec3(-1, -1, -1); Max = vec3(1, 1, 1); Init(); }
	
	void clear();
	float XSize() const;
	float YSize() const;
	float ZSize() const;
	void MoveTo(const vec3& newpos);
	void MoveByOffset(const vec3& offset);
	void Add(const vec3& p);
	void Add(const float x, const float y, const float z){ Add(vec3(x, y, z)); }
	void Init();
	bool Intersect(const Plane& n) const;
	bool Intersect(const cSphere& sphere) const;
	bool Intersect(const cAABB& A, cAABB* boxintersect) const;
	bool Intersect(const vec3& pos, float radius) const{ return cSphere::Insertsect(pos, radius); }// the ray and the ray origin are assumed to be in the same space
	bool Intersect(const vec4& pos) const;
	//RayIntersect expects rayDir to be not normalized, meaning if you want to test for an aabb hit, you must scale the ray to ensure its long enough to hit it
	float RayIntersect(const vec3& rayOrg, const vec3& rayDir) const{ return RayAABBIntersect(Min, Max, rayOrg, rayDir); }// the ray and the ray origin are assumed to be within the BV's space
	
	bool OutSideOf(cAABB& other) const;
	
	void Transform(const mat4& trans){ Min*=trans; Max*=trans; Init(); }
	void Transform(cAABB& box, const mat4& pMat) const;
	bool InsidePlane(const Plane & plane) const;
	void GetCorners(vec3* arr)  const; // the array is assumed to always have 8 slots
	vec3 operator[](size_t i) const{return vec3( (i&1) ? Max.x : Min.x, (i&2) ? Max.y : Min.y, (i&4) ? Max.z : Min.z );} ;// for getting points on the cube.. only valid up to index 0 through 7
	
};


#endif