#ifndef MYMATH_H
#define MYMATH_H
/*
/**
* MathLibrary
* Copyright (c) 2011 NoLimitsDesigns
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions
* are met:
* 1. Redistributions of source code must retain the above copyright
*    notice, this list of conditions and the following disclaimer.
* 2. Redistributions in binary form must reproduce the above copyright
*    notice, this list of conditions and the following disclaimer in the
*    documentation and/or other materials provided with the distribution.
* 3. Neither the name of the copyright holders nor the names of its
*    contributors may be used to endorse or promote products derived from
*    this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
* ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
* LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
* CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
* SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
* INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
* CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
* THE POSSIBILITY OF SUCH DAMAGE.

* If there are any concerns or questions about the code, please e-mail smasherprog@gmail.com or visit www.nolimitsdesigns.com
*/

/**
* Author: Scott Lee
*/
#include <intrin.h>
#include "float.h"
#include "math.h"
#include <iostream>
#include "stdint.h"
#include <algorithm>

#define INFINITY 3.402823466e+38F 
#define Pi  3.14159265358979323846f
#define INV_Pi  1.0f/Pi
#define PI_D_CI 2*Pi/360.0f//pi divided by circle
#define CI_D_PI  360.0f/(Pi*2)// circle divided by pi
#define MATH_EPS 0.0001f
#define TWOPI  (Pi * 2.0f)
#define ONEOVERTWOPI  (1.0f / TWOPI)
#define PIOVER180 (Pi / 180.0f)
#define ONEEIGHTYOVERPI  (180.0f / Pi)
#define PIOVERTWO (Pi /2.0f)
#ifndef max
#define max(a,b)            (((a) > (b)) ? (a) : (b))
#endif
#ifndef min
#define min(a,b)            (((a) < (b)) ? (a) : (b))
#endif

inline float wrapPi(float theta) {
	theta += Pi;
	theta -= floor(theta * ONEOVERTWOPI) * TWOPI;
	theta -= Pi;
	return theta;
}
inline void sincosf(float& s, float& c, const float theta){
	s = sinf(theta);
	c = cosf(theta);
}
//A floating-point value; 32-bit floating-point formats use IEEE 754 single-precision (s23e8 format): sign bit, 8-bit biased (127) exponent, and 23-bit mantissa. 
//16-bit floating-point formats use half-precision (s10e5 format): sign bit, 5-bit biased (15) exponent, and 10-bit mantissa.
inline uint16_t Uint16ToFloat16(uint16_t v){// this function is really only used for Direct x's  16 bit float format...
	float t(static_cast<float>(v));// convert it to a real float....
	uint16_t* p(reinterpret_cast<uint16_t*>(&t));
	uint16_t ret(0);// this is what will be returned
	ret|=*p>>13;// get the last two bytes of data, which is part of the mantisaa and shift off the extra data
	ret|=*(p+1)&0xC000;// get the sign of the number and the sign of the exponent
	ret|=(*(p+1)&0x07FF)<<3;// only get 4 bits of exponent, and the rest of the mantissa
	return ret;
}
inline float Float16ToFloat32(uint16_t v){
	float ret(0);
	uint16_t* r=reinterpret_cast<uint16_t*>(&ret);// this is what will be returned
	*(r+1)=(v&0x03ff);
	*reinterpret_cast<uint32_t*>(&ret)>>=3;//shift the mantisaa into place
	*(r+1)|=(v&0x3C00)>>3;
	*(r+1)|=v&0xC000;// get the sign
	return ret;
}
inline uint16_t Float32ToFloat16(float v){// this function is really only used for Direct x's  16 bit float format...
	uint16_t* p(reinterpret_cast<uint16_t*>(&v));
	uint16_t ret(0);// this is what will be returned
	ret|=*p>>13;// get the last two bytes of data, which is part of the mantisaa and shift off the extra data
	ret|=*(p+1)&0xC000;// get the sign of the number and the sign of the exponent
	ret|=(*(p+1)&0x07FF)<<3;// only get 4 bits of exponent, and the rest of the mantissa
	return ret;
}
// the Below code was taken from graphics.stanford.edu/~seander/bithacks.html 
static const int MultiplyDeBruijnBitPosition[32] = 
{
	0, 9, 1, 10, 13, 21, 2, 29, 11, 14, 16, 18, 22, 25, 3, 30,
	8, 12, 20, 28, 15, 17, 24, 7, 19, 27, 23, 6, 26, 5, 4, 31
};
// this works for 32 bit integers
inline unsigned int BSR(unsigned int v){
	v |= v >> 1; // first round down to one less than a power of 2 
	v |= v >> 2;
	v |= v >> 4;
	v |= v >> 8;
	v |= v >> 16;
	return MultiplyDeBruijnBitPosition[(unsigned int)(v * 0x07C4ACDDU) >> 27];
}

template<class T> inline void Swap( T &a, T &b ){ 
	const T temp(a);
	a = b;
	b= temp;
}

inline float CircleX(float x, float a, float b){// simple circle right meow, a and b are the height and width of the desired circle (NOT SQUARED) returns the y value, given a x
	return sqrtf ( (1.0f - (x*x)/(b*b)) *a*a);
}// I hope the compiler can optimize all of this away since the values will all be computable and known at runtime

// the following function will take the range [fromleft, fromright] and convert that range to [toleft, toright]
// x must be in the range [fromleft, fromright] and the returned value with be the value converted to the [toleft, toright]
struct cRange{
	float scale, invtrans, trans;
};
inline cRange CreateRange(float fromleft, float fromright, float toleft, float toright){
	float fromrange(abs(fromleft - fromright));
	float torange(abs(toleft - toright));
	cRange te;
	te.scale =torange/fromrange;
	te.invtrans = fromleft < fromright ? fromleft : fromright;// get the smallest number of the two for the invtrans
	te.trans = toleft < toright ? toleft : toright;// get the smallest number of the two for the trans
	return te;
}
inline float ChangeRange(float x, float fromleft, float fromright, float toleft, float toright){
	float fromrange(abs(fromleft - fromright));
	float torange(abs(toleft - toright));
	float scale(torange/fromrange);
	float invtrans(fromleft < fromright ? -fromleft : -fromright);
	float trans(toleft < toright ? toleft : toright);
	return (((x - invtrans) * scale) + trans);
}
inline float ChangeRange(float x, const cRange& range){
	return (((x - range.invtrans) * range.scale) + range.trans);
}
template<typename T>T Clamp(T Value, T Min, T Max){
	return (Value < Min) ? Min : (Value > Max) ? Max : Value;
}
#define DEGTORAD(deg) (deg*(Pi/180.0f))
#define RADTODEG(Rad) (Rad*(180.0f/Pi))
inline bool QuadraticFormula(const float a,const float b,const float c, float& r1, float& r2 ){
	const float q = b*b - 4*a*c;
	if( q >= 0 ){
		const float sq = sqrtf(q);
		const float d = 1 / (2*a);
		r1 = ( -b + sq ) * d;
		r2 = ( -b - sq ) * d;
		return true;//real roots
	}
	return false;//complex roots
} 

#define POWOFTWO(a) (a && ! (a & (a - 1) ) )// returns true if the number is a power of two.. false otherwise
inline float lerp(const float u, const float v, const float x) { return u * (1 - x) + v * x; }
inline float cerp(const float u0, const float u1, const float u2, const float u3, const float x){
	float p = (u3 - u2) - (u0 - u1);
	float q = (u0 - u1) - p;
	float r = u2 - u0;
	return x * (x * (x * p + q) + r) + u1;
}

inline float herp(const float u0, const float u1, const float u2, const float u3, const float x, const float tension, const float bias){
	float m0 = (u1 - u0) * (1 + bias) * (1 - tension) / 2 + (u2 - u1) * (1 - bias) * (1 - tension) / 2;
	float m1 = (u2 - u1) * (1 + bias) * (1 - tension) / 2 + (u3 - u2) * (1 - bias) * (1 - tension) / 2;
	float x2 = x * x;
	float x3 = x * x2;
	float a0 =  2 * x3 - 3 * x2 + 1;
	float a1 =      x3 - 2 * x2 + x;
	float a2 =      x3 -     x2;
	float a3 = -2 * x3 + 3 * x2;

	return a0 * u1 + a1 * m0 + a2 * m1 + a3 * u2;
}


class vec2{
public:

	union {
		struct{
			float x, y;
		};

		float data[2];
	};

	vec2() {}// when initaling a vec2or, it is left in a scalartate of unknown!!!
	vec2(const vec2 &a) : x(a.x), y(a.y) {}
	vec2(float nx, float ny) : x(nx), y(ny) {}

	void operator += (const float scalar){ x += scalar; y += scalar;}
	void operator += (const vec2 &v){ x += v.x; y += v.y;}
	void operator -= (const float scalar){ x -= scalar; y -= scalar; }
	void operator -= (const vec2 &v){ x -= v.x; y -= v.y; }
	void operator *= (const float scalar){ x *= scalar; y *= scalar; }
	void operator *= (const vec2 &v){ x *= v.x; y *= v.y; }
	void operator /= (const float scalar){ float inv(1.0f/scalar);  x*=inv; y *=inv; }// NOTE: no check for divide by zero here
	void operator /= (const vec2 &v){ x /= v.x; y /= v.y; }

	void zero() { x=y=0.0f; }// zero's out the scalartruct

	float LengthSq() const{ return x*x + y*y;}
	float Length() const { return sqrtf(LengthSq()); }
	float& operator [](size_t index) { return data[index]; }
	float operator [](size_t index) const { return data[index]; }

};

inline vec2 operator + (const vec2 &u, const vec2 &v){ return vec2(u.x + v.x, u.y + v.y); }
inline vec2 operator + (const vec2 &v, const float scalar){ return vec2(v.x + scalar, v.y + scalar);}
inline vec2 operator + (const float scalar, const vec2 &v){ return vec2(v.x + scalar, v.y + scalar);}
inline vec2 operator - (const vec2 &u, const vec2 &v){ return vec2(u.x - v.x, u.y - v.y); }
inline vec2 operator - (const vec2 &v, const float scalar){ return vec2(v.x - scalar, v.y - scalar); }
inline vec2 operator - (const float scalar, const vec2 &v){ return vec2(v.x - scalar, v.y - scalar); }
inline vec2 operator - (const vec2 &v){ return vec2(-v.x, -v.y); }
inline vec2 operator * (const vec2 &u, const vec2 &v){ return vec2(u.x * v.x, u.y * v.y); }
inline vec2 operator * (const float scalar, const vec2 &v){ return vec2(v.x * scalar, v.y * scalar); }
inline vec2 operator * (const vec2 &v, const float scalar){ return vec2(v.x * scalar, v.y * scalar); }
inline vec2 operator / (const vec2 &u, const vec2 &v){ return vec2(u.x / v.x, u.y / v.y); }// NOTE: no check for divide by zero here
inline vec2 operator / (const vec2 &v, const float scalar){ float inv(1.0f/scalar); return vec2(v.x *inv, v.y *inv); }// NOTE: no check for divide by zero here
inline vec2 operator / (const float scalar, const vec2 &v){ float inv(1.0f/scalar); return vec2(v.x *inv, v.y *inv); }// NOTE: no check for divide by zero here
inline bool operator == (const vec2 &u, const vec2 &v){ return ((u.x == v.x) & (u.y == v.y));}
inline bool operator != (const vec2 &u, const vec2 &v){ return ((u.x != v.x) | (u.y != v.y));}

