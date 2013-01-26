#include "stdafx.h"
#include "cClouds.h"
#include "../Utilities/My_Timer.h"
#include "../Graphics_Lib/cCamera.h"
#include "Global_GameData.h"

#define EPSILON 0.000001f
#define DIV_X  4
#define DIV_Y  4
#define NUM_VERTICES (DIV_X+1) * (DIV_Y+1)
#define NUM_INDICES  2*DIV_Y * (DIV_X+1) + (DIV_Y-1)*2
#define m_fG .7f
#define m_fLightScale 8.0f
#define m_vLightColor vec3( 1.2f, 1.2f, 1.2f )
#define m_fEarthRadius 21600000.0f
#define m_fAtomosHeight 30000.0f
#define m_fCloudHeight 600.0f
#define m_vRayleigh vec3( 0.3f, 0.45f, 6.5f )
#define m_vMie vec3( 0.3f, 0.3f, 0.3f )

void CCloud::Init(){

	m_vAmbientLight = vec3( 0.3f, 0.35f, 0.4f );
	// scattering
	CloudsScale = vec2(10, 10);
	m_fAmbientScale = 0.1f;

	float wndx, wndy;
	Graphics::GetBackBufferSize(wndx, wndy);
	// create render targets of density map and blurred density map.
	DensityMap.CreateRenderTarget(static_cast<int32_t>(wndx/2.0f), static_cast<int32_t>(wndy/2.0f), 1, 1, 1, FORMAT_R16);
	BlurredMap.CreateRenderTarget(static_cast<int32_t>(wndx/2.0f), static_cast<int32_t>(wndy/2.0f), 1, 1, 1, FORMAT_R16);
	
	// create render targets of cloud shadowmap 
	uint32_t nWidth = static_cast<uint32_t>(wndx);
	uint32_t nHeight = static_cast<uint32_t>(wndy);
	ShadowMap.CreateRenderTarget(nWidth, nHeight, 1, 1, 1, FORMAT_R16);
	CloudTex.Create("Cloud.bmp");

	VS_RenderCloudGrid.CompileShaderFromFile("Cloud.fx", "RenderCloud_Grid_VS");
	FormatDesc lay[1] = { FormatDesc(TYPE_VERTEX, FORMAT_FLOAT, 4, 0, false) };
	VS_RenderCloudGrid.CreateInputLayout(lay, 1);
	PS_RenderCloudGrid.CompileShaderFromFile("Cloud.fx", "RenderCloud_Grid_PS");

	VS_RenderCloudPlane.CompileShaderFromFile("Cloud.fx", "RenderCloud_Plane_VS");

	PS_RenderCloudPlane.CompileShaderFromFile("Cloud.fx", "RenderCloud_Plane_PS");

	PS_RenderCloudShadow.CompileShaderFromFile("Cloud.fx", "RenderCloud_Shadow_PS");

	VS_RenderCloudBlur.CompileShaderFromFile("Cloud.fx", "RenderCloud_Blur_VS");

	PS_RenderCloudBlur.CompileShaderFromFile("Cloud.fx", "RenderCloud_Blur_PS");
	m_fCloudCover = 0.5f;
	NeedsUpdate = true;
	m_vVelocity = vec2( 0.0025f, 0.0025f );
	m_vOffset = vec2( 0.0f, 0.0f );
	RawBuffer.Create(1, sizeof(SScatteringShaderParameters), CONSTANT_BUFFER);
	RawBuffer1.Create(1, sizeof(mat4), CONSTANT_BUFFER);
	
	CreateBuffers();
}


