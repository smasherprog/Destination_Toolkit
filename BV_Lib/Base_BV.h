#ifndef BASE_BV_H
#define BASE_BV_H
#include "../Utilities/MyMath.h"
class Sphere;
class AABB;
class OBB;

class Base_BV{
public:

	Base_BV(){};
	virtual ~Base_BV(){}

	virtual void clear()=0;

	virtual void Init(const Sphere& shp)=0;
	virtual void Init(const AABB& shp)=0;
	virtual void Init(const OBB& shp)=0;

	virtual void Add(const vec3& p)=0;
	virtual void Add(const float x, const float y, const float z){ Add(vec3(x, y, z)); }
	virtual void SetSize(const vec3& p)=0;
	virtual void SetSize(const float x, const float y, const float z){ SetSize(vec3(x, y, z)); }

	virtual void Transform(const mat4& m)=0;

	virtual vec3 GetCenter() const=0;
	virtual float XSize() const{ return 1.0f;}
	virtual float YSize() const{ return 1.0f;}
	virtual float ZSize() const{ return 1.0f;}
	virtual float LargestAxis() const { return max(max(XSize(),YSize()), ZSize()); }

	virtual float RayIntersect(const vec3& rayOrg, const vec3& rayDir)const=0;

	virtual bool Intersect(const Plane& n) const=0;
	virtual bool Intersect(const Sphere& n) const=0;
	virtual bool Intersect(const AABB& n) const=0;
	virtual bool Intersect(const OBB& n) const=0;
	virtual bool Intersect(const vec3& n) const=0;

	virtual bool InsideOf(const Plane& n) const=0;
	virtual bool InsideOf(const Sphere& n) const=0;
	virtual bool InsideOf(const AABB& n) const=0;
	virtual bool InsideOf(const OBB& n) const=0;
	virtual bool InsideOf(const vec3& n) const=0;

	virtual bool OutsideOf(const Plane& n) const{ return !Intersect(n);}
	virtual bool OutsideOf(const Sphere& n) const{ return !Intersect(n);}
	virtual bool OutsideOf(const AABB& n) const{ return !Intersect(n);}
	virtual bool OutsideOf(const OBB& n) const{ return !Intersect(n);}
	virtual bool OutsideOf(const vec3& n) const{ return !Intersect(n);}
};


#endif