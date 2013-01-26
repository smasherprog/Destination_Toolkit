#include "stdafx.h"
#include "cObjectContainer.h"
#include "../Graphics_Lib/cMesh.h"
#include "cPointLight.h"
#include "../Graphics_Lib/cCamera.h"
#include "cWoodyTree.h"

cBaseMesh* cObjectContainer::Target;
cObjectContainer::cObjectContainer(): Lod1Dist(400.0f), Lod2Dist(800.0f), Box(0), Sphere(0), Started(false) {}
cObjectContainer::~cObjectContainer() { DeInit();  }
void cObjectContainer::Init(){
	
	// get a list of the avaible models we can load
	aiString sz;
	aiGetExtensionList(&sz);

	ValidModelFiles = "Model Files";
	ValidModelFiles+="7";// empty space to insert the null char
	ValidModelFiles.back() = '\0';
	ValidModelFiles+=sz.data;

	ShowPointLights=false;
	Started=true;
	FormatDesc lay[6] = {
		FormatDesc(TYPE_VERTEX, FORMAT_FLOAT, 3, 0, 0),// position on slot 0
		FormatDesc(TYPE_TEXCOORD, FORMAT_FLOAT, 2, 1, 0),// Texcoords on slot 1
		FormatDesc(TYPE_NORMAL, FORMAT_FLOAT, 3, 1, 0),// normals on slot 2
		FormatDesc(TYPE_TANGENT, FORMAT_FLOAT, 3, 2, 0),// tangents on slot 3
		FormatDesc(TYPE_BONE, FORMAT_FLOAT, 4, 3, 0),// bone indices on slot 4
		FormatDesc(TYPE_WEIGHT, FORMAT_FLOAT, 4, 3, 0)// weights on slot 4
	};
	/*

	Shader = Renderer.CreateShader("Mesh.fxo", true);
	// --------------------first, create the shadowing techniques---------------------------------
	sPTech = Renderer.CreateTechnique(Shader, "sPTech");// for shadowing pos
	sPTTech = Renderer.CreateTechnique(Shader, "sPTTech");// for shadowing pos, tex
	// --------------------now, the normal shadowing techniques---------------------------------
	PNTech = Renderer.CreateTechnique(Shader, "PNTech");// for rendering pos, normals but a color is passed 
	PTNTech = Renderer.CreateTechnique(Shader, "PTNTech");// for rendering pos, tex, normals
	PTNTTech = Renderer.CreateTechnique(Shader, "PTNTTech");// for rendering pos, tex, normals, tangents

	PTNTReliefShadowTech = Renderer.CreateTechnique(Shader, "PTNTReliefShadowTech");// for rendering pos, tex, normals, tangent with self shadowing
	PTNTReliefSecantTech = Renderer.CreateTechnique(Shader, "PTNTReliefSecantTech");// for rendering pos, tex, normals, tangent

	DetailSteps = Renderer.CreateFloat2(Shader, "DetailSteps");

	MeshColor = Renderer.CreateFloat3(Shader, "MeshColor");
	Renderer.SetShader(Shader, PTNTReliefShadowTech);
	vec2 temp(8.00f, 0.05f);
	Renderer.SetFloat2(DetailSteps, &temp);// initialize the relief settings

	Renderer.SetShader(Shader, sPTech);
	PLayout = Renderer.CreateInputLayout(lay, 1, Shader);

	Renderer.SetTechnique(sPTTech);
	PTLayout =Renderer.CreateInputLayout(lay, 2, Shader);

	Renderer.SetTechnique(PTNTech);
	PTNLayout =Renderer.CreateInputLayout(lay, 3, Shader);

	Renderer.SetTechnique(PTNTTech);
	PTNTLayout =Renderer.CreateInputLayout(lay, 4, Shader);

	LowQualityAnimation = Renderer.CreateTechnique(Shader, "LowQualityAnimation");
	MediumQualityAnimation = Renderer.CreateTechnique(Shader, "MediumQualityAnimation");
	HighQualityAnimation = Renderer.CreateTechnique(Shader, "HighQualityAnimation");
	Renderer.SetShader(Shader, HighQualityAnimation);
	g_dualquat = Renderer.CreateFloat4x4Array(Shader, "g_dualquat");
	g_matrices = Renderer.CreateFloat4x4Array(Shader, "g_matrices");
	PTNTBWLayout =Renderer.CreateInputLayout(lay, 6, Shader);


	/////////////////////////////////////////////// LIGHT SETUP
	LightShader= Renderer.CreateShader("Lighting.fxo", true);
	PointTech = Renderer.CreateTechnique(LightShader, "PointTech");
	ShowPointTech = Renderer.CreateTechnique(LightShader, "ShowPointTech");
	Renderer.SetShader(LightShader, PointTech);
	FormatDesc lightlay[]= { 
		FormatDesc(TYPE_VERTEX, FORMAT_FLOAT, 3, 0, false), 
		FormatDesc(TYPE_TEXCOORD, FORMAT_FLOAT, 4, 1, true),
		FormatDesc(TYPE_TEXCOORD, FORMAT_FLOAT, 4, 1, true),
		FormatDesc(TYPE_COLOR, FORMAT_FLOAT, 4, 1, true),
		FormatDesc(TYPE_TEXCOORD, FORMAT_FLOAT, 4, 1, true),
		FormatDesc(TYPE_TEXCOORD, FORMAT_FLOAT, 4, 1, true),
		FormatDesc(TYPE_TEXCOORD, FORMAT_FLOAT, 4, 1, true),
	};

	PointLayout = Renderer.CreateInputLayout(lightlay, 6, LightShader);
	PointInstanceSize =20;
	PointInstanceBuffer = Renderer.CreateVertexBuffer(PointInstanceSize, sizeof(vec4)*5, DYNAMIC, 0, CPU_WRITE);

//////////////////creating the mesh for when I need a bounding volume to be drawn. .. .. . Must do this manually
	FormatDesc otherlay[] = { 
		FormatDesc(TYPE_VERTEX, FORMAT_FLOAT, 3, 0), 
		FormatDesc(TYPE_NORMAL, FORMAT_FLOAT, 3, 1)
	};
	Renderer.SetShader(Shader, PNTech);
	PNLayout = Renderer.CreateInputLayout(otherlay, 2, Shader);

	/////////////////////////////Create a Box... used for drawing around selected objects
	Box = new cMesh();// this doesnt really need to be set up like a regular mesh because its not needed that way
	GenCube(Box->Vertices, Box->Lod1.Indices);
	std::vector<vec3> norms(Box->Vertices.size());
	ComputeNormals(&Box->Vertices[0], Box->Vertices.size(), &Box->Lod1.Indices[0], Box->Lod1.Indices.size(), &norms[0]);
	Box->Lod1.Technique = PNTech;
	Box->Lod1.InputLayout = PNLayout;
	Batch* batch = Box->Lod1.CreateBatch();
	
	Box->Lod1.IB = Renderer.CreateIndexBuffer(Box->Lod1.Indices.size(), sizeof(uint16_t), IMMUTABLE, &Box->Lod1.Indices[0]);
	Box->Lod1.VB.push_back(Renderer.CreateVertexBuffer(Box->Vertices.size(), sizeof(vec3), IMMUTABLE, &Box->Vertices[0], CPU_NONE));
	
	Box->Lod1.VB.push_back(Renderer.CreateVertexBuffer(norms.size(), sizeof(vec3), IMMUTABLE, &norms[0], CPU_NONE));

	batch->numIndices= static_cast<uint32_t>(Box->Lod1.Indices.size());
	batch->StartIndex =0;
	batch->StartVertex =0;
	batch->numVertices = static_cast<uint32_t>(Box->Vertices.size());



	///////////////////////////Create a sphere used for .... Drawing spheres!!!
	Sphere = new cMesh();// this doesnt really need to be set up like a regular mesh because its not needed that way
	std::vector<TexNorm> texnorm;

	GenSphere(1.0f, 16, 16, Sphere->Vertices, texnorm, Sphere->Lod1.Indices);

	Sphere->Lod1.Technique = PTNTech;// pos normal tech
	Sphere->Lod1.InputLayout = PTNLayout;
	batch = Sphere->Lod1.CreateBatch();
	Sphere->Lod1.IB = Renderer.CreateIndexBuffer(Sphere->Lod1.Indices.size(), sizeof(uint16_t), IMMUTABLE, &Sphere->Lod1.Indices[0]) ;
	Sphere->Lod1.VB.push_back(Renderer.CreateVertexBuffer(Sphere->Vertices.size(), sizeof(vec3), IMMUTABLE, &Sphere->Vertices[0], CPU_NONE));
	Sphere->Lod1.VB.push_back(Renderer.CreateVertexBuffer(texnorm.size(), sizeof(TexNorm), IMMUTABLE, &texnorm[0], CPU_NONE));
	batch->numIndices= static_cast<uint32_t>(Sphere->Lod1.Indices.size());
	batch->StartIndex =0;
	batch->StartVertex =0;
	batch->numVertices = static_cast<uint32_t>(Sphere->Vertices.size());


//////////////////////////////////////////WOODYTREE SETUPS HERE

	branch_technique = Renderer.CreateTechnique(Shader, "branch_technique");
	leaf_technique = Renderer.CreateTechnique(Shader, "leaf_technique");
	cWoodyTree::xzip_vector = Renderer.CreateFloat4(Shader, "xzip_vector");
	cWoodyTree::dul_vector= Renderer.CreateFloat3(Shader, "dul_vector");
	cWoodyTree::ra_vector= Renderer.CreateFloat2(Shader, "ra_vector");
	cWoodyTree::is_translucent = Renderer.CreateBool(Shader, "is_translucent");
	cWoodyTree::normal_multiplier = Renderer.CreateFloat(Shader, "normal_multiplier");
	cWoodyTree::scale_vector = Renderer.CreateFloat2(Shader, "scale_vector");
	cWoodyTree::center_of_leaves = Renderer.CreateFloat3(Shader, "center_of_leaves");


	FormatDesc branch_vd [] = {
		FormatDesc(TYPE_VERTEX,   FORMAT_FLOAT, 3, 0, false),
		FormatDesc(TYPE_COLOR,    FORMAT_BYTE_UNORM, 4, 0, false),
		FormatDesc(TYPE_TEXCOORD, FORMAT_FLOAT, 2, 0, false),
		FormatDesc(TYPE_NORMAL,   FORMAT_FLOAT, 3, 0, false),
		FormatDesc(TYPE_TEXCOORD, FORMAT_FLOAT, 4, 0, false),
		FormatDesc(TYPE_TEXCOORD, FORMAT_FLOAT, 4, 0, false)
	};
	FormatDesc leaf_vd [] = {
		FormatDesc(TYPE_VERTEX,   FORMAT_FLOAT, 3, 0, false),
		FormatDesc(TYPE_COLOR,    FORMAT_BYTE_UNORM, 4, 0, false),
		FormatDesc(TYPE_TEXCOORD, FORMAT_FLOAT, 2, 0, false),
		FormatDesc(TYPE_TEXCOORD, FORMAT_FLOAT, 1, 0, false),
		FormatDesc(TYPE_TEXCOORD, FORMAT_FLOAT, 4, 0, false),
		FormatDesc(TYPE_TEXCOORD, FORMAT_FLOAT, 4, 0, false),
		FormatDesc(TYPE_TEXCOORD, FORMAT_FLOAT, 4, 0, false)
	};
	Renderer.SetShader(Shader, branch_technique);
	branch_vertex_declaration = Renderer.CreateInputLayout(branch_vd, 6, Shader);
	Renderer.SetTechnique(leaf_technique);
	leaf_vertex_declaration = Renderer.CreateInputLayout(leaf_vd, 7, Shader);

	LoadObjects("\\Mesh\\Object.myx");
	
	MeshLoader = TwNewBar("MeshToolbar");
	TwDefine("MeshToolbar iconified=true");
	TwEnumVal objecttypes[] = { {MESH, "Mesh"}, {POINTLIGHT, "Point_Light"},{WOODYTREEMESH, "Tree"} }; // Create a type for the enum shapeEV
	TwType meshtypes = TwDefineEnum("MeshTypes", objecttypes, 3);  
	TwAddVarRW(MeshLoader, "Object_Types",		meshtypes,			&LoadType,			"");
	TwAddButton(MeshLoader, "Load_Object",		TwLoadObject,		this,				"");
			 TwStructMember pointMembers[] = { 
			{ "A0", TW_TYPE_FLOAT, offsetof(vec4, x), " Min=0 Max=2 Step=0.001 " },
			{ "A1", TW_TYPE_FLOAT, offsetof(vec4, y), " Min=0 Max=2 Step=0.001 " }, 
			{ "A2", TW_TYPE_FLOAT, offsetof(vec4, z), " Min=0 Max=2 Step=0.001 " }, 	
			{ "Specular_Power", TW_TYPE_FLOAT, offsetof(vec4, w), " Min=0 Max=50 Step=1.0 " }, 	
		};
	pointType = TwDefineStruct("POINT1", pointMembers, 4, sizeof(vec4), NULL, NULL);
	*/
}
void cObjectContainer::DeInit(){
	if(!Started) return;// nothing to see here!!
	Started=false;
	SaveObjects("\\Mesh\\Object.myx");

	for(size_t i(0); i< MasterContainer.size(); i++){
		delete MasterContainer[i];
	}

	MasterContainer.clear();
	Lod1Mesh.clear();
	Lod2Mesh.clear();
	Lod3Mesh.clear();

	Foliage.clear();
	Mesh.clear(); 
	Lights.clear();

	delete Box; 
	Box=0;

	delete Sphere;
	Sphere=0;
	/*
	Renderer.ReleaseShader(Shader);
	Renderer.ReleaseInputLayout(PLayout);
	Renderer.ReleaseInputLayout(PTLayout);
	Renderer.ReleaseInputLayout(PTNLayout);
	Renderer.ReleaseInputLayout(PTNTLayout);
	Renderer.ReleaseInputLayout(PTNTBWLayout);
	Renderer.ReleaseShader(LightShader);
	Renderer.ReleaseInputLayout(PointLayout);
	Renderer.ReleaseVertexBuffer(PointInstanceBuffer);
	Renderer.ReleaseInputLayout(PNLayout);

	Renderer.ReleaseInputLayout(branch_vertex_declaration);
	Renderer.ReleaseInputLayout(leaf_vertex_declaration);
	*/

}
void cObjectContainer::SortMeshLods(){// this is just a tester function to find out how I want to sort the mesh.. not optimized and its a piece of crap.. but it will work
	//// 
	Lod1Mesh.resize(0);
	Lod2Mesh.resize(0);
	Lod3Mesh.resize(0);
	SortMeshLods(Mesh);
	SortMeshLods(Foliage);

}
void cObjectContainer::SortMeshLods(std::vector<cBaseMesh*>& lod){
	vec3 pos(Camera.Position);
	for(size_t i(0); i< lod.size(); ++i){
		cAABB& bv(lod[i]->WorldSpace_BV);
		if(!Camera.InsideFrustum(bv.Center, bv.Radius)) continue;	// outside the frustum..
		vec3 meshpos;
		lod[i]->GetPosition(meshpos.x, meshpos.y, meshpos.z);
		Lod1Mesh.push_back(lod[i]);
		// just using lod1 right now
		/*
		if(LengthSq(meshpos, pos) <=Lod1Dist) {
			Lod1Mesh.push_back(lod[i]);
		} else if(LengthSq(meshpos, pos) <=Lod2Dist){
			Lod2Mesh.push_back(lod[i]);
		} else {
			Lod3Mesh.push_back(lod[i]);
		}
		*/
	}
}

