#ifndef CPOINTLIGHT_H
#define CPOINTLIGHT_H
#include "../Graphics_Lib/cBaseMesh.h"
#include "../Graphics_Lib/MeshTypes.h"

class cPointLight: public cBaseMesh, public cSphere{
public:

	vec4 Diffuse, SpecularColor, Attenuation;//Attenuation x y and z are atten and w is the spec power
	bool On;
	std::string Name, FileName;


cPointLight(){ clear();}
virtual ~cPointLight(){}
virtual void DrawLod1(void* param1){}
virtual void DrawLod2(void* param1){}
virtual void DrawLod3(void* param1){}

virtual void DrawShadowLod1(void* param1){}
virtual void DrawShadowLod2(void* param1){}
virtual void DrawShadowLod3(void* param1){}

virtual bool Load(const std::string& file){ return true;}// doesnt mean anything yet
virtual void Save(ofstream& file);
virtual bool Load(ifstream& file);
virtual void Save(const std::string& file){ }

virtual float RayIntersect(const vec3& rayorig, const vec3& raydir){ return cSphere::RayIntersect(rayorig, raydir); }

virtual uint32_t GetType(){ return POINTLIGHT; }
virtual void clear(){ Attenuation=vec4(0.0f, 0.0f, 1.0f, 1.0f);  On=true; Diffuse = vec4(1.0f, 1.0f,1.0f,1.0f); SpecularColor = vec4(1.0f, 1.0f,1.0f,1.0f); }
void Setup();

/////////////////ugly below... lots of redoing
virtual void UpdateWorldMatrix(){ }// nothing to do here
virtual void GetScaling(float& x, float &y, float& z){ x= Radius; y = Radius; z =Radius;  }
virtual void GetScalingX(float& x) { x= Radius; }
virtual void GetScalingY(float& x) { x= Radius; }
virtual void GetScalingZ(float& x) { x= Radius; }
virtual void SetScaling(const float& x, const float &y, const float& z) {Radius=x; Radius=y; Radius=z; }
virtual void SetScalingX(const float& x) { Radius=x; }
virtual void SetScalingY(const float& x) { Radius=x; }
virtual void SetScalingZ(const float& x) { Radius=x; }

virtual void GetPosition(float& x, float &y, float& z){ x=Center.x; y =Center.y; z = Center.z;  }
virtual void GetPositionX(float& x) { x= Center.x; }
virtual void GetPositionY(float& x) { x= Center.y; }
virtual void GetPositionZ(float& x) { x= Center.z; }

virtual void SetPosition(const float& x, const float &y, const float& z){ Center.x=x; Center.y=y; Center.z=z;  }
virtual void SetPositionX(const float& x) { Center.x=x; }
virtual void SetPositionY(const float& x) { Center.y=x; }
virtual void SetPositionZ(const float& x) { Center.z=x; }


};


#endif