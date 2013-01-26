#include "stdafx.h"
#include "cAABB.h"

cAABB::cAABB(): Max(vec3(-INFINITY, -INFINITY, -INFINITY)), Min(vec3(INFINITY, INFINITY, INFINITY)){}// set min and max to opposites, so when new points are added I can use the Add function correctly
cAABB::cAABB(const cAABB& object){ memcpy(this, &object, sizeof(cAABB)); }
cAABB::cAABB(vec3* min, vec3* max): Min(*min), Max(*max) { Init(); }
cAABB::cAABB(vec3& min, vec3& max): Min(min), Max(max) { Init(); }
cAABB::cAABB(const cSphere& shp){
	Min.zero();
	Max = vec3(shp.Radius, shp.Radius, shp.Radius);
	Min+=shp.Center;
	Max+=shp.Center;
	Init();
}
void cAABB::clear(){
	Max=vec3(-INFINITY, -INFINITY, -INFINITY);
	Min=vec3(INFINITY, INFINITY, INFINITY);
	cSphere::clear();
}
float cAABB::XSize() const{ return Max.x-Min.x;}
float cAABB::YSize() const{ return Max.y-Min.y;}
float cAABB::ZSize() const{ return Max.z-Min.z;}
void cAABB::MoveByOffset(const vec3& offset){
	Max+=offset;
	Min+=offset;
	cSphere::MoveByOffset(offset);
}

void cAABB::MoveTo(const vec3& newpos){
	vec3 half(Center - Max);
	Max=newpos + Max;
	Min=newpos - Min;
	cSphere::MoveTo(newpos);
}

void cAABB::Add(const vec3& p){ // caller should call Init when done inserting points... 
	if(p.x < Min.x) Min.x = p.x;
	if(p.x > Max.x) Max.x = p.x;
	if(p.y < Min.y) Min.y = p.y;
	if(p.y > Max.y) Max.y = p.y;
	if(p.z < Min.z) Min.z = p.z;
	if(p.z > Max.z) Max.z = p.z;
}
void cAABB::Init(){// the biggest axis
	vec3 t(Max - Min);// get the std::vector between the two
	t*=.5f;// divide it by half
	Radius = t.Length();// the lenfth of it is the radius
	Center=t + Min;// add the std::vector onto the min to get the center
}

bool cAABB::Intersect(const cAABB& A, cAABB* boxintersect) const{

	if(Min.x > A.Max.x) return false;
	else if(Max.x < A.Min.x) return false;
	else if(Min.z > A.Max.z) return false;
	else if(Max.z < A.Min.z) return false;
	else if(Min.y > A.Max.y) return false;
	else if(Max.y < A.Min.y) return false;
	if(boxintersect){
		boxintersect->Min.x=max(Min.x, A.Min.x);
		boxintersect->Max.x=min(Max.x, A.Max.x);
		boxintersect->Min.y=max(Min.y, A.Min.y);
		boxintersect->Max.y=min(Max.y, A.Max.y);
		boxintersect->Min.z=max(Min.z, A.Min.z);
		boxintersect->Max.z=min(Max.z, A.Max.z);
	}
	return true;
}
bool cAABB::Intersect(const vec4& pos) const{
	if(Min.x > pos.x) return false;
	else if(Max.x < pos.x) return false;
	else if(Min.z > pos.z) return false;
	else if(Max.z < pos.z) return false;
	else if(Min.y > pos.y) return false;
	else if(Max.y < pos.y) return false;
	return true;
}
bool cAABB::Intersect(const cSphere& sphere) const{
	if(cSphere::Insertsect(sphere)) return true;
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
bool cAABB::Intersect(const Plane& n) const{
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
bool cAABB::InsidePlane(const Plane& n) const{
	vec3 h = (Max - Min)*.5f;
	vec3 pn(fabs(n.a), fabs(n.b), fabs(n.c));
	float e = Dot(h, pn);
	float s = Dot(Center, n) + n.d;
	if(s-e > 0) return true;
	if(s+e < 0) return false;
	return true;
}
void cAABB::GetCorners(vec3* arr) const{
	arr[0]=vec3(Max.x, Max.y, Max.z);
	arr[1]=vec3(Max.x, Max.y, -Min.z);
	arr[2]=vec3(Max.x, -Min.y, Max.z);
	arr[3]=vec3(Max.x, -Min.y, -Min.z);
	arr[4]=vec3(-Min.x, Max.y, Max.z);
	arr[5]=vec3(-Min.x, Max.y, -Min.z);
	arr[6]=vec3(-Min.x, -Min.y, Max.z);
	arr[7]=vec3(-Min.x, -Min.y, -Min.z);
}

void cAABB::Transform(cAABB& box, const mat4& pMat) const{
	vec3 vCenter;
	vec3 vDiagonal;
	Centroid( &vCenter );
	HalfDiag( &vDiagonal );
	vec3 vAxisX( vDiagonal.x, 0.0f, 0.0f );
	vec3 vAxisY( 0.0f, vDiagonal.y, 0.0f );
	vec3 vAxisZ( 0.0f, 0.0f, vDiagonal.z );

	// Transform the center position and the axis std::vectors.
	vCenter*=pMat;
	TransformNormal(vAxisX, pMat );
	TransformNormal(vAxisY, pMat );
	TransformNormal(vAxisZ, pMat );

	vDiagonal = vec3(
		fabsf( vAxisX.x ) + fabsf( vAxisY.x ) + fabsf( vAxisZ.x ),
		fabsf( vAxisX.y ) + fabsf( vAxisY.y ) + fabsf( vAxisZ.y ),
		fabsf( vAxisX.z ) + fabsf( vAxisY.z ) + fabsf( vAxisZ.z ) );
	box.Max=vCenter+vDiagonal;
	box.Min=vCenter-vDiagonal;
}