void cObjectContainer::DrawLights(){

	//PointLightsDrawn=0;//reset this
	vector<vec4> data;// use 4 per instance
	vec2 zw(Camera.Proj._33, Camera.Proj._43);
	float radius = .001f;
	vec4 lightPos(0.0f, 0.0f, 0.0f, 0.0f);
	vec4 Linfo, lPos;

	for(size_t i(0); i< Lights.size(); i++){
		cPointLight* l=static_cast<cPointLight*>(Lights[i]);
		if(!l->On) continue;// the light is not on.. skip it
		float radius = l->Radius;
		lightPos.x = l->Center.x;
		lightPos.y = l->Center.y;
		lightPos.z = l->Center.z;
		lightPos.w=radius;
		Linfo.z = 1.0f/radius;
		Linfo.w= l->Attenuation.w;
		lPos = lightPos;
		lPos.w=1.0f;
		lPos*=Camera.View;// transform the light into view space...to do frustum check
		float z1 = lPos.z + radius;
		if (z1 > Camera.NearPlane){// make sure the light is in our view bounds
			float z0 = max(lPos.z - radius, Camera.NearPlane);
			//Linfo.y = saturate(zw.x + zw.y / z0);// the lights near plane effect 
			//Linfo.x = saturate(zw.x + zw.y / z1);// the lights far plane effect 
			data.push_back(Linfo);
			data.push_back(lightPos);
			data.push_back(l->Diffuse);
			data.push_back(l->Attenuation);
			data.push_back(l->SpecularColor);
			//PointLightsDrawn++;
		}
	}
	/*
	if(PointLightsDrawn>0){
		if(PointLightsDrawn>=PointInstanceSize){// grow the buffer
			PointInstanceSize = PointLightsDrawn +10;
			PointInstanceBuffer = Renderer.CreateVertexBuffer(PointInstanceBuffer, PointInstanceSize, sizeof(vec4)*5 , DYNAMIC, 0, CPU_WRITE);
		}
		Renderer.UpdateVertexBuffer(PointInstanceBuffer, &data[0], PointLightsDrawn, sizeof(vec4)*5);
	} 

	Renderer.SetShader(LightShader);
	Renderer.SetTextureSlot0(Renderer.BackBufferTexture);// color rt
	Renderer.SetTextureSlot1(Renderer.NormalRT);// normal 
	Renderer.SetTextureSlot2(Renderer.DepthRT);// depth.. duhh

	Renderer.SetInputLayout(PointLayout);
	Renderer.SetVertexBuffer(0, Sphere->Lod1.VB[0]);
	Renderer.SetVertexBuffer(1, PointInstanceBuffer);
	Renderer.SetIndexBuffer(Sphere->Lod1.IB);
	Renderer.SetTopology(PRIM_TRIANGLELIST);
	Renderer.SetDepthState(Renderer.DepthTest);
	if(ShowPointLights){// this means we should render the lights as a wireframe so we can see them and target them... ok?	
		Renderer.SetRasterizerState(Renderer.RSWireNoCull);
		Renderer.SetTechnique(ShowPointTech);
		Renderer.Apply();
		Renderer.DrawIndexedInstanced(0, Sphere->Lod1.Indices.size(), PointLightsDrawn);
	} 
	Renderer.SetTechnique(PointTech);
	
	Renderer.SetRasterizerState(Renderer.RSCullBack);
	Renderer.SetBlendState(Renderer.BSBlendAdd);
	
	Renderer.Apply();
	Renderer.DrawIndexedInstanced(0, Sphere->Lod1.Indices.size(), PointLightsDrawn);
	*/
}


