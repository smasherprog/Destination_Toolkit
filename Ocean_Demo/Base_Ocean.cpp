#include "stdafx.h"
#include "Base_Ocean.h"
#include "../Utilities/Utilities.h"

Base_Ocean::Base_Ocean(){
}
Base_Ocean::~Base_Ocean(){
	DeInit();
}
void Base_Ocean::Init(float worldsize, float gridsize){
	DeInit();
	World_Size = worldsize;
	Grid_Size = gridsize;
	size_t count = static_cast<size_t>(((World_Size/Grid_Size)/8)*(World_Size/Grid_Size));
	Ocean_Grid_bits.resize(count);
	memset(&Ocean_Grid_bits[0], 0, count);
}
void Base_Ocean::DeInit(){
	Ocean_Grid_bits.resize(0);
}

bool Base_Ocean::AddGrid(vec3 tlpos){// each grid is a fixed size of 64x64 and this function ensures that all input is in that format
	// the casts below preform a floor by casting to an unsigned int, then dividing, then multiplying
	WrapCoord(tlpos.x, tlpos.y, tlpos.z);
	int x((int)tlpos.x), y((int)tlpos.z);
	vec3 t( static_cast<float>((x/Grid_Size)*Grid_Size), 0,static_cast<float>((y/Grid_Size)*Grid_Size)); // make sure that this is on a 64 boundary
	
	int arrx(x/static_cast<int>(Grid_Size)), arry(y/static_cast<int>(Grid_Size));// this is the index for each dimension

	int bitindex(arrx/8);
	bitindex=arrx - (bitindex*8);
	int biter = (int)bitindex;	
	unsigned char bitstoshift = (1<<((unsigned char)bitindex));// shift a bit down to the correct index
	arrx/=8;
	const int jump = static_cast<int>(((World_Size/Grid_Size)/8.0f));
	if((Ocean_Grid_bits[arrx + arry* jump ] & bitstoshift) == 0){// the slot is empty.. go ahead with the insert
		Ocean_Grid_bits[arrx + arry* jump] |= bitstoshift;// set the bit
		tlpos = t;
		return true;
	} // else already inserted
	return false;
}
bool Base_Ocean::GridExists(float xx, float yy){
	WrapCoord(xx);
	WrapCoord(yy);
	int x((int)xx), y((int)yy);
	vec3 t( static_cast<float>((x/Grid_Size)*Grid_Size)   , 0,static_cast<float>((y/Grid_Size)*Grid_Size)); // make sure that this is on a 64 boundary

	int arrx(x/static_cast<int>(Grid_Size)), arry(y/static_cast<int>(Grid_Size));// this is the index for each dimension

	int bitindex(arrx/8);
	bitindex=arrx - (bitindex*8);
	int biter = (int)bitindex;	
	unsigned char bitstoshift = (1<<((unsigned char)bitindex));// shift a bit down to the correct index
	arrx/=8;
	const int jump = static_cast<int>(((World_Size/Grid_Size)/8.0f));
	return (Ocean_Grid_bits[arrx + arry* jump] & bitstoshift) != 0;
}

