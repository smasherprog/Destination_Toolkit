#ifndef CWOODYTREE_H
#define CWOODYTREE_H
#if defined(USE_WOODY3D)
#include "../Woody_Lib/woody3d.h"
#include "Mesh/cBaseMesh.h"

class string;
class vec3;

class cWoodyTree: public cBaseMesh{
public:

	wd_tree_object					woody_tree;

	wd_directional_wind_emitter		wind_emitter;
	wd_beaufort_scale_level_s		wind_scale_array[3];
	wd_real_32						tree_lod;
	wd_real_32						tree_y_rotation;
	static Graphics::Buffer CBuffer0, CBuffer1, CBuffer2;

	unsigned int s_last_time_ms;
	unsigned int elapsed_time_ms;

cWoodyTree();
virtual ~cWoodyTree();
virtual void DrawLod1(void* param1);
virtual void DrawLod2(void* param1){ DrawLod1(param1); }
virtual void DrawLod3(void* param1){ DrawLod1(param1); }

virtual void DrawShadowLod1(void* param1);
virtual void DrawShadowLod2(void* param1);
virtual void DrawShadowLod3(void* param1);

virtual bool Load(const std::string& file);
virtual void Save(const std::string& file){ }

virtual uint32_t GetType(){ return WOODYTREEMESH; }
virtual float RayIntersect(const vec3& rayorig, const vec3& raydir);

};
#else 
class cWoodyTree: public cBaseMesh{

	virtual void DrawLod1(void* param1){}
	virtual void DrawLod2(void* param1){}
	virtual void DrawLod3(void* param1){}

	virtual void DrawShadowLod1(void* param1){}
	virtual void DrawShadowLod2(void* param1){}
	virtual void DrawShadowLod3(void* param1){}

	virtual float RayIntersect(const vec3& rayorig, const vec3& raydir){ return 0.0f;}
	virtual uint32_t GetType() { return 0;}
	virtual void Save(const std::string& file){}

	virtual bool Load(const std::string& file){ return false;}
};

#endif

#endif