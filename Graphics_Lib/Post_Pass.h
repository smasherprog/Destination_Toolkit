#ifndef POST_PASS_H
#define POST_PASS_H
#include "GraphicsCore.h"
#include "Shaders.h"
#include "Global_GameData.h"

struct Buffer0{		
	mat4 ShadowMapSplits[4], ShadowMapSplitsTex[4];
	vec4 SplitPositions;
	mat4 InvCameraViewProjectionFromScreen, CameraViewProjection;
	vec4 SunNormal, SunPosition, CameraPosition;
	mat4 CloudsToTex;
	vec4 ShadowSettings;

};

class Post_Pass{
public:

	Post_Pass() {
		
	}
	~Post_Pass() {DeInit();}
	void Init(){
		Shader=0;
		PS_StandardDay.CompileShaderFromFile("../PostPass.fx", "PSDay");
		//PS_StandardNight.CompileShaderFromFile("../PostPass.fx", "PSNight");
		//PS_Depth.CompileShaderFromMemory(Graphics::Shader_Defines::Debug_DepthPS, "DepthTech");
		//PS_Diffuse.CompileShaderFromMemory(Graphics::Shader_Defines::Debug_DiffusePS,  "DiffuseTech");
		//PS_Specular.CompileShaderFromMemory(Graphics::Shader_Defines::Debug_SpecularPS,  "SpecularTech");
		//PS_Stencil.CompileShaderFromMemory(Graphics::Shader_Defines::Debug_StencilPS,  "StencilTech");
		//PS_Normals.CompileShaderFromMemory(Graphics::Shader_Defines::Debug_NormalsPS, "NormalsTech");
		//PS_Shadow.CompileShaderFromMemory(Graphics::Shader_Defines::Debug_ShadowPS,  "ShadowTech");

		PS_ConstBuffer.Create(1, sizeof(Buffer0), CONSTANT_BUFFER);
	}
	void DeInit(){
		PS_Depth.Destroy();
		PS_Specular.Destroy(); 
		PS_Diffuse.Destroy(); 
		PS_Stencil.Destroy(); 
		PS_Normals.Destroy(); 
		PS_Shadow.Destroy(); 
		PS_StandardDay.Destroy(); 
		PS_StandardNight.Destroy();
	}
	void Draw(){
		
		Graphics::Texture* tex[1] = { &Graphics::Textures::RT_Base };
		Graphics::BindRenderTargets(tex, 1, &Graphics::Textures::RT_DepthStencil);

		Graphics::BlendStates::No_Blend.Bind();
		Graphics::DepthStates::NoDepthTest.Bind();
		Graphics::RasterizerStates::CullNone.Bind();

		Graphics::Shaders::VS_FullScreenQuad.Bind();
		Graphics::GeometryShader::UnBind();// just in case
		PS_StandardDay.Bind();
		PS_StandardDay.SetResource(&Graphics::Textures::RT_BackBufferTexture, 0);
		PS_StandardDay.SetResource(&Graphics::Textures::RT_Normal, 1);
		PS_StandardDay.SetResource(&Graphics::Textures::RT_Depth, 2);
		PS_StandardDay.SetSampler(Graphics::Samplers::Nearest, 0);

		Buffer0 b;
		b.CameraPosition = vec4(Camera.Position, 0);
		b.CameraViewProjection = Camera.Trans_VP;
		b.InvCameraViewProjectionFromScreen = Camera.Trans_InvVPToScreen;
		b.SunNormal = SunDir;
		PS_ConstBuffer.Update(&b);
		PS_StandardDay.SetConstantBuffer(PS_ConstBuffer);
		
		Graphics::SetTopology(PRIM_TRIANGLE_STRIP);
		Graphics::DrawVertices(4, 0);

		/*
		Renderer.ChangeRenderTargets(&Renderer.BaseRT, 1, Renderer.DepthStencilRT);
		Renderer.SetBlendState(BS_NONE);
		Renderer.SetRasterizerState(Renderer.RSCullNone);
		if(DebugTech ==StencilTech) 
			Renderer.SetDepthState(Renderer.StencilTest, 2);
		else 
			Renderer.SetDepthState(Renderer.NoDepthTest);

		Renderer.SetTextureSlot0(Renderer.BackBufferTexture);// color rt
		Renderer.SetTextureSlot1(Renderer.NormalRT);// normal 
		Renderer.SetTextureSlot2(Renderer.DepthRT);// depth.. duhh

		Renderer.SetTextureArraySlot0(GlobalLight.ShadowMapArray);

		Renderer.SetTopology(PRIM_TRIANGLE_STRIP);
		Renderer.Apply();
		Renderer.DrawVertices(4, 0);

		//} else // do a normal draw

		//GlobalLight.ApplyShadowMaps(Clouds.ShadowMap, Clouds.m_mW2S);
		*/
	}

	unsigned int Shader;
	Graphics::PixelShader  PS_Depth, PS_Specular, PS_Diffuse, PS_Stencil, PS_Normals, PS_Shadow, PS_StandardDay, PS_StandardNight;
	Graphics::Buffer PS_ConstBuffer;

};

#endif