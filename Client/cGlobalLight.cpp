#include "stdafx.h"
#include "cGlobalLight.h"
#include "../Graphics_Lib/Shaders.h"
#include "Global_GameData.h"

cGlobalLight::cGlobalLight() : Started(false), NightTime(false) {
	timer = My_Timer::tick_count::now();
	PingPong=false;

	ShadowMapDistance = 1000.0f;
	ProjSize = 15000.0f;
	Lambda = 0.93f;
	Proj.identity();
	View.identity();
	LightViewProj.identity();
	ShowSplits=false;
}

void cGlobalLight::Init(uint32_t mapsize, int samples){// samples must be odd and cannot be a fraction
	if((mapsize != Low) | (mapsize != Medium) | (mapsize != High)) mapsize = Medium;// enforce a good value
	if(Started){// if we already started....resize the resources
		if(mapsize != ShadowMapSize){// dont resize, if we dont need to... eh?
			ShadowMapArray.Resize(mapsize, mapsize, 1, 0, 5);
			ShadowDepth.Resize(mapsize, mapsize, 1, 1, 1);
		} 
	} else { // we have not started yet, create the resources
		timer = My_Timer::tick_count::now();
		
		PingPong=false;

		PS_ApplyShadowMap.CompileShaderFromFile("BlurShadowMap.fx");
		PS_ApplyShadowMapNight.CompileShaderFromFile("BlurShadowMap.fx", "PSNight");
	
		CBufferShadow0.Create(1,sizeof(ShadowcBuffer0), CONSTANT_BUFFER);
		CBufferShadow2.Create(1,sizeof(Graphics::Shader_Defines::Blur_PS_struct), CONSTANT_BUFFER);

		ShadowMapDistance = 3500.0f;
	
		ProjSize = 8000.0f;
		Lambda = 0.93f;
		Proj.identity();
		View.identity();
		LightViewProj.identity();
		ShowSplits=false;

		ShadowMapArray.CreateRenderTarget(mapsize, mapsize, 1, 0, 5, FORMAT_R32F , 1, 0, USE_MIPGEN | SAMPLE_SLICES | RENDER_SLICES);
		ShadowDepth.CreateRenderDepth(mapsize, mapsize, 1, FORMAT_D32F);
		
	}
	ChangeShadowMap = ShadowMapSize = mapsize;
	BlurSamples = samples;
	Started=true;// either way, we have started!
}
void cGlobalLight::DeInit(){
	Started=false;

	PS_ApplyShadowMap.Destroy();
	PS_ApplyShadowMapNight.Destroy();

	CBufferShadow0.Destroy();
	CBufferShadow2.Destroy();

	ShadowDepth.Destroy(); 
	ShadowMapArray.Destroy(); 
	for( int i=0; i < NUMSPLITS; i++) ShadowMapSlice[i].Destroy();

}
float GetGaussianDistribution( float x, float y, float rho )
{
   float g = 1.0f / sqrt( 2.0f * 3.141592654f * rho * rho );
   return g * exp( -(x * x + y * y) / (2 * rho * rho) );
}

