#include "stdafx.h"
#include "WeatherEffects.h"
#include "../Graphics_Lib/cCamera.h"
#include "../Utilities/cNoise.h"
#include "../Utilities/My_Timer.h"

struct cbuffer0{
	vec4   g_vPositionOffset;
	vec4   g_vVelocity;
	vec4   g_vAlpha;
	vec4   g_vForward;
	mat4   CameraViewProjectionPrev, CameraViewProjection;
	vec4   CameraPosition;
	vec4   g_vSizeScale;
};

DynamicWeather::DynamicWeather() : m_fParticleBox(30.0f), m_bBlending(false), m_fValue(0.0f), m_fDuration(10.0f), m_fTime(0.0f), WeatherEffectOn(false) {
	m_iNoiseX = 0;
	m_iNoiseY = 10000;
	m_iNoiseZ = 20000;
	for(int i = 0; i < g_nPasses; i++){
		m_fSpeedScale[i] = 0.75f + My_Timer::Random(0.5f);
		m_vOffset[i] = vec3(0.f, 0.f, 0.f);
		m_vWind[i] = vec3(0.f, 0.f, 0.f);
		m_vRandom[i] = vec3(My_Timer::Random(m_fParticleBox), My_Timer::Random(m_fParticleBox), My_Timer::Random(m_fParticleBox));
	}
	//SetOcclusionBox(vec3(-5.0f, 0.0f, -5.0f), vec3(1.0f, 1.0f, 1.0f), mIdentity);
	WeatherPatterns[0] = NoWeather;// should start here and end here, then turn off
	WeatherPatterns[1] = HeavyRain;
	WeatherPatterns[2] = LightRain;
	WeatherPatterns[3] = LightSnow;
	WeatherPatterns[4] = HeavySnow;
	CurrentWeather=BlendTo=0;

}
void DynamicWeather::BeginBlend() {
	if(!m_bBlending & (CurrentWeather != BlendTo)) {// dont allow if blending 
		WeatherEffectOn=true;
		m_BlendDest = WeatherPatterns[BlendTo];
		m_bBlending = true;
		CurrentWeather= BlendTo;
		m_fValue = 0.0f;
	}
}
void DynamicWeather::Init(){
	m_Parameters= NoWeather;

	VS_Shader.CompileShaderFromFile("Weather.fx");
	FormatDesc layo[2] = {
		FormatDesc(TYPE_VERTEX, FORMAT_FLOAT, 3, 0),
		FormatDesc(TYPE_TEXCOORD, FORMAT_FLOAT, 2, 0)
	};
	VS_Shader.CreateInputLayout(layo, 2);
	PS_Shader.CompileShaderFromFile("Weather.fx");

	SnowFlake.Create("snowflake.dds");
	RainDrop.Create("raindrop.dds");

	CBuffer0.Create(1, sizeof(cbuffer0), CONSTANT_BUFFER);

	m_nParticles = 10000;
	m_nVertices = m_nParticles * 4;
	m_nIndices = m_nParticles * 6;

	vec3 vDefaultOffset(m_fParticleBox, m_fParticleBox, m_fParticleBox);

	struct Vertex_XYZ_SUV{ vec3 position; vec2 uv; };
	std::vector<Vertex_XYZ_SUV> verts(m_nVertices);
	std::vector<uint16_t> ind(m_nIndices);

    Vertex_XYZ_SUV *pVertices= &verts[0];
    uint16_t* pIndices = &ind[0];

	for(int particle = 0; particle < m_nParticles; ++particle) {
		vec3 vPosition =  vec3(My_Timer::Random(m_fParticleBox), My_Timer::Random(m_fParticleBox), My_Timer::Random(m_fParticleBox));
		vPosition += vDefaultOffset;
		//top left
		pVertices->position = vPosition;
		pVertices->uv = vec2(0, 0);
		++pVertices;

		//top right
		pVertices->position = vPosition;
		pVertices->uv = vec2(1, 0);

		++pVertices;
		// bottom left
		pVertices->position = vPosition;

		pVertices->uv = vec2(0, 1);
		++pVertices;
		// bottom right
		pVertices->position = vPosition;
		pVertices->uv = vec2(1, 1);
	
		++pVertices;

		// indices
		*pIndices = particle * 4; pIndices++;
		*pIndices = particle * 4 + 1; pIndices++;
		*pIndices = particle * 4 + 2; pIndices++;
		*pIndices = particle * 4 + 2; pIndices++;
		*pIndices = particle * 4 + 3; pIndices++;
		*pIndices = particle * 4; pIndices++;
	}
	m_pIndexBuffer.Create(m_nIndices, 2, INDEX_BUFFER, IMMUTABLE, CPU_NONE, &ind[0]);
	m_pVertexBuffer.Create(m_nVertices, sizeof(Vertex_XYZ_SUV),  VERTEX_BUFFER, IMMUTABLE, CPU_NONE, &verts[0]);
}
void DynamicWeather::DeInit(){

	VS_Shader.Destroy();
	PS_Shader.Destroy();
	SnowFlake.Destroy();
	RainDrop.Destroy();
	CBuffer0.Destroy();
	m_pIndexBuffer.Destroy();
	m_pVertexBuffer.Destroy();

}
void DynamicWeather::Update(float fTime){
	m_fTime = fTime;
	m_nPasses = (int)(m_Parameters.fDensity + 0.999f);

	// make each pass use the wind a varying amount
	int nWrap = g_nPasses / 5;
	float fWind = m_Parameters.fWind / (float)nWrap;

	//PerlinNoiseDefault noise;
	PerlinNoiseDefault noise;
	// increment the noise lookup values for each axis
	m_iNoiseX += (int)(m_fTime * 5000.0f);
	m_iNoiseY += (int)(m_fTime * 5000.0f);
	m_iNoiseZ += (int)(m_fTime * 5000.0f);
	// get a noise std::vector
	vec3 vNoise(noise.SoftNoise1D(m_iNoiseX), noise.SoftNoise1D(m_iNoiseY) * 0.5f, noise.SoftNoise1D(m_iNoiseZ));

	for(int i = 0; i < m_nPasses; i++){
		// calculate and add the per-pass gravity and wind
		float gravity = m_Parameters.fSpeed * m_fTime * m_fSpeedScale[i];
		m_vWind[i] = vNoise * fWind * (float)((i+1)%(int)nWrap);

		// add the movement from wind
		m_vOffset[i] += m_vWind[i] * m_fTime;

		// add the movement due to gravity
		m_vOffset[i].y = m_vOffset[i].y + gravity;

		// wrap the offset to be within 0.0 - m_fParticleBox in each dimension
		if(m_vOffset[i].x > m_fParticleBox)
			m_vOffset[i].x = 0.0f;
		else if(m_vOffset[i].x < 0.0f)
			m_vOffset[i].x = m_fParticleBox;

		if(m_vOffset[i].y > m_fParticleBox)
			m_vOffset[i].y = 0.0f;
		else if(m_vOffset[i].y < 0.0f)
			m_vOffset[i].y = m_fParticleBox;

		if(m_vOffset[i].z > m_fParticleBox)
			m_vOffset[i].z = 0.0f;
		else if(m_vOffset[i].z < 0.0f)
			m_vOffset[i].z = m_fParticleBox;
	}
	if(m_bBlending){
		m_fValue += fTime;
		float fInterpolation = m_fValue / m_fDuration;
		if(fInterpolation > 1.0f){
			fInterpolation = 1.0f;
			m_bBlending = false;
			m_Parameters = m_BlendDest;
			if(m_Parameters.Type==0) WeatherEffectOn=false;// turn off
			OUTPUT_DEBUG_MSG("Effect Completed");
		} else {
			WeatherParameters result;
			result.fDensity = m_Parameters.fDensity + (m_BlendDest.fDensity - m_Parameters.fDensity) * fInterpolation;
			result.fSpeed = m_Parameters.fSpeed + (m_BlendDest.fSpeed - m_Parameters.fSpeed) * fInterpolation;
			result.fWidth = m_Parameters.fWidth + (m_BlendDest.fWidth - m_Parameters.fWidth) * fInterpolation;
			result.fLength = m_Parameters.fLength + (m_BlendDest.fLength - m_Parameters.fLength) * fInterpolation;
			result.fAlpha = m_Parameters.fAlpha + (m_BlendDest.fAlpha - m_Parameters.fAlpha) * fInterpolation;
			result.fWind = m_Parameters.fWind + (m_BlendDest.fWind - m_Parameters.fWind) * fInterpolation;
			m_Parameters = result;
			m_Parameters.Type = result.Type;
		}
	}
}

