#ifndef MESHTYPES_H
#define MESHTYPES_H
#include <vector>
#include <string>
#include "../Graphics_Lib/Globals_GraphicsCore.h"
#include "../BV_Lib/cAABB.h"
#include <map>
#include "../Utilities/MyMath.h"
#include "../Graphics_Lib/GraphicsCore.h"

void GenGrid(std::vector<uint16_t>& indices, std::vector<vec3>& vertices, uint32_t size);
void GenSphere(float mRadius, uint32_t mNumSlices, uint32_t mNumStacks, std::vector<vec3>& vertices, std::vector<Vertex_Types::Pos_Tex>& texnorm, std::vector<uint16_t>& indices);
void GenCube(std::vector<vec3>& vertices, std::vector<uint16_t>& indices);
void GenCylinder( float topRadius, float bottomRadius, float height, uint32_t numSlices, uint32_t numStacks, std::vector<vec3>& vertices, std::vector<Vertex_Types::Tex_Norm>& texnorm, std::vector<uint16_t>& indices);
template<class T>void Optimize(std::vector<vec3>& inverts, std::vector<T>& inoutindices, std::vector<vec3>& outverts){
	outverts.reserve(inverts.size());// allocate enough room so there are less allocations and copying going on 
	cSetArray<float, T> remap(3, inverts.size());
	std::vector<T> tempin(inverts.size());
	float* v = reinterpret_cast<float*>(&inverts[0]);
	size_t i(0);
	do{
		tempin[i] = remap.insert(v + (3 *i) ); 
		outverts[tempin[i]] = inverts[i];
	}while(++i!=inverts.size());
	outverts.resize(remap.size());// adjust the correct size
	i=0;
	do{
		inoutindices[i] = tempin[inoutindices[i]];
	}while(++i!=inoutindices.size());

}

template<class T>void ComputeNormals(vec3* vertices, size_t numverts, T* indices, size_t numindices, vec3* normals);
template<class T>void ComputeTangetBinormalNormal(vec3* vertices, size_t& numverts, T* indices, size_t numindices, vec3* normals, vec2* tex, vec3* tangents, vec3* binormals);
template<class T>void ComputeNormalTangent(const vec3* vertices, size_t numverts, T* indices, size_t numindices, vec3* normals, vec2* tex, vec3* tangents);
template<class T>void ComputeTangents(vec3* vertices, size_t numverts, T* indices, size_t numindices, vec3* normals, vec2* tex, vec3* tangents);

template<class T>void ComputeNormalTangent(vec2 const * const vertices, const size_t numverts, float const * const heights, T const * const indices, const size_t numindices, vec3 * const normals, vec2 const * const tex, vec3 * const tangents){
	size_t i =0;
	std::vector<vec3> binormals(numverts);
	memset(&binormals[0], 0, sizeof(vec3)* numverts);
	do{
		vec3 v0(vertices[indices[i    ]].x, heights[indices[i    ]], vertices[indices[i    ]].y);
		vec3 v1(vertices[indices[i + 1]].x, heights[indices[i + 1]], vertices[indices[i + 1]].y);
		vec3 v2(vertices[indices[i + 2]].x, heights[indices[i + 2]], vertices[indices[i + 2]].y);
		vec2 t0(tex[indices[i    ]]);
		vec2 t1(tex[indices[i + 1]]);
		vec2 t2(tex[indices[i + 2]]);
		vec3 sdir, normal, tdir;
		TangentVectors(v0, v1, v2, t0, t1, t2, sdir, tdir, normal);
		tangents[indices[i    ]] += sdir;
		tangents[indices[i + 1]] += sdir;
		tangents[indices[i + 2]] += sdir;
		binormals[indices[i    ]] += tdir;
		binormals[indices[i + 1]] += tdir;
		binormals[indices[i + 2]] += tdir;
		normals[indices[i    ]] += normal;
		normals[indices[i + 1]] += normal;
		normals[indices[i + 2]] += normal;
		i+=3;
	} while(i < numindices);
	i=0;
	do{
		normals[i].normalize();
		const vec3& n =normals[i]; 
        const vec3& t = tangents[i];
		vec3 temp = t - n * Dot(n, t);// Gram-Schmidt orthogonalize
		temp.normalize();
        tangents[i] =temp;
        tangents[i] *= (Dot(Cross(n, t), binormals[i]) < 0.0F) ? -1.0f : 1.0f;// Calculate handedness
	} while( ++i < numverts);
}

void TangentVectors(const vec3 &v0, const vec3 &v1, const vec3 &v2, const vec2 &t0, const vec2 &t1, const vec2 &t2, vec3 &sdir);
void TangentVectors(const vec3 &v0, const vec3 &v1, const vec3 &v2, const vec2 &t0, const vec2 &t1, const vec2 &t2, vec3 &sdir, vec3 &normal);
void TangentVectors(const vec3 &v0, const vec3 &v1, const vec3 &v2, const vec2 &t0, const vec2 &t1, const vec2 &t2, vec3 &sdir, vec3 &tdir, vec3 &normal);