void GetGaussianOffsets( bool bHorizontal, vec2 vViewportTexelSize, vec2* vSampleOffsets, float* fSampleWeights )
{
   // Get the center texel offset and weight
   fSampleWeights[0] = 1.0f * GetGaussianDistribution( 0, 0, 2.0f );
   vSampleOffsets[0] = vec2( 0.0f, 0.0f );
   // Get the offsets and weights for the remaining taps
   if( bHorizontal )
   {
      for( int i = 1; i < 15; i += 2 )
      {
         vSampleOffsets[i + 0] = vec2( i * vViewportTexelSize.x, 0.0f );
         vSampleOffsets[i + 1] = vec2( -i * vViewportTexelSize.x, 0.0f );
         fSampleWeights[i + 0] = 2.0f * GetGaussianDistribution( float(i + 0), 0.0f, 3.0f );
         fSampleWeights[i + 1] = 2.0f * GetGaussianDistribution( float(i + 1), 0.0f, 3.0f );
      }
   }
   else 
   {
      for( int i = 1; i < 15; i += 2 )
      {
         vSampleOffsets[i + 0] = vec2( 0.0f, i * vViewportTexelSize.y );
         vSampleOffsets[i + 1] = vec2( 0.0f, -i * vViewportTexelSize.y );
         fSampleWeights[i + 0] = 2.0f * GetGaussianDistribution( 0.0f, float(i + 0), 3.0f );
         fSampleWeights[i + 1] = 2.0f * GetGaussianDistribution( 0.0f, float(i + 1), 3.0f );
      }
   }
}
void cGlobalLight::PrepShadowMap(vec2 minmax){
	vec3 lookat = Camera.Position;
	vec3 eyepos = (SunDir * ShadowMapDistance) + lookat;
	SunPos = eyepos;
	vec3 vLight( 0.0f, 0.0f, 0.0f );
	vec3 vUp( 0.0f, 1.0f, 0.0f );

	
	// transform ground and cloud bounding box to the light coordinate
	cAABB bbGroundInView, bv;

	View.setupLookAt(eyepos, lookat, vUp);

	bbGroundInView.Min = vec3(-ProjSize, minmax.x - 5000, -ProjSize) + Camera.Position;
	bbGroundInView.Max = vec3(ProjSize, minmax.y + 5000, ProjSize) + Camera.Position;

	bbGroundInView.Transform(bv, View);
	// minimize bounding box The view frustom should be take into account as well.
	cAABB bound;
	bound.Max = bv.Max;
	bound.Min = bv.Min;
	if ((bound.Min.x < bound.Max.x) & (bound.Min.y < bound.Max.y) & (bound.Min.z < bound.Max.z)) {
		vec3 vCenter;
		bound.Centroid(&vCenter);
		vec3 vDiag = bound.Max - bound.Min;
		// Move the view position to the center of the bounding box. z is located behined the volume.
		vec3 vEye(vCenter);
		vEye.z = vCenter.z - 0.5f * vDiag.z;
		vec3 vMove = vLight - vEye;
		mat4 mTrans;
		mTrans.setupTranslation(vMove);
		Proj.setupOrtho(vDiag.x, vDiag.y, 1.0f, Camera.FarPlane);//check x or y neg
		LightViewProj=View* mTrans* Proj;// Compute world to shadow map projection matrix
	} 
 	
	ShadowMapArray.ClearRenderTargets();

	RecomputeSplitDistances();
	RecomputeSplitMatrices();
	
}
mat4* cGlobalLight::ShadowMapPass(int32_t i){
	if(ShadowMapSize != ChangeShadowMap) Init(ChangeShadowMap, BlurSamples);
	uint32_t slice = i;	
	ShadowMapArray.ClearRenderTargets();
	ShadowDepth.ClearDepthTargets(true, false);
	Graphics::Texture* tex[] = { &ShadowMapArray };
	Graphics::BindRenderTargets(tex, 1, &ShadowDepth, -1, &slice);
	return &SplitViewProj[i];
}

void cGlobalLight::BlurShadowMaps(){

	Graphics::SetTopology(PRIM_TRIANGLE_STRIP);
	Graphics::Shaders::VS_FullScreenQuad.Bind();
	Graphics::Shaders::PS_Blur.Bind();
	Graphics::DepthStates::NoDepthTest.Bind();
	Graphics::BlendStates::No_Blend.Bind();
	Graphics::RasterizerStates::CullNone.Bind();
	Graphics::Shaders::PS_Blur.SetSampler(Graphics::Samplers::AnisotropicClampUVW);
	Graphics::Shader_Defines::Blur_PS_struct bs;

	float blr = static_cast<float>(BlurSamples);
	for(uint32_t i(0); i< NUMSPLITS; i++){
		Graphics::UnBindRenderTargets();

		Graphics::Shaders::PS_Blur.SetResource(&ShadowMapArray, i);
		bs.UDLR = true;
		*reinterpret_cast<vec4*>(bs.Settings) = vec4(1.0f/blr, 1.0f/static_cast<float>(ShadowMapSize), blr, 0.0f);
		CBufferShadow2.Update(&bs);
		Graphics::Shaders::PS_Blur.SetConstantBuffer(CBufferShadow2);

		uint32_t slice=4;// this is the last array slice which i use for bluring

		Graphics::Texture* tex[] = { &ShadowMapArray };
        Graphics::BindRenderTargets(tex, 1, 0, -1, &slice);// dont have a depth buffer bound
        Graphics::DrawVertices(4, 0);
	
		Graphics::UnBindRenderTargets();

		slice =4;
        Graphics::Shaders::PS_Blur.SetResource(&ShadowMapArray, slice);
		bs.UDLR=false;
		CBufferShadow2.Update(&bs);
		Graphics::Shaders::PS_Blur.SetConstantBuffer(CBufferShadow2);
	
        Graphics::BindRenderTargets(tex, 1, 0, -1, &i);// dont have a depth buffer bound 
        Graphics::DrawVertices(4, 0);
	}
	ShadowMapArray.GenerateMips();
}

