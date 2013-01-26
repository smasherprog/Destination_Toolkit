#ifndef BASE_OCEAN_H
#define BASE_OCEAN_H
#include "../Utilities/MyMath.h"
#include <vector>

class Base_Ocean{
public:

	Base_Ocean();
	virtual ~Base_Ocean();

	void Init(float worldsize, float gridsize);
	void DeInit();

	virtual void Add_Ocean_Grid(char* data, unsigned int size, size_t channel)=0;

	bool AddGrid(vec3 tlpos);// each grid is a fixed size of 64x64
	bool GridExists(const vec3& tlpos){ return GridExists(tlpos.x, tlpos.z); }
	bool GridExists(float xx, float yy);
	void WrapCoord(float& coord) { if(coord >= World_Size) coord -= World_Size; else if(coord < 0.0f) coord += World_Size; }
	void WrapCoord(float& coordx, float& coordy, float& coordz) { WrapCoord(coordx); WrapCoord(coordy); WrapCoord(coordz);  }

	virtual void GridAdded(const vec3& tlpos)=0;// each grid is a fixed size of 64x64

	std::vector<unsigned char> Ocean_Grid_bits;
protected:
	float World_Size, Grid_Size;
};
#endif