inline float Dot(const vec2& a, const vec2 &b) { return a.x*b.x + a.y*b.y; }
inline vec2 Min (const vec2& a, const vec2& b){ return vec2(min(a.x,b.x),min(a.y,b.y)); }
inline vec2 Max (const vec2& a, const vec2& b){ return vec2(max(a.x,b.x),max(a.y,b.y)); }
inline vec2 Floor (const vec2& a ){ return vec2(floor(a.x),floor(a.y)); }

inline vec2 Lerp(const vec2 &u, const vec2 &v, const vec2 &x){ return u + x * (v - u); }
inline vec2 Lerp(const vec2 &u, const vec2 &v, const float x){ return u + x * (v - u); }

inline vec2 Serp(const vec2 &u0, const vec2 &u1, const vec2 &u2, const vec2 &u3, const float x){
	vec2 p = (u3 - u2) - (u0 - u1);
	vec2 q = (u0 - u1) - p;
	vec2 r = u2 - u0;
	return x * (x * (x * p + q) + r) + u1;
}
inline float Length(const vec2 &a, const vec2 &b) {return (a-b).Length();}
inline float LengthSq(const vec2 &a, const vec2 &b) { return (a-b).LengthSq();}

inline bool CloseTo(const vec2& p1, const vec2& p2, const float epison = .001f){  vec2 temp(p1 - p2); return (abs(Dot(temp, temp)) < epison); }
inline vec2 abs(const vec2& v){ return vec2(abs(v.x), abs(v.y));  }
inline std::ostream& operator<<(std::ostream& strem, const vec2& obj){
	strem<<"x="<<obj.x<<"  y="<<obj.y;
	return strem;
}


/*	--------------------------------VEC3---------------------------------------------- */										


class vec3 {
public:

	union{
		float data[3];
		struct{
			float x, y, z;
		};
	};
	vec3(){}// when initaling a std::vector, it is left in a scalartate of unknown!!!
	vec3(float nx, float ny, float nz): x(nx), y(ny), z(nz) {}
	vec3(const vec2& v): x(v.x), y(v.y), z(0.0f) {}
	vec3(const vec3 &a) : x(a.x), y(a.y), z(a.z) {}
	vec3(const float* v) : x(*v), y(*(v+1)), z(*(v+2)) {}// array initialization
	vec3(const vec2 &iv, const float iz): x(iv.x), y(iv.y), z(iz) {}
	vec3(const float ix, const vec2 &iv): x(ix), y(iv.x), z(iv.y) {}

	void operator += (const float scalar){ x += scalar; y += scalar; z += scalar; }
	void operator += (const vec3 &v){ x += v.x; y += v.y; z += v.z;}
	void operator -= (const float scalar){ x -= scalar; y -= scalar; z -= scalar; }
	void operator -= (const vec3 &v){ x -= v.x; y -= v.y; z -= v.z; }
	void operator *= (const float scalar){ x *= scalar; y *= scalar; z *= scalar; } 
	void operator *= (const vec3 &v){ x *= v.x; y *= v.y; z *= v.z; } 
	void operator /= (const float scalar){ float inv(1.0f/ scalar); x *=inv; y *=inv; z *=inv; }
	void operator /= (const vec3 &v){ x /= v.x; y /= v.y; z /= v.z; }

	// utility
	float& operator [](size_t index) { return data[index]; }
	float operator [](size_t index) const { return data[index]; }
	void zero() { x=y=z=0.0f; }// zero's out the scalartruct
	void	normalize() { float inv(1.0f /Length()); x *= inv; y *= inv; z *= inv; }// no check for div by zero!!!!!!
	float LengthSq() const{ return x*x + y*y + z*z;}
	float Length() const{ return sqrtf(LengthSq()); }
};

inline vec3 operator + (const vec3 &u, const vec3 &v){ return vec3(u.x + v.x, u.y + v.y, u.z + v.z); } 
inline vec3 operator + (const vec3 &v, const float scalar){ return vec3(v.x + scalar, v.y + scalar, v.z + scalar); } 
inline vec3 operator + (const float scalar, const vec3 &v){ return vec3(v.x + scalar, v.y + scalar, v.z + scalar); } 
inline vec3 operator - (const vec3 &u, const vec3 &v){ return vec3(u.x - v.x, u.y - v.y, u.z - v.z); }
inline vec3 operator - (const vec3 &v, const float scalar){ return vec3(v.x - scalar, v.y - scalar, v.z - scalar); }
inline vec3 operator - (const float scalar, const vec3 &v){ return vec3(v.x - scalar, v.y - scalar, v.z - scalar); } 
inline vec3 operator - (const vec3 &v){ return vec3(-v.x, -v.y, -v.z); } 
inline vec3 operator * (const vec3 &u, const vec3 &v){ return vec3(u.x * v.x, u.y * v.y, u.z * v.z); } 
inline vec3 operator * (const float scalar, const vec3 &v){ return vec3(v.x * scalar, v.y * scalar, v.z * scalar);} 
inline vec3 operator * (const vec3 &v, const float scalar){ return vec3(v.x * scalar, v.y * scalar, v.z * scalar); }

inline vec3 operator / (const vec3 &u, const vec3 &v){ return vec3(u.x / v.x, u.y / v.y, u.z / v.z); } // no check for div by zero!!!!!!
inline vec3 operator / (const vec3 &v, const float scalar){ float inv(1.0f/scalar); return vec3(v.x *inv, v.y *inv, v.z *inv); }// no check for div by zero!!!!!!
inline vec3 operator / (const float scalar, const vec3 &v){ float inv(1.0f/scalar); return vec3(inv* v.x, inv* v.y, inv* v.z); } // no check for div by zero!!!!!!
inline bool operator == (const vec3 &u, const vec3 &v){ return ((u.x == v.x) & (u.y == v.y) & (u.z == v.z)); }
inline bool operator != (const vec3 &u, const vec3 &v){ return ((u.x != v.x) | (u.y != v.y) | (u.z != v.z)); }

inline vec3 Min (const vec3& a, const vec3& b){ return vec3(min(a.x,b.x),min(a.y,b.y), min(a.z,b.z)); }
inline vec3 Max (const vec3& a, const vec3& b){ return vec3(max(a.x,b.x),max(a.y,b.y), max(a.z,b.z) ); }

inline vec3 Lerp(const vec3 &u, const vec3 &v, const vec3 &x){ return u + x * (v - u); }
inline vec3 Lerp(const vec3 &u, const vec3 &v, const float x){ return u + x * (v - u); }
vec3 Serp(const vec3 &u0, const vec3 &u1, const vec3 &u2, const vec3 &u3, const float x);

inline float Dot(const vec3& a, const vec3& b){ return b.x*a.x + b.y*a.y + b.z*a.z; }


inline bool CloseTo(const vec3& p1, const vec3& p2, const float epison = .001f){ vec3 temp(p1 - p2); return (abs(Dot(temp, temp)) < epison); }
inline vec3 abs(const vec3& v){ return vec3(abs(v.x), abs(v.y), abs(v.z));  }
inline vec3 Cross(const vec3 &a, const vec3 &b) {	return vec3(a.y*b.z - a.z*b.y, a.z*b.x - a.x*b.z, a.x*b.y - a.y*b.x); }

inline float Length(const vec3 &a, const vec3 &b) {return (a-b).Length();}
inline float LengthSq(const vec3 &a, const vec3 &b) { return (a-b).LengthSq();}
inline vec3 floor(const vec3& a) { return vec3(floor(a.x), floor(a.y), floor(a.z) ); }
inline vec3 ceil(const vec3& a) { return vec3(ceil(a.x), ceil(a.y), ceil(a.z) ); }
/*	--------------------------------TRIANGLE---------------------------------------------- */										


struct cTriangle{
	uint32_t Indices[3];// THIS IS RELATIVE TO ITS OWN INDEX BUFFER IF THERE IS ONE
	vec3 Vertices[3];//THE ACTUAL OBJECT SPACE POSITIONS!!!
};


/*	--------------------------------PLANE---------------------------------------------- */	



class Plane{
public:
	union {
		struct { vec3 Normal; float pad;};
		struct { float a, b, c, d; };
	};
	Plane() {}
	Plane(const cTriangle& tri){
		Normal =  Cross(tri.Vertices[1]-tri.Vertices[0],tri.Vertices[2]-tri.Vertices[0]); 
		Normal.normalize();
		d = -Dot(Normal,tri.Vertices[0]);
	}
	Plane(const vec3& p0, const vec3& n ): Normal(n), d(-Dot(Normal,p0) ){}//Lengthance from the plane to the origin from a normal and a point 
	Plane(const vec3& p0, const vec3& p1, const vec3& p2 ){  
		Normal =  Cross(p1-p0,p2-p0); 
		Normal.normalize();
		d = -Dot(Normal,p0);
	}
	Plane(const float ap, const float bp, const float cp, const float dp) : a(ap), b(bp), c(cp), d(dp) {}
	float LengthToPoint( const vec3& p ) const { return Dot(Normal, p) + d; }//signed Length from the plane topoint 'p' along the unit normal
	void Normalize(){
		float mag = 1.0f/Normal.Length();
		a = a * mag;
		b = b * mag;
		c = c * mag;
		d = d * mag;
	}
}; 


