#include "stdafx.h"
#include "Static_Mesh.h"
#include "../Assimp/include/assimp/Scene.h"
#include "../Assimp/include/assimp/PostProcess.h"
#include "../Assimp/include/assimp/cimport.h"

void Static_Mesh::ExtractMaterials(Batch& batch, const aiMaterial* pcMat){
	Material* mat = batch.GetMaterial();
	mat->Diffuse = vec4(1.0f, 1.0f, 1.0f, 1.0f);
	aiGetMaterialColor(pcMat,AI_MATKEY_COLOR_DIFFUSE, (aiColor4D*)&mat->Diffuse);
	mat->Specular = vec4(1.0f, 1.0f, 1.0f, 1.0f);
	aiGetMaterialColor(pcMat,AI_MATKEY_COLOR_SPECULAR, (aiColor4D*)&mat->Specular);
	mat->Ambient = vec4(1.0f, 1.0f, 1.0f, 1.0f);
	aiGetMaterialColor(pcMat,AI_MATKEY_COLOR_AMBIENT,(aiColor4D*)&mat->Ambient);
	mat->Emissive = vec4(1.0f, 1.0f, 1.0f, 1.0f);
	aiGetMaterialColor(pcMat,AI_MATKEY_COLOR_EMISSIVE, (aiColor4D*)&mat->Emissive);
	mat->Power =1.0f;
	aiGetMaterialFloat(pcMat,AI_MATKEY_SHININESS_STRENGTH,&mat->Power);
}

void Static_Mesh::LoadMaterials(const aiMesh* mesh, aiMaterial** mMaterials, Batch *batch, std::string& path){
	aiString szPath;
	std::string basefolder = Asset_Dir + "Mesh\\";
	///////////////////DIFFUSE TEXTURE
	if( AI_SUCCESS == aiGetMaterialString(mMaterials[mesh->mMaterialIndex], AI_MATKEY_TEXTURE_DIFFUSE(0), &szPath) ){
		std::string filename(StripPath(szPath.data));// sometimes the textures are stored with their full path, which is not correct, strip it
		Graphics::Texture* diffuse = batch->GetDiffuseMap();
		diffuse->Create(path + filename);
		diffuse->Save_To_Disk(basefolder + StripFileExtention(filename) + ".dds");
	}  else {
		OUTPUT_DEBUG_MSG("failure getting the diffuse texture from assimp model "<<szPath.length);
	}
	/////////////////////// BUMP MAP
	if( AI_SUCCESS == aiGetMaterialString(mMaterials[mesh->mMaterialIndex], AI_MATKEY_TEXTURE_HEIGHT(0),&szPath) ){
		std::string filename(StripPath(szPath.data));// sometimes the textures are stored with their full path, which is not correct, strip it
		Graphics::Texture* normalbump = batch->GetNormal_Bump();

		if(GetFileExtention(filename) == ".dds"){// file extention correct, load it
			normalbump->Create(path + filename);
			normalbump->Save_To_Disk(basefolder + StripFileExtention(filename) + ".dds");
		} else {// convert it to a dds file
			Graphics::Texture::GenerateNormalBumpFromBump(path + filename, basefolder + StripFileExtention(filename) + ".dds");
			normalbump->Create(basefolder + StripFileExtention(filename) + ".dds");
		}
		//////////////////NORMAL MAP
	} else if( AI_SUCCESS == aiGetMaterialString(mMaterials[mesh->mMaterialIndex], AI_MATKEY_TEXTURE_NORMALS(0),&szPath) ){// if there is a bump, I generate a normal map from it, so I dont bother with this
		std::string filename(StripPath(szPath.data));// sometimes the textures are stored with their full path, which is not correct, strip it

		Graphics::Texture* normalmap = batch->GetNormalMap();
		normalmap->Create(path + filename);
		normalmap->Save_To_Disk(basefolder + StripFileExtention(filename) + ".dds");

	} 
	ExtractMaterials(*batch, mMaterials[mesh->mMaterialIndex]);
}


