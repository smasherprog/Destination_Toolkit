#include "stdafx.h"
#include "FXAA.h"
#include "Shaders.h"
#include <sstream>

FXAABlender::FXAABlender() {	
	Init();
}
FXAABlender::~FXAABlender() {	
	DeInit();
}
void FXAABlender::Init(){
	Blending =true;
	QualitySubpix = .75f;
	QualityEdgeThreshold = .166f;
	QualityEdgeThresholdMin = .0833f;
	QualityPreset= 23;
	onresizeid = Graphics::RegisterForOnResize(std::bind(&FXAABlender::ReCompile, this, std::placeholders::_1, std::placeholders::_2));
	Compile();
}
void FXAABlender::DeInit(){
	Graphics::UnRegisterForOnResize(onresizeid);
	PS.Destroy();
	PS_ConstBuffer.Destroy();
}
void FXAABlender::Compile(){
	unsigned int x, y;
	Graphics::GetBackBufferSize(x, y);
	ReCompile(x, y);
}
void FXAABlender::ReCompile(unsigned int width, unsigned int height){

	// convert the numbers to a string to pass to the shader
	std::ostringstream co;
	co<<QualitySubpix;
	std::string str0 = co.str();
	char* ptr0 = new char[str0.size() +1];// the extra 1 is for the null terminator
	memcpy(ptr0, str0.c_str(), str0.size() +1);
	co.str("");// clear the stream
	co<<QualityEdgeThreshold;
	str0 = co.str();
	char* ptr1 = new char[str0.size() +1];// the extra 1 is for the null terminator
	memcpy(ptr1, str0.c_str(), str0.size() +1);
	co.str("");// clear the stream
	co<<QualityEdgeThresholdMin;
	str0 = co.str();
	char* ptr2 = new char[str0.size() +1];// the extra 1 is for the null terminator
	memcpy(ptr2, str0.c_str(), str0.size() +1);
	co.str("");// clear the stream
	co<<(1.0f/static_cast<float>(width));
	str0 = co.str();
	char* ptr3 = new char[str0.size() +1];// the extra 1 is for the null terminator
	memcpy(ptr3, str0.c_str(), str0.size() +1);
	co.str("");// clear the stream
	co<<(1.0f/static_cast<float>(height));
	str0 = co.str();
	char* ptr4 = new char[str0.size() +1];// the extra 1 is for the null terminator
	memcpy(ptr4, str0.c_str(), str0.size() +1);
	
	co.str("");// clear the stream
	co<<QualityPreset;
	str0 = co.str();
	char* ptr5 = new char[str0.size() +1];// the extra 1 is for the null terminator
	memcpy(ptr5, str0.c_str(), str0.size() +1);


	Graphics::Shader_Macro macro[] = {
		{"FXAAQUALITYSUBPIX", ptr0 }, 
		{"FXAAQUALITYEDGETHRESHOLD", ptr1}, 
		{"FXAAQUALITYEDGETHRESHOLDMIN", ptr2 },
		{"FXAAQUALITYRCPFRAME_X", ptr3 },
		{"FXAAQUALITYRCPFRAME_Y", ptr4 },
		{"FXAA_QUALITY__PRESET", ptr5 },	
		{"FXAA_HLSL_4", "1" },
		{"FXAA_HLSL_5", "0" },
		{NULL, NULL}
	};

	PS.CompileShaderFromFile("FXAA.fx", "PS", "ps_4_0", macro);// make sure to force a re-compile
	delete [] ptr0;
	delete [] ptr1;
	delete [] ptr2;
	delete [] ptr3;
	delete [] ptr4;
	delete [] ptr5;

}
void FXAABlender::Blend(Graphics::Texture& in, Graphics::Texture& out){
	Graphics::Texture* tex[] = {&out};
	if(!Blending){
		Graphics::Internal::DeviceContext->CopyResource( out.Texture_, in.Texture_);// just copy .. no blending
		Graphics::BindRenderTargets(tex, 1);
		return;
	}

	Graphics::Shaders::VS_FullScreenQuad.Bind();
	Graphics::GeometryShader::UnBind();
	Graphics::SetTopology(PRIM_TRIANGLE_STRIP);
	PS.Bind();

	Graphics::BindRenderTargets(tex, 1);
	Graphics::DepthStates::NoDepthTest.Bind();
	Graphics::RasterizerStates::CullNone.Bind();
	Graphics::BlendStates::No_Blend.Bind();
	PS.SetSampler(Graphics::Samplers::AnisotropicClampUVW);
	PS.SetResource(&in);
	Graphics::DrawVertices(4, 0);
}


