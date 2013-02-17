#ifndef ASSIMP_HELPERS_H
#define ASSIMP_HELPERS_H

#include "../Assimp/include/assimp/Scene.h"
#include "../Assimp/include/assimp/PostProcess.h"
#include "../Assimp/include/assimp/cimport.h"
#include "Batch.h"

inline void ExtractMaterials(Batch& batch, const aiMaterial* pcMat){
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


inline void LoadMaterials(const aiMesh* mesh, aiMaterial** mMaterials, Batch *batch, std::string& path, const std::string& asset_dir){
	aiString szPath;
	std::string basefolder = asset_dir + "Mesh\\";
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

#endif