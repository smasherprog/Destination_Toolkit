#ifndef OBB_H 
#define OBB_H
#include "../Utilities/MyMath.h"
#include "AABB.h"

class OBB final: public Base_BV{
public:

	OBB(){};
	virtual ~OBB(){}

	virtual void clear()  override{ BV.clear();}

	virtual void Init(const Sphere& shp) override{ BV.Init(shp);}
	virtual void Init(const AABB& shp) override{ BV.Init(shp);}
	virtual void Init(const OBB& shp) override{ BV.Init(shp);}

	virtual void Add(const vec3& p) override{ BV.Add(p);}
	virtual void Add(const float x, const float y, const float z) override{ Add(vec3(x, y, z)); }
	virtual void SetSize(const vec3& p) override{ BV.Add(p);}
	virtual void SetSize(const float x, const float y, const float z) override{ SetSize(vec3(x, y, z)); }
	// its best to just pass rotation information here
	virtual void Transform(const mat4& m) override{ World*=m; InvWorld=World; InvWorld.inverse(); }

	virtual vec3 GetCenter() const override{ return BV.GetCenter()*World; }
	virtual float XSize() const{ return BV.XSize();}
	virtual float YSize() const{ return BV.YSize();}
	virtual float ZSize() const{ return BV.ZSize();}

	virtual float RayIntersect(const vec3& rayOrg, const vec3& rayDir)const override {return INFINITY;}

	virtual bool Intersect(const Plane& n)const override {return false;}
	virtual bool Intersect(const Sphere& n) const override {return false;}
	virtual bool Intersect(const AABB& n) const override {return false;}
	virtual bool Intersect(const OBB& n) const override {return false;}
	virtual bool Intersect(const vec3& n) const override {return false;}

	virtual bool InsideOf(const Plane& n) const override {return false;}
	virtual bool InsideOf(const Sphere& n) const override {return false;}
	virtual bool InsideOf(const AABB& n) const override {return false;}
	virtual bool InsideOf(const OBB& n)const override {return false;}
	virtual bool InsideOf(const vec3& n) const override {return false;}

private:
	AABB BV;
	mat4 World, InvWorld;
};

#endif