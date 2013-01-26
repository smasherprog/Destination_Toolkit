#include "stdafx.h"
#include "MyMath.h"

uint32_t ABSVAL = 0x7FFFFFF;
const vec4 ABSROWVAL(*(reinterpret_cast<float*>(&ABSVAL)), *(reinterpret_cast<float*>(&ABSVAL)), *(reinterpret_cast<float*>(&ABSVAL)), *(reinterpret_cast<float*>(&ABSVAL)) );
	
bool Intersect(const vec3& raydir, const vec3& rayorigin, const Plane& plane, vec3& pointhit){// assumes an infinite length ray
	float bot(Dot(raydir, plane));
	if(bot >= 0) return false;// no intersection, the plane and the ray are parallel or the ray points in the same direction as the plane normal

	float scale=(-Dot(rayorigin, plane) - plane.d)/bot;// this is the length of the ray needed to intersect with the plane . . . 
	pointhit = (raydir*scale) + rayorigin;// scale the raydir to the correct length, then add that onto the orgin to correctly translate the point
	return true;
}
//rayDelta is NOT normalized, meaning if you want to test for an aabb hit, you must scale the ray to ensure its long enough to hit it
float RayAABBIntersect(const vec3& min, const vec3& max, const vec3& rayOrg, const vec3& rayDelta){
	// check for point inside box, trivial reject
	const float kNoIntersection = INFINITY;

	// Check for point inside box, trivial reject, and determine parametric
	// distance to each front face

	bool inside = true;

	float xt, xn;
	if (rayOrg.x < min.x) {
		xt = min.x - rayOrg.x;
		if (xt > rayDelta.x) return kNoIntersection;
		xt /= rayDelta.x;
		inside = false;
		xn = -1.0f;
	} else if (rayOrg.x > max.x) {
		xt = max.x - rayOrg.x;
		if (xt < rayDelta.x) return kNoIntersection;
		xt /= rayDelta.x;
		inside = false;
		xn = 1.0f;
	} else {
		xt = -1.0f;
	}

	float yt, yn;
	if (rayOrg.y < min.y) {
		yt = min.y - rayOrg.y;
		if (yt > rayDelta.y) return kNoIntersection;
		yt /= rayDelta.y;
		inside = false;
		yn = -1.0f;
	} else if (rayOrg.y > max.y) {
		yt = max.y - rayOrg.y;
		if (yt < rayDelta.y) return kNoIntersection;
		yt /= rayDelta.y;
		inside = false;
		yn = 1.0f;
	} else {
		yt = -1.0f;
	}

	float zt, zn;
	if (rayOrg.z < min.z) {
		zt = min.z - rayOrg.z;
		if (zt > rayDelta.z) return kNoIntersection;
		zt /= rayDelta.z;
		inside = false;
		zn = -1.0f;
	} else if (rayOrg.z > max.z) {
		zt = max.z - rayOrg.z;
		if (zt < rayDelta.z) return kNoIntersection;
		zt /= rayDelta.z;
		inside = false;
		zn = 1.0f;
	} else {
		zt = -1.0f;
	}

	// Inside box?

	if (inside) {
		return 0.0f;
	}

	// Select farthest plane - this is
	// the plane of intersection.

	int which = 0;
	float t = xt;
	if (yt > t) {
		which = 1;
		t = yt;
	}
	if (zt > t) {
		which = 2;
		t = zt;
	}

	switch (which) {

		case 0: // intersect with yz plane
		{
			float y = rayOrg.y + rayDelta.y*t;
			if (y < min.y || y > max.y) return kNoIntersection;
			float z = rayOrg.z + rayDelta.z*t;
			if (z < min.z || z > max.z) return kNoIntersection;
		} break;

		case 1: // intersect with xz plane
		{
			float x = rayOrg.x + rayDelta.x*t;
			if (x < min.x || x > max.x) return kNoIntersection;
			float z = rayOrg.z + rayDelta.z*t;
			if (z < min.z || z > max.z) return kNoIntersection;
		} break;

		case 2: // intersect with xy plane
		{
			float x = rayOrg.x + rayDelta.x*t;
			if (x < min.x || x > max.x) return kNoIntersection;
			float y = rayOrg.y + rayDelta.y*t;
			if (y < min.y || y > max.y) return kNoIntersection;

		} break;
	}

	// Return parametric point of intersection

	return t;
}
vec2 RayRayIntersect(vec3& ray1, const vec3& origin1, float lengthof2, vec3& ray2, const vec3& origin2){
	vec3 temp(Cross(ray1, ray2));// cross product of the normalized std::vectors
	float det = temp.LengthSq();// magnitude squared
	if(abs(det) < .01f) return vec2(INFINITY, INFINITY);// if det ==0.0f, but there might be floating point precision error, so make sure.. mmkay? this means the two rays are basically parallel
	det=1.0f/det;// inverse
	vec3 p2mp1(origin2-origin1);
	vec3 temp1(Cross(p2mp1, ray1));
	float t2 = Dot(temp1, temp)*det;
	if((t2 < 0.0f )| (t2 > lengthof2)) return vec2(INFINITY, INFINITY);// the rays did not intersect
	temp1 = Cross(p2mp1, ray2);
	float t1 = Dot(temp1, temp)*det;
	return vec2(t1, t2);// returns the distance each ray needs to be scaled to hit each other
}

