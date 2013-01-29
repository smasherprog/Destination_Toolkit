#include "stdafx.h"
#include "Sphere.h"
#include "AABB.h"
#include "OBB.h"

void Sphere::Init(const Sphere& shp) { Center = shp.Center; Radius = shp.Radius;}
void Sphere::Init(const AABB& shp) { Center = shp.GetCenter(); Radius = shp.LargestAxis();}
void Sphere::Init(const OBB& shp) {	Center = shp.GetCenter(); Radius = shp.LargestAxis();}

bool Sphere::Intersect(const Sphere& n) const{
	vec3 h(n.Center - Center);  
	float d(Dot(h, h));
	if(d > n.Radius*n.Radius + Radius*Radius) return false;
	return true;
}
bool Sphere::Intersect(const AABB& n) const{
	return n.Intersect(*this);
}
bool Sphere::Intersect(const vec3& n) const{
	vec3 h(n-Center);	
	float d(Dot(h, h));
	if(d > Radius*Radius) return false;
	return true;
}