#include "stdafx.h"
#include "Base_Terrain.h"
#include <fstream>
#include "../Utilities/Utilities.h"

void Base_Terrain::Init(){
	OUTPUT_DEBUG_MSG("Calling Base_Terrain::Init()");
	Running=true;
	LoadHeights();
	LoadUVs();
	LoadVersioning();
	OUTPUT_DEBUG_MSG("Finished Base_Terrain::Init()");
}
void Base_Terrain::DeInit(){
	OUTPUT_DEBUG_MSG("Calling Base_Terrain::DeInit()");
	if(!Running) return;
	Running=false;
	SaveHeights();
	SaveUVs();
	SaveVersioning();
	OUTPUT_DEBUG_MSG("Finished Base_Terrain::DeInit()");
}

float Base_Terrain::GetHeight(unsigned int x, unsigned int y) const{
	x &= MAPFILESIZEBITS;
	y &= MAPFILESIZEBITS;
	return Heights[x + y*MAPFILESIZE];
}
Packed_Uvs Base_Terrain::GetUvs(unsigned int x, unsigned int y) const{
	x &= MAPFILESIZEBITS;
	y &= MAPFILESIZEBITS;
	return Uvs[x + y*MAPFILESIZE];
}

void Base_Terrain::SaveHeights(){

	std::string hfile = Asset_Dir + "Terrain\\";
	hfile +=HEIGHTMAPFILE;
	hfile +=+".raw";
	std::ofstream inFile(hfile.c_str(), std::ios_base::binary);
	if(inFile){
		std::vector<short int> temps(MAPFILESIZE*MAPFILESIZE);
		for(unsigned int i =0; i< temps.size(); i++){
			temps[i] = static_cast<short int>(Heights[i]);
		}
		inFile.write((char*)&temps[0], (std::streamsize)MAPFILESIZE*MAPFILESIZE*sizeof(uint16_t));// Read the RAW bytes.
	}
	inFile.close();// Done with file.
	Heights.resize(0);
}

void Base_Terrain::LoadHeights(){
	Heights.resize(MAPFILESIZE*MAPFILESIZE);
	std::string hfile =Asset_Dir;
	hfile +="Terrain\\";
	hfile +=HEIGHTMAPFILE;
	hfile +=+".raw";
	std::ifstream inFile(hfile.c_str(), std::ios_base::binary);
	if(inFile){
		std::vector<short int> temps(MAPFILESIZE*MAPFILESIZE);
		inFile.read((char*)&temps[0], (std::streamsize)MAPFILESIZE*MAPFILESIZE*sizeof(uint16_t));// Read the RAW bytes.
		inFile.close();// Done with file.
		for(unsigned int i =0; i< temps.size(); i++){
			Heights[i] = static_cast<float>(temps[i]);
		}
	} else {
		OUTPUT_DEBUG_MSG("Failed while calling CreateHeightMap. Cannot open the file: "<<hfile);
		memset(&Heights[0], 0, MAPFILESIZE*MAPFILESIZE*sizeof(float));
	}
}

void Base_Terrain::SaveUVs(){
	std::string ufile = Asset_Dir + "Terrain\\" + UVMAPFILE+ ".raw";
	std::ofstream inFile(ufile.c_str(), std::ios_base::binary);
	if(inFile){
		inFile.write((char*)&Uvs[0], (std::streamsize)MAPFILESIZE*MAPFILESIZE*sizeof(Packed_Uvs));// Read the RAW bytes.
	} 
	inFile.close();// Done with file.
	Uvs.resize(0);
}

