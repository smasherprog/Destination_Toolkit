#ifndef CSTATICLOADER_H
#define CSTATICLOADER_H
#include "MeshTypes.h"
#include <string>
#include "../Assimp/include/assimp/Scene.h"
#include "../Assimp/include/assimp/PostProcess.h"
#include "../Assimp/include/assimp/cimport.h"
class cMesh;
struct aiMaterial;
/*
namespace cBaseMeshLoader {
	
	bool Load(const std::string& xfile, cMesh& mesh);
	void ExtractMaterials(Batch& batch, const aiMaterial* pcMat);
	void LoadMaterials(const aiMesh* mesh, aiMaterial** mMaterials, Graphics::Texture& diffuse, Graphics::Texture& normal, std::string& path);
};


namespace cDSMLoader{
	bool Load(const std::string& filename, cMesh& mesh);
	void Save(const std::string& filename, cMesh& mesh);
	void SaveBatch(ofstream& file, Mesh_Lod& lod);
	void LoadBatch(ifstream& file, Mesh_Lod& lod);
};
*/
#endif