void cObjectContainer::DrawMesh(){
	/*
	SortMeshLods();
	//////////////standard settings for all mesh
	Renderer.SetShader(Shader);
	Renderer.SetTopology(PRIM_TRIANGLELIST);
	Renderer.SetRasterizerState(Renderer.RSCullBack);// cull the back of all geometry
	Renderer.SetBlendState(BS_NONE);
	Renderer.SetDepthState(Renderer.StencilWDepthTest, 2);// use z depth checking

	for(size_t i(0); i != Lod1Mesh.size(); i++){
		Lod1Mesh[i]->DrawLod1(&g_matrices);
	}
	for(size_t i(0); i != Lod2Mesh.size(); i++){
		Lod2Mesh[i]->DrawLod2(&g_matrices);
	}
	for(size_t i(0); i != Lod3Mesh.size(); i++){
		Lod3Mesh[i]->DrawLod3(&g_matrices);
	}
	*/

}

void cObjectContainer::Draw4ShadowMap(const Plane planes[6]){
	/*
	Renderer.SetShader(Shader, sPTech);
	Renderer.SetTopology(PRIM_TRIANGLELIST);
	Renderer.SetRasterizerState(Renderer.RSCullBack);// cull the back of all geometry
	Renderer.SetBlendState(BS_NONE);
	Renderer.SetDepthState(Renderer.DepthTest);// use z depth checking
	Renderer.SetInputLayout(PLayout);
	for(size_t i(0); i != Lod1Mesh.size(); i++){
		Lod1Mesh[i]->DrawShadowLod1(&g_dualquat);
	}
	for(size_t i(0); i != Lod2Mesh.size(); i++){
		Lod2Mesh[i]->DrawShadowLod1(&g_matrices);
	}
	for(size_t i(0); i != Lod3Mesh.size(); i++){
		Lod3Mesh[i]->DrawShadowLod1(&g_matrices);
	}
	*/
}
bool cObjectContainer::insert(cBaseMesh* temp){
	uint32_t type = temp->GetType();
	if(WOODYTREE(type)) return insertfoliage(temp);// foliage has to be inserted differently
	else if(LIGHTTYPE(type)) return insertlight(temp);
	else if(MESHTYPE(type)) return insertmesh(temp);
	else return false;// not recognized type
}
bool cObjectContainer::insertfoliage(cBaseMesh* temp){
	Foliage.push_back(temp);
	// this setups is a little bit different... I use what I have. The lods dont contain the normal information here
	// these do not reflect the lod of the trees
	cWoodyTree* t = static_cast<cWoodyTree*>(temp);// cast to the correct type
	//t->Lod1.Technique = branch_technique;/// this should be the
	//t->Lod2.Technique = branch_technique;
	//t->Lod3.Technique = leaf_technique;

	MasterContainer.push_back(temp);
	return true;
}
bool cObjectContainer::insertlight(cBaseMesh* temp){
	Lights.push_back(temp);
	MasterContainer.push_back(temp);
	return true;
}
bool cObjectContainer::insertmesh(cBaseMesh* temp){
	Mesh.push_back(temp);

	bool hasbones = temp->HasBones();
	/*
	if(!hasbones){
		temp->Lod1.Technique = PTNTTech;
		temp->Lod1.InputLayout = PTNTLayout;

		temp->Lod2.Technique = PTNTech;
		temp->Lod2.InputLayout = PTNTLayout;

		temp->Lod3.Technique = PTNTech;
		temp->Lod3.InputLayout = PTNLayout;
	} else {

		temp->Lod1.Technique = HighQualityAnimation;
		temp->Lod1.InputLayout = PTNTBWLayout;

		temp->Lod2.Technique = MediumQualityAnimation;
		temp->Lod2.InputLayout = PTNTBWLayout;

		temp->Lod3.Technique = LowQualityAnimation;
		temp->Lod3.InputLayout = PTNTBWLayout;
	} 
	*/
	MasterContainer.push_back(temp);
	return true;
}

