#include "stdafx.h"
#include "AABB.h"
#include "Sphere.h"
#include "OBB.h"

void AABB::Init(const Sphere& shp) { Max = vec3(shp.Radius, shp.Radius, shp.Radius); Min = -Max;} 
void AABB::Init(const AABB& shp) { Max = shp.Max; Min = shp.Min;}
void AABB::Init(const OBB& shp) {	Max = vec3(shp.XSize(), shp.YSize(), shp.ZSize()); Min = -Max;}

void AABB::Add(const vec3& p){ // caller should call Init when done inserting points... 
	if(p.x < Min.x) Min.x = p.x;
	if(p.x > Max.x) Max.x = p.x;
	if(p.y < Min.y) Min.y = p.y;
	if(p.y > Max.y) Max.y = p.y;
	if(p.z < Min.z) Min.z = p.z;
	if(p.z > Max.z) Max.z = p.z;
}
void AABB::SetSize(const vec3& p){// this will not move the box, only scale it
	vec3 scalar(p.x/XSize(), p.y/YSize(), p.z/ZSize());
	Max*=scalar;
	Min*=scalar;
}

bool AABB::Intersect(const AABB& A) const{
	if(Min.x > A.Max.x) return false;
	else if(Max.x < A.Min.x) return false;
	else if(Min.z > A.Max.z) return false;
	else if(Max.z < A.Min.z) return false;
	else if(Min.y > A.Max.y) return false;
	else if(Max.y < A.Min.y) return false;
	return true;
}
bool AABB::Intersect(const vec3& pos) const{
	if(Min.x > pos.x) return false;
	else if(Max.x < pos.x) return false;
	else if(Min.z > pos.z) return false;
	else if(Max.z < pos.z) return false;
	else if(Min.y > pos.y) return false;
	else if(Max.y < pos.y) return false;
	return true;
}
bool AABB::Intersect(const Sphere& sphere) const{
    float d = 0;
    for( size_t i=0 ; i<3 ; i++ ){
		float e=sphere.Center[i]- Min[i];
        if(e<0 ){
			if(e < -sphere.Radius) return false;
			d += e*e;
		}
		e=sphere.Center[i] - Max[i];
        if(e>0 ){
			if(e > sphere.Radius) return false;
            d += e*e; 
        }
    }
	return d <= sphere.Radius*sphere.Radius;
}
bool AABB::Intersect(const Plane& n) const{
	float minD, maxD;
	if(n.a >0.0f){
		minD = n.a*Min.x; maxD = n.a*Max.x;
	} else {
		minD = n.a*Max.x; maxD = n.a*Min.x;
	}
	if(n.b >0.0f){
		minD += n.b*Min.y; maxD += n.b*Max.y;
	} else {
		minD += n.b*Max.y; maxD += n.b*Min.y;
	}
	if(n.c >0.0f){
		minD += n.c*Min.z; maxD += n.c*Max.z;
	} else {
		minD += n.c*Max.z; maxD += n.c*Min.z;
	}
	if(minD>=n.d) {
		return false;// compleltly on the front side
	}
	if(maxD<=n.d) {
		return true;// compleltly on the back side
	}
	return false;// straddle the plane
}
bool AABB::InsideOf(const Plane& n) const{
	vec3 h = (Max - Min)*.5f;
	vec3 pn(fabs(n.a), fabs(n.b), fabs(n.c));
	float e = Dot(h, pn);
	float s = Dot(GetCenter(), n) + n.d;
	if(s-e > 0) return true;
	if(s+e < 0) return false;
	return true;
}

void AABB::GetCorners(vec3* arr) const{
	arr[0]=vec3(Max.x, Max.y, Max.z);
	arr[1]=vec3(Max.x, Max.y, -Min.z);
	arr[2]=vec3(Max.x, -Min.y, Max.z);
	arr[3]=vec3(Max.x, -Min.y, -Min.z);
	arr[4]=vec3(-Min.x, Max.y, Max.z);
	arr[5]=vec3(-Min.x, Max.y, -Min.z);
	arr[6]=vec3(-Min.x, -Min.y, Max.z);
	arr[7]=vec3(-Min.x, -Min.y, -Min.z);
}


