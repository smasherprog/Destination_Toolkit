#include "stdafx.h"
#include "Terrain.h"
#include "../Graphics_Lib/Globals_GraphicsCore.h"
#include "../Common_Game_Objects_Lib/cWorldDefines.h"
#include <sstream>
#include <fstream>
#include "../Graphics_Lib/MeshTypes.h"
#include "../Graphics_Lib/cCamera.h"
#include "../Utilities/Thread.h"
#include <memory>
#include "../Utilities/My_Timer.h"

struct buff0{
	mat4 ViewProjection;
	vec4 CameraPosition;
};
void GenGrid(std::vector<vec4>&grids, float scale){
	const float gridsize = scale*TERRAIN_PATCH_SIZE;
	const float topy = gridsize;
	const float boty = -gridsize;
	const float startx = -gridsize*2;
	// top row first
	for(float x =startx; x <= gridsize; x+=gridsize){
		grids.push_back(vec4(x, topy, gridsize, gridsize));
	}
	// bottom row next
	for(float x =startx; x <= gridsize; x+=gridsize){
		grids.push_back(vec4(x, boty, gridsize, gridsize));
	}
	// left and right patches
	grids.push_back(vec4(startx, 0, gridsize, gridsize));
	grids.push_back(vec4(startx, boty, gridsize, gridsize));
	grids.push_back(vec4(topy, 0, gridsize, gridsize));
	grids.push_back(vec4(topy, boty, gridsize, gridsize));
}


void Terrain::Init(){
	EditorSize=64;
	SelectedTex=1;
	EditorFunction=0;
	EditorColor = vec4(.5f, .5f, 0, 0);
	Base_Terrain::Init();
	InitTempGrid();
	My_Timer::tick_count start = My_Timer::tick_count::now();
	InitTextures();
	OUTPUT_DEBUG_MSG("Time takent to InitTextures is "<<(My_Timer::tick_count::now() - start).seconds())
		InitGrids();
	HS.CompileShaderFromFile("Terrain_.fx");
	DS.CompileShaderFromFile("Terrain_.fx");
	DS_Editor.CompileShaderFromFile("Terrain_.fx", "DSEditor");
	PS.CompileShaderFromFile("Terrain_.fx");
	CBuffer0.Create(1, sizeof(buff0), CONSTANT_BUFFER);
	CBuffer1.Create(1, sizeof(vec4), CONSTANT_BUFFER);
	StartingPos.zero();	
	Editing=DrawWireFrame=false;
	EditorPos.zero();
}

void Terrain::DeInit(){	
	if(!Running) return;// the base class sets this to false when its destructor runs so I can use this to test whether to run my destructor as well
	Base_Terrain::DeInit();
	DeInitTempGrid();
	DeInitTextures();
	DeInitGrids();
	HS.Destroy();
	DS.Destroy();
	DS_Editor.Destroy();
	PS.Destroy();
	CBuffer0.Destroy();
	CBuffer1.Destroy();
}
void Terrain::Update(){
	bool update = false;

	if(Camera.Position.x >= StartingPos.x + (PATCHSIZE )){
		StartingPos.x+=PATCHSIZE;
		update = true;
	} 
	if( Camera.Position.x <= StartingPos.x - (PATCHSIZE)){// the camera has moved too far on the positive x axis, update the grid
		StartingPos.x-=PATCHSIZE ;
		update = true;
	} 
	if(Camera.Position.z >= StartingPos.z  + (PATCHSIZE )){
		StartingPos.z+=PATCHSIZE;
		update = true;
	} 
	if(Camera.Position.z <= StartingPos.z - (PATCHSIZE)){// the camera has moved too far on the positive z axis, update the grid
		StartingPos.z-=PATCHSIZE;
		update = true;
	}

	if(update) Threading::RunTask(std::bind(&Terrain::BeginBuildingNext, this));


}
void Terrain::Draw(){
	std::vector<vec4> grids;
	for(unsigned int i=0; i< Grids.size(); i++){
		float x = Grids[i].x + (Grids[i].z*.5f);
		float z = Grids[i].y + (Grids[i].z*.5f);
		float y = GetHeight((unsigned int)x, (unsigned int)z);
		if(Camera.InsideFrustum(vec3(x, y, z), Grids[i].z*.25f + Grids[i].z*.5f)){// estimate of the radius, this will be a little bit bigger than the actual radius, but thats fine
			grids.push_back(Grids[i]);
		}
	}
	if(grids.empty() && !Editing) return;// empty get out
	if(Points.Size < grids.size() + (Editing ? 1: 0)){// resize
		Points.Create(grids.size() + (Editing ? 1: 0), sizeof(vec4), VERTEX_BUFFER, DYNAMIC, CPU_WRITE, 0);
	}

	vec4* ptr = (vec4*)Points.Map(MAP_WRITE_DISCARD);
	memcpy(ptr, &grids[0], sizeof(vec4)* grids.size());
	ptr+=grids.size();
	if(Editing)
	{
		vec4 temp(EditorPos.x, EditorPos.z, EditorSize, EditorSize);
		memcpy(ptr, &temp, sizeof(vec4));
	}
	Points.UnMap();


	if(DrawWireFrame) Graphics::RasterizerStates::WireBack.Bind();
	else Graphics::RasterizerStates::CullBack.Bind();
	Graphics::DepthStates::StencilSetWDepthTest.Bind(2);
	Graphics::BlendStates::No_Blend.Bind();
	buff0 temp;
	temp.ViewProjection = Camera.Trans_VP;
	temp.CameraPosition = vec4(Camera.Position, 0);
	CBuffer0.Update(&temp);

	Graphics::Shaders::VS_PreHSPassThrough.Bind();
	HS.Bind();
	HS.SetConstantBuffer(CBuffer0);

	DS.Bind();
	DS.SetConstantBuffer(CBuffer0);

	DS.SetResource(&Buffer_NormalHeight, 0);
	DS.SetResource(&Buffer_Uvs, 1);
	DS.SetSampler(Graphics::Samplers::Linear);

	PS.Bind();
	PS.SetResource(&Diffuse, 0);
	PS.SetResource(&Normals, 1);
	PS.SetSampler(Graphics::Samplers::Linear);
	Graphics::SetTopology(PRIM_1_CONTROL_POINT);

	Points.BindAsVertexBuffer(0);

	Graphics::DrawVertices(Grids.size(), 0);

	if(Editing){
		DS_Editor.Bind();
		DS_Editor.SetConstantBuffer(CBuffer0);
		int buff[4];
		buff[0] = SelectedTex;
		CBuffer1.Update(buff);

		DS_Editor.SetConstantBuffer(CBuffer1, 1);
		Graphics::DrawVertices(1, Grids.size());
	}

	Graphics::HullShader::UnBind();
	Graphics::DomainShader::UnBind();

}

