#ifndef CMESHCONTAINER_H
#define CMESHCONTAINER_H
#include "../Graphics_Lib/Globals_GraphicsCore.h"
#include "../Graphics_Lib/MeshTypes.h"


class cBaseMesh;
class cMesh;

class cObjectContainer {// NOT THREAD SAFE YET@!!!
public:
	
	std::vector<cBaseMesh*> Lod1Mesh, Lod2Mesh, Lod3Mesh;
	
	std::vector<cBaseMesh*> Foliage, Mesh, Lights;
	float Lod1Dist, Lod2Dist;
	/////////////////////////MESH RELATED STUFF
	/*
	ShaderID Shader;
	ShaderTechniqueID sPTech, sPTTech, PTNTReliefShadowTech, PTNTReliefSecantTech, PTNTech, PTNTTech, PNTech;
	InputLayoutID PLayout, PTLayout, PTNLayout, PTNTLayout, PTNTBWLayout, PNLayout;
	ShaderFloat3ID MeshColor;
	ShaderFloat2ID DetailSteps;
	ShaderTechniqueID LowQualityAnimation, MediumQualityAnimation, HighQualityAnimation;
	ShaderFloat4x4ArrayID g_dualquat, g_matrices;
	/////////////////////////LIGHTS RELATED STUFF
	ShaderID LightShader;
	ShaderTechniqueID PointTech, ShowPointTech;
	InputLayoutID PointLayout;
	VertexBufferID PointInstanceBuffer;
	size_t PointInstanceSize, PointLightsDrawn;
	/////////////////WOODYTREE RELATED STUFF
	InputLayoutID				branch_vertex_declaration;
	InputLayoutID				leaf_vertex_declaration;
	
	ShaderTechniqueID			branch_technique;
	ShaderTechniqueID			leaf_technique;
	*/
	////////////VARIOUS MESH
	cMesh *Box, *Sphere;
	
	std::vector<cBaseMesh*> MasterContainer;
	bool Started, ShowPointLights;
	static cBaseMesh* Target;
	//TwBar *MeshLoader;
	//TwType pointType;
	quat DummyQuat;
	std::string ValidModelFiles;

cObjectContainer();
virtual ~cObjectContainer();

void Init();
void DeInit();
void DrawBV(cBaseMesh* target);

bool insert(cBaseMesh* temp);
bool erase(cBaseMesh* temp);
bool erase(cBaseMesh* temp, std::vector<cBaseMesh*>& cont);

bool insertfoliage(cBaseMesh* temp);
bool insertlight(cBaseMesh* temp);
bool insertmesh(cBaseMesh* temp);


void DrawMesh();
void DrawLights();

void SortMeshLods();
void SortMeshLods(std::vector<cBaseMesh*>& lod);
void Draw4ShadowMap(const Plane planes[6]);

cBaseMesh* GetTarget(){ return Target; }
void SetTarget(cBaseMesh* tar);

void LoadObjects(const std::string fname);
void SaveObjects(const std::string fname);

void Hit(std::vector<cBaseMesh*>& objects, std::vector<cBaseMesh*>& targets, std::vector<float>& Lengthances, vec3 rayOrigin, vec3 rayDir);

cBaseMesh* LoadObject(const std::string filename, const vec3& pos, const std::string name, const uint32_t type);// loading an object from a file
cBaseMesh* CreateObject(const vec3& pos, const std::string name, const uint32_t type);// createing a new object 
cBaseMesh* CreateObject(uint32_t type);

virtual void LoseFocus() {Target=0;}
virtual bool Hit(const vec3& screenpos);

virtual void OnResize(){ }// all classes who derive ths MUST define this function
virtual void MoveByOffset(const vec3& offset){}// these dont make any sense for this contain class
virtual void MoveTo(const vec3& newpos){}// these dont make any sense for this contain class
virtual void MoveTo(const vec4& newpos){}// these dont make any sense for this contain class
virtual bool KeyUp(const uint32_t& key);
virtual bool MouseButtonDown(const uint32_t &button);
virtual void DrawImmediate(){ Draw(); }
virtual void Draw(){} // do not call this since each object needs to be drawn differently 

};

#endif