void cGlobalLight::ApplyShadowMaps(Graphics::Texture& cloudsmap, mat4 cloudsshadow){
	Graphics::Shaders::VS_FullScreenQuad.Bind();
	vec2 settings;
	Graphics::PixelShader* shader =0;
    if(SunDir.y < 0.1f){
		settings.x = .2f;// cap the darkest at .2
		shader= &PS_ApplyShadowMapNight;
		PS_ApplyShadowMapNight.Bind();
	} else{
		settings.x = SunDir.y; 
		shader= &PS_ApplyShadowMap;
		PS_ApplyShadowMap.Bind();
	}
	
	if(ShowSplits) settings.y = 1.0f;
	else settings.y=0;

	ShadowcBuffer0 b;
	memcpy(&b.ShadowMapSplits, SplitViewProj, NUMSPLITS*sizeof(mat4));
	memcpy(&b.ShadowMapSplitsTex, SplitViewProjTex, NUMSPLITS*sizeof(mat4));
	for(int i =0; i< NUMSPLITS; i++){
		b.ShadowMapSplits[i].Transpose();
		b.ShadowMapSplitsTex[i].Transpose();
	}
	memcpy(&b.SplitPositions, SplitPos, sizeof(vec4));
	b.InvCameraViewProjectionFromScreen = Camera.InvVPToScreen;
	b.InvCameraViewProjectionFromScreen.Transpose();
	b.CameraViewProjection = Camera.VP;
	b.CameraViewProjection.Transpose();
	b.SunNormal = vec4(SunDir, 0.0f);
	b.SunPosition = vec4(SunPos, 0.0f);
	b.CameraPosition = vec4(Camera.Position, 0.0f); 
	b.CloudsToTex = cloudsshadow;
	b.CloudsToTex.Transpose();
	b.ShadowSettings = settings;
	CBufferShadow0.Update(&b);
	Graphics::Texture* tex[] = { &Graphics::Textures::RT_Base };
	Graphics::BindRenderTargets(tex, 1, &Graphics::Textures::RT_DepthStencil);// change to background, 
	Graphics::BlendStates::No_Blend.Bind();
	Graphics::RasterizerStates::CullNone.Bind();
	Graphics::DepthStates::StencilTest.Bind(2);// use the stencil to only shade the correct pixels

	shader->SetResource(&Graphics::Textures::RT_BackBufferTexture, 0);
	shader->SetResource(&Graphics::Textures::RT_Normal, 1);
	shader->SetResource(&Graphics::Textures::RT_Depth, 2);
	shader->SetResource(&cloudsmap, 3);
	shader->SetResource(&ShadowMapArray, 4);

	Graphics::SetTopology(PRIM_TRIANGLE_STRIP);
    Graphics::DrawVertices(4, 0);       

}


