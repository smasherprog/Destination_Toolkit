#include "stdafx.h"
#include "cMeshLoader.h"
#include "cMesh.h"
#include <iostream>

/*
bool cBaseMeshLoader::Load(const std::string& xfile, cMesh& basemesh){// add an lod check for a mesh with a poly count less than 400-500. there isnt a point in reducing it
	//OUTPUT_DEBUG_MSG("Attempting to Load Static mesh");

	std::string ext = GetFileExtention(xfile);
	if(ext == ".dsm") return cDSMLoader::Load(xfile, basemesh);
	
	const aiScene* load =aiImportFile(xfile.c_str(), aiProcessPreset_TargetRealtime_MaxQuality | aiProcess_ConvertToLeftHanded );
	if(load==NULL) {
		cout<<"Could not load file using aiImportFile"<<endl;
		return false;
	}
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
	
	std::vector<TexNorm> texnorm(numverts);
	std::vector<vec3> tang(numverts);
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

void cBaseMeshLoader::LoadMaterials(const aiMesh* mesh, aiMaterial** mMaterials, Graphics::Texture& diffuse, Graphics::Texture& normal, std::string& path){
	aiString szPath;
///////////////////DIFFUSE TEXTURE

	if( AI_SUCCESS == aiGetMaterialString(mMaterials[mesh->mMaterialIndex], AI_MATKEY_TEXTURE_DIFFUSE(0), &szPath) ){
		std::string fe(StripPath(szPath.data));// sometimes the textures are stored with their full path, which is not correct, strip it
		std::string tes = path + szPath.data;
		ifstream fchecker(tes.c_str());
		if(!fchecker){// if the image cannot be found, prompt the user to find it, or a replacement
			OUTPUT_DEBUG_MSG("Unable to find the diffuse texture "<<tes<<"   Please, Provide a valid texture HOOK GUI CODE HERE!!");
			//Renderer.FindImage(tes);
			fe = StripPath(tes);// in case the name changed,
		} else fchecker.close();
		
		OUTPUT_DEBUG_MSG("diffuse texture ="<<tes);
		if(GetFileExtention(fe) == ".dds"){// file extention correct, copy it to the mesh directory
			
			OUTPUT_DEBUG_MSG("Copying "<<fe<<" To "<<Asset_Dir);
			CopyFileA(tes.c_str(), (Asset_Dir + "Mesh\\" + fe).c_str(), true);
			diffuse.Create(Asset_Dir + "Mesh\\" + fe);// now load the texture
		} else {// convert it to a dds file
			OUTPUT_DEBUG_MSG("Converting to  "+ Asset_Dir + "Mesh\\" + StripFileExtention(fe) + ".dds");
			Graphics::Texture::ConvertToDDS(tes, Asset_Dir + "Mesh\\" + StripFileExtention(fe) + ".dds");
			diffuse.Create(Asset_Dir + "Mesh\\" + StripFileExtention(fe) + ".dds");
		}
		if(diffuse.Empty()){
			OUTPUT_DEBUG_MSG("failure loading the texture 1"<<szPath.length);
			return;
		}
	}  else {
		OUTPUT_DEBUG_MSG("failure loading the texture 2"<<szPath.length);
		return;
	}
/////////////////////// BUMP MAP
	if( AI_SUCCESS == aiGetMaterialString(mMaterials[mesh->mMaterialIndex], AI_MATKEY_TEXTURE_HEIGHT(0),&szPath) ){
		std::string fe(StripPath(szPath.data));// sometimes the textures are stored with their full path, which is not correct, strip it
		std::string tes = path + szPath.data;
		ifstream fchecker(tes.c_str());
		if(!fchecker){// if the image cannot be found, prompt the user to find it, or a replacement
			OUTPUT_DEBUG_MSG("Unable to find the bump texture "<<tes<<"   Please, Provide a valid texture HOOK GUI CODE HERE!!");
			//Renderer.FindImage(tes);
			fe = StripPath(tes);// in case the name changed,
		} else fchecker.close();
		OUTPUT_DEBUG_MSG("bump texture ="<<tes);
		if(GetFileExtention(fe) == ".dds"){// file extention correct, copy it to the mesh directory
			CopyFileA(tes.c_str(), (Asset_Dir + "Mesh\\" + fe).c_str(), true);
			normal.Create(Asset_Dir + "Mesh\\" + fe);// now load the texture
		} else {// convert it to a dds file
			Graphics::Texture::GenerateNormalBumpFromBump(tes, Asset_Dir + "Mesh\\" + StripFileExtention(fe) + ".dds");
			normal.Create(Asset_Dir + "Mesh\\" + StripFileExtention(fe) + ".dds");
		}
//////////////////NORMAL MAP
	} else if( AI_SUCCESS == aiGetMaterialString(mMaterials[mesh->mMaterialIndex], AI_MATKEY_TEXTURE_NORMALS(0),&szPath) ){// if there is a bump, I generate a normal map from it, so I dont bother with this
		std::string fe(StripPath(szPath.data));// sometimes the textures are stored with their full path, which is not correct, strip it
		std::string tes = path + szPath.data;
		ifstream fchecker(tes.c_str());
		if(!fchecker){// if the image cannot be found, prompt the user to find it, or a replacement
			OUTPUT_DEBUG_MSG("Unable to find the normal texture "<<tes<<"   Please, Provide a valid texture HOOK GUI CODE HERE!!");
			//Renderer.FindImage(tes);
			fe = StripPath(tes);// in case the name changed,
		} else fchecker.close();
		OUTPUT_DEBUG_MSG("normal texture ="<<tes);
		if(GetFileExtention(fe) == ".dds"){// file extention correct, copy it to the mesh directory
			CopyFileA(tes.c_str(), (Asset_Dir + "Mesh\\" + fe).c_str(), true);
			normal.Create(Asset_Dir + "Mesh\\" + fe);// now load the texture
		} else {// convert it to a dds file
			Graphics::Texture::ConvertToDDS(tes, Asset_Dir + "Mesh\\" + StripFileExtention(fe) + ".dds");
			normal.Create(Asset_Dir + "Mesh\\" + StripFileExtention(fe) + ".dds");
		}
	} 

}
void cBaseMeshLoader::ExtractMaterials(Batch& batch, const aiMaterial* pcMat){
	batch.Diffuse = vec4(1.0f, 1.0f, 1.0f, 1.0f);
	aiGetMaterialColor(pcMat,AI_MATKEY_COLOR_DIFFUSE, (aiColor4D*)&batch.Diffuse);
	batch.Specular = vec4(1.0f, 1.0f, 1.0f, 1.0f);
	aiGetMaterialColor(pcMat,AI_MATKEY_COLOR_SPECULAR, (aiColor4D*)&batch.Specular);
	batch.Ambient = vec4(1.0f, 1.0f, 1.0f, 1.0f);
	aiGetMaterialColor(pcMat,AI_MATKEY_COLOR_AMBIENT,(aiColor4D*)&batch.Ambient);
	batch.Emissive = vec4(1.0f, 1.0f, 1.0f, 1.0f);
	aiGetMaterialColor(pcMat,AI_MATKEY_COLOR_EMISSIVE, (aiColor4D*)&batch.Emissive);
	batch.Power =1.0f;
	aiGetMaterialFloat(pcMat,AI_MATKEY_SHININESS_STRENGTH,&batch.Power);
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
void cDSMLoader::SaveBatch(ofstream& file, Mesh_Lod& lod){
	
	uint32_t indexstride= static_cast<uint32_t>(lod.IB.Stride);
	uint32_t numindex = static_cast<uint32_t>(lod.IB.Size);

	file.write(reinterpret_cast<char*>(&numindex), sizeof(uint32_t));// write the number of indices
	file.write(reinterpret_cast<char*>(&indexstride), sizeof(uint32_t));// write the index stride
	if(indexstride != 0 && numindex !=0){// only write if there is something to write
		file.write(reinterpret_cast<char*>(&lod.Indices[0]), indexstride *numindex);// write out the indices
	}
	uint32_t numbatches =static_cast<uint32_t>(lod.Batches.size());
	file.write(reinterpret_cast<char*>(&numbatches), sizeof(uint32_t));// write the  number of batches

	for(uint32_t i(0); i< numbatches; i++){
		Batch *bt = lod.Batches[i];
				
		file.write(reinterpret_cast<char*>(&bt->numVertices), sizeof(bt->numVertices));// num of indices for this particular batch
		file.write(reinterpret_cast<char*>(&bt->numIndices), sizeof(bt->numIndices));// num of indices for this particular batch

		file.write(reinterpret_cast<char*>(&bt->StartIndex), sizeof(bt->StartIndex));// num of indices for this particular batch
		file.write(reinterpret_cast<char*>(&bt->StartVertex), sizeof(bt->StartVertex));// num of indices for this particular batch

		file.write(reinterpret_cast<char*>(&bt->Specular), sizeof(bt->Specular));// num of indices for this particular batch
		file.write(reinterpret_cast<char*>(&bt->Diffuse), sizeof(bt->Diffuse));// num of indices for this particular batch	

		file.write(reinterpret_cast<char*>(&bt->Ambient), sizeof(bt->Ambient));// num of indices for this particular batch
		file.write(reinterpret_cast<char*>(&bt->Emissive), sizeof(bt->Emissive));// num of indices for this particular batch
		file.write(reinterpret_cast<char*>(&bt->Power), sizeof(bt->Power));// num of indices for this particular batch

		string temp = StripPath(bt->texture.FileName);
		uint32_t temp1 = static_cast<uint32_t>(temp.size());
		file.write(reinterpret_cast<char*>(&temp1), sizeof(uint32_t));// size in bytes of the filename
		file.write(temp.c_str(), temp1);// write the filename

		temp = StripPath(bt->normal.FileName);
		temp1 = static_cast<uint32_t>(temp.size());
		file.write(reinterpret_cast<char*>(&temp1), sizeof(uint32_t));// size in bytes of the filename
		file.write(temp.c_str(), temp1);// write the filename
	}

}

void cDSMLoader::LoadBatch(ifstream& file, Mesh_Lod& lod){
	
	uint32_t indexstride= 0;
	uint32_t numindex = 0;

	file.read(reinterpret_cast<char*>(&numindex), sizeof(uint32_t));//  the number of indices
	file.read(reinterpret_cast<char*>(&indexstride), sizeof(uint32_t));//  the index stride
	lod.Indices.resize(numindex *indexstride/2);
	if(indexstride != 0 && numindex !=0){// only read if there is something there
		file.read(reinterpret_cast<char*>(&lod.Indices[0]), indexstride *numindex);// out the indices
		lod.IB.Create(numindex, indexstride, INDEX_BUFFER, IMMUTABLE, CPU_NONE, &lod.Indices[0]);
	}
	uint32_t numbatches = static_cast<uint32_t>(lod.Batches.size());
	file.read(reinterpret_cast<char*>(&numbatches), sizeof(uint32_t));//the  number of batches

	for(uint32_t i(0); i< numbatches; i++){
		Batch *bt = lod.CreateBatch();
				
		file.read(reinterpret_cast<char*>(&bt->numVertices), sizeof(bt->numVertices));// num of indices for this particular batch
		file.read(reinterpret_cast<char*>(&bt->numIndices), sizeof(bt->numIndices));// num of indices for this particular batch

		file.read(reinterpret_cast<char*>(&bt->StartIndex), sizeof(bt->StartIndex));// num of indices for this particular batch
		file.read(reinterpret_cast<char*>(&bt->StartVertex), sizeof(bt->StartVertex));// num of indices for this particular batch

		file.read(reinterpret_cast<char*>(&bt->Specular), sizeof(bt->Specular));// num of indices for this particular batch
		file.read(reinterpret_cast<char*>(&bt->Diffuse), sizeof(bt->Diffuse));// num of indices for this particular batch	

		file.read(reinterpret_cast<char*>(&bt->Ambient), sizeof(bt->Ambient));// num of indices for this particular batch
		file.read(reinterpret_cast<char*>(&bt->Emissive), sizeof(bt->Emissive));// num of indices for this particular batch
		file.read(reinterpret_cast<char*>(&bt->Power), sizeof(bt->Power));// num of indices for this particular batch
		char temp[MAX_PATH];
		uint32_t temp1 =0;

		file.read(reinterpret_cast<char*>(&temp1), sizeof(uint32_t));// size in bytes of the filename
		file.read(temp, temp1);// the filename
		temp[temp1]=0;
		std::string stemp = Asset_Dir +"Mesh\\";
		stemp+= temp;
		if(temp1!=0)
			bt->texture.Create(stemp);

		file.read(reinterpret_cast<char*>(&temp1), sizeof(uint32_t));// size in bytes of the filename
		file.read(temp, temp1);// the filename
		temp[temp1]=0;
		stemp = Asset_Dir + "Mesh\\";
		stemp+= temp;
		if(temp1!=0)
			bt->normal.Create(stemp);
	}

}

*/