void CCloud::DeInit(){
	VS_RenderCloudGrid.Destroy();
	VS_RenderCloudPlane.Destroy();

	VS_RenderCloudBlur.Destroy();
	PS_RenderCloudGrid.Destroy();
	PS_RenderCloudPlane.Destroy();
	PS_RenderCloudShadow.Destroy();
	PS_RenderCloudBlur.Destroy();

	DensityMap.Destroy();
	BlurredMap.Destroy();
	ShadowMap.Destroy();
	CloudTex.Destroy();

	RawBuffer.Destroy();
	RawBuffer1.Destroy();
	GridVB.Destroy();
	GridIB.Destroy();

}
void CCloud::CreateBuffers(){
	
	const uint16_t nCellNumX(64);
	const uint16_t nCellNumZ(64);
	m_vStartXZ = vec2( -20000.0f, -20000.0f );
	m_vCellSizeXZ = vec2( (80000.0f)/(float)nCellNumX, (80000.0f)/(float)nCellNumZ );

	GridVertices = (nCellNumX + 1)*(nCellNumZ + 1);

	std::vector<vec4> otherverts(GridVertices);
	vec4* pVertices = &otherverts[0];
	// The vertex buffer includes only x and z index in the grid and they are scaled in the vertex shader.
	// The height y is computed in the vertex shader using horizontal Lengthance from view point.
	float fScaleX = 1.0f/(static_cast<float>(nCellNumX)+1.0f);
	float fScaleZ = 1.0f/(static_cast<float>(nCellNumZ)+1.0f);
	for (UINT z = 0; z < nCellNumZ+1; ++z) {
		for (UINT x = 0; x < nCellNumX+1; ++x) {
			pVertices->x = (float)x;     // x index
			pVertices->y = (float)z;     // z index
			pVertices->z = x * fScaleX;  // texture coordinate u
			pVertices->w = z * fScaleZ;  // texture coordinate v
			++pVertices;
		}
	}
	GridVB.Create(GridVertices, sizeof(vec4), VERTEX_BUFFER, IMMUTABLE, CPU_NONE, &otherverts[0]);
	
	// Initialize x and z components of the bounding box  MaxY is changed at every frame according to the eye height.
	m_bound.Min = vec3( m_vStartXZ.x, 0.0f, m_vStartXZ.y );
	vec2 vEndXZ( m_vCellSizeXZ.x * nCellNumX, m_vCellSizeXZ.y * nCellNumZ );
	vEndXZ = vEndXZ + m_vStartXZ;
	m_bound.Max = vec3( vEndXZ.x, 0.0f, vEndXZ.y  );

	GridIndices = (nCellNumX+2) * 2 * nCellNumZ - 2;
	std::vector<uint16_t> ind(GridIndices);
	uint16_t* pIndex = &ind[0];
	USHORT nVertexNumX = (USHORT)(nCellNumX+1);
	for ( SHORT x = (SHORT)nCellNumX; 0 <= x; --x ) {
		*pIndex++ = x;
		*pIndex++ = nVertexNumX + x;
	}
	for ( USHORT z = 1; z < (SHORT)nCellNumZ; ++z ) {
		*pIndex++ = z*nVertexNumX;
		*pIndex++ = z*nVertexNumX + nCellNumX;
		for ( SHORT x = nCellNumX; 0 <= x; --x ) {
			*pIndex++ = z*nVertexNumX + x;
			*pIndex++ = (z+1)*nVertexNumX + x;
		}
	}
	GridIB.Create(GridIndices, sizeof(uint16_t), INDEX_BUFFER, IMMUTABLE, CPU_NONE, &ind[0]);
}