void DynamicWeather::Draw(){
	BeginBlend();
	if(!WeatherEffectOn) return;// get out.. nothing to see here
	VS_Shader.Bind();
	PS_Shader.Bind();

	Update(My_Timer::DT);
	Graphics::RasterizerStates::CullNone.Bind();
	Graphics::BlendStates::Add.Bind();
	Graphics::DepthStates::NoDepthTest.Bind();

	m_pVertexBuffer.BindAsVertexBuffer();
	m_pIndexBuffer.BindAsIndexBuffer();

	Graphics::SetTopology(PRIM_TRIANGLELIST);

	if(m_Parameters.Type ==0) PS_Shader.SetResource(&RainDrop);
	else PS_Shader.SetResource(&SnowFlake);
	PS_Shader.SetSampler(Graphics::Samplers::Linear);

	float fAlpha = m_Parameters.fDensity;
	for(int i = 0; i < m_nPasses; ++i){
		SetPassConstants(i, fAlpha);// set pass specific constants needed to render weather
		Graphics::DrawIndexed(0, m_nIndices);
		fAlpha -= 1.0f;// subtract alpha
	}
}

void DynamicWeather::SetPassConstants(int i, float fAlpha){
	cbuffer0 buffer0;
	buffer0.CameraPosition = vec4(Camera.Position, 0.0f);
	buffer0.CameraViewProjection = Camera.Trans_VP;

	vec3 vCameraPosition = Camera.Position;
	vec3 vCameraMovement = m_vCameraPositionPrev - vCameraPosition;
	m_vCameraPositionPrev = vCameraPosition;

	// calculate a fade value based on the camera speed
	
	float fCameraSpeed = vCameraMovement.Length() * (1.0f / m_fTime);
	fCameraSpeed = (fCameraSpeed - m_Parameters.fAlphaFadeSpeedBegin) / m_Parameters.fAlphaFadeSpeedEnd;
	if(fCameraSpeed < 0.0f)
		fCameraSpeed = 0.0f;
	else if(fCameraSpeed > 1.0f)
		fCameraSpeed = 1.0f;
	
	m_fAlphaFade = 1.0f - fCameraSpeed; 
	if(m_fAlphaFade > 0.0f){// set std::vector to control width and height of particles
		float fLengthScale = (m_Parameters.fLength / m_Parameters.fSpeed) / m_fTime;
		float fVerticalZoom = Camera.Proj._22 / 1.778341f;  // account for fov changes
		float fWidth = m_Parameters.fWidth * fVerticalZoom;
		vec2 vSizeScale(fWidth, fLengthScale);
		buffer0.g_vSizeScale = vec4(vSizeScale, 0.0f, 0.0f);
		buffer0.CameraViewProjectionPrev = m_mWorldViewProjPrev;

		m_mWorldViewProjPrev=Camera.Trans_VP;
		mat4 viewMatrix = Camera.View;
		float fBoxOffset = m_fParticleBox * 0.5f;// set a forward shift std::vector - this gets a greater portion of the box inside the view frustum
		vec4 vForward(	viewMatrix._13 * fBoxOffset,
						viewMatrix._23 * fBoxOffset,
						viewMatrix._33 * fBoxOffset,
						viewMatrix._43);
		buffer0.g_vForward = vForward;

		//m_pPixelShaderOcclusionConstantTable->SetMatrix(pd3dDevice, "g_mOcclusionMatrix", &m_OcclusionBox.mWorldToUnitCube);
	}

	mat4 viewMatrix = Camera.View;

	// set the position offset for this pass
	
	float fBoxOffset = m_fParticleBox * 0.5f;
	vec3 vForward(	viewMatrix._13 * fBoxOffset,
					viewMatrix._23 * fBoxOffset,
					viewMatrix._33 * fBoxOffset);

	// combine all offsets together
	vec3 vOffset = vCameraPosition + m_vRandom[i] + m_vOffset[i] + vForward;
	vec4 vPositionOffset(	fmodf(vOffset.x, m_fParticleBox),
							fmodf(vOffset.y, m_fParticleBox),
							fmodf(vOffset.z, m_fParticleBox),
							0.0f);
	buffer0.g_vPositionOffset = vPositionOffset;

	// set the velocity std::vector for this pass
	vec4 vVelocity(	m_vWind[i].x * m_fTime,
					m_vWind[i].y * m_fTime + m_fSpeedScale[i] * m_Parameters.fSpeed * m_fTime,
					m_vWind[i].z * m_fTime,
					0.0f);
	buffer0.g_vVelocity = vVelocity;
	// set colour for this pass

	if(fAlpha > 1.0f)
		fAlpha = 1.0f;
	fAlpha = fAlpha * m_Parameters.fAlpha * m_fAlphaFade;
	vec4 vAlpha(fAlpha, fAlpha, fAlpha, fAlpha);
	buffer0.g_vAlpha = vAlpha;

	CBuffer0.Update(&buffer0);
	VS_Shader.SetConstantBuffer(CBuffer0);
}

