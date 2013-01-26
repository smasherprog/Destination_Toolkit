#ifndef SKYDOME_H
#define SKYDOME_H
#include "../Graphics_Lib\Globals_GraphicsCore.h"
#include "../Graphics_Lib\GraphicsCore.h"

class vec3;
class mat4;

class Sky{
public:


	Graphics::Buffer DomeVertexBuffer, DomeIndexBuffer;
	Graphics::VertexShader VS_SkyScatter, VS_SkyCubeMap;
	Graphics::GeometryShader GS_SkyCubeMap;
	Graphics::PixelShader PS_SkyScatter, PS_Update, PS_Create_OpticalDepth;
	Graphics::Texture txMie, txRayleigh, txOpticalDepth, SkyCubeMap;
	Graphics::Buffer CBuffer0, CBuffer1;

	const vec3* SunDir;
	const float* FarPlane;

	Sky():SunDir(0), FarPlane(0) {}
	~Sky(){ DeInit(); }
	void Init(vec3* sundir, const float* farplane);
	void DeInit();
	void Draw(const mat4& view, const mat4& proj);

};

#endif // SKY_H