void cGlobalLight::RecomputeSplitDistances() {

	float SplitNear(Camera.NearPlane), SplitFar(Camera.FarPlane);
	float SplitRange(SplitFar - SplitNear),  SplitRatio(SplitFar / SplitNear);

	for (unsigned int i = 0; i < NUMSPLITS; ++i) {
		float p = i / static_cast<float>(NUMSPLITS);
		float LogSplit     = SplitNear * std::pow(SplitRatio, p);
		float UniformSplit = SplitNear + SplitRange * p;
		float Split        = Lambda * (LogSplit - UniformSplit) + UniformSplit;// Lerp between the two schemes
		SplitDistances[i]=Split;
	}
	// Just for simplicty later, push the camera far plane as the last "split"
	SplitDistances[NUMSPLITS] = SplitFar;

	// Update the effect variable NOTE: Single 4-tuple, so maximum of five splits right now
	float *EffectSplits = SplitPos;
	float CameraSplitRatio = SplitFar / (SplitFar - SplitNear);
	unsigned int EffectSplitIndex;
	for (EffectSplitIndex = 1; EffectSplitIndex < (NUMSPLITS-1); ++EffectSplitIndex) {
		// NOTE: Have to rescale the splits. The effect expects them to be in post-projection,
		// but pre-homogenious divide [0,...,far] range, relative to the REAL camera near and far.
		float Value = (SplitDistances[EffectSplitIndex] - SplitNear) * CameraSplitRatio;
		EffectSplits[EffectSplitIndex-1] = Value;
	}
	// Fill in the rest with "a large number"
	float MaxFloat = INFINITY;
	for (; EffectSplitIndex <= 4; ++EffectSplitIndex) {
		EffectSplits[EffectSplitIndex-1] = MaxFloat;
	}
}
void cGlobalLight::RecomputeSplitMatrices(){


	// Extract some useful camera-related information
	mat4 invcam(Camera.InvView);
	float XScaleInv = 1.0f / Camera.Proj._11;
	float YScaleInv = 1.0f / Camera.Proj._22;
	vec3 Corners[8];
	vec4 cornerproj[8];
	cAABB frustum;
	mat4 camviewtolightviewproj(invcam * LightViewProj);
	for (unsigned int i = 0; i < NUMSPLITS; ++i) {
		frustum.clear();
		float Near = SplitDistances[i];
		float Far  = SplitDistances[i+1];
		//near corners in view space....
		float NX = XScaleInv * Near;
		float NY = YScaleInv * Near;
		Corners[0] = vec3(-NX,  NY, Near);
		Corners[1] = vec3( NX,  NY, Near);
		Corners[2] = vec3(-NX, -NY, Near);
		Corners[3] = vec3( NX, -NY, Near);
		// Far corners (in view space)
		float FX = XScaleInv * Far;
		float FY = YScaleInv * Far;
		Corners[4] = vec3(-FX,  FY, Far);
		Corners[5] = vec3( FX,  FY, Far);
		Corners[6] = vec3(-FX, -FY, Far);
		Corners[7] = vec3( FX, -FY, Far);
		for(uint32_t j(0); j< 8; j++) {	// Transform corners into projected light space
			Transform(Corners[j], cornerproj[j], camviewtolightviewproj);
			vec3 temp;
			Transform(Corners[j], temp, Camera.InvView);
			frustum.Add(temp);
		}
		vec2 min_(1.0f, 1.0f), max_(-1.0f, -1.0f);	
		for (unsigned int c = 0; c < 8; ++c) {
			const vec4& p = cornerproj[c];
			if (p.z < 0.0f) {
				// In front of near clipping plane! Be conservative...
				min_ = vec2(-1, -1);
				max_ = vec2( 1,  1);
				break;
			} else {// Update boundaries
				vec2 v(p.x, p.y);
				v *= 1.0f / p.w;
				min_ = Min(min_, v);
				max_ = Max(max_, v);
			}
		}
		vec2 Dim = max_ - min_;// Degenerate slice?
		if ((max_.x <= -1.0f) | (max_.y <= -1.0f) | (min_.x >= 1.0f) | (min_.y >= 1.0f) | (Dim.x <= 0.0f) | (Dim.y <= 0.0f)) {
			min_ = vec2(-1, -1);
			max_ = vec2( 1,  1);
		}	
		// Clamp to valid range
		min_.x = Clamp(min_.x, -1.0f, 1.0f);
		min_.y = Clamp(min_.y, -1.0f, 1.0f);
		max_.x = Clamp(max_.x, -1.0f, 1.0f);
		max_.y = Clamp(max_.y, -1.0f, 1.0f);
		vec2 Scale, Offset;// Compute scale and offset
		Scale.x = 2.0f / (max_.x - min_.x);
		Scale.y = 2.0f / (max_.y - min_.y);
		Offset.x = -0.5f * (max_.x + min_.x) * Scale.x;
		Offset.y = -0.5f * (max_.y + min_.y) * Scale.y;
		// Store scale factors for later use when blurring Adjust projection matrix to "zoom in" on the target region
		mat4 Zoom(Scale.x,     0.0f,  0.0f,   0.0f,
				     0.0f,  Scale.y,  0.0f,   0.0f,
					 0.0f,     0.0f,  1.0f,   0.0f,
				 Offset.x, Offset.y,  0,   1.0f);
		mat4 lit(LightViewProj * Zoom);
		SplitViewProj[i]=lit;// Compute new composite matrices and store
		mat4 mProj2Tex(// Compute world to shadowmap texture coordinate matrix
			0.5f,  0.0f, 0.0f, 0.0f,
			0.0f, -0.5f, 0.0f, 0.0f,
			0.0f,  0.0f, 1.0f, 0.0f,
			0.5f,  0.5f, 0.0f, 1.0f );
		SplitViewProjTex[i]=lit*mProj2Tex;
		FrustumBVs[i]=frustum;
		}
	}
