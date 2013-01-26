#ifndef CTERRAIN_H
#define CTERRAIN_H
#include <vector>
#include "../Utilities/MyMath.h"
#include "../Common_Game_Objects_Lib/cWorldDefines.h"


class cServerTerrain{
public:
        
	float* HeightMap;
	cUVHelper* UVMap;
	bool TerrainStarted;

cServerTerrain();
~cServerTerrain();
void Init();
void DeInit();

void LoadHeightMap();
void LoadUVMap();
void SaveHeightMap();
void SaveUvMap();

float average(uint32_t i, uint32_t j);

bool Intersect(const vec3& pos);
bool GetContactPoint(const vec3 &raydir, const vec3& rayorigin, vec3& outraydir, vec3& outrayorigin);
Plane GetPlane(const vec3& pos);
void GetTriangle(const vec3& pos, cTriangle& tri);

std::string GetFileName(std::string basename, uint32_t x, uint32_t y);

void WrapArraySpace(int32_t& x) const ;
void WrapFileSpace(int32_t& x) const ;
void Wrap(vec3& pos) const{ Wrap(pos.x); Wrap(pos.z); }
void Wrap(float &x) const;
float GetHeight(float x, float z) const;
float GetHeight(const vec3& pos) const { return GetHeight(pos.x, pos.z); }

float binary_ray_intersect(const vec3& dp, const vec3& ds, float best_depth);

float linear_ray_intersect(const vec3& dp, const vec3& ds);

};

#endif