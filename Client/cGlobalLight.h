#ifndef CGLOBALLIGHT_H
#define CGLOBALLIGHT_H
#include "../Graphics_Lib/Globals_GraphicsCore.h"
#include "../Graphics_Lib/cCamera.h"
#include "../Graphics_Lib\GraphicsCore.h"
#include "../Utilities\My_Timer.h"
#include "../Physics_Lib/cAABB.h"

#define NUMSPLITS 4

class cGlobalLight{
public:

	struct ShadowcBuffer0{		
		mat4 ShadowMapSplits[NUMSPLITS], ShadowMapSplitsTex[NUMSPLITS];
		vec4 SplitPositions;
		mat4 InvCameraViewProjectionFromScreen, CameraViewProjection;
		vec4 SunNormal, SunPosition, CameraPosition;
		mat4 CloudsToTex;
		vec2 ShadowSettings;
	};
		

	enum MAPFILESIZEs { Low = 512, Medium = 1024, High = 2048};

	Graphics::PixelShader PS_ApplyShadowMap,  PS_ApplyShadowMapNight;
	Graphics::Buffer CBufferShadow0, CBufferShadow2;
	Graphics::Texture ShadowDepth, ShadowMapArray, ShadowMapSlice[NUMSPLITS];

	uint32_t ShadowMapSize, ChangeShadowMap;
	mat4 LightViewProj, Proj, View;
	float SplitPos[NUMSPLITS];

	My_Timer::tick_count timer;
	
	bool PingPong, Started, NightTime;
	unsigned int BlurSamples;

	float  ShadowMapDistance, ProjSize, Lambda;
	bool ShowSplits;
	mat4 SplitViewProj[NUMSPLITS], SplitViewProjTex[NUMSPLITS];
	cAABB FrustumBVs[NUMSPLITS];
	vec2 SplitScales[NUMSPLITS];
	float SplitDistances[NUMSPLITS];


	cGlobalLight();
	~cGlobalLight(){ DeInit(); }

	void Init(uint32_t mapsize, int samples);
	void DeInit();

	void PrepShadowMap(vec2 minmax);
	void BlurShadowMaps();
	void ApplyShadowMaps(Graphics::Texture& cloudsmap, mat4 cloudsshadow);

	mat4* ShadowMapPass(int32_t i);

protected:
	
	void BlurMap(uint32_t i);	
	void RecomputeSplitDistances();
	void RecomputeSplitMatrices();	
	void BlurShadowMap(Graphics::Texture& map);
};


#endif