void CCloud::Draw(){// Render clouds into redertargets before scene rendering  Cloud shadowmap, densitymap are rendered and then the density map is blurred.
	
	static vec3 xy(247, 3454364, 45);
	Update(My_Timer::DT);// build all the stuff that needs to goto the video card
	static float timer = 500;
	timer+=My_Timer::DT;
	Graphics::GeometryShader::UnBind();// clear the GS shader
	bool changed = false;
	unsigned int numrts=0;
	void* rts[MAX_RTS] = {0};
	Graphics::ViewPort vp;
	Graphics::GetRenderTargets(numrts, rts, vp);
	void* dv = Graphics::GetDepthTarget();
	Graphics::Texture* tex [1] = { 0 };

	if( (!CloseTo(Camera.Look, xy)) | (NeedsUpdate) | ( timer > 1.0f/30.0f) ){// this needs to be updated at least 30 times a frame because as the camera moves, the clouds are rebuilt
		changed = true;
		timer = 0;
		xy=Camera.Look;

		NeedsUpdate =false;
		// Pass 1 : Render clouds to a shadow map 
		Graphics::BlendStates::No_Blend.Bind();
		Graphics::DepthStates::NoDepthTest.Bind();
		Graphics::RasterizerStates::CullFront.Bind();// Since the cloud grid is viewed from outside, reverse cullmode.
		
		float clrColor[4] = {1.0f,1.0f, 1.0f, 1.0f };
		
		float clrColorblk[4] = {0.0f,0.0f, 0.0f, 0.0f };
		//ShadowMap.ClearRenderTargets(clrColor);
		//DensityMap.ClearRenderTargets(clrColorblk);
		
		tex[0] =  &ShadowMap;
		Graphics::BindRenderTargets(tex, 1);// no depth texture...


		// Clouds are always far away so shadowmap of clouds does not have to have depth. Only transparency is stored to the shadowmap. 
		
		VS_RenderCloudGrid.Bind();
		VS_RenderCloudGrid.SetConstantBuffer(RawBuffer);
		RawBuffer1.Update(&m_mW2SProj_Trans);
		VS_RenderCloudGrid.SetConstantBuffer(RawBuffer1, 1);
		PS_RenderCloudShadow.Bind();

		PS_RenderCloudShadow.SetResource(&CloudTex);
		PS_RenderCloudShadow.SetSampler(Graphics::Samplers::Linear);
		
		PS_RenderCloudShadow.SetConstantBuffer(RawBuffer);

		GridVB.BindAsVertexBuffer();
		GridIB.BindAsIndexBuffer();
		Graphics::SetTopology(PRIM_TRIANGLE_STRIP);
		Graphics::DrawIndexed(0, GridIndices);
	
		// Pass 2 : Render cloud density 
		tex[0] = &DensityMap;
		Graphics::BindRenderTargets(tex, 1);// no depth texture...
		
		Graphics::RasterizerStates::CullBack.Bind();
		RawBuffer1.Update(&Camera.Trans_VP);

		VS_RenderCloudGrid.SetConstantBuffer(RawBuffer1, 1);

		PS_RenderCloudGrid.Bind();
		PS_RenderCloudGrid.SetConstantBuffer(RawBuffer);

		PS_RenderCloudGrid.SetResource(&CloudTex);
		PS_RenderCloudGrid.SetSampler(Graphics::Samplers::Linear);

		Graphics::DrawIndexed(0, GridIndices);
		
		// Pass 3 : Blur the density map
		tex[0] = &BlurredMap;
		Graphics::BindRenderTargets(tex, 1);
		//BlurredMap.ClearRenderTargets(clrColor);
		//D3DX10SaveTextureToFile(Renderer.Texture[m_pDensityMap.ID].texture, D3DX10_IFF_DDS, L"m_pDensityMap.dds");
	
		VS_RenderCloudBlur.Bind();
		VS_RenderCloudBlur.SetConstantBuffer(RawBuffer);
		PS_RenderCloudBlur.Bind();
		PS_RenderCloudBlur.SetConstantBuffer(RawBuffer);
		PS_RenderCloudBlur.SetResource(&DensityMap, 0);
		PS_RenderCloudBlur.SetSampler(Graphics::Samplers::LinearClampUVW, 0);

		Graphics::SetTopology(PRIM_TRIANGLE_STRIP);
		Graphics::DrawVertices(4, 0);
		//D3DX10SaveTextureToFile(Renderer.Texture[m_pBlurredMap.ID].texture, D3DX10_IFF_DDS, L"m_pBlurredMap.dds");
		// now draw the sky
		
	}
	
	if(changed) Graphics::BindRenderTargets(numrts, rts,  dv, vp);// reset the render targets if they changed

	Graphics::BlendStates::SrcAlpha.Bind();
	Graphics::DepthStates::StencilTest.Bind(4);
	Graphics::RasterizerStates::CullBack.Bind();// Since the cloud grid is viewed from outside, reverse cullmode.

	VS_RenderCloudPlane.Bind();
	VS_RenderCloudPlane.SetConstantBuffer(RawBuffer);
	PS_RenderCloudPlane.Bind();
	PS_RenderCloudPlane.SetConstantBuffer(RawBuffer);
	PS_RenderCloudPlane.SetResource(&DensityMap, 0);
	PS_RenderCloudPlane.SetResource(&BlurredMap, 1);

	PS_RenderCloudPlane.SetSampler(Graphics::Samplers::LinearClampUVW, 0);
	Graphics::SetTopology(PRIM_TRIANGLE_STRIP);
	Graphics::DrawVertices(4, 0);

}

