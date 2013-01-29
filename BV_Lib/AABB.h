#ifndef AABB_H
#define AABB_H
#include "Base_BV.h"
class Sphere;
class OBB;

class AABB final: public Base_BV{
public:

	vec3 Min, Max;

	AABB(){ clear(); }
	AABB(const AABB& o){ Init(o); }
	AABB(const Sphere& o){ Init(o); }
	AABB(const OBB& o){ Init(o); }
	AABB(const vec3& min, const vec3& max);

	virtual ~AABB(){}

	virtual void clear() override{ Max=vec3(-INFINITY, -INFINITY, -INFINITY); Min=vec3(INFINITY, INFINITY, INFINITY); }

	virtual void Init(const Sphere& shp) override;
	virtual void Init(const AABB& shp) override;
	virtual void Init(const OBB& shp) override;

	virtual void Add(const vec3& p) override;
	virtual void SetSize(const vec3& p) override;
	//mat4 MUST NOT HAVE ROTATION, if it does the AABB will be an OBB and the code will not work.
	virtual void Transform(const mat4& m) override{  Max*=m; Min*=m; }

	virtual vec3 GetCenter() const override{ return ((Max-Min)/2.0f)+Min;}
	virtual float XSize() const override{ return Max.x-Min.x;}
	virtual float YSize() const override{ return Max.y-Min.y;}
	virtual float ZSize() const override{ return Max.z-Min.z;}

	virtual float RayIntersect(const vec3& rayOrg, const vec3& rayDir)const override{ return RayAABBIntersect(Min, Max, rayOrg, rayDir); }

	virtual bool Intersect(const Plane& n) const override;
	virtual bool Intersect(const Sphere& n) const override;
	virtual bool Intersect(const AABB& n) const override;
	virtual bool Intersect(const OBB& n) const override{return false;}
	virtual bool Intersect(const vec3& n) const override;

	virtual bool InsideOf(const Plane& n) const override;
	virtual bool InsideOf(const Sphere& n) const override{return false;}
	virtual bool InsideOf(const AABB& n) const override{return false;}
	virtual bool InsideOf(const OBB& n) const override{return false;}
	virtual bool InsideOf(const vec3& n) const override{ return Intersect(n); }

	void GetCorners(vec3* arr)  const; // the array is assumed to always have 8 slots
	vec3 operator[](size_t i) const{return vec3( (i&1) ? Max.x : Min.x, (i&2) ? Max.y : Min.y, (i&4) ? Max.z : Min.z );} ;// for getting points on the cube.. only valid up to index 0 through 7
	
};


#endif