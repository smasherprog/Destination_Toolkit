#ifndef BATCH_H
#define BATCH_H

#include "../Graphics_Lib/Globals_GraphicsCore.h"
#include "../Graphics_Lib/GraphicsCore.h"

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