/*	--------------------------------VEC3 CONT'D---------------------------------------------- */
inline float Dot(const vec3& a, const Plane& b){ return b.a*a.x + b.b*a.y + b.c*a.z; }
inline bool Intersect(const vec3& raydir, const vec3& rayorigin, const Plane& plane, vec3& pointhit){// assumes an infinite length ray
	float bot(Dot(raydir, plane));
	if(bot >= 0) return false;// no intersection, the plane and the ray are parallel or the ray points in the same direction as the plane normal

	float scale=(-Dot(rayorigin, plane) - plane.d)/bot;// this is the length of the ray needed to intersect with the plane . . . 
	pointhit = (raydir*scale) + rayorigin;// scale the raydir to the correct length, then add that onto the orgin to correctly translate the point
	return true;
}
inline float RayAABBIntersect(const vec3& min, const vec3& max, const vec3& rayOrg, const vec3& rayDelta){
	// check for point inside box, trivial reject
	// Check for point inside box, trivial reject, and determine parametric
	// distance to each front face

	bool inside = true;

	float xt, xn;
	if (rayOrg.x < min.x) {
		xt = min.x - rayOrg.x;
		if (xt > rayDelta.x) return INFINITY;
		xt /= rayDelta.x;
		inside = false;
		xn = -1.0f;
	} else if (rayOrg.x > max.x) {
		xt = max.x - rayOrg.x;
		if (xt < rayDelta.x) return INFINITY;
		xt /= rayDelta.x;
		inside = false;
		xn = 1.0f;
	} else {
		xt = -1.0f;
	}

	float yt, yn;
	if (rayOrg.y < min.y) {
		yt = min.y - rayOrg.y;
		if (yt > rayDelta.y) return INFINITY;
		yt /= rayDelta.y;
		inside = false;
		yn = -1.0f;
	} else if (rayOrg.y > max.y) {
		yt = max.y - rayOrg.y;
		if (yt < rayDelta.y) return INFINITY;
		yt /= rayDelta.y;
		inside = false;
		yn = 1.0f;
	} else {
		yt = -1.0f;
	}

	float zt, zn;
	if (rayOrg.z < min.z) {
		zt = min.z - rayOrg.z;
		if (zt > rayDelta.z) return INFINITY;
		zt /= rayDelta.z;
		inside = false;
		zn = -1.0f;
	} else if (rayOrg.z > max.z) {
		zt = max.z - rayOrg.z;
		if (zt < rayDelta.z) return INFINITY;
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
			if (y < min.y || y > max.y) return INFINITY;
			float z = rayOrg.z + rayDelta.z*t;
			if (z < min.z || z > max.z) return INFINITY;
		} break;

	case 1: // intersect with xz plane
		{
			float x = rayOrg.x + rayDelta.x*t;
			if (x < min.x || x > max.x) return INFINITY;
			float z = rayOrg.z + rayDelta.z*t;
			if (z < min.z || z > max.z) return INFINITY;
		} break;

	case 2: // intersect with xy plane
		{
			float x = rayOrg.x + rayDelta.x*t;
			if (x < min.x || x > max.x) return INFINITY;
			float y = rayOrg.y + rayDelta.y*t;
			if (y < min.y || y > max.y) return INFINITY;

		} break;
	}

	// Return parametric point of intersection

	return t;
}
// this function will find the intersection of ray1 with ray two.  ray1 is assumed infinit length, ray2 however, is of finite length
// I use this for testing whether my mouse click hits a line segment in the world. 
inline vec2 RayRayIntersect(vec3& ray1, const vec3& origin1, float lengthof2, vec3& ray2, const vec3& origin2){
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
// find the intersection of two rays given a direction and starting position for each
inline vec2 RayRayIntersect(vec3& ray1, const vec3& origin1, vec3& ray2, const vec3& origin2){
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
inline std::ostream& operator<<(std::ostream& strem, const vec3& obj){
	strem<<"x="<<obj.x<<"  y="<<obj.y<<"  z="<<obj.z;
	return strem;
}

inline int32_t GetRGBAFromHeights(float t, float b, float l, float r, float bump){
	vec3 tanZ(0.0f, (t-b), 1.0f);
	vec3 tanX(1.0f, (r-l), 0.0f);
	vec3 N;
	N = Cross(tanZ, tanX);
	N.normalize();
	int8_t rgba[4];// this needs to be stored in little endian format, which is why the info is stored as a bgr instead of rgb
	rgba[3]= static_cast<int8_t>(bump);
	float x= N.x * 127.0f;
	rgba[2] = static_cast<int8_t>(x);// CAST!!!
	float y= N.y * 127.0f;
	rgba[1] = static_cast<int8_t>(y);// CAST!!!
	float z= N.z * 127.0f;
	rgba[0] = static_cast<int8_t>(z);// CAST!!!		
	return *reinterpret_cast<int32_t*>(rgba);
}

/* --------------------------------------VEC4------------------------------------ */


class vec4{
public:
	union{
#ifdef _WIN64
		__m128 vec;
#endif
		float data[4];
		struct{
			float x, y, z, w;
		};
		struct{ 
			float top, left, right, bottom;
		};
	};	
	vec4() {}// when initaling a vec4, it is left in a state of unknown!!!
	vec4(const vec3 &a) : x(a.x), y(a.y), z(a.z), w(0.0f) {}
	vec4(float nx, float ny, float nz, float nw) : x(nx), y(ny), z(nz), w(nw) {}
	vec4(const vec2& iv, const float nz, const float nw): x(iv.x), y(iv.y), z(nz), w(nw) {}
	vec4(const float nx, const vec2& iv, const float nw): x(nx), y(iv.x), z(iv.y), w(nw) {}
	vec4(const float nx, const float ny, const vec2& iv): x(nx), y(ny), z(iv.x), w(iv.y) {}
	vec4(const vec2& nv0, const vec2& nv1): x(nv0.x), y(nv0.y), z(nv1.x), w(nv1.y) {}
	vec4(const vec3& iv, const float iw): x(iv.x), y(iv.y), z(iv.z), w(iw) {}
	vec4(const float ix, const vec3& iv): x(ix), y(iv.x), z(iv.y), w(iv.z) {}
	vec4(const float* v) : x(*v), y(*(v+1)), z(*(v+2)), w(*(v+3)) {}// array initialization

#ifdef _WIN64
	vec4(__m128& ve) : vec(ve) {}
	vec4(const vec4 &a) : vec(a.vec) {}
	void operator += (const float scalar){ vec = _mm_add_ps(vec, _mm_set_ps1(scalar));}
	void operator += (const vec4 &v){ vec = _mm_add_ps(vec, v.vec);}
	void operator -= (const float scalar){ vec = _mm_sub_ps(vec, _mm_set_ps1(scalar));}
	void operator -= (const vec4 &v){ vec = _mm_sub_ps(vec, v.vec);}
	void operator *= (const float scalar){ vec = _mm_mul_ps(vec, _mm_set_ps1(scalar));}
	void operator *= (const vec4 &v){ vec = _mm_mul_ps(vec, v.vec);}
	void operator /= (const float scalar){ vec = _mm_div_ps(vec, _mm_set_ps1(scalar));}
	void operator /= (const vec4 &v){ vec = _mm_div_ps(vec, v.vec);}
	void zero() { vec = _mm_setzero_ps(); }// zero's out the struct
	void normalize() { 
		vec = _mm_mul_ps(vec, _mm_set_ps1(1.0f / Length()));
	}
	float LengthSq() const{
		__m128 temp(_mm_mul_ps(vec, vec)); 
		float* t(reinterpret_cast<float*>(&temp));
		return t[0]+ t[1]+t[2]+t[3];
	}
	void floor() { x = floorf(x); y=floorf(y); z=floorf(z); w=floorf(w);  }
	void ceil() { x = ceilf(x); y=ceilf(y); z=ceilf(z); w=ceilf(w);  }
#else 
	vec4(const vec4 &a) : x(a.x), y(a.y), z(a.z), w(a.w) {}
	void operator += (const float scalar){ 	x += scalar; y += scalar; z += scalar; w += scalar; }
	void operator += (const vec4 &v){  x += v.x; y += v.y; z += v.z; w += v.w; }
	void operator -= (const float scalar){ 	x -= scalar;y -= scalar;z -= scalar;w -= scalar; }
	void operator -= (const vec4 &v){ 	x -= v.x;y -= v.y;z -= v.z; w -= v.w;}
	void operator *= (const float scalar){ 	x *= scalar;y *= scalar;z *= scalar; w *= scalar;}
	void operator *= (const vec4 &v){ 	x *= v.x;y *= v.y;z *= v.z; w *= v.w;}
	void operator /= (const float scalar){ 	float inv(1.0f/scalar); x *= inv; y *= inv; z *= inv; w *= inv;}
	void operator /= (const vec4 &v){  x /= v.x;y /= v.y;z /= v.z; w /= v.w;}
	void zero() { x=y=z=w=0.0f; }// zero's out the struct
	void normalize() {  float inv(1.0f /Length()); x *= inv; y *= inv; z *= inv; w *= inv; } // no check for div by zero!!!!!!
	float LengthSq() const{ return x*x + y*y + z*z + w*w; }
#endif
	operator uint32_t () const {// convert from [0, 1] to rgba 8 
		uint32_t dwR = x >= 1.0f ? 0xff : x <= 0.0f ? 0x00 : (uint32_t) (x * 255.0f + 0.5f);
		uint32_t dwG = y >= 1.0f ? 0xff : y <= 0.0f ? 0x00 : (uint32_t) (y * 255.0f + 0.5f);
		uint32_t dwB = z >= 1.0f ? 0xff : z <= 0.0f ? 0x00 : (uint32_t) (z * 255.0f + 0.5f);
		uint32_t dwA = w >= 1.0f ? 0xff : w <= 0.0f ? 0x00 : (uint32_t) (w * 255.0f + 0.5f);
		return (dwA << 24) | (dwR << 16) | (dwG << 8) | (dwB << 0);
	}

	float Length() const{ return sqrtf(LengthSq()); }
	float& operator [](size_t index) { return data[index]; }
	float operator [](size_t index) const { return data[index]; }

};

#ifdef _WIN64

inline vec4 operator + (const vec4 &u, const vec4 &v){ return vec4(_mm_add_ps(u.vec, v.vec)); }
inline vec4 operator + (const vec4 &v, const float s){ return vec4(_mm_add_ps(v.vec, _mm_set_ps1(s))); }
inline vec4 operator + (const float s, const vec4 &v){ return vec4(_mm_add_ps(_mm_set_ps1(s), v.vec)); }
inline vec4 operator - (const vec4 &u, const vec4 &v){ return vec4(_mm_sub_ps(u.vec, v.vec)); }
inline vec4 operator - (const vec4 &v, const float s){ return vec4(_mm_sub_ps(v.vec, _mm_set_ps1(s))); }
inline vec4 operator - (const float s, const vec4 &v){ return vec4(_mm_sub_ps( _mm_set_ps1(s), v.vec)); }
inline vec4 operator - (const vec4 &v){ return vec4(_mm_mul_ps(_mm_set_ps1(-1.0f), v.vec));  } 
inline vec4 operator * (const vec4 &u, const vec4 &v){ return vec4(_mm_mul_ps(u.vec, v.vec)); }
inline vec4 operator * (const float s, const vec4 &v){ return vec4(_mm_mul_ps(_mm_set_ps1(s), v.vec)); }
inline vec4 operator * (const vec4 &v, const float s){ return vec4(_mm_mul_ps( v.vec, _mm_set_ps1(s))); }
inline vec4 operator / (const vec4 &u, const vec4 &v){ return vec4(_mm_div_ps(u.vec, v.vec)); }
inline vec4 operator / (const vec4 &v, const float s){ return vec4(_mm_div_ps(v.vec, _mm_set_ps1(s))); }
inline vec4 operator / (const float s, const vec4 &v){ return vec4(_mm_div_ps(_mm_set_ps1(s), v.vec)); }

inline vec4 Min (const vec4& a, const vec4& b){ return vec4(_mm_min_ps(a.vec, b.vec)); }
inline vec4 Max (const vec4& a, const vec4& b){ return vec4(_mm_max_ps(a.vec, b.vec)); }

inline float Dot(const vec4& a, const vec4& b){
	__m128 ts(_mm_mul_ps(a.vec, b.vec));
	float* t(reinterpret_cast<float*>(&ts));
	return t[0] + t[1]+t[2]+t[3];// using the _mm_hadd_ps function is slower than doing this... dont believe me? try it
}
inline vec4 floor(const vec4& a) { return vec4(floorf(a.x),floorf(a.y),floorf(a.z),floorf(a.w));  }
inline vec4 ceil(const vec4& a) { return vec4(ceilf(a.x),ceilf(a.y),ceilf(a.z),ceilf(a.w));  }
#else
inline vec4 operator + (const vec4 &u, const vec4 &v){ return vec4(u.x + v.x, u.y + v.y, u.z + v.z, u.w + v.w); }
inline vec4 operator + (const vec4 &v, const float s){ return vec4(v.x + s, v.y + s, v.z + s, v.w + s); }
inline vec4 operator + (const float s, const vec4 &v){ return vec4(v.x + s, v.y + s, v.z + s, v.w + s); }
inline vec4 operator - (const vec4 &u, const vec4 &v){ return vec4(u.x - v.x, u.y - v.y, u.z - v.z, u.w - v.w); }
inline vec4 operator - (const vec4 &v, const float s){ return vec4(v.x - s, v.y - s, v.z - s, v.w - s); }
inline vec4 operator - (const float s, const vec4 &v){ return vec4(v.x - s, v.y - s, v.z - s, v.w - s); }
inline vec4 operator - (const vec4 &v){ return vec4(-v.x, -v.y, -v.z, -v.w); }
inline vec4 operator * (const vec4 &u, const vec4 &v){ return vec4(u.x * v.x, u.y * v.y, u.z * v.z, u.w * v.w); } 
inline vec4 operator * (const float s, const vec4 &v){ return vec4(v.x * s, v.y * s, v.z * s, v.w * s); } 
inline vec4 operator * (const vec4 &v, const float s){ return vec4(v.x * s, v.y * s, v.z * s, v.w * s); }
inline vec4 operator / (const vec4 &u, const vec4 &v){ return vec4(u.x / v.x, u.y / v.y, u.z / v.z, u.w / v.w); }
inline vec4 operator / (const vec4 &v, const float s){ return vec4(v.x / s, v.y / s, v.z / s, v.w / s); }
inline vec4 operator / (const float s, const vec4 &v){ return vec4(s / v.x, s / v.y, s / v.z, s / v.w); } 
inline float Dot(const vec4& a, const vec4& b){ return b.x*a.x + b.y*a.y + b.z*a.z + b.w*a.w; }
inline vec4 Min (const vec4& a, const vec4& b){ return vec4(min(a.x,b.x),min(a.y,b.y), min(a.z,b.z), min(a.w,b.w)); }
inline vec4 Max (const vec4& a, const vec4& b){ return vec4(max(a.x,b.x),max(a.y,b.y), max(a.z,b.z), max(a.w,b.w) ); }

#endif

inline bool operator == (const vec4 &u, const vec4 &v){ return (u.x == v.x) & (u.y ==v.y) & ( u.z==v.z) & ( u.w==v.w); }
inline bool operator != (const vec4 &u, const vec4 &v){ return (u.x != v.x) | (u.y !=v.y) | ( u.z!=v.z) | ( u.w!=v.w); }

inline vec4 Lerp(const vec4 &u, const vec4 &v, const vec4 &x){ return u + x * (v - u); }
inline vec4 Lerp(const vec4 &u, const vec4 &v, const float x){ return u + x * (v - u); }
inline vec4 Serp(const vec4 &u0, const vec4 &u1, const vec4 &u2, const vec4 &u3, const float x){
	vec4 p = (u3 - u2) - (u0 - u1);
	vec4 q = (u0 - u1) - p;
	vec4 r = u2 - u0;
	return x * (x * (x * p + q) + r) + u1;
}
inline bool CloseTo(const vec4& p1, const vec4& p2, const float epison = MATH_EPS){   vec4 temp(p1 - p2); return (abs(Dot(temp, temp)) < epison); }

extern const vec4 ABSROWVAL;

inline vec4 abs(const vec4& v){
#ifdef _WIN64
	return vec4(_mm_and_ps(v.vec, ABSROWVAL.vec ));
#else
	return vec4(abs(v.x), abs(v.y), abs(v.z), abs(v.w));
#endif
}
inline std::ostream& operator<<(std::ostream& strem, const vec4& obj){
	strem<<"x="<<obj.x<<"  y="<<obj.y<<"  z="<<obj.z<<"  w="<<obj.w;
	return strem;
}
inline void copy(vec4& dst, __m128* src){// copy used for 32 bit because the compiler cannot guarantee alignment of __m128 on the stack
	dst.x = *(reinterpret_cast<float*>(src) + 0);
	dst.y = *(reinterpret_cast<float*>(src) + 1);
	dst.z = *(reinterpret_cast<float*>(src) + 2);
	dst.w = *(reinterpret_cast<float*>(src) + 3);
}
inline void copy( __m128* dst, const vec4& src){// copy used for 32 bit because the compiler cannot guarantee alignment of __m128 on the stack
	*(reinterpret_cast<float*>(dst) + 0) = src.x;
	*(reinterpret_cast<float*>(dst) + 1) = src.y;
	*(reinterpret_cast<float*>(dst) + 2) = src.z;
	*(reinterpret_cast<float*>(dst) + 3) = src.w;
}


/* --------------------------------------QUAT------------------------------------ */



class mat4;// forward declaration
class euler;// forward declaration
class quat {
public:

	union{
		float data[4];
#ifdef _WIN64
		__m128 vec;

#endif		
		struct {
			vec4 row_v;
		};
		struct{
			float x, y, z, w;
		};
	};

	quat() {}
	quat(const vec4& vec4) : row_v(vec4) {}
#ifdef _WIN64
	quat(const __m128 v) : vec(v) {}
	quat(const float nx, const float ny, const float nz, const float nw){ vec = _mm_set_ps(nw, nz, ny, nx); }
	void	identity() { vec = _mm_set_ps(1.0f, 0.0f, 0.0f, 0.0f); }
#else 
	quat(const float nx, const float ny, const float nz, const float nw) : x(nx), y(ny), z(nz), w(nw) {}
	void	identity() { x=y=z=0.0f; w=1.0f; }
#endif

	void SetupRotation(const vec3& rots){ SetupRotation(rots.x, rots.y, rots.z);} 
	inline void SetupRotation(const euler &orientation);
	void setToRotateAboutX(float theta) { sincosf(x, w, theta * .5f); y =z = 0.0f; }// theta is expected to be in radians
	void setToRotateAboutY(float theta) { sincosf(y, w, theta * .5f); x =z = 0.0f; }// theta is expected to be in radians
	void setToRotateAboutZ(float theta) { sincosf(z, w, theta * .5f); x =y = 0.0f; }// theta is expected to be in radians
	void setToRotateAboutAxis(const vec3 &axis, float theta) {// theta is expected to be in radians
		float sinThetaOver2;
		sincosf(sinThetaOver2, w, theta * .5f);
		x = axis.x * sinThetaOver2;
		y = axis.y * sinThetaOver2;
		z = axis.z * sinThetaOver2;
	}
	void SetupRotation(const float heading, const float pitch, const float bank){// angles are expected to be in radians
		float	sp, sb, sh;// Compute sine and cosine of the half angles
		float	cp, cb, ch;
		sincosf(sp, cp, pitch * 0.5f);
		sincosf(sb, cb, bank * 0.5f);
		sincosf(sh, ch, heading * 0.5f);
		w =  ch*cp*cb + sh*sp*sb;
		x =  ch*sp*cb + sh*cp*sb;
		y = -ch*sp*sb + sh*cp*cb;
		z = -sh*sp*cb + ch*cp*sb;
	}

	void operator *=(const quat &a) {
		*this= quat(w*a.x + x*a.w + z*a.y - y*a.z,
			w*a.y + y*a.w + x*a.z - z*a.x,
			w*a.z + z*a.w + y*a.x - x*a.y,
			w*a.w - x*a.x - y*a.y - z*a.z);
	}
	float& operator [](size_t index) { return data[index]; }
	float operator [](size_t index) const { return data[index]; }


	void normalize() {  row_v.normalize(); } // no check for div by zero!!!!!!
	float getRotationAngle() const { return acosf(w) * 2.0f; }
	vec3 getRotationAxis() const {
		float sinThetaOver2Sq = 1.0f - w*w;// Compute sin^2(theta/2).  Remember that w = cosf(theta/2), and sin^2(x) + cos^2(x) = 1
		if (sinThetaOver2Sq <= 0.0f) return vec3(1.0f, 0.0f, 0.0f);// Protect against numerical imprecision
		// Identi_42 quaternion, or numerical imprecision.  Just return any valid vector, since it doesn't matter
		float	oneOverSinThetaOver2 = 1.0f / sqrtf(sinThetaOver2Sq);// Compute 1 / sinf(theta/2)
		return vec3(x * oneOverSinThetaOver2, y * oneOverSinThetaOver2, z * oneOverSinThetaOver2 );// Return axis of rotation
	}

	inline void frommatrix(const mat4& pm);
};
inline std::ostream& operator<<(std::ostream& strem, const quat& obj){
	strem<<obj.row_v;
	return strem;
}

inline float Dot(const quat &a, const quat &b) {  return Dot(a.row_v, b.row_v); } 

inline quat slerp(const quat &q0, const quat &q1, float t) {
	if (t <= 0.0f) return q0;// Check for out-of range parameter and return edge points if so
	if (t >= 1.0f) return q1;
	float cosOmega = Dot(q0, q1);// Compute "cosine of angle between quaternions" using Dot product
	// If negative Dot, use -q1.  Two quaternions q and -q represent the same rotation, but may produce different slerp.  We chose q or -q to rotate using the acute angle.

	vec4 tempq1(q1.row_v);
	if (cosOmega < 0.0f) {
		tempq1 *= vec4(-1.0f, -1.0f, -1.0f, -1.0f);
		cosOmega = -cosOmega;
	}
	// We should have two unit quaternions, so Dot should be <= 1.0 Compute interpolation fraction, checking for quaternions almost exactly the same
	float k0, k1;
	if (cosOmega > 0.9999f) {// Very close - just use linear interpolation, which will protect againt a divide by zero
		k0 = 1.0f-t;
		k1 = t;
	} else {// Compute the sin of the angle using the trig identi_42 sin^2(omega) + cos^2(omega) = 1
		float sinOmega = sqrtf(1.0f - cosOmega*cosOmega);
		float omega = atan2(sinOmega, cosOmega);// Compute the angle from its sin and cosine
		float oneOverSinOmega = 1.0f / sinOmega;// Compute inverse of denominator, so we only have to divide once
		k0 = sinf((1.0f - t) * omega) * oneOverSinOmega;// Compute interpolation parameters
		k1 = sinf(t * omega) * oneOverSinOmega;
	}
	// Interpolate
	return (tempq1*vec4(k1, k1, k1, k1)) + (vec4(k0, k0, k0, k0) * vec4(q0.row_v));
}

//NOTE standard QUATERNION multiplication is dont opposite from standard multiplication. For example with matricies, you mutliply in the order you want the operations to occur from left to right. Like Scaling * Rotation * Translation
//With quaternions, that should be opposite. However,I deviate from the standard and wrote the multiplication to occur as you would expect with the left handed coordinate system. So, write your quaternion multiplications in order of the way you want the rotations to occur
inline quat operator *(const quat &a, const quat &b)  {// quat cross product, which concatonates multiple angular displacements.  
	//The order of multiplication, from left to right, corresponds to the order that the angular displacements are applied.  This is backwards from the *standard* definition of quaternion multiplication. 
	return quat(b.w*a.x + b.x*a.w + b.z*a.y - b.y*a.z,
		b.w*a.y + b.y*a.w + b.x*a.z - b.z*a.x,
		b.w*a.z + b.z*a.w + b.y*a.x - b.x*a.y,
		b.w*a.w - b.x*a.x - b.y*a.y - b.z*a.z);
}


inline quat conjugate(const quat &q) {return quat(q.w,-q.x,-q.y,-q.z); }// Compute the quaternion conjugate.  This is the quaternian with the opposite rotation as the original quaternian. 

inline quat pow(const quat &q, float exponent) {
	if (fabs(q.w) > .9999f) return q;// Check for the case of an identical quaternion. This will protect against divide by zero
	float	alpha = acosf(q.w);// Extract the half angle alpha (alpha = theta/2)
	float	newAlpha = alpha * exponent;// Compute new alpha value
	float	mult = sinf(newAlpha) / sinf(alpha);// Compute new xyz values
	return quat( q.x * mult, q.y * mult, q.z * mult, cosf(newAlpha));
}


/* --------------------------------------EULER------------------------------------ */


class euler {
public:

	union{
		struct{
			float heading, pitch, bank;
		};
	};
	euler() {}
	euler(float heading, float pitch, float bank): heading(heading), pitch(pitch), bank(bank) {}
	euler(vec3& ang): heading(ang.x), pitch(ang.y), bank(ang.z) {}
	void identity() { heading=pitch=bank=0.0f; }

	void canonize() {
		pitch = wrapPi(pitch); // Now, check for "the back side" of the matrix, pitch outside the canonical range of -pi/2 ... pi/2
		if (pitch < -PIOVERTWO) {
			pitch = -Pi - pitch;
			heading += Pi;
			bank += Pi;
		} else if (pitch > PIOVERTWO) {
			pitch = Pi - pitch;
			heading += Pi;
			bank += Pi;
		}// OK, now check for the gimbel lock case (within a slight tolerance)
		if (fabs(pitch) > PIOVERTWO - 1e-4) { // We are in gimbel lock.  Assign all rotation about the vertical axis to heading
			heading += bank;
			bank = 0.0f;
		} else bank = wrapPi(bank); // Not in gimbel lock.  Wrap the bank angle in canonical range 
		// Wrap heading in canonical range
		heading = wrapPi(heading);
	}
	void fromquat(const quat &q) {// Setup the Euler angles, given an object->inertial rotation quaternion
		float sp = -2.0f * (q.y*q.z - q.w*q.x);// Extract sinf(pitch)
		if (fabs(sp) > 0.9999f) {// Check for Gimbel lock, giving slight tolerance for numerical imprecision
			pitch = PIOVERTWO * sp;// Looking straight up or down
			heading = atan2(-q.x*q.z + q.w*q.y, 0.5f - q.y*q.y - q.z*q.z);// Compute heading, slam bank to zero
			bank = 0.0f;
		} else {// Compute angles.  We don't have to use the "safe" asin function because we already checked for range errors when checking for Gimbel lock
			pitch	= asinf(sp);
			heading	= atan2(q.x*q.z + q.w*q.y, 0.5f - q.x*q.x - q.y*q.y);
			bank	= atan2(q.x*q.y + q.w*q.z, 0.5f - q.x*q.x - q.z*q.z);
		}
	}
	inline void fromObjectToWorldMatrix(const mat4 &m);

};

inline std::ostream& operator<<(std::ostream& strem, const euler& obj){
	strem<<"heading="<<obj.heading<<"  pitch="<<obj.pitch<<"  bank="<<obj.bank;
	return strem;
}
/* --------------------------------------MAT4------------------------------------ */

const vec4 one(1.0f, 1.0f, 1.0f, 1.0f);
const vec4 two(2.0f, 2.0f, 2.0f, 2.0f);
class mat4;
mat4	operator*(const mat4& a, const mat4 &b);

class mat4 {
public:

	union {
		struct {
			float        _11, _12, _13, _14;
			float        _21, _22, _23, _24;
			float        _31, _32, _33, _34;
			float        _41, _42, _43, _44;
		};
		float d[16];
		float data[4][4];
#ifdef _WIN64
		__m128 rows[4];
		struct {
			__m128 row0, row1, row2, row3;
		};
#endif
		struct {// Doesnt this matrix class have a shit ton of names for everything? good thing this doesnt cause slowdown, but it sure looks like crap doesnt it? oh well
			vec4 row0_v, row1_v, row2_v, row3_v;
		};
		struct{
			vec4 rows_v[4];
		};
	};
	mat4() {}// do nothing... 
	mat4(float m_11, float m_12, float m_13, float m_14, 
		float m_21, float m_22, float m_23, float m_24,  
		float m_31, float m_32, float m_33, float m_34, 
		float m_41, float m_42, float m_43, float m_44): 
	_11(m_11), _12(m_12), _13(m_13), _14(m_14), 
		_21(m_21), _22(m_22), _23(m_23), _24(m_24),  
		_31(m_31), _32(m_32), _33(m_33), _34(m_34), 
		_41(m_41), _42(m_42), _43(m_43), _44(m_44) {}
#ifdef _WIN64
	mat4(const __m128& nrow0, const  __m128& nrow1, const  __m128& nrow2, const __m128& nrow3): row0(nrow0), row1(nrow1), row2(nrow2), row3(nrow3) {}
	mat4(const mat4& obj) : row0(obj.row0), row1(obj.row1), row2(obj.row2), row3(obj.row3) {}
#endif
	mat4(const vec4& nrow0, const  vec4& nrow1, const  vec4& nrow2, const vec4& nrow3): row0_v(nrow0), row1_v(nrow1), row2_v(nrow2), row3_v(nrow3) {}
	mat4(const Plane plane):
		_11(plane.a * plane.a), _12(plane.a * plane.b), _13(plane.a * plane.c), _14(plane.a * plane.d), 
		_21(plane.b * plane.a), _22(plane.b * plane.b), _23(plane.b * plane.c), _24(plane.b * plane.d),  
		_31(plane.c * plane.a), _32(plane.c * plane.b), _33(plane.c * plane.c), _34(plane.c * plane.d), 
		_41(plane.d * plane.a), _42(plane.d * plane.b), _43(plane.d * plane.c), _44(plane.d * plane.d) {}



	void setupfromeuler(const euler &orientation) {
		float	sh,ch, sp,cp, sb,cb;
		sincosf(sh, ch, orientation.heading);
		sincosf(sp, cp, orientation.pitch);
		sincosf(sb, cb, orientation.bank);

		row0_v = vec4(ch * cb + sh * sp * sb,   -ch * sb + sh * sp * cb, sh * cp,0.0f);
		row1_v = vec4(sb * cp, cb * cp, -sp, 0.0f);
		row2_v = vec4(-sh * cb + ch * sp * sb,  sb * sh + ch * sp * cb,ch * cp, 0.0f);
		row3_v = vec4(0.0f, 0.0f, 0.0f, 1.0f);

	}
	void setupfromquat(const quat &q) {// Setup the matrix to perform a rotation, given the angular displacement in quaternion form. The translation portion is reset.
		row0_v = vec4(-(q.y*q.y + q.z*q.z),  (q.x*q.y - q.w*q.z),	(q.x*q.z + q.w*q.y),		0.0f);
		row0_v *= two;
		_11+=1.0f;
		row1_v = vec4((q.x*q.y + q.w*q.z), -(q.x*q.x + q.z*q.z), (q.y*q.z - q.w*q.x),	0.0f);
		row1_v *= two;
		_22+=1.0f;
		row2_v = vec4((q.x*q.z - q.w*q.y),	(q.y*q.z + q.w*q.x),	-(q.x*q.x + q.y*q.y),	0.0f);
		row2_v *= two;
		_33+=1.0f;
		row3_v = vec4(0.0f, 0.0f, 0.0f, 1.0f);
	}

	void	identity() {// backwards looking, but that is because of little endian
		row0_v = vec4(1.0f, 0.0f, 0.0f, 0.0f);
		row1_v = vec4(0.0f, 1.0f, 0.0f, 0.0f);
		row2_v = vec4(0.0f, 0.0f, 1.0f, 0.0f);
		row3_v = vec4(0.0f, 0.0f, 0.0f, 1.0f);
	}

	void	zeroTranslation() { row3_v = vec4(0.0f, 0.0f, 0.0f, 1.0f);}
	void	setupTranslation(const vec3 &d) {
		row0_v = vec4(1.0f, 0.0f, 0.0f, 0.0f);
		row1_v = vec4(0.0f, 1.0f, 0.0f, 0.0f);
		row2_v = vec4(0.0f, 0.0f, 1.0f, 0.0f);
		row3_v = vec4(d, 1.0f);
	}
	vec3 getTranslation() const { return vec3(_41, _42, _43); }
	void	setupTranslation(float x, float y, float z) { setupTranslation(vec3(x, y, z)); }
	void	setTranslation(const vec3 &d) { row3_v = vec4(d, 1.0f);}
	void	setTranslation(float x, float y, float z){ setTranslation(vec3(x, y, z)); }
	void clearRotation(){
		_12 = _13 = _14 = _21 = _23 = _24 = _31 = _32 = _34 =0.0f;
	}
	// The axis of rotation is specified using a 1-based index:	1 => rotate about the x-axis	2 => rotate about the y-axis	3 => rotate about the z-axis
	// theta is the amount of rotation, in radians.  The left-hand rule is used to define "positive" rotation. The translation portion is reset.
	void	setupRotateX(float theta) {
		float	s, c;
		sincosf(s, c, theta);// Get sin and cosine of rotation angle
		row0_v = vec4(1.0f, 0.0f, 0.0f, 0.0f);
		row1_v = vec4(0.0f, c,    s,    0.0f);
		row2_v = vec4(0.0f, -s,   c,    0.0f);
		row3_v = vec4(0.0f, 0.0f, 0.0f, 1.0f);
	}
	void	setupRotateY(float theta) {
		float	s, c;
		sincosf(s, c, theta);// Get sin and cosine of rotation angle
		row0_v = vec4(c,    0.0f,  -s,   0.0f);
		row1_v = vec4(0.0f, 1.0f,   0.0f, 0.0f);
		row2_v = vec4(s,    0.0f,   c,    0.0f);
		row3_v = vec4(0.0f, 0.0f,	0.0f, 1.0f);
	}
	void	setupRotateZ(float theta) {
		float	s, c;
		sincosf(s, c, theta);// Get sin and cosine of rotation angle
		row0_v = vec4(c,    s,    0.0f, 0.0f);
		row1_v = vec4( -s,  c,    0.0f, 0.0f);
		row2_v = vec4(0.0f, 0.0f, 1.0f, 0.0f);
		row3_v = vec4(0.0f, 0.0f, 0.0f, 1.0f);
	}
	// Setup the matrix to perform a rotation about an arbitrary axis. The axis of rotation must pass through the origin. axis defines the axis of rotation, and must be a unit std::vector.
	// theta is the amount of rotation, in radians.  The left-hand rule is used to define "positive" rotation. The translation portion is reset.
	void	setupRotate(const vec3 &axis, float theta) {
		float	s, c;
		sincosf(s, c, theta);// Get sin and cosine of rotation angle
		float	a = 1.0f - c;
		float	ax = a * axis.x;
		float	ay = a * axis.y;
		float	az = a * axis.z;

		// Set the matrix elements.  There is still a little more
		// opportunity for optimization due to the many common
		// subexpressions.  We'll let the compiler handle that...

		_11 = ax*axis.x + c;
		_12 = ax*axis.y + axis.z*s;
		_13 = ax*axis.z - axis.y*s;
		_14 = 0;

		_21 = ay*axis.x - axis.z*s;
		_22 = ay*axis.y + c;
		_23 = ay*axis.z + axis.x*s;
		_24 =0;

		_31 = az*axis.x + axis.y*s;
		_32 = az*axis.y - axis.x*s;
		_33 = az*axis.z + c;
		_34 =0;
		// Reset the translation portion
		row3_v.zero();
		_44=1.0f;
	}


	vec3 getScaling() const { return vec3(_11, _22, _23); }
	void	setupScale(const vec3 &s) { setupScale(s.x, s.y, s.z); }// Setup the matrix to perform scale on each axis.  For uniform scale by k, use a std::vector of the form vec3(k,k,k)
	void	setupScale(float x, float y, float z){ 
		row0_v = vec4(x,    0.0f, 0.0f, 0.0f);
		row1_v = vec4(0.0f, y,    0.0f, 0.0f);
		row2_v = vec4(0.0f, 0.0f, z,    0.0f);
		row3_v = vec4(0.0f, 0.0f, 0.0f, 1.0f);
	}
	void	setupScale(float unif){ setupScale(unif, unif, unif); }
	void	setupScaleAlongAxis(const vec3 &axis, float k) {// Setup the matrix to perform scale along an arbitrary axis. The axis is specified using a unit std::vector.
		float	a = k - 1.0f;
		float	ax = a * axis.x;
		float	ay = a * axis.y;
		float	az = a * axis.z;
		// Fill in the matrix elements.  We'll do the common subexpression optimization ourselves here, since diagonallyopposite matrix elements are equal
		_11 = ax*axis.x + 1.0f;
		_22 = ay*axis.y + 1.0f;
		_32 = az*axis.z + 1.0f;
		_12 = _21 = ax*axis.y;
		_13 = _31 = ax*axis.z;
		_23 = _32 = ay*axis.z;
		row3_v = vec4(0.0f, 0.0f, 0.0f, 1.0f);
	}
	// The _42pe of shear is specified by the 1-based "axis" index.  The effect of transforming a point by the matrix is described by the pseudocode
	void	setupShearX(float s, float t) {
		row0_v = vec4(1.0f, s,    t,    0.0f);
		row1_v = vec4(0.0f, 1.0f, 0.0f, 0.0f);
		row2_v = vec4(0.0f, 0.0f, 1.0f, 0.0f);
		row3_v = vec4(0.0f, 0.0f, 0.0f, 1.0f);
	}
	void	setupShearY(float s, float t) {
		row0_v = vec4(1.0f, 0.0f, 0.0f, 0.0f);
		row1_v = vec4( s,  1.0f,  t,    0.0f);
		row2_v = vec4(0.0f, 0.0f, 1.0f, 0.0f);
		row3_v = vec4(0.0f, 0.0f, 0.0f, 1.0f);
	}
	void	setupShearZ(float s, float t) {
		row0_v = vec4(1.0f, 0.0f, 0.0f, 0.0f);
		row1_v = vec4(0.0f, 1.0f, 0.0f, 0.0f);
		row2_v = vec4(   s, t,    1.0f, 0.0f);
		row3_v = vec4(0.0f, 0.0f, 0.0f, 1.0f);
	}


	void	setupLookAt(const vec3& eye, const vec3& at, const vec3& up){
		vec3 zaxis(at - eye);
		zaxis.normalize();
		vec3 xaxis(Cross(up, zaxis));
		xaxis.normalize();
		vec3 yaxis(Cross(zaxis, xaxis));
		row0_v = vec4(xaxis.x, yaxis.x, zaxis.x, 0.0f);
		row1_v = vec4(xaxis.y, yaxis.y, zaxis.y, 0.0f);
		row2_v = vec4(xaxis.z, yaxis.z, zaxis.z, 0.0f);
		row3_v = vec4(-Dot(xaxis, eye), -Dot(yaxis, eye), -Dot(zaxis, eye), 1.0f);
	}

	void	setupProject(float fovY, float aspect, float zn, float zf) {
		float yScale(1.0f/tanf(fovY*0.5f));
		float invfmn(1.0f/(zf-zn));
		row0_v = vec4(yScale/aspect, 0.0f,    0.0f,          0.0f);
		row1_v = vec4(0.0f,          yScale,  0.0f,          0.0f);
		row2_v = vec4(0.0f,          0.0f,    zf*invfmn,     1.0f );
		row3_v = vec4(0.0f,          0.0f,    -zn*zf*invfmn, 0.0f);
	}
	void	setupOrtho(float w, float h, float zn, float zf){
		row0_v = vec4(2.0f/w, 0.0f,    0.0f,       0.0f);
		row1_v = vec4(0.0f,   2.0f/h,  0.0f,       0.0f);
		row2_v = vec4(0.0f,   0.0f,    1.0f/(zf-zn),0.0f);
		row3_v = vec4(0.0f,   0.0f,    zn/(zn-zf),  1.0f);
	}
	// Setup the matrix to perform a reflection about a plane parallel to a cardinal plane.
	void	setupReflectX(float k) {
		row0_v = vec4(-1.0f,    0.0f, 0.0f, 0.0f);
		row1_v = vec4(0.0f,     1.0f, 0.0f, 0.0f);
		row2_v = vec4(0.0f,     0.0f, 1.0f, 0.0f);
		row3_v = vec4(2.0f * k, 0.0f, 0.0f, 1.0f);
	}
	void	setupReflectY(float k) {
		row0_v = vec4(1.0f,  0.0f,    0.0f, 0.0f);
		row1_v = vec4(0.0f, -1.0f,    0.0f, 0.0f);
		row2_v = vec4(0.0f,  1.0f,    1.0f, 0.0f);
		row3_v = vec4(0.0f,  2.0f * k,0.0f,  1.0f);

	}
	void	setupReflectZ(float k) {
		row0_v = vec4(1.0f, 0.0f, 0.0f, 0.0f);
		row1_v = vec4(0.0f, 1.0f, 0.0f, 0.0f);
		row2_v = vec4(0.0f, 0.0f, -1.0f, 0.0f);
		row3_v = vec4(0.0f, 0.0f, 2.0f * k, 1.0f);
	}
	void	setupReflect(const vec3 &n) {// Setup the matrix to perform a reflection about an arbitrary plane through the origin.  The unit std::vector n is perpendicular to the plane. The translation portion is reset.
		float	ax = -2.0f * n.x;
		float	ay = -2.0f * n.y;
		float	az = -2.0f * n.z;
		// Fill in the matrix elements.  We'll do the common subexpression optimization ourselves here, since diagonally opposite matrix elements are equal
		_11 = 1.0f + ax*n.x;
		_22 = 1.0f + ay*n.y;
		_32 = 1.0f + az*n.z;
		_12 = _21 = ax*n.y;
		_13 = _31 = ax*n.z;
		_23 = _32 = ay*n.z;
		row3_v = vec4(1.0f, 0.0f, 0.0f, 1.0f);
	}

	inline __m128 _mm_dot_ps(__m128 v1, __m128 v2){
		__m128 mul0 = _mm_mul_ps(v1, v2);
		__m128 swp0 = _mm_shuffle_ps(mul0, mul0, _MM_SHUFFLE(2, 3, 0, 1));
		__m128 add0 = _mm_add_ps(mul0, swp0);
		__m128 swp1 = _mm_shuffle_ps(add0, add0, _MM_SHUFFLE(0, 1, 2, 3));
		__m128 add1 = _mm_add_ps(add0, swp1);
		return add1;
	}
	void	inverse() {
#ifndef _WIN64
		__m128 row0, row1, row2, row3;
		copy(&row0, row0_v);
		copy(&row1, row1_v);
		copy(&row2, row2_v);
		copy(&row3, row3_v);
#endif
		__m128 Fac0;
		{
			__m128 Swp0a = _mm_shuffle_ps(row3, row2, _MM_SHUFFLE(3, 3, 3, 3));
			__m128 Swp0b = _mm_shuffle_ps(row3, row2, _MM_SHUFFLE(2, 2, 2, 2));
			__m128 Swp00 = _mm_shuffle_ps(row2, row1, _MM_SHUFFLE(2, 2, 2, 2));
			__m128 Swp01 = _mm_shuffle_ps(Swp0a, Swp0a, _MM_SHUFFLE(2, 0, 0, 0));
			__m128 Swp02 = _mm_shuffle_ps(Swp0b, Swp0b, _MM_SHUFFLE(2, 0, 0, 0));
			__m128 Swp03 = _mm_shuffle_ps(row2, row1, _MM_SHUFFLE(3, 3, 3, 3));
			Fac0 =_mm_sub_ps(_mm_mul_ps(Swp00, Swp01), _mm_mul_ps(Swp02, Swp03));
		}
		__m128 Fac1;
		{
			__m128 Swp0a = _mm_shuffle_ps(row3, row2, _MM_SHUFFLE(3, 3, 3, 3));
			__m128 Swp0b = _mm_shuffle_ps(row3, row2, _MM_SHUFFLE(1, 1, 1, 1));
			__m128 Swp00 = _mm_shuffle_ps(row2, row1, _MM_SHUFFLE(1, 1, 1, 1));
			__m128 Swp01 = _mm_shuffle_ps(Swp0a, Swp0a, _MM_SHUFFLE(2, 0, 0, 0));
			__m128 Swp02 = _mm_shuffle_ps(Swp0b, Swp0b, _MM_SHUFFLE(2, 0, 0, 0));
			__m128 Swp03 = _mm_shuffle_ps(row2, row1, _MM_SHUFFLE(3, 3, 3, 3));
			Fac1 =_mm_sub_ps(_mm_mul_ps(Swp00, Swp01), _mm_mul_ps(Swp02, Swp03));
		}
		__m128 Fac2;
		{
			__m128 Swp0a = _mm_shuffle_ps(row3, row2, _MM_SHUFFLE(2, 2, 2, 2));
			__m128 Swp0b = _mm_shuffle_ps(row3, row2, _MM_SHUFFLE(1, 1, 1, 1));
			__m128 Swp00 = _mm_shuffle_ps(row2, row1, _MM_SHUFFLE(1, 1, 1, 1));
			__m128 Swp01 = _mm_shuffle_ps(Swp0a, Swp0a, _MM_SHUFFLE(2, 0, 0, 0));
			__m128 Swp02 = _mm_shuffle_ps(Swp0b, Swp0b, _MM_SHUFFLE(2, 0, 0, 0));
			__m128 Swp03 = _mm_shuffle_ps(row2, row1, _MM_SHUFFLE(2, 2, 2, 2));
			Fac2 =_mm_sub_ps(_mm_mul_ps(Swp00, Swp01), _mm_mul_ps(Swp02, Swp03));
		}
		__m128 Fac3;
		{
			__m128 Swp0a = _mm_shuffle_ps(row3, row2, _MM_SHUFFLE(3, 3, 3, 3));
			__m128 Swp0b = _mm_shuffle_ps(row3, row2, _MM_SHUFFLE(0, 0, 0, 0));
			__m128 Swp00 = _mm_shuffle_ps(row2, row1, _MM_SHUFFLE(0, 0, 0, 0));
			__m128 Swp01 = _mm_shuffle_ps(Swp0a, Swp0a, _MM_SHUFFLE(2, 0, 0, 0));
			__m128 Swp02 = _mm_shuffle_ps(Swp0b, Swp0b, _MM_SHUFFLE(2, 0, 0, 0));
			__m128 Swp03 = _mm_shuffle_ps(row2, row1, _MM_SHUFFLE(3, 3, 3, 3));
			Fac3 = _mm_sub_ps(_mm_mul_ps(Swp00, Swp01), _mm_mul_ps(Swp02, Swp03));
		}
		__m128 Fac4;
		{
			__m128 Swp0a = _mm_shuffle_ps(row3, row2, _MM_SHUFFLE(2, 2, 2, 2));
			__m128 Swp0b = _mm_shuffle_ps(row3, row2, _MM_SHUFFLE(0, 0, 0, 0));
			__m128 Swp00 = _mm_shuffle_ps(row2, row1, _MM_SHUFFLE(0, 0, 0, 0));
			__m128 Swp01 = _mm_shuffle_ps(Swp0a, Swp0a, _MM_SHUFFLE(2, 0, 0, 0));
			__m128 Swp02 = _mm_shuffle_ps(Swp0b, Swp0b, _MM_SHUFFLE(2, 0, 0, 0));
			__m128 Swp03 = _mm_shuffle_ps(row2, row1, _MM_SHUFFLE(2, 2, 2, 2));
			Fac4 = _mm_sub_ps(_mm_mul_ps(Swp00, Swp01), _mm_mul_ps(Swp02, Swp03));
		}
		__m128 Fac5;
		{
			__m128 Swp0a = _mm_shuffle_ps(row3, row2, _MM_SHUFFLE(1, 1, 1, 1));
			__m128 Swp0b = _mm_shuffle_ps(row3, row2, _MM_SHUFFLE(0, 0, 0, 0));
			__m128 Swp00 = _mm_shuffle_ps(row2, row1, _MM_SHUFFLE(0, 0, 0, 0));
			__m128 Swp01 = _mm_shuffle_ps(Swp0a, Swp0a, _MM_SHUFFLE(2, 0, 0, 0));
			__m128 Swp02 = _mm_shuffle_ps(Swp0b, Swp0b, _MM_SHUFFLE(2, 0, 0, 0));
			__m128 Swp03 = _mm_shuffle_ps(row2, row1, _MM_SHUFFLE(1, 1, 1, 1));
			Fac5 = _mm_sub_ps(_mm_mul_ps(Swp00, Swp01), _mm_mul_ps(Swp02, Swp03));
		}
		__m128 SignA = _mm_set_ps( 1.0f,-1.0f, 1.0f,-1.0f);
		__m128 SignB = _mm_set_ps(-1.0f, 1.0f,-1.0f, 1.0f);
		__m128 Temp0 = _mm_shuffle_ps(row1, row0, _MM_SHUFFLE(0, 0, 0, 0));
		__m128 Vec0 = _mm_shuffle_ps(Temp0, Temp0, _MM_SHUFFLE(2, 2, 2, 0));
		__m128 Temp1 = _mm_shuffle_ps(row1, row0, _MM_SHUFFLE(1, 1, 1, 1));
		__m128 Vec1 = _mm_shuffle_ps(Temp1, Temp1, _MM_SHUFFLE(2, 2, 2, 0));
		__m128 Temp2 = _mm_shuffle_ps(row1, row0, _MM_SHUFFLE(2, 2, 2, 2));
		__m128 Vec2 = _mm_shuffle_ps(Temp2, Temp2, _MM_SHUFFLE(2, 2, 2, 0));
		__m128 Temp3 = _mm_shuffle_ps(row1, row0, _MM_SHUFFLE(3, 3, 3, 3));
		__m128 Vec3 = _mm_shuffle_ps(Temp3, Temp3, _MM_SHUFFLE(2, 2, 2, 0));
		__m128 Sub00 = _mm_sub_ps(_mm_mul_ps(Vec1, Fac0), _mm_mul_ps(Vec2, Fac1));
		__m128 Add00 = _mm_add_ps(Sub00, _mm_mul_ps(Vec3, Fac2));
		__m128 Inv0 = _mm_mul_ps(SignB, Add00);
		__m128 Sub01 = _mm_sub_ps(_mm_mul_ps(Vec0, Fac0), _mm_mul_ps(Vec2, Fac3));
		__m128 Add01 = _mm_add_ps(Sub01, _mm_mul_ps(Vec3, Fac4));
		__m128 Inv1 = _mm_mul_ps(SignA, Add01);
		__m128 Sub02 = _mm_sub_ps(_mm_mul_ps(Vec0, Fac1), _mm_mul_ps(Vec1, Fac3));
		__m128 Add02 = _mm_add_ps(Sub02, _mm_mul_ps(Vec3, Fac5));
		__m128 Inv2 = _mm_mul_ps(SignB, Add02);
		__m128 Sub03 = _mm_sub_ps(_mm_mul_ps(Vec0, Fac2), _mm_mul_ps(Vec1, Fac4));
		__m128 Add03 = _mm_add_ps(Sub03, _mm_mul_ps(Vec2, Fac5));
		__m128 Inv3 = _mm_mul_ps(SignA, Add03);
		__m128 Row0 = _mm_shuffle_ps(Inv0, Inv1, _MM_SHUFFLE(0, 0, 0, 0));
		__m128 Row1 = _mm_shuffle_ps(Inv2, Inv3, _MM_SHUFFLE(0, 0, 0, 0));
		__m128 Row2 = _mm_shuffle_ps(Row0, Row1, _MM_SHUFFLE(2, 0, 2, 0));
		__m128 Det0 = _mm_dot_ps(row0, Row2);
		__m128 onee = _mm_set_ps1(1.0f); 
		__m128 Rcp0 = _mm_div_ps(onee, Det0);
		row0 = _mm_mul_ps(Inv0, Rcp0);
		row1 = _mm_mul_ps(Inv1, Rcp0);
		row2 = _mm_mul_ps(Inv2, Rcp0);
		row3 = _mm_mul_ps(Inv3, Rcp0);
#ifndef _WIN64
		copy(row0_v, &row0);
		copy(row1_v, &row1);
		copy(row2_v, &row2);
		copy(row3_v, &row3);
#endif
	}

	mat4& operator=(const mat4& rhs){
		row0_v = rhs.row0_v;
		row1_v = rhs.row1_v;
		row2_v = rhs.row2_v;
		row3_v = rhs.row3_v;
		return *this;
	}

	void operator*=(const float &b) {  
		float inv(1.0f/b);
		*this = mat4( row0_v*inv, row1_v*inv, row2_v*inv, row3_v*inv);	
	}

	void operator-=(const mat4 &b) {
		row0_v-=b.row0_v;
		row1_v-=b.row1_v;
		row2_v-=b.row2_v;
		row3_v-=b.row3_v;
	}
	void operator+=(const mat4 &b) {
		row0_v+=b.row0_v;
		row1_v+=b.row1_v;
		row2_v+=b.row2_v;
		row3_v+=b.row3_v;
	}
#ifdef _WIN64
	void Transpose(){ _MM_TRANSPOSE4_PS(row0, row1, row2, row3); } 
#else
	void Transpose(){ 
		std::swap( _12, _21);
		std::swap( _13, _31);
		std::swap( _14, _41);
		std::swap( _23, _32);
		std::swap( _24, _42);
		std::swap( _34, _43);
	}
#endif
	void operator/=(const float &b){ *this*=(1.0f/b); }
	void operator*=(const mat4 &b) { *this = *this * b; }
	float& operator[](size_t i){ return d[i]; }
	float operator[](size_t i) const { return d[i]; }
};
inline float determinant(const mat4& m){
	return m._11*(m._22*(m._33*m._44 - m._34*m._43) - m._32*(m._23*m._44 + m._24*m._43) + m._42*(m._23 *m._34 - m._24 * m._33)) 
		- m._21*(m._12*(m._33*m._44 - m._34*m._43) - m._32*(m._14*m._44 + m._14*m._43) + m._42*(m._13 *m._34 - m._14 * m._33)) 
		+ m._31*(m._12*(m._23*m._44 - m._24*m._43) - m._22*(m._13*m._44 + m._14*m._43) + m._42*(m._13 *m._24 - m._14 * m._23)) 
		- m._41*(m._12*(m._23*m._34 - m._24*m._33) - m._22*(m._13*m._34 + m._14*m._33) + m._32*(m._13 *m._24 - m._14 * m._23));
}
inline mat4 operator - (const mat4 &a, const mat4 &b) {  return mat4(a.row0_v-b.row0_v, a.row1_v-b.row1_v, a.row2_v-b.row2_v, a.row3_v-b.row3_v); }
inline mat4 operator + (const mat4 &a, const mat4 &b) { return mat4(a.row0_v+b.row0_v, a.row1_v+b.row1_v, a.row2_v+b.row2_v, a.row3_v+b.row3_v); }
inline mat4	operator*(const mat4& a, const float &b) { return mat4(a.row0_v*b, a.row1_v*b, a.row2_v*b, a.row3_v*b); }
inline mat4 operator/ (const mat4 &m, const float &b){ return mat4(m*(1.0f/b)); }
#ifdef _WIN64
inline mat4	operator*(const mat4& a, const mat4 &b) {
	__m128 r0;
	{
		__m128 e0 = _mm_shuffle_ps(a.row0, a.row0, _MM_SHUFFLE(0, 0, 0, 0));
		__m128 e1 = _mm_shuffle_ps(a.row0, a.row0, _MM_SHUFFLE(1, 1, 1, 1));
		__m128 e2 = _mm_shuffle_ps(a.row0, a.row0, _MM_SHUFFLE(2, 2, 2, 2));
		__m128 e3 = _mm_shuffle_ps(a.row0, a.row0, _MM_SHUFFLE(3, 3, 3, 3));
		r0 = _mm_add_ps(_mm_add_ps(_mm_mul_ps(b.row0, e0), _mm_mul_ps(b.row1, e1)), _mm_add_ps(_mm_mul_ps(b.row2, e2), _mm_mul_ps(b.row3, e3)));
	}
	__m128 r1;
	{
		__m128 e0 = _mm_shuffle_ps(a.row1, a.row1, _MM_SHUFFLE(0, 0, 0, 0));
		__m128 e1 = _mm_shuffle_ps(a.row1, a.row1, _MM_SHUFFLE(1, 1, 1, 1));
		__m128 e2 = _mm_shuffle_ps(a.row1, a.row1, _MM_SHUFFLE(2, 2, 2, 2));
		__m128 e3 = _mm_shuffle_ps(a.row1, a.row1, _MM_SHUFFLE(3, 3, 3, 3));
		r1 = _mm_add_ps(_mm_add_ps(_mm_mul_ps(b.row0, e0), _mm_mul_ps(b.row1, e1)), _mm_add_ps(_mm_mul_ps(b.row2, e2), _mm_mul_ps(b.row3, e3)));
	}
	__m128 r2;
	{
		__m128 e0 = _mm_shuffle_ps(a.row2, a.row2, _MM_SHUFFLE(0, 0, 0, 0));
		__m128 e1 = _mm_shuffle_ps(a.row2, a.row2, _MM_SHUFFLE(1, 1, 1, 1));
		__m128 e2 = _mm_shuffle_ps(a.row2, a.row2, _MM_SHUFFLE(2, 2, 2, 2));
		__m128 e3 = _mm_shuffle_ps(a.row2, a.row2, _MM_SHUFFLE(3, 3, 3, 3));
		r2 =_mm_add_ps(_mm_add_ps(_mm_mul_ps(b.row0, e0), _mm_mul_ps(b.row1, e1)), _mm_add_ps(_mm_mul_ps(b.row2, e2), _mm_mul_ps(b.row3, e3)));
	}
	__m128 r3;
	{
		__m128 e0 = _mm_shuffle_ps(a.row3, a.row3, _MM_SHUFFLE(0, 0, 0, 0));
		__m128 e1 = _mm_shuffle_ps(a.row3, a.row3, _MM_SHUFFLE(1, 1, 1, 1));
		__m128 e2 = _mm_shuffle_ps(a.row3, a.row3, _MM_SHUFFLE(2, 2, 2, 2));
		__m128 e3 = _mm_shuffle_ps(a.row3, a.row3, _MM_SHUFFLE(3, 3, 3, 3));
		r3 = _mm_add_ps(_mm_add_ps(_mm_mul_ps(b.row0, e0), _mm_mul_ps(b.row1, e1)), _mm_add_ps(_mm_mul_ps(b.row2, e2), _mm_mul_ps(b.row3, e3)));
	}
	return mat4(r0, r1, r2, r3);

}
#else
inline mat4 operator*(const mat4& a, const mat4 &b) {

	return mat4(
		a._11*b._11 + a._12*b._21 + a._13*b._31 + a._14*b._41,
		a._11*b._12 + a._12*b._22 + a._13*b._32 + a._14*b._42,
		a._11*b._13 + a._12*b._23 + a._13*b._33 + a._14*b._43,
		a._11*b._14 + a._12*b._24 + a._13*b._34 + a._14*b._44,

		a._21*b._11 + a._22*b._21 + a._23*b._31 + a._24*b._41,
		a._21*b._12 + a._22*b._22 + a._23*b._32 + a._24*b._42,
		a._21*b._13 + a._22*b._23 + a._23*b._33 + a._24*b._43,
		a._21*b._14 + a._22*b._24 + a._23*b._34 + a._24*b._44,

		a._31*b._11 + a._32*b._21 + a._33*b._31 + a._34*b._41,
		a._31*b._12 + a._32*b._22 + a._33*b._32 + a._34*b._42,
		a._31*b._13 + a._32*b._23 + a._33*b._33 + a._34*b._43,
		a._31*b._14 + a._32*b._24 + a._33*b._34 + a._34*b._44,

		a._41*b._11 + a._42*b._21 + a._43*b._31 + a._44*b._41,
		a._41*b._12 + a._42*b._22 + a._43*b._32 + a._44*b._42,
		a._41*b._13 + a._42*b._23 + a._43*b._33 + a._44*b._43,
		a._41*b._14 + a._42*b._24 + a._43*b._34 + a._44*b._44);
}

#endif


inline mat4	abs( const mat4 &mat ){  
	return mat4( abs(mat.row0_v), abs(mat.row1_v), abs(mat.row2_v), abs(mat.row3_v));
}

inline std::ostream& operator<<(std::ostream& strem, const mat4& obj){
	strem<<"row0  "<<std::endl<<obj.row0_v<<"row1  "<<obj.row1_v<<std::endl<<"row 2  "<<obj.row2_v<<std::endl<<"row3  "<<obj.row3_v<<std::endl;
	return strem;
}
inline vec4	operator*(const vec4 &p, const mat4 &m) {
	return vec4(
		p.x*m._11 + p.y*m._21 + p.z*m._31 + p.w*m._41, 
		p.x*m._12 + p.y*m._22 + p.z*m._32 + p.w*m._42, 
		p.x*m._13 + p.y*m._23 + p.z*m._33 + p.w*m._43, 
		p.x*m._14 + p.y*m._24 + p.z*m._34 + p.w*m._44);
}
inline vec3	operator*(const vec3 &p, const mat4 &m) {
	return vec3( 
		p.x*m._11 + p.y*m._21 + p.z*m._31 + m._41, 
		p.x*m._12 + p.y*m._22 + p.z*m._32 + m._42, 
		p.x*m._13 + p.y*m._23 + p.z*m._33 + m._43);
}
inline void	operator*=(vec3 &p, const mat4 &m) {
	p=vec3(
		p.x*m._11 + p.y*m._21 + p.z*m._31 + m._41, 
		p.x*m._12 + p.y*m._22 + p.z*m._32 + m._42, 
		p.x*m._13 + p.y*m._23 + p.z*m._33 + m._43);

}
inline void	operator*=(vec4 &p, const mat4 &m) {
	p= vec4(
		p.x*m._11 + p.y*m._21 + p.z*m._31 + p.w*m._41, 
		p.x*m._12 + p.y*m._22 + p.z*m._32 + p.w*m._42, 
		p.x*m._13 + p.y*m._23 + p.z*m._33 + p.w*m._43, 
		p.x*m._14 + p.y*m._24 + p.z*m._34 + p.w*m._44);
}

inline void	Transform(const vec3& in, vec4& out, const mat4& mat){ out=in; out.w=1.0f; out*=mat; }
inline void	Transform(const vec4& in, vec4& out, const mat4& mat){ out=in; out*=mat; }
inline void	Transform(const vec3& in, vec3& out, const mat4& mat){ out=in; out*=mat; }
inline void	TransformNormal(const vec3& in, vec4& out, const mat4& mat){ out=in; out.w=0.0f; out*=mat; }
inline void	TransformNormal(const vec3& in, vec3& out, const mat4& mat){
	out.x=in.x*mat._11 + in.y*mat._21 + in.z*mat._31;
	out.y=in.x*mat._12 + in.y*mat._22 + in.z*mat._32;
	out.z=in.x*mat._13 + in.y*mat._23 + in.z*mat._33;
}
inline void	TransformNormal(vec3& inout, const mat4& m){ 
	inout = vec3(inout.x*m._11 + inout.y*m._21 + inout.z*m._31, 
		inout.x*m._12 + inout.y*m._22 + inout.z*m._32,
		inout.x*m._13 + inout.y*m._23 + inout.z*m._33);
};
inline void	TransformNormal(vec4& inout, const mat4& m){ inout.w=0.0f; inout*=m; }

// returns the screen coords of a 3d coord.  The coords will be in terms of pixels, with the top left corner being (0, 0)
inline vec2 Project(const vec3& worldspacepos, const mat4& view, const mat4& proj, const float screenwidth, const float screenheight){
	vec4 w(worldspacepos, 1.0f);
	w*=(view*proj);
	w/=w.w;
	return vec2( ((w.x+1.0f) * screenwidth *.5f) + 1.0f, ((1.0f - w.y) * screenheight *.5f) + 1.0f);
}
// returns a normalized vector and an origin from a screenspace coord. Since the input is a 2d screen space coord, all the information cannot be retrived, i.e. where exactly does the point exist? It could exist along the ray starting at the origin
//screenspace should be in terms of pixels with the top left being (0, 0)
inline void UnProject(const vec2& screenspacepos, const mat4& view, const mat4& proj, const float screenwidth, const float screenheight, vec3& outray, vec3& outorigin){
	float vx = ( 2.0f*screenspacepos.x/screenwidth  - 1.0f)/proj._11;// Compute picking ray in view space.
	float vy = (-2.0f*screenspacepos.y/screenheight + 1.0f)/proj._22;
	mat4 inverseV(view);
	inverseV.inverse();
	outorigin = vec3(0.0f, 0.0f, 0.0f);
	outray = vec3(vx, vy, 1.0f);
	outorigin*=inverseV;// this function actually translated the point
	TransformNormal(outray, inverseV);// this function does not translate the vector, it only changes the direction into the new coordinate system
	outray.normalize();
}
inline mat4 GetTexScaleBiasMatrix(float size){// Calculate a matrix to transform points to  texture coordinates
	float fTexOffset = 0.5f + (0.5f / size);
	return mat4(       0.5f,        0.0f,   0.0f,  0.0f,
		0.0f,       -0.5f,   0.0f,  0.0f,
		0.0f,        0.0f,   1.0f,  0.0f,
		fTexOffset,  fTexOffset,   0.0f,  1.0f);
}
inline mat4 mirrorMatrix(const vec3 &planeNormal, const float planeOffset){
	return mat4(
		1 - 2 * planeNormal.x * planeNormal.x,
		- 2 * planeNormal.x * planeNormal.y,
		- 2 * planeNormal.x * planeNormal.z,
		- 2 * planeNormal.x * planeOffset,

		- 2 * planeNormal.y * planeNormal.x,
		1 - 2 * planeNormal.y * planeNormal.y,
		- 2 * planeNormal.y * planeNormal.z,
		- 2 * planeNormal.y * planeOffset,

		- 2 * planeNormal.z * planeNormal.x,
		- 2 * planeNormal.z * planeNormal.y,
		1 - 2 * planeNormal.z * planeNormal.z,
		- 2 * planeNormal.z * planeOffset,

		0, 0, 0, 1);
}


inline void quat::frommatrix(const mat4& pm){
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

inline void quat::SetupRotation(const euler &orientation){ SetupRotation(orientation.heading, orientation.pitch, orientation.bank); }

inline void euler::fromObjectToWorldMatrix(const mat4 &m) {// The matrix is assumed to be orthogonal.  The translation portion is ignored.
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



inline bool InsideFrustum(const vec3& point, const Plane planes[6]){// make sure the point is in WORLD SPACE.. as the view frustum is in WORLD SPACE
	for(size_t i(0); i< 6; i++){// this will test all 6 planes, but will return as early as possible		
		if(Dot(point, planes[i]) - planes[i].d <= 0.0f) return false;// early out, if outside of one, outside of all....
	}
	return true;// inside the frustum. Unfortunaly, all planes must be checked 
}
inline bool InsideFrustum(const vec3& point, float radiussquared,const Plane planes[6]){// for spheres. Take note that I use squared Length below..
	for(size_t i=0; i<6; i++){    
		if(Dot(point, planes[i]) + planes[i].d <= -radiussquared ) return false;
	}
	return true;
}

inline void ExtractPlanesD3D( Plane* p_planes, const mat4& comboMatrix, bool normalize) {
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

/*
	fov must be in radians, dy_pixels is the number of pixels of movment, screen_y_size is the total number of pixels in the y dimension of the screen
	This function is used to find out how many radians of movement has occurned given a specific number of pixels.
	For example, you move your mouse 3 pixels to the left and you want to know how many radians of movement that is, call this function.
*/
inline float Screen_y_Pixels_To_Radians(int dy_pixels, float fov, float screen_y_size){
	return (fov/screen_y_size)*(float)dy_pixels;
}
//fov must be in radians, dx_pixels is the number of pixels of movment, screen_x_size is the total number of pixels in the x dimension of the screen. aspect is the aspect of the camera
inline float Screen_x_Pixels_To_Radians(int dx_pixels, float fov, float screen_x_size, float aspect){
	return ((fov/screen_x_size)*(float)dx_pixels)/aspect;
}



inline float Sphere_RayIntersect(const vec3 center, const float radius, const vec3& rayOrigin, const vec3& rayDir){
	vec3 l(center - rayOrigin);
	float s(Dot(l, rayDir));// Lengthance
	float lsq(Dot(l, l));
	if((s < 0.0f) & (lsq > radius*radius)) return INFINITY;// missed the sphere
	float msq(lsq - s*s);
	if(msq > radius*radius) return INFINITY;// missed the sphere
	return s;
}






#endif