bool cObjectContainer::erase(cBaseMesh* temp){
	if(temp==NULL) return false;
	uint32_t type = temp->GetType();
	if(WOODYTREE(type)) return erase(temp, Foliage);
	else if(LIGHTTYPE(type)) return erase(temp, Lights);
	else if(MESHTYPE(type)) return erase(temp, Mesh);
	else return false;// not recognized type
}

bool cObjectContainer::erase(cBaseMesh* temp, std::vector<cBaseMesh*>& cont){
	for(std::vector<cBaseMesh*>::iterator i(cont.begin()); i !=cont.end(); i++){
		if(*i == temp){
			cont.erase(i);
			break;
		}
	}
	for(std::vector<cBaseMesh*>::iterator i(MasterContainer.begin()); i !=MasterContainer.end(); i++){
		if(*i == temp) {
			delete *i;
			MasterContainer.erase(i);
			return true;
		}
	}
	OUTPUT_DEBUG_MSG("couldnt find the object");
	return false;
}
//STATICMESH, ANIMATEDMESH, POINTLIGHT, WOODYTREEMESH,
cBaseMesh* cObjectContainer::CreateObject(uint32_t type){
	switch(type){
		case(MESH):
			return new cMesh; 
		case(POINTLIGHT):
			return new cPointLight; 
		case(WOODYTREEMESH):
			return new cWoodyTree; 
		default:
			return 0;
	};
}