void DynamicWeather::CalculateOcclusionMatrix()
{
	/*
	// the purpose of this matrix is to go from worldspace to box space

	// get std::vector for size of box
	vec3 vScale = m_OcclusionBox.vMax - m_OcclusionBox.vMin;
	// get std::vector to scale box to unit cube
	vec3 vScaleRcp = vec3(1.0f / vScale.x, 1.0f / vScale.y, 1.0f / vScale.z);

	// get std::vector to translate from m_OcclusionBox.vMin to (0, 0, 0)
	vec3 vTranslate(m_OcclusionBox.vMin.x * vScaleRcp.x,
							m_OcclusionBox.vMin.y * vScaleRcp.y,
							m_OcclusionBox.vMin.z * vScaleRcp.z);
	vTranslate = -vTranslate;
	
	// flip the Z to take into account sampling the V coordinate
	if(true)
	{
		vScaleRcp.z = -vScaleRcp.z;
		vTranslate.z = 1.0f - vTranslate.z;
	}

	// construct matrix to go from world space to box space
	mat4 mScale, mTranslate;
	mat4Scaling(&mScale, vScaleRcp.x, vScaleRcp.y, vScaleRcp.z);
	mat4Translation(&mTranslate, vTranslate.x, vTranslate.y, vTranslate.z);
	mat4 mScaleTranslate = mScale * mTranslate;

	mat4 mScaleTranslateInverse;
	mat4Inverse(&mScaleTranslateInverse, NULL, &mScaleTranslate);
	
	// multiply by the inverse transform then apply bounds
	m_OcclusionBox.mUnitCubeToWorld = mScaleTranslateInverse * m_OcclusionBox.mTransform;
	mat4Inverse(&m_OcclusionBox.mWorldToUnitCube, NULL, &m_OcclusionBox.mUnitCubeToWorld);
	*/
}

void DynamicWeather::SetOcclusionBox(const vec3& vMin, const vec3& vMax, const mat4& mTransform)
{
	/*
	m_OcclusionBox.mTransform = mTransform;
	m_OcclusionBox.vMin = vMin;
	m_OcclusionBox.vMax = vMax;

	// get bounding sphere taking into account transformation
	CalculateOcclusionMatrix();
	*/
}

