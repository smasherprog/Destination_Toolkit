#include "stdafx.h"
#include "cSphere.h"

bool cSphere::SweepIntersect( const vec3& newpos,const Plane& plane, vec3& hitpos, float& timeofhit){
	const float d0 = plane.LengthToPoint( Center );
	const float d1 = plane.LengthToPoint( newpos );
	if( fabsf(d0) <= Radius){//check if it was touching on previous frame
		hitpos = Center;
		timeofhit = 0;
		return true; 
	}
	if( (d0>Radius) & (d1<Radius) ){//check if the sphere penetrated during this frame
		timeofhit = (d0-Radius)/(d0-d1); //normalized time
		hitpos = (1-timeofhit)*Center + timeofhit*newpos; //point of first contact
		return true; 
	}
	return false;
} 

float cSphere::RayIntersect(const vec3& rayOrigin, const vec3& rayDir){
	vec3 l(Center - rayOrigin);
	float s(Dot(l, rayDir));// Lengthance
	float lsq(Dot(l, l));
	if((s < 0.0f) & (lsq > Radius*Radius)) return INFINITY;// missed the sphere
	float msq(lsq - s*s);
	if(msq > Radius*Radius) return INFINITY;// missed the sphere
	return s;
}
