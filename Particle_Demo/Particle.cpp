#include "stdafx.h"
#include "Particle.h"
#include "../Utilities/MyMath.h"

#define MAX_PARTICLES 30000
struct cbuffer{
	mat4 g_mWorldViewProj;
	mat4 g_invview;
    vec4 g_fGlobalTime;
    vec4 g_fElapsedTime;
	vec4 g_vFrameGravity;
};
struct Particle_Vertex
{
    vec3 pos;
    vec3 vel;
    float Timer;
    float Type;
};

void Particle_Group::Init(){
	First_Run=false;
	ReloadShaders();
	Particle_Vertex v;
	v.pos = vec3(0, 0, 0);
	v.vel = vec3(0, 40, 0);
	v.Timer=0;
	v.Type=0;

	Graphics::Buffer particle_start;
	particle_start.Create(1, sizeof(Particle_Vertex), VERTEX_BUFFER, DEFAULT, CPU_NONE, &v);
	Particle_In.Create(MAX_PARTICLES, sizeof(Particle_Vertex), STREAM_BUFFER_OR_VERTEX_BUFFER);
	Particle_Out.Create(MAX_PARTICLES, sizeof(Particle_Vertex), STREAM_BUFFER_OR_VERTEX_BUFFER);
	CBuffer.Create(1, sizeof(cbuffer), CONSTANT_BUFFER);


    int iNumRandValues = 1024;
    srand( timeGetTime() );
	std::vector<float> ptr;
	ptr.resize(iNumRandValues * 4);
    for( int i = 0; i < iNumRandValues * 4; i++ )
    {
        ptr[i] = float( ( rand() % 10000 ) - 5000 );
    }
	Random_Tex.Create(&ptr[0], iNumRandValues, 1, 1, DEFAULT, CPU_NONE, NO_FLAG, FORMAT_RGBA32F);
	Particle_Tex.Create("particle.dds");
	particle_start.BindAsVertexBuffer();
	_Update(0.0f);
	Graphics::DrawVertices(1, 0);
	Graphics::Buffer::UnBindSOBuffer();
	Graphics::Buffer::UnBindVertexBuffer();
	std::swap(From, To);
}
void Particle_Group::DeInit(){
	First_Run=false;
	Particle_In.Destroy();
	Particle_Out.Destroy();
	VS_Passthrough.Destroy();
	VS_Main.Destroy();
	GS_StreamOut.Destroy();
	GS_Main.Destroy();
	PS_Main.Destroy();
	CBuffer.Destroy();
}
void Particle_Group::ReloadShaders(){
	
	VS_Main.CompileShaderFromFile("Fireworks_Particle.fx","VSScenemain");

	FormatDesc fd[4] ={
		FormatDesc(),
		FormatDesc(TYPE_NORMAL),
		FormatDesc(TYPE_WEIGHT, FORMAT_FLOAT, 1),// i used a weight here as the type just because I dont need 50 different names for the same purpose
		FormatDesc(TYPE_WEIGHT, FORMAT_FLOAT, 1),// i used a weight here as the type just because I dont need 50 different names for the same purpose
	};
	VS_Main.CreateInputLayout(fd, 4);
	VS_Passthrough.CompileShaderFromFile("Fireworks_Particle.fx", "VSPassThroughmain");
	VS_Passthrough.CreateInputLayout(fd, 4);
	FormatDesc_SO fdso[4] = {
		FormatDesc_SO(0, "POSITION", 0, 0, 3, 0),
		FormatDesc_SO(0, "NORMAL", 0, 0, 3, 0),
		FormatDesc_SO(0, "BLENDWEIGHT", 0, 0, 1, 0),
		FormatDesc_SO(0, "BLENDWEIGHT", 1, 0, 1, 0),
	};
	
	GS_StreamOut.CompileShaderFromFile("Fireworks_Particle.fx", fdso, 4,  "GSAdvanceParticlesMain");
	GS_Main.CompileShaderFromFile("Fireworks_Particle.fx","GSScenemain");
	PS_Main.CompileShaderFromFile("Fireworks_Particle.fx","PSScenemain");
}
void Particle_Group::_Update(float dt){	
	Graphics::VertexShader::UnBind();
	Graphics::GeometryShader::UnBind();

	VS_Passthrough.Bind();
	GS_StreamOut.Bind();

	Graphics::RasterizerStates::CullNone.Bind();
	Graphics::DepthStates::NoDepthTest.Bind();
	Graphics::BlendStates::No_Blend.Bind();
	Graphics::SetTopology(PRIM_POINTLIST);
	To->BindAsSOBuffer();
	GS_StreamOut.SetResource(&Random_Tex,0);
	GS_StreamOut.SetSampler(Graphics::Samplers::Linear);
	cbuffer cb;
	static float totaldt = 0.0f;
	totaldt+=dt;

	cb.g_fElapsedTime = vec4(dt,0,0,0);
	cb.g_fGlobalTime = vec4(totaldt,0,0,0);
	cb.g_vFrameGravity = vec4(0, -9.8f, 0, 0) * dt;
	CBuffer.Update(&cb);
	GS_StreamOut.SetConstantBuffer(CBuffer);
}
void Particle_Group::Update(float dt){
	From->BindAsVertexBuffer();
	_Update(dt);
	Graphics::DrawAuto();
	Graphics::Buffer::UnBindSOBuffer();
	Graphics::Buffer::UnBindVertexBuffer();
	std::swap(From, To);
}

void Particle_Group::Draw(mat4& view, mat4& proj, float dt){

	Graphics::HullShader::UnBind();
	Graphics::DomainShader::UnBind();
	VS_Main.Bind();
	GS_Main.Bind();
	PS_Main.Bind();
	Graphics::BlendStates::SrcAlpha.Bind();

	cbuffer cb;
	static float totaldt = 0.0f;
	totaldt+=dt;
	PS_Main.SetResource(&Particle_Tex);
	PS_Main.SetSampler(Graphics::Samplers::LinearClampUVW);
	cb.g_fElapsedTime = vec4(dt,0,0,0);
	cb.g_fGlobalTime = vec4(totaldt,0,0,0);
	cb.g_vFrameGravity = vec4(0, -9.8f, 0, 0) *dt;
	cb.g_invview = view;
	cb.g_invview.inverse();
	cb.g_invview.Transpose();
	cb.g_mWorldViewProj = view*proj;
	cb.g_mWorldViewProj.Transpose();
	CBuffer.Update(&cb);
	GS_Main.SetConstantBuffer(CBuffer);

	Graphics::SetTopology(PRIM_POINTLIST);
	From->BindAsVertexBuffer();
	Graphics::DrawAuto();
	Graphics::Buffer::UnBindVertexBuffer();
}