void Base_Terrain::LoadUVs(){
	Uvs.resize(MAPFILESIZE*MAPFILESIZE);
	std::string hfile =Asset_Dir;
	hfile +="Terrain\\";
	hfile +=UVMAPFILE;
	hfile +=+".raw";

	std::ifstream inFile(hfile.c_str(), std::ios_base::binary);
	if(inFile){
		inFile.read((char*)&Uvs[0], (std::streamsize)MAPFILESIZE*MAPFILESIZE*sizeof(Packed_Uvs));// Read the RAW bytes.
		inFile.close();// Done with file.
	} else {// auto generate some texture information
		OUTPUT_DEBUG_MSG("Failed while calling UVMapFile. Cannot open the file: "<<hfile<<"Generating UvMap");
		for(size_t i(0); i<MAPFILESIZE*MAPFILESIZE; i++){
			const float fSnowLine = 200.0f;
			const float fTerrainLine = 0.0f;
			const float fRocksLine = -200.0f;

			const float fSnowTerrainBandSize = 100.0f;
			const float fTerrainRocksBandSize = 100.0f;
			const float fRocksLavaBandSize = 100.0;
			uint8_t fTex0Alpha = 0;
			uint8_t fTex1Alpha = 0;
			uint8_t fTex0Index = 0;
			uint8_t fTex1Index = 1;
			float h = Heights[i];
			if(h >= fSnowLine) {           // snow and terrain line			
				fTex0Index = 101;
				fTex1Index = 1;
				fTex0Alpha = static_cast<uint8_t>(min(1.f, (h - fSnowLine)/fSnowTerrainBandSize )*255.0f);
			} else if(h >= fTerrainLine) {   // rocks and terrain 			
				fTex0Index = 2;
				fTex1Index = 1;
				fTex1Alpha = static_cast<uint8_t>(min(1.f,(h - fTerrainLine)/fTerrainRocksBandSize)*255.0f);
			}else if(h >= fRocksLine){        // lava and rocks	
				fTex0Index = 2;
				fTex1Index = 3;
				fTex0Alpha = static_cast<uint8_t>(min(1.f,(h - fRocksLine)/fRocksLavaBandSize)*255.0f);
			}else{
				fTex0Index = 2;
				fTex1Index = 3;
				fTex0Alpha = 0;
			}
			Packed_Uvs temp;
			temp.Alpha = fTex0Alpha;
			temp.Indices = (fTex0Index<<4) | (fTex1Index);
			Uvs[i]=temp;
		}
		OUTPUT_DEBUG_MSG("Finished Generating UvMap");
	}
}
void Base_Terrain::SaveVersioning(){
	const unsigned int elements =MAPFILESIZE/PATCHSIZE * MAPFILESIZE/PATCHSIZE;
	std::string hfile =Asset_Dir;
	hfile +="Terrain\\";
	hfile +="HeightVersion.vrs";

	std::ofstream file(hfile.c_str());
	if(file){
		file.write((char*)&HeightVersionInfo[0], sizeof(unsigned int)*elements);
	}
	file.close();
	hfile =Asset_Dir;
	hfile +="Terrain\\";
	hfile +="UVVersion.vrs";
	file.open(hfile.c_str());
	if(file){
		file.write((char*)&UVVersionInfo[0], sizeof(unsigned int)*elements);
	}
	file.close();
	HeightVersionInfo.resize(0);
	UVVersionInfo.resize(0);
}
void Base_Terrain::LoadVersioning(){
	const unsigned int elements = MAPFILESIZE/PATCHSIZE * MAPFILESIZE/PATCHSIZE;
	HeightVersionInfo.resize(elements );
	UVVersionInfo.resize(elements);
	std::string hfile =Asset_Dir;
	hfile +="Terrain\\";
	hfile +="HeightVersion.vrs";
	std::ifstream file(hfile.c_str());
	if(!file){
		memset(&HeightVersionInfo[0], 0, sizeof(unsigned int)*elements);
	} else {
		file.read((char*)&HeightVersionInfo[0], sizeof(unsigned int)*elements);
		file.close();
	}
	hfile =Asset_Dir;
	hfile +="Terrain\\";
	hfile +="UVVersion.vrs";
	file.open(hfile.c_str());
	if(!file){
		memset(&UVVersionInfo[0], 0, sizeof(unsigned int)*elements);
	} else {
		file.read((char*)&UVVersionInfo[0], sizeof(unsigned int)*elements);
		file.close();
	}
}
#define STEPSIZE 50.0f// step size...
#define MAXLINEARSTEPS 50
#define MAXBINARYSTEPS 250

float binary_ray_intersect(const Base_Terrain& ter, const vec3& origin, const vec3& ray){ 

	float scalar = STEPSIZE*.5f;// this is going to get the point in the middle of beg and end that the ray needs to be scaled
	vec3 temp;

	for( size_t i=0;i<MAXBINARYSTEPS;i++ ){
		temp = origin+ray*scalar;

		float t=ter.GetHeightAt(temp);
		if(abs(t-temp.y) > .5f){// still work to do
			if(t >= temp.y){// under the terrain, move back
				scalar -= scalar * .5f;// subtract half of scalar from itself
			} else {// not under the terrain.. move forward
				scalar += scalar * .5f; // add half of scalar onto itself
			}
		} else {// we are close enough for a hit
			return scalar;
		}
	}
	return scalar;
}

float linear_ray_intersect(const Base_Terrain& ter, const vec3& origin, const vec3& dir){ 
	float raylength=0.0f;// the current length of the starting position
	for( size_t i=0; i<MAXLINEARSTEPS;i++ ){// Linear search for first point inside object (first hit)
		raylength+=STEPSIZE;
		vec3 rayposition = origin+dir*raylength;// temp is the new position
		float terrainheight=ter.GetHeightAt(rayposition);
		if (terrainheight > rayposition.y){ //If the height of the terrain is greater than the height of our new rays position, stop we are under the terrain
			return raylength;
		}// otherwise, our height is still above the terrains height, keep going
	}
	return raylength;
}

vec3 Base_Terrain::GetPosOfIntersection(const vec3& origin, const vec3& dir) const{
	vec3 o(origin), d(dir);
	//do a quick search to find the general area of intersect
	float lengthofray = linear_ray_intersect(*this, o, d);
	// now do an exact search
	o = o + d*(lengthofray - STEPSIZE);// advance the origin to one step back of the intersection point found in the linear search
	// now binary search will start at o on the normal of d
	lengthofray = binary_ray_intersect(*this, o, d);
	return (o + d*lengthofray);
}
float Base_Terrain::GetHeightAt(vec3 pos) const{

	WrapCoord(pos.x);
	WrapCoord(pos.z);
	float c = pos.x*(1.0f/CELLSPACING);
	float d = pos.z*(1.0f/CELLSPACING);

	// Get the row and column we are in.
	unsigned int col = static_cast<unsigned int>(c);
	unsigned int row = static_cast<unsigned int>(d);

	// Grab the heights of the cell we are in.
	// A*--*B
	//  | /|
	//  |/ |
	// C*--*D

	float A = Heights[row*MAPFILESIZE + col];
	float B = Heights[row*MAPFILESIZE + col + 1];
	float C = Heights[(row+1)*MAPFILESIZE + col];
	float D = Heights[(row+1)*MAPFILESIZE + col + 1];
	// Where we are relative to the cell.
	float s = c - (float)col;
	float t = d - (float)row;
	// If upper triangle ABC.
	if(s + t <= 1.0f ) {
		float uy = B - A;
		float vy = C - A;
		return (A + s*uy + t*vy);
	} else {// lower triangle DCB.
		float uy = C - D;
		float vy = B - D;
		return (D + (1.0f-s)*uy + (1.0f-t)*vy);
	}
}