void Terrain::DeInitGrids(){
	Points.Destroy();
}

void Terrain::InitGrids(){
	// first 8 grids go from  [ (-256, 128) , (128, 128) ]
	for(float x =-256.0f; x <= 128.0f; x+=128.0f){
		for(float y =-128.0f; y <= 128.0f; y+=128.0f){
			Grids.push_back(vec4(x, y, 128, 128));
		}
	}
	float scale =1;
	for(unsigned int i =0; i< TERRAIN_GRID_LEVELS; i++){
		scale*=2;
		GenGrid(Grids, scale);
	}
}


void Terrain::DeInitTempGrid(){
	Verticies.resize(0);
	TexCoords.resize(0);
}

void Terrain::InitTempGrid(){
	Verticies.resize(TERRAIN_TEX_SIZE*TERRAIN_TEX_SIZE);
	TexCoords.resize(TERRAIN_TEX_SIZE*TERRAIN_TEX_SIZE);
	float invsize = 1.0f/(TERRAIN_TEX_SIZE-1);// the grid goes from NUMROWS-1 to 0
	for(size_t i(0); i < TERRAIN_TEX_SIZE; i++){// each grid will go from (0, 0) to (1, 1)
		for(size_t j(0); j < TERRAIN_TEX_SIZE; j++){// each grid will go from (0, 0) to (1, 1)
			Verticies[i*TERRAIN_TEX_SIZE + j] = vec2((float)i, (float)j);
			TexCoords[i*TERRAIN_TEX_SIZE + j] = Verticies[i*TERRAIN_TEX_SIZE + j]*TERRAIN_TEX_SIZE;
		}
	}
	Indicies.resize(PATCHINDICES);
	unsigned int off =0;
	for(uint32_t i(0); i< TERRAIN_TEX_SIZE-1; i++){
		for(uint32_t j(0); j< TERRAIN_TEX_SIZE-1; j++){
			Indicies[off    ] = i*TERRAIN_TEX_SIZE + j;
			Indicies[off + 1] = i*TERRAIN_TEX_SIZE + j + 1;
			Indicies[off + 2] = (i+1)*TERRAIN_TEX_SIZE + j;
			Indicies[off + 3] = (i+1)*TERRAIN_TEX_SIZE + j;
			Indicies[off + 4] = i*TERRAIN_TEX_SIZE + j + 1;
			Indicies[off + 5] = (i+1)*TERRAIN_TEX_SIZE + j + 1;
			off+=6;
		}
	}
}

void Terrain::DeInitTextures(){
	Diffuse.Destroy();
	Normals.Destroy();
	Buffer_NormalHeight.Destroy();
	Buffer_Tangents.Destroy();
	Buffer_Uvs.Destroy();
}