void CCloud::Update(float dt){// Update cloud position.  The cloud is animated by scrolling uv 

	// look at the scene from light
	//cout<<"Starting loop"<<endl;
	vec3 vLight( 0.0f, 0.0f, 0.0f );
	vec3 vUp( 0.0f, 1.0f, 0.0f );
	mat4 mW2Light;
	vec3 tempsun = SunDir;
	mW2Light.setupLookAt(  vLight, -tempsun, vUp);
	//OUTPUT_DEBUG_MSG("Sundir"<<-tempsun);
	// transform ground and cloud bounding box to the light coordinate
	cAABB bbGroundInView, bbCloudInView, bv;
	bbGroundInView.Min = vec3(-Camera.FarPlane, -2550, -Camera.FarPlane) + Camera.Position;
	bbGroundInView.Max = vec3(Camera.FarPlane, 2550, Camera.FarPlane)+ Camera.Position;


	m_bound.Transform(bbCloudInView, mW2Light);
	bbGroundInView.Transform(bv, mW2Light);
	//OUTPUT_DEBUG_MSG("bbCloudInView bound"<<bbCloudInView.Max<<"bbCloudInView bound"<<bbCloudInView.Min);
	//OUTPUT_DEBUG_MSG("bbGroundInView bound"<<bbGroundInView.Max<<"bbGroundInView bound"<<bbGroundInView.Min);

	// minimize bounding box The view frustom should be take into account as well.
	cAABB bound;
	bound.Max = Min(bv.Max, bbCloudInView.Max);
	bound.Min = Max(bv.Min, bbCloudInView.Min);
	bound.Min.z = bbCloudInView.Min.z;
	//bound.Max = bv.Max;
	//bound.Min = bv.Min;
	//OUTPUT_DEBUG_MSG("max bound"<<bound.Max<<"min bound"<<bound.Min);

	// if there is a valid volume 
	if (bound.Min.x < bound.Max.x && bound.Min.y < bound.Max.y && bound.Min.z < bound.Max.z) {
	
		vec3 vCenter;
		bound.Centroid(&vCenter);
		vec3 vDiag = bound.Max - bound.Min;
		// Move the view position to the center of the bounding box. z is located behined the volume.
		vec3 vEye(vCenter);
		vEye.z = vCenter.z - 0.5f * vDiag.z;
		vec3 vMove = vLight - vEye;
		mat4 mTrans;
		mTrans.setupTranslation(vMove);
		//OUTPUT_DEBUG_MSG("diag"<<vDiag<<"move"<<vMove);
		mat4 mProj;// Orthogonal projection matrix

		mProj.setupOrtho(vDiag.x, vDiag.y, 0.0f, vDiag.z );//check x or y neg

		m_mW2SProj_Trans = m_mW2SProj=(mW2Light* mTrans) * mProj;// Compute world to shadow map projection matrix
		m_mW2SProj_Trans.Transpose();
		mat4 mProj2Tex(// Compute world to shadowmap texture coordinate matrix
			0.5f,  0.0f, 0.0f, 0.0f,
			0.0f, -0.5f, 0.0f, 0.0f,
			0.0f,  0.0f, 1.0f, 0.0f,
			0.5f,  0.5f, 0.0f, 1.0f );
		m_mW2S = m_mW2SProj*mProj2Tex;
	} 
	vec3 te = SunDir;// dont modify sundir
	if(te.y<0.0f){
		te.y=0;
		
	}
	te.normalize();
	vec4 vProjPos= vec4(te, 0);
	
	Transform( vProjPos, vProjPos, Camera.VP );

	vec4 vBlurDir;
	if ( ( fabsf(vProjPos.w) < EPSILON )|| ( fabsf(vProjPos.z) < EPSILON ) ) {// if dot( litdir, ray ) == 0.0f : directional. light is stil directional in projection space.
		vProjPos.w = vProjPos.z = 0.0f;
		vProjPos.normalize();
		vProjPos.y = -vProjPos.y;
		vBlurDir = vec4( 0.0f, 0.0f, -vProjPos.x, -vProjPos.y );	// directional blur	

	} else { // otherwise : point blur. light direction is a position in projection space.
		if ( 0.0f < vProjPos.w ) {
			vProjPos*=1.0f/vProjPos.w;
			vProjPos.x =  0.5f * vProjPos.x + 0.5f; //  
			vProjPos.y = -0.5f * vProjPos.y + 0.5f; // upside down

			vBlurDir = vec4( 1.0f, 1.0f, -vProjPos.x, -vProjPos.y );
		} else { // invert  if light comes from behind the camera.
			vProjPos*=1.0f/vProjPos.w;
			vProjPos.x =  0.5f * vProjPos.x + 0.5f; //  
			vProjPos.y = -0.5f * vProjPos.y + 0.5f; // upside down
			vBlurDir = vec4( -1.0f, -1.0f, vProjPos.x, vProjPos.y );

		 }
	}
	
	// increment uv scrolling parameters 
	vec2 vec = m_vVelocity * dt;
	m_vOffset +=vec;

	// Adjust the height so that clouds are always above. cloud height = m_fDefaultHeight + m_fFallOffHeight * squareLengthance_in_horizontal
	float fRange = Camera.FarPlane*.5f;
	float fHeight = fRange*.12f;
	m_fDefaultHeight = fHeight + Camera.Position.y;
	m_fFallOffHeight  = - ( 0.1f / fRange ) * (  Camera.Position.y / fHeight + 1.0f );
	// Update Bounding Box
	m_bound.Max.y = m_fDefaultHeight;
	// look at the scene from light
	
	// vRayleigh : rgb : 3/(16*PI) * Br           w : -2*g
	params.vOff= vBlurDir;
	//cout<<vBlurDir;

	params.vUVParam=vec4( CloudsScale.x, CloudsScale.y, m_vOffset.x, m_vOffset.y );
	params.vXZParam=vec4( m_vCellSizeXZ.x, m_vCellSizeXZ.y, m_vStartXZ.x, m_vStartXZ.y );
	//cout<<"param.vUVParam"<<param.vUVParam<<"param.vXZParam"<<param.vXZParam;
	params.vHeightPix.x=m_fFallOffHeight;
	params.vHeightPix.y=  m_fDefaultHeight;
	params.vHeightPix.z= 1.f / static_cast<float>(DensityMap.Width);
	params.vHeightPix.w= 1.f / static_cast<float>(DensityMap.Height);
	params.vRayleigh= m_vRayleigh*3.0f/(16.0f*Pi);
	params.vRayleigh.w = -2.0f * m_fG;
	// vMie : rgb : 1/(4*PI) * Bm * (1-g)^2  w : (1+g^2)
	float       fG = 1.0f -m_fG;
	params.vMie = m_vMie*fG*fG/(4.0f*Pi);
	params.vMie.w = 1.0f + m_fG * m_fG;
	vec3 vSum = m_vRayleigh + m_vMie;
	// vESun : rgb : Esun/(Br+Bm)             w : R
	params.vESun.x = m_fLightScale * m_vLightColor.x/vSum.x;
	params.vESun.y = m_fLightScale * m_vLightColor.y/vSum.y;
	params.vESun.z = m_fLightScale * m_vLightColor.z/vSum.z;
	params.vESun.w = m_fEarthRadius;
	// vSum  : rgb : (Br+Bm)                  w : h(2R+h)
	// scale by inverse of farclip to apply constant scattering effect in case farclip is changed.
	params.vSum = vSum * 1.0f/Camera.FarPlane;
	params.vSum.w = m_fAtomosHeight * (2.0f*m_fEarthRadius + m_fAtomosHeight);
	// ambient term of scattering
	params.vAmbient = m_vAmbientLight *m_fAmbientScale;
	params.vAmbient.w = 1.0f/sqrtf( params.vSum.w );
	float fCloudCover = (m_fCloudCover - 0.7f) / (1.0f - 0.7f);
	fCloudCover = max( 0.0f, fCloudCover );
	// mie scattering is caused by vapor.
	float fMie = 0.05f * (1.0f - fCloudCover) + 1.5f * fCloudCover;
	m_vMie = vec3( fMie, fMie, fMie );
	// rayleigh scattering 
	float fRayleigh =  0.9f*fCloudCover + 1.0f*(1.0f-fCloudCover);
	vec3 vFineRayleigh( 0.05f, 0.15f, 1.5f );
	m_vRayleigh = vFineRayleigh * fRayleigh;
	vec3 vFineAmbient( 0.3f, 0.35f, 0.4f ), vCloudyAmbient( 0.35f, 0.35f, 0.35f );
	m_vAmbientLight = Lerp(vFineAmbient, vCloudyAmbient, fCloudCover);
	// when cloudy, ambient term of scattering is risen
	m_fAmbientScale = 0.5f * (1.0f - fCloudCover) + 1.0f * fCloudCover;
	params.vParam = vec4( params.vESun.w, params.vSum.w, m_fAtomosHeight, m_fCloudCover);
	//params.SunDir = vec4(-*SunDir);
	params.CameraPosition = vec4(Camera.Position, 0);
	params.InvCameraViewProjection = Camera.Trans_InvVP;
	RawBuffer.Update(&params);
}


