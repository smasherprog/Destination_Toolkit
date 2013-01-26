#include "stdafx.h"
#include "cServerTerrain.h"
#include <fstream>
#include <string>
#include "../Utilities/MyMath.h"
#include <sstream>
#include "../Utilities/My_Timer.h"
#include "../Utilities/Utilities.h"
#define MAPFILES 1

cServerTerrain::cServerTerrain(): TerrainStarted(false), HeightMap(0), UVMap(0) {	}
cServerTerrain::~cServerTerrain(){ DeInit(); }

void cServerTerrain::Init(){// this is uses quanitized clipping rings for geometry
	My_Timer::tick_count start = My_Timer::tick_count::now();
	OUTPUT_DEBUG_MSG("Allocating Heightmap, size in bytes: "<<MAPFILESIZE*MAPFILESIZE*MAPFILES*MAPFILES*4);
	HeightMap = new float[MAPFILESIZE*MAPFILESIZE*MAPFILES*MAPFILES];
	
	UVMap = new cUVHelper[MAPFILESIZE*MAPFILESIZE*MAPFILES*MAPFILES];
    LoadHeightMap();// load the heightmap
	LoadUVMap();// this must be called after loadheight map
	OUTPUT_DEBUG_MSG("Time to load UV map and heightmap"<<(My_Timer::tick_count::now() - start).seconds());
	TerrainStarted=true;
}

void cServerTerrain::DeInit(){
	if(!TerrainStarted) return;// nothing to see here
	TerrainStarted=false;
	delete [] HeightMap;
	HeightMap=0;
	delete [] UVMap;
	UVMap=0;
	//SaveHeightMap();
	//SaveUvMap();
}

std::string cServerTerrain::GetFileName(std::string basename, uint32_t x, uint32_t y){
	std::ostringstream oks;
	oks<<"Terrain\\"<<basename<<x<<y<<".raw";
	return std::string(oks.str());
}

void cServerTerrain::LoadUVMap(){

	for(size_t ix(0); ix<MAPFILES; ix++){
		for(size_t iz(0); iz<MAPFILES; iz++){
			std::string hfile = GetFileName(UVMAPFILE, ix, iz);
			std::ifstream inFile(hfile.c_str(), std::ios_base::binary);
			if(inFile){
				inFile.read((char*)(UVMap +ix*MAPFILESIZE*MAPFILES + iz*MAPFILESIZE), (std::streamsize)MAPFILESIZE*MAPFILESIZE*sizeof(cUVHelper));// Read the RAW bytes.
				inFile.close();// Done with file.
			} else {
				OUTPUT_DEBUG_MSG("Failed while calling CreateHeightMap. Cannot open the file: "<<hfile);
				return;
			}
		}
	}

}

void cServerTerrain::SaveHeightMap(){
	std::ofstream file(HEIGHTMAPFILE, std::ios_base::binary | std::ios_base::trunc);
    if(file){
		std::vector<int16_t> in(MAPFILESIZE*MAPFILESIZE);
		for(size_t i(0); i< MAPFILESIZE*MAPFILESIZE; i++){
			in[i] = static_cast<int16_t>(HeightMap[i]+ 32767.0f);
		}
		file.write((char*)&in[0], (std::streamsize)MAPFILESIZE*MAPFILESIZE*sizeof(int16_t));// write the RAW bytes.
		file.close();// Done with file.
    } else {
		OUTPUT_DEBUG_MSG("Failed while calling SaveHeightMap. Cannot open the file: "<<HEIGHTMAPFILE);
	}
}

void cServerTerrain::SaveUvMap(){
	std::ofstream file(UVMAPFILE, std::ios_base::binary| std::ios_base::trunc);
    if(file){
		file.write((char*)&UVMap[0], (std::streamsize)MAPFILESIZE*MAPFILESIZE*sizeof(cUVHelper));// Read the RAW bytes.
		file.close();// Done with file.
    } else{
		OUTPUT_DEBUG_MSG("Failed while calling UvHeightMap. Cannot open the file: "<<UVMAPFILE);
	}
}
void cServerTerrain::LoadHeightMap(){
    std::vector<uint16_t> in(MAPFILESIZE*MAPFILESIZE);// A height for each vertex
	for(size_t ix(0); ix<MAPFILES; ix++){
		for(size_t iz(0); iz<MAPFILES; iz++){
			std::string hfile = GetFileName(HEIGHTMAPFILE, ix, iz);
			std::ifstream inFile(hfile.c_str(), std::ios_base::binary);
			if(inFile){
				inFile.read((char*)&in[0], (std::streamsize)MAPFILESIZE*MAPFILESIZE*sizeof(uint16_t));// Read the RAW bytes.
				inFile.close();// Done with file.
			} else {
				OUTPUT_DEBUG_MSG("Failed while calling CreateHeightMap. Cannot open the file: "<<hfile);
				for(uint32_t i = 0; i < MAPFILESIZE*MAPFILESIZE; ++i){
					HeightMap[i] = 1;// keep a copy for me!!
				}
			}
			// Copy the array data into a float array, and scale and offset the heights.
			for(uint32_t i = 0; i < MAPFILESIZE*MAPFILESIZE; ++i){// the heightmap I keep.....
				HeightMap[ix*MAPFILESIZE*MAPFILES + iz*MAPFILESIZE+ i] = (static_cast<float>(in[i]) -32767.0f);// keep a copy for me!!

			}
		}
	}
    //smooth();
}

void cServerTerrain::Wrap(float &x) const{
    if(x >= WORLDSIZE/2) x -= WORLDSIZE;
    else if( x <= -WORLDSIZE/2) x += WORLDSIZE;
}
void cServerTerrain::WrapArraySpace(int32_t& x) const {
	if(x <0) x+=(MAPFILES * MAPFILESIZE);
	else if(x >= MAPFILES * MAPFILESIZE) x-=MAPFILES * MAPFILESIZE;
}
void cServerTerrain::WrapFileSpace(int32_t& x) const {
	if(x < 0) x+=MAPFILES;
	else if(x >= MAPFILES) x-=MAPFILES;
}
bool cServerTerrain::Intersect(const vec3& pos){
	float y = GetHeight(pos.x, pos.z);
	return pos.y <= y;
}

float cServerTerrain::GetHeight(float x, float z) const{

    // Transform from terrain local space to "cell" space.
    float c = x;
    float d = z;

	c+=(MAPFILESIZE*MAPFILES/2);
	d=(MAPFILESIZE*MAPFILES/2)-d;

    // Get the row and column we are in.
    int32_t col = static_cast<int32_t>(c);
	int32_t row = static_cast<int32_t>(d);

    // Grab the heights of the cell we are in.
    // A*--*B
    //  | /|
    //  |/ |
    // C*--*D

    if(row >= MAPFILESIZE*MAPFILES-1) row=MAPFILESIZE*MAPFILES-2;
    if(col >= MAPFILESIZE*MAPFILES-1) col=MAPFILESIZE*MAPFILES-2;
    
    float A = HeightMap[row*MAPFILESIZE*MAPFILES + col];
    float B = HeightMap[row*MAPFILESIZE*MAPFILES + col + 1];
    float C = HeightMap[(row+1)*MAPFILESIZE*MAPFILES + col];
    float D = HeightMap[(row+1)*MAPFILESIZE*MAPFILES + col + 1];
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