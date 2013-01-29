#include "stdafx.h"
#include "OBB.h"
#include "AABB.h"
/*
bool OBB::Intersect(const OBB &b) const{
	//Cheap Sphere-Sphere intersection test
	float l = (b.GetCenter() - GetCenter()).LengthSq();
	float myrad = LargestAxis();
	float orad=b.LargestAxis();
	if(l > (orad*orad + myrad*myrad)) return false;
	//More expensive OBB-OBB intersection test
	
	mat4 r1(MatRot);
	r1.inverse();
	mat4 matB(b.MatRot*r1);
	vec3 vCenterB((b.Center - Center)*r1);
	vec3 XAxis(matB._11,matB._21,matB._31);
	vec3 YAxis(matB._12,matB._22,matB._32);
	vec3 ZAxis(matB._13,matB._23,matB._33);
	float x_size(XSize()), y_size(YSize()), z_size(ZSize());
	//15 tests
	//1 (Ra)x
	if(fabs(vCenterB.x) > x_size + b.x_size * fabs(XAxis.x) + b.Size.y * fabs(XAxis.y) + b.Size.z * fabs(XAxis.z))
		return false;
	//2 (Ra)y
	if(fabs(vCenterB.y) > Size.y + b.x_size * fabs(YAxis.x) + b.Size.y * fabs(YAxis.y) + b.Size.z * fabs(YAxis.z))
		return false;
	//3 (Ra)z
	if(fabs(vCenterB.z) > Size.z + b.x_size * fabs(ZAxis.x) + b.Size.y * fabs(ZAxis.y) + b.Size.z * fabs(ZAxis.z))
		return false;

	//4 (Rb)x
	if(fabs(vCenterB.x*XAxis.x+vCenterB.y*YAxis.x+vCenterB.z*ZAxis.x) > (b.x_size+x_size*fabs(XAxis.x) + Size.y * fabs(YAxis.x) + Size.z*fabs(ZAxis.x)))
		return false;
	//5 (Rb)y
	if(fabs(vCenterB.x*XAxis.y+vCenterB.y*YAxis.y+vCenterB.z*ZAxis.y) > (b.Size.y+x_size*fabs(XAxis.y) + Size.y * fabs(YAxis.y) + Size.z*fabs(ZAxis.y)))
		return false;
	//6 (Rb)z
	if(fabs(vCenterB.x*XAxis.z+vCenterB.y*YAxis.z+vCenterB.z*ZAxis.z) > (b.Size.z+x_size*fabs(XAxis.z) + Size.y * fabs(YAxis.z) + Size.z*fabs(ZAxis.z)))
		return false;

	//7 (Ra)x X (Rb)x
	if(fabs(vCenterB.z*YAxis.x-vCenterB.y*ZAxis.x) > Size.y*fabs(ZAxis.x) + Size.z*fabs(YAxis.x) + b.Size.y*fabs(XAxis.z) + b.Size.z*fabs(XAxis.y))
		return false;
	//8 (Ra)x X (Rb)y
	if(fabs(vCenterB.z*YAxis.y-vCenterB.y*ZAxis.y) > Size.y*fabs(ZAxis.y) + Size.z*fabs(YAxis.y) + b.x_size*fabs(XAxis.z) + b.Size.z*fabs(XAxis.x))
		return false;
	//9 (Ra)x X (Rb)z
	if(fabs(vCenterB.z*YAxis.z-vCenterB.y*ZAxis.z) > Size.y*fabs(ZAxis.z) + Size.z*fabs(YAxis.z) + b.x_size*fabs(XAxis.y) + b.Size.y*fabs(XAxis.x))
		return false;

	//10 (Ra)y X (Rb)x
	if(fabs(vCenterB.x*ZAxis.x-vCenterB.z*XAxis.x) > x_size*fabs(ZAxis.x) + Size.z*fabs(XAxis.x) + b.Size.y*fabs(YAxis.z) + b.Size.z*fabs(YAxis.y))
		return false;
	//11 (Ra)y X (Rb)y
	if(fabs(vCenterB.x*ZAxis.y-vCenterB.z*XAxis.y) > x_size*fabs(ZAxis.y) + Size.z*fabs(XAxis.y) + b.x_size*fabs(YAxis.z) + b.Size.z*fabs(YAxis.x))
		return false;
	//12 (Ra)y X (Rb)z
	if(fabs(vCenterB.x*ZAxis.z-vCenterB.z*XAxis.z) > x_size*fabs(ZAxis.z) + Size.z*fabs(XAxis.z) + b.x_size*fabs(YAxis.y) + b.Size.y*fabs(YAxis.x))
		return false;

	//13 (Ra)z X (Rb)x
	if(fabs(vCenterB.y*XAxis.x-vCenterB.x*YAxis.x) > x_size*fabs(YAxis.x) + Size.y*fabs(XAxis.x) + b.Size.y*fabs(ZAxis.z) + b.Size.z*fabs(ZAxis.y))
		return false;
	//14 (Ra)z X (Rb)y
	if(fabs(vCenterB.y*XAxis.y-vCenterB.x*YAxis.y) > x_size*fabs(YAxis.y) + Size.y*fabs(XAxis.y) + b.x_size*fabs(ZAxis.z) + b.Size.z*fabs(ZAxis.x))
		return false;
	//15 (Ra)z X (Rb)z
	if(fabs(vCenterB.y*XAxis.z-vCenterB.x*YAxis.z) > x_size*fabs(YAxis.z) + Size.y*fabs(XAxis.z) + b.Size.x*fabs(ZAxis.y) + b.Size.y*fabs(ZAxis.x))
		return false;
		
	return true;
}

float OBB::Intersect(const vec3& ray, const vec3& origin) const {
	vec3 org(origin*InvWorld);
	vec3 dir;
	TransformNormal(ray, dir, InvWorld);
	return RayAABBIntersect(-Size, Size, org, dir);
}
bool OBB::Intersect(const vec3 &point) const {
	vec3 pnt(point*InvWorld);
	if(abs(pnt.x) > Size.x)return false;
	if(abs(pnt.y) > Size.y)return false;
	if(abs(pnt.z) > Size.z)return false;
	return true;
}

void OBB::GetContactPoint(const vec3 &raydir, const vec3& rayorigin, vec3& outraydir, vec3& outrayorigin){
	vec3 org(rayorigin*InvWorld), dir;
	TransformNormal(raydir, dir, InvWorld);
	Plane planes[] = {	Plane(0.0f, 0.0f, -1.0f, -Size.z),
						Plane(0.0f, 0.0f, 1.0f,  -Size.z),
						Plane(0.0f, -1.0f, 0.0f, -Size.y),
						Plane(0.0f, 1.0f, 0.0f,  -Size.y),
						Plane(-1.0f, 0.0f, 0.0f, -Size.x),
						Plane(1.0f, 0.0f, 0.0f,  -Size.x)};
	vec3 result, normal;
	int numPlanes = 0;
	int numIntersections = 0;
	for(int i=0; i<6; i++){
		float d = org.x * planes[i].a + org.y * planes[i].b + org.z * planes[i].c;
		if(d > -planes[i].d){
			vec3 r;
			if(::Intersect(dir, org, planes[i], r)){// need the global namespace there otherwise, the compiler cant see the function call
				numPlanes++;				
				if((abs(r.x) <= Size.x) & (abs(r.y) <= Size.y) & (abs(r.z) <= Size.z)){
					r*=World;
					outrayorigin = r;
					normal = vec3(planes[i].a, planes[i].b, planes[i].c);
					numIntersections++;
				}
			} 
		}
	}
	if(numIntersections == 0){ //Warning! OBB No Intersections!
		outraydir = -raydir;
		outrayorigin = rayorigin;
	}
	TransformNormal(normal, outraydir, World);
}
*/