void cObjectContainer::LoadObjects(const std::string fname){
	OUTPUT_DEBUG_MSG("Calling void cObjectContainer::LoadObjects() ");
	ifstream file( (Asset_Dir + fname).c_str(), ios::binary);// this file contains the world mesh stuff
	if(file){// if the file does not exist... skip and contntinue loading	
		uint32_t e(0);
		file.read(reinterpret_cast<char*>(&e), sizeof(uint32_t));// read how many 
		for(uint32_t i(0); i< e; i++) {
			uint32_t ty;
			file.read(reinterpret_cast<char*>(&ty), sizeof(uint32_t));// get the type of this object
			cBaseMesh* temp = CreateObject(ty);
			if(temp ==NULL) OUTPUT_DEBUG_MSG("Error loading Mesh file, type not recognized!!");
			if(temp->Load(file)) {
				insert(temp);
				OUTPUT_DEBUG_MSG("Successful insertion!!");
			} else delete temp;
		}
		file.close();
	} else OUTPUT_DEBUG_MSG("cannot open the file for tbe cObjectContainer::LoadObjects()");
	OUTPUT_DEBUG_MSG("Total of "<<MasterContainer.size()<<" number of objects Loaded")
}
void cObjectContainer::SaveObjects(const std::string fname){
	ofstream file((Asset_Dir + fname).c_str(), ios::trunc | ios::binary);// this file contains the world mesh stuff
	if(file){// if the file does not exist... skip and contntinue loading
		uint32_t e = static_cast<uint32_t>(MasterContainer.size());
		file.write(reinterpret_cast<char*>(&e), sizeof(uint32_t));// write how many 
		for(uint32_t i(0); i< MasterContainer.size(); i++) {
			e=MasterContainer[i]->GetType();
			file.write(reinterpret_cast<char*>(&e), sizeof(uint32_t));// write the type of this object
			MasterContainer[i]->Save(file);// let the object save itself
		}
		file.close();
		OUTPUT_DEBUG_MSG("Saved "<<MasterContainer.size()<<" number of objects");
	}
}
void cObjectContainer::Hit(std::vector<cBaseMesh*>& objects, std::vector<cBaseMesh*>& targets, std::vector<float>& Lengthances, vec3 rayOrigin, vec3 rayDir){
	for(size_t i(0); i< objects.size(); ++i){
		float s = objects[i]->RayIntersect(rayOrigin, rayDir);
		if(s==INFINITY) continue;// missed it
		targets.push_back(objects[i]);
		Lengthances.push_back(s);// add the Lengthance
	}
}