bool Static_Mesh::Init(){
	return true;
}
void Static_Mesh::DeInit(){
	OUTPUT_DEBUG_MSG("Shutting down Static Mesh");
	Base_Mesh::DeInit();
	Bounding_Volume.CreateDefault();
	for(int i=0; i<MAX_VERTEXSTREAM; i++) VB[i].Destroy();
	IB.Destroy();
	for(size_t i =0; i< Batches.size(); i++) delete Batches[i];
	Batches.clear();
	CBuffer0.Destroy();
	CBuffer1.Destroy();
}
void Static_Mesh::Generate_BV(){
	if(Vertices.empty()) return Bounding_Volume.CreateDefault();// create a small bounding volume to test against
	size_t i(0);
	do{	Bounding_Volume.Add(Vertices[i]); } while(++i!=Vertices.size());
	Bounding_Volume.Init();
}
float Static_Mesh::RayIntersect(const vec3& rayorig, const vec3& raydir) const {
	mat4 inverseW(GetWorld());
	inverseW.inverse();
	vec3 org(rayorig*inverseW), di(raydir);// transform these to the mesh's space so the checks are in object space, not world space
	TransformNormal(di, inverseW);
	// do all checks in OBJECT SPACE!!!
	di*=20000.0f;
	if(Bounding_Volume.RayIntersect(org, di) == INFINITY) return INFINITY;// did not hit the aabb, therefore, we did not hit the object
	if(Index_Stride == 4){
		return RayTriangleIntersect(org, di, &Vertices[0], reinterpret_cast<const uint32_t*>(&Indices[0]),Indices.size()*2);
	} else {
		return RayTriangleIntersect(org, di, &Vertices[0],&Indices[0],Indices.size());
	}
}

