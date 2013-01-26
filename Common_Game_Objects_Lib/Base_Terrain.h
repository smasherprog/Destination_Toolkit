#ifndef BASE_TERRAIN_H
#define BASE_TERRAIN_H

#include <vector>
#include "../Utilities/MyMath.h"
#include "../Common_Game_Objects_Lib/cWorldDefines.h"

class Base_Terrain{
public:

	std::vector<float> Heights;
	std::vector<Packed_Uvs> Uvs;
	std::vector<unsigned int> HeightVersionInfo, UVVersionInfo;
	bool Running;

	Base_Terrain():Running(false) {}
	virtual ~Base_Terrain(){ DeInit();}

	void Init();
	void DeInit();

	void LoadHeights();
	void LoadUVs();
	void LoadVersioning();

	void SaveHeights();
	void SaveUVs();
	void SaveVersioning();

	vec3 GetPosOfIntersection(const vec3& origin, const vec3& dir) const;
	float GetHeightAt(vec3 pos) const;

	float GetHeight(unsigned int x, unsigned int y) const;
	Packed_Uvs GetUvs(unsigned int x, unsigned int y) const;
};
#endif