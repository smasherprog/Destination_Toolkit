#ifndef CAMERA_H
#define CAMERA_H
#include "../Utilities/MyMath.h"
#include "../Physics_Lib/cAABB.h"


class cCamera{
public:

	vec3 Position, Right, Up, Look;
	mat4 View, Proj, VP, InvView, InvProj, InvVP, InvVPToScreen;
	mat4 Trans_View, Trans_Proj, Trans_VP, Trans_InvView, Trans_InvProj, Trans_InvVP, Trans_InvVPToScreen;
	Plane Planes[6];// frustum planes... normalized, built each frame
	float NearPlane, FarPlane, Width, Height, Fov, Aspect, Walking, Strafing, Rotating, MetresPerSecond;
	

	cCamera();

	void PerFrame();
	void OnResize(float h, float w);
	void setLens(float fovY, float height, float width, float zn, float zf);
	void strafe(float dt);
	void walk(float dt);
	void Pitch(int32_t deltay);
	void Heading(int32_t deltax);

	void rebuildView();
	bool InsideFrustum(const vec3& point);
	bool InsideFrustum(const vec3& point, float radiussqrd);// for spheres
	bool InsideFrustum(const cAABB& bv);// do an exact test against the BV


};
bool InsideFrustum(const vec3& point, const Plane planes[6]);
bool InsideFrustum(const cAABB& bv, const Plane planes[6]);
bool InsideFrustum(const vec3& point, float radius, const Plane planes[6]);
void ExtractPlanesD3D(Plane* p_planes, const mat4& comboMatrix, bool normalize = false);

inline void NormalizePlane(Plane & plane){
	float mag = 1.0f/sqrtf(plane.a * plane.a + plane.b * plane.b + plane.c * plane.c);
	plane.a = plane.a * mag;
	plane.b = plane.b * mag;
	plane.c = plane.c * mag;
	plane.d = plane.d * mag;
}

extern cCamera Camera;


#endif 