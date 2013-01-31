#ifndef Sphere_H
#define Sphere_H
#include "Base_BV.h"
class AABB;

class Sphere final: public Base_BV{
public:

	vec3 Center;
	float Radius;
	
	Sphere(){ clear(); }
	Sphere(const AABB& o){ Init(o); }
	Sphere(const Sphere& o){ Init(o); }
	Sphere(const OBB& o){ Init(o); }
	Sphere(vec3 center, float radius) : Center(center), Radius(radius) {}

	virtual void clear() override { Center.zero(); Radius =1.0f; }

	virtual void Init(const Sphere& shp) override;
	virtual void Init(const AABB& shp) override;
	virtual void Init(const OBB& shp) override;

	///FINISH ADD FUNCTION.. should expand sphere to include point if its not within its bounds
	virtual void Add(const vec3& p) override{}
	virtual void SetSize(const vec3& p) override{Radius = max(max(p.x,p.y), p.z);}
	virtual void Transform(const mat4& m) override{	Center*=m; Radius*= max(max(m._11,m._22), m._33);}

	virtual vec3 GetCenter() const override{ return Center;}
	virtual float XSize() const override{ return Radius*2.0f;}
	virtual float YSize() const override{ return Radius*2.0f;}
	virtual float ZSize() const override{ return Radius*2.0f;}

	virtual float RayIntersect(const vec3& rayOrg, const vec3& rayDir)const override{ return Sphere_RayIntersect(Center, Radius, rayOrg, rayDir);}

	virtual bool Intersect(const Plane& n) const override{return false;}
	virtual bool Intersect(const Sphere& n) const override;
	virtual bool Intersect(const AABB& n) const override;
	virtual bool Intersect(const OBB& n) const override{return false;}
	virtual bool Intersect(const vec3& n) const override;

	virtual bool InsideOf(const Plane& n) const override{return false;}
	virtual bool InsideOf(const Sphere& n) const override{ return !Intersect(n);}
	virtual bool InsideOf(const AABB& n) const override{return false;}
	virtual bool InsideOf(const OBB& n) const override{return false;}
	virtual bool InsideOf(const vec3& n) const override { return Intersect(n);}

};




#endif