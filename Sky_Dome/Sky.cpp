#include "stdafx.h"
#include "Sky.h"
#include "../Utilities/MyMath.h"

#define DomeN 64
#define OpticalDepthN 256
#define RayleighMieN 256
#define Latitude DomeN/2
#define Longitude DomeN
#define DVSize (Longitude * Latitude)*2
#define DISize ((Longitude - 1) * (Latitude - 1) * 2)*2*3

void Sky::Init(vec3* sundir, const float* farplane){
	DeInit();
	SunDir = sundir;
	FarPlane=farplane;

	FormatDesc lay[2];
	lay[0].Size =3;
	lay[0].Type = TYPE_VERTEX;
	lay[1].Size=2;// most common
	lay[1].Type = TYPE_TEXCOORD;// most common
	
	VS_SkyScatter.CompileShaderFromFile("Skyscat.fx");
	VS_SkyScatter.CreateInputLayout(lay, 2);
	//VS_SkyCubeMap.CompileShaderFromFile("../Skyscat.fx", "VS_CUBEMAP");
	//VS_SkyCubeMap.CreateInputLayout(lay, 2);

	GS_SkyCubeMap.CompileShaderFromFile("Skyscat.fx");
	
	PS_SkyScatter.CompileShaderFromFile("Skyscat.fx", "PS");
	PS_Update.CompileShaderFromFile("Skyscat.fx", "PS_UPDATE_FAST");
	PS_Create_OpticalDepth.CompileShaderFromFile("Skyscat.fx", "PS_CREATE_OPTICALDEPTH");

	CBuffer0.Create(1, sizeof(mat4) + sizeof(vec4), CONSTANT_BUFFER); 
	CBuffer1.Create(1, sizeof(mat4)*6, CONSTANT_BUFFER); 
	txMie.CreateRenderTarget(RayleighMieN, RayleighMieN / 2, 1, 1, 1, FORMAT_RGBA16F);
	txRayleigh.CreateRenderTarget(RayleighMieN, RayleighMieN / 2, 1, 1, 1, FORMAT_RGBA16F);
	txOpticalDepth.CreateRenderTarget(OpticalDepthN, OpticalDepthN, 1, 1, 1, FORMAT_RGBA16F);
	//SkyCubeMap.CreateRenderTarget(OpticalDepthN, OpticalDepthN, 1, 1, 6, FORMAT_RGB10A2, 1, 0, CUBEMAP);

	struct tempdome{ vec3 v; vec2 uv;};
	std::vector<tempdome> pVertices(DVSize);
	UINT DomeIndex = 0;
	for( uint32_t i = 0; i < Longitude; i++ ){
		const float MoveXZ = 100.0f * ( i / static_cast<float>(Longitude - 1) ) * Pi / 180.0f;
		for( uint32_t   j = 0; j < Latitude; j++ ){	
			const float MoveY = Pi * j / (Latitude - 1) ;
			pVertices[DomeIndex].v.x = sinf( MoveXZ ) * cosf( MoveY  );
			pVertices[DomeIndex].v.y = cosf( MoveXZ );
			pVertices[DomeIndex].v.z = sinf( MoveXZ ) * sinf( MoveY  );
			pVertices[DomeIndex].v *= 10.0f;
			pVertices[DomeIndex].uv.x = 0.5f / float( Longitude ) + i / float( Longitude );	// [0.5, Texsize-0.5] 
			pVertices[DomeIndex].uv.y = 0.5f / float( Latitude ) + j / float( Latitude );	// [0.5, Texsize-0.5]
			DomeIndex++;
		}
	}

	for( uint32_t  i = 0; i < Longitude; i++ ){
		const float MoveXZ = 100.0f * ( i / float(Longitude - 1) ) * Pi / 180.0f;
		for( uint32_t  j = 0; j < Latitude; j++ ){	
			const float MoveY = (Pi * 2.0f) - (Pi * j / (Latitude - 1)) ;
			pVertices[DomeIndex].v.x = sinf( MoveXZ ) * cosf( MoveY  );
			pVertices[DomeIndex].v.y = cosf( MoveXZ );
			pVertices[DomeIndex].v.z = sinf( MoveXZ ) * sinf( MoveY  );
			pVertices[DomeIndex].v *= 10.0f;
			pVertices[DomeIndex].uv.x = 0.5f / float( Longitude ) + i / float( Longitude );	// [0.5, Texsize-0.5] 
			pVertices[DomeIndex].uv.y = 0.5f / float( Latitude ) + j / float( Latitude );	// [0.5, Texsize-0.5]
			DomeIndex++;
		}
	}
	DomeVertexBuffer.Create(DVSize, sizeof(tempdome), VERTEX_BUFFER, IMMUTABLE, CPU_NONE, &pVertices[0]);
	
	std::vector<unsigned short> pIndices(DISize);
	DomeIndex=0;

	for( unsigned short i = 0; i < Longitude - 1; i++){
		for( unsigned short j = 0; j < Latitude - 1; j++){
			pIndices[DomeIndex++] = i * Latitude + j;
			pIndices[DomeIndex++]  = (i + 1) * Latitude + j;
			pIndices[DomeIndex++]  = (i + 1) * Latitude + j + 1;

			pIndices[DomeIndex++]  = (i + 1) * Latitude + j + 1;
			pIndices[DomeIndex++] = i * Latitude + j + 1;
			pIndices[DomeIndex++]  = i * Latitude + j;
		}
	}
	const UINT Offset = Latitude * Longitude;
	for( unsigned short i = 0; i < Longitude - 1; i++){
		for( unsigned short j = 0; j < Latitude - 1; j++){
			pIndices[DomeIndex++]  = Offset + i * Latitude + j;
			pIndices[DomeIndex++] = Offset + (i + 1) * Latitude + j + 1;
			pIndices[DomeIndex++] = Offset + (i + 1) * Latitude + j;

			pIndices[DomeIndex++] = Offset + i * Latitude + j + 1;
			pIndices[DomeIndex++] = Offset + (i + 1) * Latitude + j + 1;
			pIndices[DomeIndex++]  = Offset + i * Latitude + j;
		}
	}

	DomeIndexBuffer.Create(DISize, sizeof(unsigned short), INDEX_BUFFER, IMMUTABLE, CPU_NONE, &pIndices[0]);
	// create the optical depth
	Graphics::BlendStates::No_Blend.Bind();
	Graphics::RasterizerStates::CullNone.Bind();
	Graphics::DepthStates::NoDepthTest.Bind();

	Graphics::Shaders::VS_FullScreenQuad.Bind();
	PS_Create_OpticalDepth.Bind();
	Graphics::Texture* tex[1] = { &txOpticalDepth };
	Graphics::BindRenderTargets(tex, 1);/// Set new RT
	Graphics::SetTopology(PRIM_TRIANGLE_STRIP);
	Graphics::DrawVertices( 4, 0 );
	tex[0] = &Graphics::Textures::RT_BackBufferTexture;
	Graphics::BindRenderTargets(tex, 1, &Graphics::Textures::RT_DepthStencil);
	/*
	mat4 ViewCMProj[6];
	////work on creating the cubemap
	vec3 vEyePt = vec3( 0, 0, 0);
	vec3 vLookDir;
	vec3 vUpDir;

	vLookDir = vec3( 1.0f, 0.0f, 0.0f );
	vUpDir   = vec3( 0.0f, 1.0f, 0.0f );
	ViewCMProj[0].setupLookAt(vEyePt, vLookDir, vUpDir );
	vLookDir = vec3(-1.0f, 0.0f, 0.0f );
	vUpDir   = vec3(0.0f, 1.0f, 0.0f );
	ViewCMProj[1].setupLookAt(vEyePt, vLookDir, vUpDir );
	vLookDir = vec3( 0.0f, 1.0f, 0.0f );
	vUpDir   = vec3( 0.0f, 0.0f, -1.0f );
	ViewCMProj[2].setupLookAt(vEyePt, vLookDir, vUpDir );
	vLookDir = vec3( 0.0f, -1.0f, 0.0f );
	vUpDir   = vec3( 0.0f, 0.0f, 1.0f );
	ViewCMProj[3].setupLookAt(vEyePt, vLookDir, vUpDir );
	vLookDir = vec3( 0.0f, 0.0f, 1.0f );
	vUpDir   = vec3( 0.0f, 1.0f, 0.0f );
	ViewCMProj[4].setupLookAt(vEyePt, vLookDir, vUpDir );
	vLookDir = vec3( 0.0f, 0.0f, -1.0f );
	vUpDir   = vec3( 0.0f, 1.0f, 0.0f );
	ViewCMProj[5].setupLookAt(vEyePt, vLookDir, vUpDir );

	mat4 Proj;
	Proj.setupProject( Pi * 0.5f, 1.0f, 1.0f, 100.0f );
	for( int i = 0; i < 6; i++ ) ViewCMProj[i] *= Proj;
	CBuffer1.Update(&ViewCMProj);// this only needs to be done once...
	*/
}
void Sky::DeInit(){
	DomeVertexBuffer.Destroy();

	DomeIndexBuffer.Destroy();
	VS_SkyScatter.Destroy();
	VS_SkyCubeMap.Destroy();
	GS_SkyCubeMap.Destroy();
	PS_SkyScatter.Destroy();
	PS_Update.Destroy();
	PS_Create_OpticalDepth.Destroy();
	txMie.Destroy(); 
	txRayleigh.Destroy();
	txOpticalDepth.Destroy();
	SkyCubeMap.Destroy();
	CBuffer0.Destroy();
	CBuffer1.Destroy();
}
void Sky::Draw(const mat4& view, const mat4& proj){

	static vec3 lastsundir = vec3(56, 56, 56);// seed this so the first run succeeds

	mat4 modview(view);
	modview._41 = modview._42 = modview._43 = 0.0f;
	mat4 scale;
	float farscale = *FarPlane/11.0f;// this scales the dome to be on the farplane
	scale.setupScale(farscale, farscale, farscale);
	mat4 MP =  scale*  modview * proj;

	struct tempstruct {mat4 obj; vec4 sun;};
	tempstruct t;
	t.obj = MP;
	t.obj.Transpose();
	t.sun = vec4(*SunDir, 0.0f);


	CBuffer0.Update(&t);
	Graphics::BlendStates::No_Blend.Bind();
	bool changed = false;
	unsigned int numrts=0;
	void* rts[MAX_RTS] = {0};
	Graphics::ViewPort vp;
	Graphics::GetRenderTargets(numrts, rts, vp);
	void* dv = Graphics::GetDepthTarget();
	if(!CloseTo(lastsundir, *SunDir)){// only update if the sun has changed
		changed=true;
		lastsundir =*SunDir;
		Graphics::Shaders::VS_FullScreenQuad.Bind();
		PS_Update.Bind();
		Graphics::RasterizerStates::CullNone.Bind();
		Graphics::DepthStates::NoDepthTest.Bind();
		PS_Update.SetResource(&txOpticalDepth);
		PS_Update.SetSampler(Graphics::Samplers::LinearClampUVW);
		PS_Update.SetConstantBuffer(CBuffer0);
		Graphics::Texture* mrt[2] = { &txRayleigh, &txMie };// Set new RT
		Graphics::BindRenderTargets(mrt, 2); // this will set the depth texture to none as well
		Graphics::SetTopology(PRIM_TRIANGLE_STRIP);
		Graphics::DrawVertices( 4, 0 );

		//D3DX10SaveTextureToFile( Renderer.Texture[txMie.ID].texture , D3DX10_IFF_DDS, L"Mie.dds" );
		//D3DX10SaveTextureToFile(Renderer.Texture[txRayleigh.ID].texture, D3DX10_IFF_DDS, L"Rayleigh.dds");
	}
	if(changed) Graphics::BindRenderTargets(numrts, rts,  dv, vp);// reset the render targets if they changed
	Graphics::RasterizerStates::CullBack.Bind();
	Graphics::DepthStates::StencilTest.Bind(0);
	
	VS_SkyScatter.Bind();
	Graphics::GeometryShader::UnBind();// clear the GS shader
	Graphics::DomainShader::UnBind();// clear the DS shader
	Graphics::HullShader::UnBind();// clear the HS shader
	PS_SkyScatter.Bind();

	VS_SkyScatter.SetConstantBuffer(CBuffer0);
	PS_SkyScatter.SetConstantBuffer(CBuffer0);

	PS_SkyScatter.SetSampler(Graphics::Samplers::LinearClampUVW);
	PS_SkyScatter.SetResource(&txMie, 0);
	PS_SkyScatter.SetResource(&txRayleigh, 1);

	DomeVertexBuffer.BindAsVertexBuffer();
	DomeIndexBuffer.BindAsIndexBuffer();
	Graphics::SetTopology(PRIM_TRIANGLELIST);

	Graphics::DrawIndexed(0, DISize);
	//D3DX10SaveTextureToFile(Renderer.Texture[Renderer.BaseRT.ID].texture, D3DX10_IFF_DDS, L"base.dds");

}	