void Terrain::InitTextures(){

	Diffuse.CreateArray(DiffuseTextures, sizeof(DiffuseTextures)/sizeof(std::string), 1024, DEFAULT, CPU_NONE, NO_FLAG, FORMAT_DXT1);// compress the textures
	Normals.CreateArray( NormalTextures, sizeof(NormalTextures)/sizeof(std::string),1024, DEFAULT, CPU_NONE, NO_FLAG, FORMAT_DXT1);// compress the textures

	std::vector<vec3> normals(TERRAIN_TEX_SIZE*TERRAIN_TEX_SIZE), tangents(TERRAIN_TEX_SIZE*TERRAIN_TEX_SIZE);
	std::vector<vec4> norsandheights(TERRAIN_TEX_SIZE*TERRAIN_TEX_SIZE);
	memset(&normals[0], 0, sizeof(vec3)*TERRAIN_TEX_SIZE*TERRAIN_TEX_SIZE);
	memset(&tangents[0], 0, sizeof(vec3)*TERRAIN_TEX_SIZE*TERRAIN_TEX_SIZE);

	ComputeNormalTangent(&Verticies[0], TERRAIN_TEX_SIZE*TERRAIN_TEX_SIZE, &Heights[0], &Indicies[0], PATCHINDICES, &normals[0], &TexCoords[0], &tangents[0]);
	for(int i=0; i< TERRAIN_TEX_SIZE*TERRAIN_TEX_SIZE; i++){
		norsandheights[i]= vec4(normals[i], Heights[i]);
	}

	Buffer_NormalHeight.Create(&norsandheights[0], TERRAIN_TEX_SIZE, TERRAIN_TEX_SIZE, 0, FORMAT_RGBA32F);
	Staging_Buffer_NormalHeight.Create(0, TERRAIN_TEX_SIZE, TERRAIN_TEX_SIZE, 0, STAGING, CPU_WRITE, NO_FLAG, FORMAT_RGBA32F);

	Buffer_Tangents.Create(&tangents[0], TERRAIN_TEX_SIZE, TERRAIN_TEX_SIZE, 0, FORMAT_RGB32F);
	Staging_Buffer_Tangents.Create(0, TERRAIN_TEX_SIZE, TERRAIN_TEX_SIZE, 0, STAGING, CPU_WRITE, NO_FLAG, FORMAT_RGB32F);

	Buffer_Uvs.Create(&Uvs[0], TERRAIN_TEX_SIZE, TERRAIN_TEX_SIZE, 0, FORMAT_RG8);
	Staging_Buffer_Uvs.Create(0, TERRAIN_TEX_SIZE, TERRAIN_TEX_SIZE, 0, STAGING, CPU_WRITE, NO_FLAG, FORMAT_RG8);

}
void Terrain::BeginBuildingNext(){

	OUTPUT_DEBUG_MSG("Starting to build new textures in the background . .  ");
	std::vector<vec3> normals(TERRAIN_TEX_SIZE*TERRAIN_TEX_SIZE), tangents(TERRAIN_TEX_SIZE*TERRAIN_TEX_SIZE);
	std::vector<vec4> norsandheights(TERRAIN_TEX_SIZE*TERRAIN_TEX_SIZE);
	std::vector<float> heights(TERRAIN_TEX_SIZE*TERRAIN_TEX_SIZE);
	std::vector<Packed_Uvs> uvs(TERRAIN_TEX_SIZE*TERRAIN_TEX_SIZE);

	memset(&normals[0], 0, sizeof(vec3)*TERRAIN_TEX_SIZE*TERRAIN_TEX_SIZE);
	memset(&tangents[0], 0, sizeof(vec3)*TERRAIN_TEX_SIZE*TERRAIN_TEX_SIZE);
	unsigned int i=0;
	unsigned int startingx = static_cast<unsigned int>(StartingPos.x);
	unsigned int startingy = static_cast<unsigned int>(StartingPos.z);
	for(unsigned int y=startingy; y< TERRAIN_TEX_SIZE + startingy; y++){
		for(unsigned int x=startingx; x< TERRAIN_TEX_SIZE + startingx; x++){
			heights[i]= GetHeight(x, y);
			uvs[i] = GetUvs(x, y);
			i+=1;
		}
	}

	ComputeNormalTangent(&Verticies[0], TERRAIN_TEX_SIZE*TERRAIN_TEX_SIZE, &heights[0], &Indicies[0], PATCHINDICES, &normals[0], &TexCoords[0], &tangents[0]);
	for(int i=0; i< TERRAIN_TEX_SIZE*TERRAIN_TEX_SIZE; i++){
		norsandheights[i]= vec4(normals[i], heights[i]);
	}
	Graphics::AquireLock();
	My_Timer::tick_count start = My_Timer::tick_count::now();
	Staging_Buffer_NormalHeight.Update(&norsandheights[0], TERRAIN_TEX_SIZE*TERRAIN_TEX_SIZE, sizeof(vec4));
	Graphics::Texture::Copy(Buffer_NormalHeight, Staging_Buffer_NormalHeight);
	Staging_Buffer_Tangents.Update(&tangents[0], TERRAIN_TEX_SIZE*TERRAIN_TEX_SIZE, sizeof(vec3));
	Graphics::Texture::Copy(Buffer_Tangents, Staging_Buffer_Tangents);
	Staging_Buffer_Uvs.Update(&uvs[0], TERRAIN_TEX_SIZE*TERRAIN_TEX_SIZE, sizeof(Packed_Uvs));
	Graphics::Texture::Copy(Buffer_Uvs, Staging_Buffer_Uvs);
	Graphics::ReleaseLock();
	OUTPUT_DEBUG_MSG("time taken to build . . . "<<(My_Timer::tick_count::now() - start).seconds());
}
