#ifndef DYNAMICWEATHER_H
#define DYNAMICWEATHER_H
#include "../Graphics_Lib/GraphicsCore.h"
#include "../Utilities/MyMath.h"

#define g_nPasses 20

class WeatherParameters{
public:
	WeatherParameters() : fDensity(5.0f), fSpeed(8.0f), fLength(0.5f), fWidth(0.05f), fAlpha(0.1f), fWind(0.0f), fAlphaFadeSpeedBegin(10.0f), fAlphaFadeSpeedEnd(40.0f) {}
	WeatherParameters(	float fDen, float fSp, float fLen, float fWi, float fAl, float fWin, uint32_t ty):
	fDensity(fDen), fSpeed(fSp), fLength(fLen), fWidth(fWi), fAlpha(fAl), fWind(fWin), fAlphaFadeSpeedBegin(10.0f), fAlphaFadeSpeedEnd(40.0f), Type(ty){}
		
	float fDensity;
	float fSpeed;
	float fLength;
	float fWidth;
	float fAlpha;
	float fWind;
	float fAlphaFadeSpeedBegin;
	float fAlphaFadeSpeedEnd;
	uint32_t Type;

};
#define NoWeather WeatherParameters(1.0f, 4.5f,0.5f,0.02f,0.01f,1.0f, 0)
#define HeavyRain WeatherParameters(10.0f, 8.0f, 0.25f, 0.01f, 0.15f, 6.0f, 1)
#define LightRain WeatherParameters(3.0f,4.5f,0.15f,0.02f,0.10f,1.0f, 2)
#define LightSnow WeatherParameters(5.0f, 1.5f, 0.03f, 0.03f, 0.3f, 3.0f, 3)
#define HeavySnow WeatherParameters(15.0f, 1.5f, 0.05f, 0.05f, 0.3f, 5.0f, 4)

struct WeatherOcclusionBox{
	vec3		vMin;
	vec3		vMax;
	mat4		mTransform;
	mat4		mWorldToUnitCube;
	mat4		mUnitCubeToWorld;
};

class DynamicWeather{
public:
	DynamicWeather(); 

	void Init();
	void DeInit();
	void Draw();
	void Update(float fTime);
	void SetOcclusionBox(const vec3& vMin, const vec3& vMax, const mat4& mTransform);
	void BeginBlend();
	void CalculateOcclusionMatrix();
	void SetPassConstants(int iPass, float fAlpha);

	Graphics::VertexShader VS_Shader;
	Graphics::PixelShader PS_Shader;

	Graphics::Texture SnowFlake, RainDrop;
	Graphics::Buffer CBuffer0;
	Graphics::Buffer m_pIndexBuffer, m_pVertexBuffer;

	int	m_nParticles, m_nVertices, m_nIndices, m_iNoiseX, m_iNoiseY, m_iNoiseZ, m_nPasses;
	// rendering parameters

	float m_fParticleBox, m_fTime, m_fSpeedColourScale, m_fAlphaFade, m_fSpeedScale[g_nPasses], m_fDuration, m_fValue;
	vec3						m_vWind[g_nPasses], m_vOffset[g_nPasses], m_vRandom[g_nPasses], m_vCameraPositionPrev;
	mat4 m_mWorldViewProjPrev;
	bool						m_bBlending, WeatherEffectOn;
	uint32_t		BlendTo, CurrentWeather;
	WeatherParameters			m_BlendDest, m_Parameters, WeatherPatterns[5];
};


#endif





