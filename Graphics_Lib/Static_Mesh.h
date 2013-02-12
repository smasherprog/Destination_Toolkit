#ifndef STATIC_MESH_H
#define STATIC_MESH_H

#include "Base_Mesh.h"

struct aiMesh;
struct aiMaterial;

class Static_Mesh : public Base_Mesh{
public:

	Static_Mesh(){ DeInit();}
	virtual ~Static_Mesh() {DeInit();}

	virtual bool Init();
	virtual void DeInit();
	virtual float Ray_Tri_Intersect(const vec3& rayorig, const vec3& raydir) override;
	virtual float Ray_BV_Intersect(const vec3& rayorig, const vec3& raydir) override;

	virtual bool Save(const std::string& file);
	virtual bool Load(const std::string& file);
	
	virtual void Draw(const Base_Camera* camera) override;

	virtual float Get_Max_x_Size() const override{ return Bounding_Volume.XSize(); }
	virtual float Get_Max_y_Size()const override{ return Bounding_Volume.YSize(); }
	virtual float Get_Max_z_Size()const override{ return Bounding_Volume.ZSize(); }
	virtual vec3 Get_Center() const override{ return GetPosition() + Bounding_Volume.GetCenter(); }

	std::vector<Batch*> Batches;
protected:
	
	bool Load_Assimp(const std::string& file);
	bool Load_MyFormat(const std::string& file);
	void Generate_BV();
	static void LoadMaterials(const aiMesh* mesh, aiMaterial** mMaterials, Batch *batch, std::string& path);
	static void ExtractMaterials(Batch& batch, const aiMaterial* pcMat);

	Graphics::Buffer CBuffer0, CBuffer1;
	Graphics::Buffer VB[MAX_VERTEXSTREAM];
	Graphics::Buffer IB;// index buffer

	std::vector<vec3> Vertices;// this is used to calculate intersections
	std::vector<uint16_t> Indices;
	AABB Bounding_Volume;

};

#endif