cBaseMesh* cObjectContainer::LoadObject(const std::string filename, const vec3& pos, const std::string name, const uint32_t type){
	if(type != POINTLIGHT){
		ifstream f(filename.c_str());// this filename should be the complete path, not a relative one
		if(!f){
			OUTPUT_DEBUG_MSG("Error loading Object! The file, " + filename + " does not exist.");
			return 0;
		}
		f.close();
	}
	if(type >= NULL_TYPE) {
		OUTPUT_DEBUG_MSG("cObjectContainer::LoadObject() called, but the type passed is not valid");
		return 0;
	}
	cBaseMesh* temp = CreateObject(type);
	if(temp->Load(filename)){
		if(insert(temp)){
			temp->SetPosition(pos.x, pos.y, pos.z);
			temp->InitAABB();
			temp->UpdateWorldMatrix();
			temp->SetName(name);
			temp->Save(filename);
			return temp;// make sure the insertion was good
		}
	}	
	OUTPUT_DEBUG_MSG("Failed to load mesh file " + filename);
	delete temp;
	return 0;
}
cBaseMesh* cObjectContainer::CreateObject(const vec3& pos, const std::string name, const uint32_t type){
	if(type == WOODYTREEMESH) return 0;
	else if(type >= NULL_TYPE) {
		OUTPUT_DEBUG_MSG("cObjectContainer::LoadObject() called, but the type passed is not valid");
		return 0;
	}
	cBaseMesh* temp = CreateObject(type);
	if(insert(temp)){
		temp->SetPosition(pos.x, pos.y, pos.z);
		temp->SetName(name);
		return temp;// make sure the insertion was good
	}	
	OUTPUT_DEBUG_MSG("The function failed to load cObjectContainer::CreateObject");
	delete temp;
	return 0;
}
bool cObjectContainer::MouseButtonDown(const uint32_t &button){
	

	//if(Hit(g_MousePos_V)){

		return true;
	//} else { 
	
		return 0;
	//}
}
bool cObjectContainer::Hit(const vec3& screenpos){

	float vx=0;// = (+2.0f*screenpos.x/g_WindowDimensions_V.x  - 1.0f)/Camera.Proj._11;// Compute picking ray in view space.
	float vy=0;// = (-2.0f*screenpos.y/g_WindowDimensions_V.y + 1.0f)/Camera.Proj._22;
	mat4 inverseV(Camera.InvView);
	vec3 rayOrigin(0.0f, 0.0f, 0.0f);
	vec3 rayDir(vx, vy, 1.0f);
	rayOrigin*=inverseV;// this function actually translated the point
	TransformNormal(rayDir, inverseV);// this function does no translate the std::vector, it only changes the direction into the new coordinate system
	std::vector<cBaseMesh*> targets;
	std::vector<float> Lengthances;
	std::string empty;
	
	Hit(Foliage, targets, Lengthances,  rayOrigin, rayDir);
	Hit(Mesh, targets, Lengthances,  rayOrigin, rayDir);
	Hit(Lights, targets, Lengthances,  rayOrigin, rayDir);
	// figure out which target we really hit
	cBaseMesh* test(0);
	float cLength(INFINITY);
	for(size_t i(0); i< targets.size(); i++){
		if(Lengthances[i] < cLength){
			cLength=Lengthances[i];
			test=targets[i];
		}
	}
	SetTarget(test);
	return test!=NULL;
}
void cObjectContainer::SetTarget(cBaseMesh* ob){
	Target = ob; 
}

bool cObjectContainer::KeyUp(const uint32_t& key) { 
	

		return true;

}