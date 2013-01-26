#ifndef CCLOUDS_H
#define CCLOUDS_H
#include "../Graphics_Lib/Globals_GraphicsCore.h"
#include "../Graphics_Lib/GraphicsCore.h"
#include "../Physics_Lib/cAABB.h"
#include "../Utilities/MyMath.h"

struct SScatteringShaderParameters {
	vec4 vRayleigh;   // rgb : 3/(16*PI) * Br           w : -2*g
	vec4 vMie;        // rgb : 1/(4*PI) * Bm * (1-g)^2  w : (1+g^2)
	vec4 vESun;       // rgb : Esun/(Br+Bm)             w : R
	vec4 vSum;        // rgb : (Br+Bm)                  w : h(2R+h)
	vec4 vAmbient;    // rgb : ambient
	vec4 vUVParam;
	vec4 vXZParam;
	vec4 vOff;
	vec4 vParam;
	vec4 vHeightPix;
	//vec4 SunDir;

	vec4 CameraPosition;
	mat4 InvCameraViewProjection;
};

class CCloud {
public:

	void Init();
	void DeInit();
	void CreateBuffers();
	void Update(float dt);
	void Draw();


	Graphics::VertexShader VS_RenderCloudGrid, VS_RenderCloudPlane, VS_RenderCloudBlur;
	Graphics::PixelShader PS_RenderCloudGrid, PS_RenderCloudPlane, PS_RenderCloudShadow, PS_RenderCloudBlur;
	Graphics::Buffer RawBuffer, GridVB, GridIB, RawBuffer1;
	Graphics::Texture CloudTex, ShadowMap, BlurredMap, DensityMap;

	mat4 m_mW2SProj, m_mW2S, m_mW2SProj_Trans;
	uint32_t GridVertices, GridIndices;           // the number of vertices the number of triangles
	vec3 m_vAmbientLight;
	vec2 m_vStartXZ, m_vCellSizeXZ, m_vVelocity, m_vOffset, CloudsScale;      // minimum x,z position 
	float m_fDefaultHeight, m_fFallOffHeight, m_fCloudCover, m_fAmbientScale;      // cloud height above the view position					    
	cAABB m_bound;        // bounding box of the grid
	bool NeedsUpdate;
	SScatteringShaderParameters params;
};



#endif 