float RayTriangleIntersect(const vec3& rayorig, const vec3& ray, const vec3& p0, const vec3& p1, const vec3& p2);
float RayTriangleIntersect(const vec3& rayorig, const vec3& ray, const vec3* verts, const uint16_t* indices, const size_t numindices);
float RayTriangleIntersect(const vec3& rayorig, const vec3& ray, const vec3* verts, const uint32_t* indices, const size_t numindices);

template<class T>void RemapIndices(T* ind, std::vector<vec3>& movedverts, std::vector<vec2>& movedtex, size_t numindices, std::vector<vec2>& tex, std::vector<vec3>& verts){
	size_t i(0), index(0);
	do{// for each ind 
		i=0;
		do{// for each vertex
			if( (movedverts[ind[index]] == verts[i]) & (movedtex[ind[index]] == tex[i]) ){// found a matching vertex using both tex coords and verts
				ind[index] = static_cast<T>(i);// remap the vertex 
				break;
			} 
		} while(++i!=verts.size());	
	} while(++index != numindices);
}
template<class T>void RemapIndices(T* ind, std::vector<vec3>& movedverts, size_t numindices, std::vector<vec3>& verts){
	size_t i(0), index(0);
	do{// for each ind 
		i=0;
		do{// for each vertex
			if( (movedverts[ind[index]] == verts[i])){// found a matching vertex
				ind[index] = static_cast<T>(i);// remap the vertex 
				break;
			} 
		} while(++i!=verts.size());	
	} while(++index != numindices);
}

void Subdivide(std::vector<vec4>& vertices, std::vector<uint16_t>& indices);

class Object_Transform{
public:
	mat4 World,  View_Proj, Inv_Trans_World;
};
class Material {
public:
	Material(): Specular(vec4(1.0f, 1.0f, 1.0f, 1.0f)), Diffuse(vec4(1.0f, 1.0f, 1.0f, 1.0f)), Ambient(vec4(0.0f, 0.0f,0.0f, 1.0f)), Emissive(vec4(0.0f, 0.0f,0.0f, 1.0f)), Power(1.0f)  {}
	vec4 Specular, Diffuse, Ambient, Emissive;
	float Power;
	float padd[3];
};
class Batch : public NoCopy{
public:

	unsigned int NumIndices, StartIndex, NumVerts;
	
	Batch():NumIndices(0), StartIndex(0), NumVerts(0) {}
	virtual ~Batch() {  clear(); }
	void clear(){
		VS.Destroy();
		PS.Destroy();
		for(size_t i=0; i< Maps.size(); i++) Maps[i]->Destroy();
		Maps.clear();
	}

	Graphics::Texture* GetDiffuseMap(){ for(size_t i=0; i< Maps.size(); i++) {if(Maps[i]->Get_MapType() == Diffuse_Map) return Maps[i];} Maps.push_back(new Graphics::Texture()); Maps.back()->Set_MapType(Diffuse_Map);return Maps.back(); }
	Graphics::Texture* GetNormalMap(){ for(size_t i=0; i< Maps.size(); i++) {if(Maps[i]->Get_MapType() == Normal_Map) return Maps[i];} Maps.push_back(new Graphics::Texture());Maps.back()->Set_MapType(Normal_Map); return Maps.back(); }
	Graphics::Texture* GetNormal_Bump(){ for(size_t i=0; i< Maps.size(); i++) {if(Maps[i]->Get_MapType() == Normal_Bump_Map) return Maps[i];} Maps.push_back(new Graphics::Texture());Maps.back()->Set_MapType(Normal_Bump_Map); return Maps.back(); }
	
	Graphics::Texture* GetMap(Map_Type m) {for(size_t i=0; i< Maps.size(); i++) {if(Maps[i]->Get_MapType() == m) return Maps[i];} Maps.push_back(new Graphics::Texture()); Maps.back()->Set_MapType(m); return Maps.back(); }

	Graphics::VertexShader* GetVS() { return &VS;}
	Graphics::PixelShader* GetPS() { return &PS;}

	Graphics::Texture* GetFirstMap(){ if(Maps.size() ==0) return NULL; return Maps[0];}
	unsigned int GetNum_ofMaps(){ return (unsigned int)Maps.size(); }

	Material* GetMaterial() {return &_Material;}
	bool Has_NormalMap(){ for(size_t i=0; i< Maps.size(); i++) {if(Maps[i]->Get_MapType() == Diffuse_Map) return true; } return false;}
	bool Has_DiffuseMap(){for(size_t i=0; i< Maps.size(); i++) { if(Maps[i]->Get_MapType() == Normal_Map) return  true; }  return false;}
	bool Has_BumpMap(){for(size_t i=0; i< Maps.size(); i++) { if(Maps[i]->Get_MapType() == Normal_Bump_Map) return  true; }  return false;}

	void Load(std::ifstream& stream);
	void Save(std::ofstream& stream);

protected:
	std::vector<Graphics::Texture*> Maps;
	Material _Material;
	Graphics::VertexShader VS;
	Graphics::PixelShader PS;
};


#endif