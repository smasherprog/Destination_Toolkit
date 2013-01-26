#ifndef WORLDINFORMATION_H
#define WORLDINFORMATION_H
#include <string>
#include "Math/vec3.h"

#define WRows 256// must be 255 and 255// i use a uint8_t for indexing the array and i use the wraping to make it all work. . . vital
#define WCols WRows
#define FeetInWContainer 32 // this is how many feet there are in each cUpdateArray position..
#define BytesToShift4WContainer 5// this should always be the amount of bites needed to be shifted to divide by FeetInWContainer
#define FeetInWorld (FeetInWContainer * WRows) // total feet across one span of the world

#define CollisionSize 8 // feet spacing in each collision array position
#define CollisionArraySize (FeetInWContainer / CollisionSize) // how many collision arrays positions per each cupdate array
#define BytesToShift4Collision 3 // this should always be the amount of bites needed to be shifted to divide by CollisionSize

#define FeetSpacingInHeightMap 4
#define BytesToShift4Height 2 // this should always be the amount of bites needed to be shifted to divide by FeetSpacingInHeightMap
#define WHeightMapRows (FeetInWorld / FeetSpacingInHeightMap)// how many array positions there are for the height map
#define WHeightMapCols (FeetInWorld / FeetSpacingInHeightMap)// how many array positions there are for the height map


#define DHeightMapChange 0.5f
#define NDHeightMapChange -0.5f


#define DX (1/FeetSpacingInHeightMap)
#define DZ (1/FeetSpacingInHeightMap)

#define MaxInsertionAttempts 5 
//#define niFeetInWorld -1024


const vec3 GuaranteedGoodSpot(0.0f, 0.0f, 0.0f);
const std::string WorldHeightMap="WorldHeightMap.map";


#endif