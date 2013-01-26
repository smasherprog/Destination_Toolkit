#ifndef CNOISE_H
#define CNOISE_H
#include "MyMath.h"

class cNoise { // Simplex noise in 2D, 3D and 4D
public:

	static uint8_t perm[512];
	static uint8_t simplex[64][4];

	static float noise( float x );
    static float noise( float x, float y );
    static float noise( float x, float y, float z );
    static float noise( float x, float y, float z, float w );

// 1D, 2D, 3D and 4D float Perlin noise, with a specified integer period
 
    static float pnoise( float x, int px );
    static float pnoise( float x, float y, int px, int py );
    static float pnoise( float x, float y, float z, int px, int py, int pz );
    static float pnoise( float x, float y, float z, float w, int px, int py, int pz, int pw );

    static float  grad( int hash, float x );
    static float  grad( int hash, float x, float y );
    static float  grad( int hash, float x, float y , float z );
    static float  grad( int hash, float x, float y, float z, float t );
};

template<int prime0, int prime1, int prime2>
class PerlinNoise
{
public:
	PerlinNoise() {};
	~PerlinNoise() {};

	float SoftNoise1D(int x);

	float Noise1D(int x);
	float Noise1D(int x, int offset);
	float Noise2D(int x, int y);
	float Noise2D(int x, int y, int offset);
	float Noise3D(int x, int y, int z);
	float Noise3D(int x, int y, int z, int offset);

public:
	float Interpolate(float a, float b, float t);
};

template<int prime0, int prime1, int prime2>
inline float PerlinNoise<prime0, prime1, prime2>::Noise1D(int x)
{
	x = (x << 13) ^ x;
	x = x * (x * x * prime0 + prime1) + prime2;
	return ((x & 0x7fffffff) / 1073741823.5f) - 1.f;
}

template<int prime0, int prime1, int prime2>
inline float PerlinNoise<prime0, prime1, prime2>::Noise1D(int x, int offset)
{
	return Noise1D(x + offset);
}

template<int prime0, int prime1, int prime2>
inline float PerlinNoise<prime0, prime1, prime2>::Noise2D(int x, int y)
{
	return Noise1D(x + y * 57);
}

template<int prime0, int prime1, int prime2>
inline float PerlinNoise<prime0, prime1, prime2>::Noise2D(int x, int y, int offset)
{
	return Noise1D(x + y * 57 + offset);
}

template<int prime0, int prime1, int prime2>
inline float PerlinNoise<prime0, prime1, prime2>::Noise3D(int x, int y, int z)
{
	return Noise1D(x + y * 57 + z * 149);
}

template<int prime0, int prime1, int prime2>
inline float PerlinNoise<prime0, prime1, prime2>::Noise3D(int x, int y, int z, int offset)
{
	return Noise1D(x + y * 57 + z * 149 + offset);
}

template<int prime0, int prime1, int prime2>
// Interpolate between a and b where t is the amount (0 to 1)
inline float PerlinNoise<prime0, prime1, prime2>::Interpolate(float a, float b, float t){
	t = (1.f - cos(t * Pi)) * 0.5f; // Change t to a curve
	return a * (1.f - t) + b * t;
}

template<int prime0, int prime1, int prime2>
// interpolate between consecutive noise values
inline float PerlinNoise<prime0, prime1, prime2>::SoftNoise1D(int x)
{
	float a = Noise1D((x>>16));
	float b = Noise1D((x>>16) + 1);
	float t = (x & 0xffff)/65536.f;
	return Interpolate(a, b, t);
}

// some default primes
typedef PerlinNoise<19379, 819233, 1266122899>	PerlinNoiseDefault;

#endif