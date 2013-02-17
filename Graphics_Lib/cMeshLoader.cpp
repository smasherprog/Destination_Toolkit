#include "stdafx.h"
#include "Assimp_Helper.h"
#include <iostream>

/*
bool Load(const std::string& xfile, cMesh& basemesh){// add an lod check for a mesh with a poly count less than 400-500. there isnt a point in reducing it
	//OUTPUT_DEBUG_MSG("Attempting to Load Static mesh");

	std::string ext = GetFileExtention(xfile);
	
	const aiScene* load =aiImportFile(xfile.c_str(), aiProcessPreset_TargetRealtime_MaxQuality | aiProcess_ConvertToLeftHanded );

	bool hasbones =false;
	size_t numverts(0), currentvertex(0), currentindex(0), numindices(0);
	for (unsigned int i = 0; i < load->mNumMeshes;++i) {
		numverts+=load->mMeshes[i]->mNumVertices;
		numindices+=load->mMeshes[i]->mNumFaces*3;
		if(load->mMeshes[i]->HasBones()) hasbones=true;
		if(load->mMeshes[i]->HasTextureCoords(0)) OUTPUT_DEBUG_MSG(" Founds tex coords");
	}

	uint16_t indexstride=2;
	if(numverts >= 65536) indexstride=4;// the indecies index vertices, so if there are more verts than a 16 bit numbder can hold goto 32 bit indices
	
	
	basemesh.Vertices.resize(numverts);
	std::vector<uint16_t>& indices(basemesh.Lod1.Indices);

	struct wb{  vec4 bones; vec4 weights; };// just here for convenience!
	std::vector<wb> weightsnbones;
	if(hasbones){
		weightsnbones.resize(numverts);
		memset(&weightsnbones[0], 0, sizeof(wb)*numverts);
	}
	std::vector<std::string> bonenames;
	indices.resize((indexstride/2)*numindices);

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
		//OUTPUT_DEBUG_MSG("Loading SubMesh, Named: "<<mesh->mName.data<<" Containing "<<mesh->mNumVertices<<" Number of Verts");
		for( unsigned int a = 0; a < mesh->mNumBones; a++)	{
			const aiBone* bone = mesh->mBones[a];
			size_t bonein(-1);
			for(size_t ib(0); ib< bonenames.size(); ib++){
				string tname = bone->mName.data;
				if(tname == bonenames[ib]){// found the bone.. break
					bonein=ib;
					break;
				}
			}
			if(bonein ==-1){//  did not find the bone, this is a new one push back
				bonein = bonenames.size();// get the index before insertion
				bonenames.push_back(bone->mName.data);
			}
			// there should only be 4 per vertex here because assimp guaranteees it, but if there are more, we are ok
			for( unsigned int b = 0; b < bone->mNumWeights; b++){
				if(			weightsnbones[bone->mWeights[b].mVertexId+ currentvertex].weights.x <= 0.f) {
							weightsnbones[bone->mWeights[b].mVertexId+ currentvertex].bones.x = static_cast<float>(bonein);
							weightsnbones[bone->mWeights[b].mVertexId+ currentvertex].weights.x = bone->mWeights[b].mWeight;
				} else if(	weightsnbones[bone->mWeights[b].mVertexId+ currentvertex].weights.y <= 0.f){
							weightsnbones[bone->mWeights[b].mVertexId+ currentvertex].bones.y = static_cast<float>(bonein);
							weightsnbones[bone->mWeights[b].mVertexId+ currentvertex].weights.y = bone->mWeights[b].mWeight;
				} else if(	weightsnbones[bone->mWeights[b].mVertexId+ currentvertex].weights.z <= 0.f){
							weightsnbones[bone->mWeights[b].mVertexId+ currentvertex].bones.z = static_cast<float>(bonein);
							weightsnbones[bone->mWeights[b].mVertexId+ currentvertex].weights.z = bone->mWeights[b].mWeight;
				} else if(	weightsnbones[bone->mWeights[b].mVertexId+ currentvertex].weights.w <= 0.f){
							weightsnbones[bone->mWeights[b].mVertexId+ currentvertex].bones.w = static_cast<float>(bonein);
							weightsnbones[bone->mWeights[b].mVertexId+ currentvertex].weights.w = bone->mWeights[b].mWeight;
				}
				
			}
	
		}
		// normals and tangents were created.. use those
		for (unsigned int x = 0; x < mesh->mNumVertices;++x){
			basemesh.Vertices[x + currentvertex] = *reinterpret_cast<vec3*>(&mesh->mVertices[x]);
			texnorm[x + currentvertex].norm =  *reinterpret_cast<vec3*>(&mesh->mNormals[x]);
			texnorm[x + currentvertex].tex =  *reinterpret_cast<vec2*>(&mesh->mTextureCoords[0][x]);			
			tang[x + currentvertex] = *reinterpret_cast<vec3*>(&mesh->mTangents[x]);

		}
		// check whether we can use 16 bit indices for our format... the ASSIMPOBLARBLA uses 32 bit indices for all theirs..
		if (indexstride == 4){
			uint32_t* pbData = reinterpret_cast<uint32_t*>(&indices[currentindex]);
			for (unsigned int x = 0; x < mesh->mNumFaces;++x){
				for (unsigned int a = 0; a < 3 ;++a) {
					*pbData++ = static_cast<uint32_t>(mesh->mFaces[x].mIndices[a]+ currentvertex);
				}
			}
		} else {
			uint16_t* pbData = reinterpret_cast<uint16_t*>(&indices[currentindex]);
			for (unsigned int x = 0; x < mesh->mNumFaces;++x){
				for (unsigned int a = 0; a < 3 ;++a) {
					*pbData++ = static_cast<uint16_t>(mesh->mFaces[x].mIndices[a]+ currentvertex);
				}
			}
		}
		//load the textures
		std::string pathtomodel(GetPath(xfile));
		Batch *batch(basemesh.Lod1.CreateBatch());

		LoadMaterials(mesh, load->mMaterials, batch->texture, batch->normal, pathtomodel);

		const aiMaterial* pcMat = load->mMaterials[mesh->mMaterialIndex];
		
		ExtractMaterials(*batch, pcMat);
		batch->numIndices=mesh->mNumFaces*3;
		batch->StartIndex = static_cast<uint32_t>(currentindex);
		batch->numVertices= mesh->mNumVertices;
		batch->texture = diffuse;
		batch->normal = normal;
		
		batch=basemesh.Lod2.CreateBatch();
		ExtractMaterials(*batch, pcMat);

		batch->texture = diffuse;
		
		batch->normal = normal;

		batch=basemesh.Lod3.CreateBatch();
		batch->texture = diffuse;
		batch->normal = normal;
			
		ExtractMaterials(*batch, pcMat);
		// make sure to increment the ref count for thesse so they are properly destroyed

		currentvertex+=mesh->mNumVertices;
		currentindex+=mesh->mNumFaces*3;
	}

	basemesh.mAnimator.Init(load);
	aiReleaseImport(load);// free the resrouces
	if(currentvertex==0) {// this could happen, if so GET OUTOF HERE
		OUTPUT_DEBUG_MSG("Problem loading the mesh, there were no vertices loaded. Failed to load the mesh");
		return false;
	}
	basemesh.Vertices.resize(currentvertex);
	indices.resize(currentindex*indexstride/2);
	basemesh.Lod1.VB[POS_INDEX].Create(currentvertex, sizeof(vec3), VERTEX_BUFFER, IMMUTABLE, CPU_NONE, &basemesh.Vertices[0] );
	basemesh.Lod1.VB[TEX_NORM_INDEX].Create(currentvertex, sizeof(TexNorm), VERTEX_BUFFER, IMMUTABLE, CPU_NONE, &texnorm[0]);
	basemesh.Lod1.VB[TANG_INDEX].Create(currentvertex, sizeof(vec3), VERTEX_BUFFER, IMMUTABLE, CPU_NONE, &tang[0]);
	if(hasbones)
		basemesh.Lod1.VB[WEIGHT_BONE_INDEX].Create(currentvertex, sizeof(wb), VERTEX_BUFFER, IMMUTABLE, CPU_NONE, &weightsnbones[0]);
	basemesh.Lod1.NumVBS = 4;
	basemesh.Lod1.IB.Create(currentindex, indexstride, INDEX_BUFFER, IMMUTABLE, CPU_NONE, &indices[0]); // create index buffer!
	//lod2 and 3
	basemesh.Lod3.VB[POS_INDEX]=basemesh.Lod2.VB[POS_INDEX] = basemesh.Lod1.VB[POS_INDEX];
	basemesh.Lod3.VB[TEX_NORM_INDEX]=basemesh.Lod2.VB[TEX_NORM_INDEX] = basemesh.Lod1.VB[TEX_NORM_INDEX];
	basemesh.Lod2.VB[TANG_INDEX] = basemesh.Lod1.VB[TANG_INDEX]; // no tangents on lod3 needed
	basemesh.Lod3.VB[WEIGHT_BONE_INDEX]=basemesh.Lod2.VB[WEIGHT_BONE_INDEX] = basemesh.Lod1.VB[3];
	basemesh.Lod2.NumVBS = 4;
	basemesh.Lod3.NumVBS = 3;
	OUTPUT_DEBUG_MSG("Finished Loading the Mesh");
	basemesh.FileName =xfile;
	return true;
}

void cDSMLoader::Save(const std::string& filename, cMesh& mesh){
	std::string newfile =Asset_Dir+ "Mesh\\" + StripFileExtention(StripPath(filename)) + ".dsm";

	OUTPUT_DEBUG_MSG("Saving the filename "<<newfile);
	ofstream file(newfile.c_str(), ios::binary);

	uint32_t dwNumVerts(static_cast<uint32_t>(mesh.Vertices.size())), vertexbuffers(static_cast<uint32_t>(mesh.Lod1.NumVBS));
	file.write(reinterpret_cast<char*>(&dwNumVerts), sizeof(uint32_t));// write the number of vertices
	file.write(reinterpret_cast<char*>(&vertexbuffers), sizeof(uint32_t));// write the number of vertex buffers

	for(size_t i(0); i< vertexbuffers; i++){// write out each vertex buffer
		void* ptr;
		uint32_t vertexstride=static_cast<uint32_t>(mesh.Lod1.VB[i].Stride);
		file.write(reinterpret_cast<char*>(&vertexstride), sizeof(uint32_t));// write the stride of the vertex buffer
		Graphics::Buffer v;
		v.Create(dwNumVerts, vertexstride, VERTEX_BUFFER, STAGING, CPU_READ, 0);
		Graphics::Buffer::Copy(v, mesh.Lod1.VB[i]);
		ptr = v.Map(MAP_READ);	
		file.write(reinterpret_cast<char*>(ptr), dwNumVerts*vertexstride);
		v.UnMap();
		v.Destroy();
	}

	SaveBatch(file, mesh.Lod1);
	SaveBatch(file, mesh.Lod2);
	SaveBatch(file, mesh.Lod3);

	mesh.mAnimator.Save(file);// save animations skeleton the usual stuff!

	file.close();
	mesh.FileName = StripPath(newfile);
}
bool cDSMLoader::Load(const std::string& filename, cMesh& mesh){
	//OUTPUT_DEBUG_MSG("Loading the filename "<<filename);
	ifstream file(filename.c_str(), ios::binary);
	if(!file) {
		OUTPUT_DEBUG_MSG("Could not locate file");
		return false;// no file
	}
	
	uint32_t dwNumVerts(0), vertexbuffers(0);
	file.read(reinterpret_cast<char*>(&dwNumVerts), sizeof(uint32_t));// the number of vertices
	file.read(reinterpret_cast<char*>(&vertexbuffers), sizeof(uint32_t));// the number of vertex buffers
	mesh.Vertices.resize(dwNumVerts);
	// the first vertex buffer is always the position
	uint32_t vertexstride(0);
	file.read(reinterpret_cast<char*>(&vertexstride), sizeof(uint32_t));// write the stride of the vertex buffer
	file.read(reinterpret_cast<char*>(&mesh.Vertices[0]), dwNumVerts*vertexstride);
	mesh.Lod1.VB[0].Create(dwNumVerts, vertexstride, VERTEX_BUFFER, IMMUTABLE, CPU_NONE, &mesh.Vertices[0]);

	std::vector<char> buffer;
	for(size_t i(1); i< vertexbuffers; i++){// start on 1 since we loaded the position already		
		file.read(reinterpret_cast<char*>(&vertexstride), sizeof(uint32_t));// write the stride of the vertex buffer
		buffer.resize(vertexstride* dwNumVerts);
		file.read(reinterpret_cast<char*>(&buffer[0]), dwNumVerts*vertexstride);// read the data
		mesh.Lod1.VB[i].Create(dwNumVerts, vertexstride, VERTEX_BUFFER, IMMUTABLE, CPU_NONE, &buffer[0]);
	}

	LoadBatch(file, mesh.Lod1);
	LoadBatch(file, mesh.Lod2);
	LoadBatch(file, mesh.Lod3);
	mesh.mAnimator.Load(file);


	mesh.Lod1.NumVBS = vertexbuffers;
	//lod2 and 3
	mesh.Lod3.VB[POS_INDEX]=mesh.Lod2.VB[POS_INDEX] = mesh.Lod1.VB[POS_INDEX];
	mesh.Lod3.VB[TEX_NORM_INDEX]=mesh.Lod2.VB[TEX_NORM_INDEX] = mesh.Lod1.VB[TEX_NORM_INDEX];
	mesh.Lod2.VB[TANG_INDEX]=mesh.Lod1.VB[TANG_INDEX]; // no tangents on lod3 needed
	mesh.Lod3.VB[WEIGHT_BONE_INDEX]=mesh.Lod2.VB[WEIGHT_BONE_INDEX] = mesh.Lod1.VB[WEIGHT_BONE_INDEX];
	mesh.Lod2.NumVBS = vertexbuffers;
	mesh.Lod3.NumVBS = 3;

	file.close();
	mesh.FileName = StripPath(filename);
	return true;
}
*/