bool Static_Mesh::Load(const std::string& file){
	OUTPUT_DEBUG_MSG("Static_Mesh::Load file: '" + file+"'");
	if(!FileExists(file)) {
		OUTPUT_DEBUG_MSG("File does not exist");
		return false;
	}
	std::string ext = GetFileExtention(file);
	DeInit();// make sure to free any resources
	bool ret = false;
	if(ext == ".dsm") ret = Load_MyFormat(file);//.dsm is my file extention for models
	else ret = Load_Assimp(file);
	//Configure and load the appropriate shaders

	CBuffer0.Create(1, sizeof(Object_Transform), CONSTANT_BUFFER);
	CBuffer1.Create(1, sizeof(Material), CONSTANT_BUFFER);
	if(ret) Generate_BV();
	return ret;
}
bool Static_Mesh::Load_Assimp(const std::string& file){
	const aiScene* load =aiImportFile(file.c_str(), aiProcessPreset_TargetRealtime_MaxQuality | aiProcess_ConvertToLeftHanded );
	if(load==NULL) {
		OUTPUT_DEBUG_MSG("Could not load the Model in Static_Mesh::Load_MyFormat file: '" + file+"'");
		return false;
	}
	size_t numverts(0), currentvertex(0), currentindex(0), numindices(0);
	// go through the mesh counting all the verts and indices that I will need
	for (unsigned int i = 0; i < load->mNumMeshes;++i) {
		numverts+=load->mMeshes[i]->mNumVertices;
		numindices+=load->mMeshes[i]->mNumFaces*3;
	}
	Index_Stride=2;
	if(numverts >= 65536) Index_Stride=4;

	std::vector<Vertex_Types::Tex_Norm> texnorm(numverts);
	std::vector<vec3> tang(numverts);
	Vertices.resize(numverts);
	Indices.resize((Index_Stride/2)*numindices);
	std::vector<Vertex_Types::Pos_Tex_Norm_Tang> tempverts(numverts);

	for (unsigned int i = 0; i < load->mNumMeshes;++i){
		Graphics::Texture diffuse, normal;
		const aiMesh* mesh = load->mMeshes[i];
		if(mesh->mPrimitiveTypes != aiPrimitiveType_TRIANGLE) {
			OUTPUT_DEBUG_MSG("There are errors with this submesh, named: "<<mesh->mName.data<<"   Please, fix it");
			if(mesh->mPrimitiveTypes == aiPrimitiveType_LINE){
				OUTPUT_DEBUG_MSG("Problem: The mesh containes lines when it should only contain triangles");
			}else {
				OUTPUT_DEBUG_MSG("Problem: The mesh containes points when it should only contain triangles");
			}
			continue;
		}

		if (!mesh->HasTextureCoords(0))	{
			OUTPUT_DEBUG_MSG("There are errors with this submesh, named: "<<mesh->mName.data<<"   Please, fix it");
			OUTPUT_DEBUG_MSG("Problem: The mesh containes no texcoords, which means there will just be color displayed. This engine does not support color mesh displays, only textured mesh!");
			continue;
		} 
		if(!mesh->HasTangentsAndBitangents()) {
			OUTPUT_DEBUG_MSG("There are errors with this submesh, named: "<<mesh->mName.data<<"   Please, fix it");
			OUTPUT_DEBUG_MSG("Problem: Tangents were not created. No known fix");
			continue;
		}
		// normals and tangents were created.. use those
		for (unsigned int x = 0; x < mesh->mNumVertices;++x){
			Vertices[x + currentvertex] = tempverts[x + currentvertex].Pos = *reinterpret_cast<vec3*>(&mesh->mVertices[x]);
			tempverts[x + currentvertex].Tex =  *reinterpret_cast<vec2*>(&mesh->mTextureCoords[0][x]);
			tempverts[x + currentvertex].Norm = *reinterpret_cast<vec3*>(&mesh->mNormals[x]);
			tempverts[x + currentvertex].Tang = *reinterpret_cast<vec3*>(&mesh->mTangents[x]);
		}
		// check whether we can use 16 bit indices for our format... the ASSIMPOBLARBLA uses 32 bit indices for all theirs..
		if (Index_Stride == 4){
			uint32_t* pbData = reinterpret_cast<uint32_t*>(&Indices[currentindex]);
			for (unsigned int x = 0; x < mesh->mNumFaces;++x){
				for (unsigned int a = 0; a < 3 ;++a) {
					*pbData++ = static_cast<uint32_t>(mesh->mFaces[x].mIndices[a]+ currentvertex);
				}
			}
		} else {
			uint16_t* pbData = reinterpret_cast<uint16_t*>(&Indices[currentindex]);
			for (unsigned int x = 0; x < mesh->mNumFaces;++x){
				for (unsigned int a = 0; a < 3 ;++a) {
					*pbData++ = static_cast<uint16_t>(mesh->mFaces[x].mIndices[a]+ currentvertex);
				}
			}
		}
		//load the textures
		std::string pathtomodel(GetPath(file));
		Batch *batch = new Batch();

		LoadMaterials(mesh, load->mMaterials, batch, pathtomodel);
		batch->NumIndices=mesh->mNumFaces*3;
		batch->StartIndex = static_cast<uint32_t>(currentindex);
		batch->NumVerts= mesh->mNumVertices;
		// make sure to increment the ref count for thesse so they are properly destroyed

		currentvertex+=mesh->mNumVertices;
		currentindex+=mesh->mNumFaces*3;

		//For now, there will be a new shader for each material. I will create a shader cache where the graphics lib will cache the shaders and issue out already created shaders like it does with textures.
		Graphics::Shader_Macro macro1[] = {
			{"NORMALMAP", "1" }, 
			{NULL, NULL}
		};
		Graphics::Shader_Macro macro0[] = {
			{"NORMALMAP", "0" }, 
			{NULL, NULL}
		};

		Graphics::Shader_Macro* ptr = nullptr;
		if(batch->Has_NormalMap()) ptr = macro1;
		else ptr = macro0;

		batch->GetVS()->CompileShaderFromFile("Static_Mesh.fx", "VS", "vs_4_0", ptr);

		FormatDesc lay[] = {
			FormatDesc(),
			FormatDesc(TYPE_TEXCOORD, FORMAT_FLOAT, 2),
			FormatDesc(TYPE_NORMAL, FORMAT_FLOAT, 3),
			FormatDesc(TYPE_TANGENT, FORMAT_FLOAT, 3)
		};
		batch->GetVS()->CreateInputLayout(lay, 4);
		batch->GetPS()->CompileShaderFromFile("Static_Mesh.fx", "PS", "ps_4_0", ptr);

		Batches.push_back(batch);
	}
	aiReleaseImport(load);// free the resrouces
	if(currentvertex==0) {// this could happen, if so GET OUTOF HERE
		OUTPUT_DEBUG_MSG("Problem loading the mesh, there were no vertices loaded. Failed to load the mesh");
		return false;
	}
	VB[0].Create(currentvertex, sizeof(Vertex_Types::Pos_Tex_Norm_Tang), VERTEX_BUFFER, IMMUTABLE, CPU_NONE, &tempverts[0] );
	IB.Create(currentindex, Index_Stride, INDEX_BUFFER, IMMUTABLE, CPU_NONE, &Indices[0]); // create index buffer!
	OUTPUT_DEBUG_MSG("Finished Loading the Mesh");
	FileName =file;
	return true;
}
bool Static_Mesh::Load_MyFormat(const std::string& file){
	std::ifstream f(file.c_str(), std::ios::binary);
	if(!f.is_open()) {
		OUTPUT_DEBUG_MSG("Could not open the file in Static_Mesh::Load_MyFormat: '"+ file + "'");
		return false;
	}
	Base_Mesh::Load(f);
	IB.Load(f);
	unsigned int temp=0;
	f.read((char*)&temp, sizeof(unsigned int));
	Vertices.resize(temp);
	f.read((char*)&temp, sizeof(unsigned int));// number of vertex buffers
	for(unsigned int i(0); i< temp; i++){// write out each vertex buffer
		VB[i].Load(f);
	}
	temp = (unsigned int)Batches.size();// number of batches
	f.read((char*)&temp, sizeof(unsigned int));// number of batches
	Batches.resize(temp);
	for(size_t i=0; i< temp; i++){
		Batch *b = new Batch();
		b->Load(f);
		Batches[i] = b;
	}
	FileName = file;
	return true;
}