vec2 RayRayIntersect(vec3& ray1, const vec3& origin1, vec3& ray2, const vec3& origin2){
	vec3 temp(Cross(ray1, ray2));// cross product of the normalized std::vectors
	float det = temp.LengthSq();// magnitude squared
	if(abs(det) < .01f) return vec2(INFINITY, INFINITY);// if det ==0.0f, but there might be floating point precision error, so make sure.. mmkay? this means the two rays are basically parallel
	det=1.0f/det;// inverse
	vec3 p2mp1(origin2-origin1);
	vec3 temp1(Cross(p2mp1, ray1));
	float t2 = Dot(temp1, temp)*det;
	temp1 = Cross(p2mp1, ray2);
	float t1 = Dot(temp1, temp)*det;
	return vec2(t1, t2);// returns the distance the rays need to be scaled by to intersect with each other
}
/* --------------------------------------QUAT------------------------------------ */
void quat::frommatrix(const mat4& pm){
	float wsqrdminus1 = pm._11 + pm._22 + pm._33;
	float xsqrdminus1 = pm._11 - pm._22 - pm._33;
	float ysqrdminus1 = pm._22 - pm._11 - pm._33;
	float zsqrdminus1 = pm._33 - pm._11 - pm._22;
	int biggestindex =0;
	float biggest = wsqrdminus1;
	if(xsqrdminus1 > biggest){
		biggest=xsqrdminus1;
		biggestindex=1;
	}
	if(ysqrdminus1 > biggest){
		biggest=ysqrdminus1;
		biggestindex=2;
	}
	if(zsqrdminus1 > biggest){
		biggest=zsqrdminus1;
		biggestindex=3;
	}
	float biggestval = sqrtf(biggest+ 1.0f)*.5f;
	float mult = .25f / biggestval;
	switch(biggestindex){
		case 0:
			w = biggestval;
			x= (pm._23 - pm._32) * mult;
			y= (pm._31 - pm._13) * mult;
			z= (pm._12 - pm._21) * mult;
			break;
		case 1:
			x = biggestval;
			w= (pm._23 - pm._32) * mult;
			y= (pm._12 + pm._21) * mult;
			z= (pm._31 + pm._13) * mult;
			break;
		case 2:
			y = biggestval;
			w= (pm._31 - pm._13) * mult;
			x= (pm._12 + pm._21) * mult;
			z= (pm._23 + pm._32) * mult;
			break;
		case 3:
			z = biggestval;
			w= (pm._13 - pm._21) * mult;
			x= (pm._31 + pm._13) * mult;
			y= (pm._23 + pm._32) * mult;
			break;
	};
}
void quat::SetupRotation(const euler &orientation){ SetupRotation(orientation.heading, orientation.pitch, orientation.bank); }
/* --------------------------------------EULER------------------------------------ */
void euler::fromObjectToWorldMatrix(const mat4 &m) {// The matrix is assumed to be orthogonal.  The translation portion is ignored.
	float	sp = -m._32;// Extract sin32(pitch) from _32.
	if (fabs(sp) > 9.99999f) {// Check for Gimbel lock
		pitch = PIOVERTWO * sp;// Looking straight up or down
		heading = atan2(-m._23, m._11);// Compute heading, slam bank to zero
		bank = 0.0f;
	} else {
		heading = atan2(m._31, m._33);
		pitch = asinf(sp);
		bank = atan2(m._12, m._22);
	}
}