bool Static_Mesh::Save(const std::string& file){
	std::ofstream f(file.c_str(), std::ios::trunc| std::ios::binary);
	if(!f.is_open()) {
		OUTPUT_DEBUG_MSG("Could not open the file in Static_Mesh::Save: '"+ file + "'");
		return false;
	}
	Base_Mesh::Save(f);
	IB.Save(f);
	unsigned int temp=(unsigned int)Vertices.size();
	f.write((char*)&temp, sizeof(unsigned int));
	temp=0;
	for(size_t i(0); i< MAX_VERTEXSTREAM; i++){// write out each vertex buffer
		if(VB[i].Empty()) continue;
		temp+=1;
	}
	f.write((char*)&temp, sizeof(unsigned int));// number of vertex buffers
	for(size_t i(0); i< MAX_VERTEXSTREAM; i++){// write out each vertex buffer
		if(VB[i].Empty()) continue;
		VB[i].Save(f);
	}
	temp = (unsigned int)Batches.size();// number of batches
	f.write((char*)&temp, sizeof(unsigned int));// number of batches
	for(size_t i=0; i< Batches.size(); i++){
		Batches[i]->Save(f);
	}
	FileName = file;
	return true;
}
void Static_Mesh::Draw(const mat4& view, const mat4& proj){
	Graphics::SetTopology(PRIM_TRIANGLELIST);

	Graphics::DepthStates::DepthTest.Bind();
	Graphics::RasterizerStates::CullBack.Bind();
	Graphics::BlendStates::No_Blend.Bind();
	Object_Transform cb;
	cb.Inv_Trans_World = cb.World = GetWorld();
	cb.View_Proj= view*proj;
	cb.View_Proj.Transpose();
	cb.World.Transpose();
	cb.Inv_Trans_World.inverse();
	cb.Inv_Trans_World.Transpose();
	CBuffer0.Update(&cb);

	VB[0].BindAsVertexBuffer();
	IB.BindAsIndexBuffer();


	for(auto beg = Batches.begin(); beg != Batches.end(); beg++){
		Batch& batch = **beg;

		batch.GetVS()->Bind();
		batch.GetVS()->SetConstantBuffer(CBuffer0, 0);

		batch.GetPS()->Bind();
		batch.GetPS()->SetSampler(Graphics::Samplers::Linear);
		CBuffer1.Update(batch.GetMaterial());
		batch.GetPS()->SetConstantBuffer(CBuffer1, 0);

		Graphics::Texture *t = batch.GetFirstMap();
		for(unsigned int slot=0; slot<batch.GetNum_ofMaps(); slot++) batch.GetPS()->SetResource(t + slot, slot);

		Graphics::DrawIndexed((*beg)->StartIndex, (*beg)->NumIndices);
	}
}