/* --------------------------------------MAT4------------------------------------ */

void mat4::operator/=(const float &b){ *this*=(1.0f/b); }
void mat4::operator*=(const mat4 &b) { *this = *this * b; }

bool InsideFrustum(const vec3& point, const Plane planes[6]){// make sure the point is in WORLD SPACE.. as the view frustum is in WORLD SPACE
	for(size_t i(0); i< 6; i++){// this will test all 6 planes, but will return as early as possible		
		if(Dot(point, planes[i]) - planes[i].d <= 0.0f) return false;// early out, if outside of one, outside of all....
	}
	return true;// inside the frustum. Unfortunaly, all planes must be checked 
}
bool InsideFrustum(const vec3& point, float radius,const Plane planes[6]){// for spheres. Take not that I use squared Length below..
	for(size_t i=0; i<6; i++){    
		if(Dot(point, planes[i]) + planes[i].d <= -radius ) return false;
	}
	return true;
}

void ExtractPlanesD3D( Plane* p_planes, const mat4& comboMatrix, bool normalize) {
	// Left clipping plane
	p_planes[0].a = comboMatrix._14 + comboMatrix._11;
	p_planes[0].b = comboMatrix._24 + comboMatrix._21;
	p_planes[0].c = comboMatrix._34 + comboMatrix._31;
	p_planes[0].d = comboMatrix._44 + comboMatrix._41;
	// Right clipping plane
	p_planes[1].a = comboMatrix._14 - comboMatrix._11;
	p_planes[1].b = comboMatrix._24 - comboMatrix._21;
	p_planes[1].c = comboMatrix._34 - comboMatrix._31;
	p_planes[1].d = comboMatrix._44 - comboMatrix._41;
	// Near clipping plane
	p_planes[2].a = comboMatrix._13;
	p_planes[2].b = comboMatrix._23;
	p_planes[2].c = comboMatrix._33;
	p_planes[2].d = comboMatrix._43;
	// Far clipping plane
	p_planes[3].a = comboMatrix._14 - comboMatrix._13;
	p_planes[3].b = comboMatrix._24 - comboMatrix._23;
	p_planes[3].c = comboMatrix._34 - comboMatrix._33;
	p_planes[3].d = comboMatrix._44 - comboMatrix._43;
	// Top clipping plane
	p_planes[4].a = comboMatrix._14 - comboMatrix._12;
	p_planes[4].b = comboMatrix._24 - comboMatrix._22;
	p_planes[4].c = comboMatrix._34 - comboMatrix._32;
	p_planes[4].d = comboMatrix._44 - comboMatrix._42;
	// Bottom clipping plane
	p_planes[5].a = comboMatrix._14 + comboMatrix._12;
	p_planes[5].b = comboMatrix._24 + comboMatrix._22;
	p_planes[5].c = comboMatrix._34 + comboMatrix._32;
	p_planes[5].d = comboMatrix._44 + comboMatrix._42;
	// Normalize the plane equations, if requested
	if (normalize == true) {
		p_planes[0].Normalize();
		p_planes[1].Normalize();
		p_planes[2].Normalize();
		p_planes[3].Normalize();
		p_planes[4].Normalize();
		p_planes[5].Normalize();
	}
}
