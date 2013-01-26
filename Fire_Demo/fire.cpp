#include "stdafx.h"
#include "fire.h"
#include "../Utilities/MyMath.h"
#include "../Graphics_Lib/MeshTypes.h"

#include <random>

struct MatrixBufferType
{
	mat4 world;
	mat4 view;
	mat4 projection;
	vec2 texoffset_x;
	float padd[2];
};

struct NoiseBufferType
{
	float frameTime;
	vec3 scrollSpeeds;
	vec3 scales;
	float pad;
};

struct DistortionBufferType
{
	vec2 distortion1;
	vec2 distortion2;
	vec2 distortion3;
	float distortionScale;
	float distortionBias;
};

void Fire::Init(vec3* campos, uint32_t grid_size){
	Grid_Size = grid_size;
	Cam_Pos=campos;
	ReloadShaders();
	MatrixBuffer.Create(1, sizeof(MatrixBufferType), CONSTANT_BUFFER);
	NoiseBuffer.Create(1, sizeof(NoiseBufferType), CONSTANT_BUFFER);
	DistortionBuffer.Create(1, sizeof(DistortionBufferType), CONSTANT_BUFFER);
	Noise_Tex.Create("noise01.dds");
	Alpha_Tex.Create("alpha01.dds");
	Fire_Tex.Create("fire01.dds");
	RT_Fire.CreateRenderTarget(512, 512);

	std::random_device seed;
	std::mt19937 rd(seed());

	struct temp {unsigned char x, y, z, a; };
	unsigned int g = Grid_Size+2;
	std::vector<temp> randtex(g*g);
	for(unsigned int y=0; y< g; y++){
		for(unsigned int x=0; x< g; x++){
			randtex[y*g + x].x = rd()%255;
			randtex[y*g + x].y = rd()%255;
			randtex[y*g + x].z = rd()%255;
		}
	}
	// smooth out the texture 
	std::vector<temp> randtex1(g*g);
	for(unsigned int y=1; y< g-1; y++){
		for(unsigned int x=1; x< g-1; x++){
			unsigned int nx =0;
			unsigned int ny =0;
			unsigned int nz =0;
			for(unsigned int a=0; a<3; a++){
				for(unsigned int b=0; b<3; b++){
					nx += (unsigned int)randtex[((y-1) + a)*g + ((x-1) + b) ].x;
					ny += (unsigned int)randtex[((y-1) + a)*g + ((x-1) + b) ].y;
					nz += (unsigned int)randtex[((y-1) + a)*g + ((x-1) + b) ].z;
				}
			}
			nx/=9;
			ny/=9;
			nz/=9;
			randtex1[y*g + x].x=(unsigned char)nx;
			randtex1[y*g + x].y=(unsigned char)ny;
			randtex1[y*g + x].z=(unsigned char)nz;
		}
	}

	randtex.resize(Grid_Size*Grid_Size);
	for(unsigned int y=0; y< Grid_Size; y++){
		for(unsigned int x=0; x< Grid_Size; x++){
			randtex[y*Grid_Size + x].x=randtex1[(y+1)*g + x+1].x;
			randtex[y*Grid_Size + x].y=randtex1[(y+1)*g + x+1].y;
			randtex[y*Grid_Size + x].z=randtex1[(y+1)*g + x+1].z;
		}
	}


	Rand_Tex.Create(&randtex[0], Grid_Size, Grid_Size, 1);
	std::vector<vec3> v;
	std::vector<uint16_t> ind;
	GenGrid(ind, v, Grid_Size);

	Num_Verts = v.size();
	Num_Ind = ind.size();
	Vertex_Buffer.Create(v.size(), sizeof(vec3), VERTEX_BUFFER, IMMUTABLE, CPU_NONE, &v[0]);
	Index_Buffer.Create(ind.size(), sizeof(uint16_t), INDEX_BUFFER, IMMUTABLE, CPU_NONE, &ind[0]);

}
void Fire::DeInit(){
	Build_VS.Destroy();
	Build_PS.Destroy();
	Render_VS.Destroy();
	Render_PS.Destroy();
	MatrixBuffer.Destroy();
	NoiseBuffer.Destroy();
	DistortionBuffer.Destroy();
	Noise_Tex.Destroy();
	Alpha_Tex.Destroy();
	Fire_Tex.Destroy();
	RT_Fire.Destroy();
	Rand_Tex.Destroy();
	Index_Buffer.Destroy();
	Vertex_Buffer.Destroy();
}
void Fire::ReloadShaders(){

	Build_VS.CompileShaderFromFile("Fire.fx","Build_FireVertexShader");
	FormatDesc fd[] ={
		FormatDesc(),
	};
	Build_VS.CreateInputLayout(fd,1);
	Build_PS.CompileShaderFromFile("Fire.fx","Build_FirePixelShader");

	std::ostringstream s;
	s<<Grid_Size;
	std::string temp = s.str();
	std::vector<char> st(temp.size() +1);
	memcpy(&st[0], temp.c_str(), temp.size()+1);
	Graphics::Shader_Macro mac [] = {
		{"GRID_SIZE", &st[0] }, 
		{NULL, NULL}
	};

	Render_VS.CompileShaderFromFile("Fire.fx","FireVertexShader", "vs_4_0", mac);
	Render_VS.CreateInputLayout(fd,1);
	Render_PS.CompileShaderFromFile("Fire.fx","FirePixelShader");
}

void Fire::Draw(const mat4& view, const mat4& proj, float totaltime){


	Graphics::GeometryShader::UnBind();
	Graphics::DomainShader::UnBind();
	Graphics::HullShader::UnBind();
	Build_VS.Bind();
	Build_PS.Bind();

	Graphics::RasterizerStates::CullNone.Bind();
	Graphics::DepthStates::NoDepthTest.Bind();
	Graphics::BlendStates::SrcAlpha.Bind();

	NoiseBufferType nb;
	nb.frameTime = totaltime;
	nb.scales = vec3(1.0f, 2.0f, 3.0f);
	nb.scrollSpeeds = vec3(1.3f, 2.1f, 2.3f);
	NoiseBuffer.Update(&nb);
	Build_VS.SetConstantBuffer(NoiseBuffer);

	DistortionBufferType db;
	db.distortion1 = vec2(0.1f, 0.2f);
	db.distortion2 = vec2(0.1f, 0.3f);
	db.distortion3 = vec2(0.1f, 0.1f);
	db.distortionScale = 0.8f;
	db.distortionBias = 0.5f;
	DistortionBuffer.Update(&db);
	Build_PS.SetConstantBuffer(DistortionBuffer);

	Build_PS.SetResource(&Fire_Tex);
	Build_PS.SetResource(&Noise_Tex, 1);
	Build_PS.SetResource(&Alpha_Tex,2);

	Build_PS.SetSampler(Graphics::Samplers::Linear);
	Build_PS.SetSampler(Graphics::Samplers::LinearClampUVW, 1);

	unsigned int numrts=0;
	void* rts[MAX_RTS] = {0};
	Graphics::ViewPort vp;
	Graphics::GetRenderTargets(numrts, rts, vp);
	void* dv = Graphics::GetDepthTarget();
	Graphics::Texture* mrt[1] = { &RT_Fire};// Set new RT
	Graphics::BindRenderTargets(mrt, 1); // this will set the depth texture to none as well
	Graphics::ClearRenderTargets(true, false, false);
	Graphics::SetTopology(PRIM_TRIANGLE_STRIP);
	Graphics::DrawVertices(4, 0);

	Graphics::BindRenderTargets(numrts, rts,  dv, vp);// reset the render targets

	Render_VS.Bind();
	Render_PS.Bind();
	Render_PS.SetResource(&RT_Fire);
	Render_VS.SetResource(&Noise_Tex,0);
	Render_PS.SetSampler(Graphics::Samplers::Linear);
	Vertex_Buffer.BindAsVertexBuffer();
	Index_Buffer.BindAsIndexBuffer();
	Graphics::SetTopology(PRIM_TRIANGLELIST);

	mat4 rot, trans;
	trans.setupTranslation(0, 0, 0);
	MatrixBufferType mt;
	mt.projection = proj;
	mt.projection.Transpose();
	mt.view = view;
	mt.view.Transpose();
	rot.identity();

	mat4 scale;
	scale.setupScale(.5f, 1, 1);// scale the flame panel on the x axis since I am using two panels on the x axis
	mat4 fliptransx;
	fliptransx.setupTranslation(-1.0f, 0, 0);// move the grid so the center of the grid is at 0, 0, 0   Currently, the grid goes from 0 to 1.
	float angle = atan(Cam_Pos->z/Cam_Pos->x) - 2*(Pi/6.0f);
	Render_VS.SetResource(&Rand_Tex);
	Render_VS.SetSampler(Graphics::Samplers::Linear);
	Render_VS.SetConstantBuffer(NoiseBuffer, 1);

	// the reason for two loops is so that the flame panels are always rendered back to front. Otherwise, the flame panels are easy to see
	mt.texoffset_x.x=0.0f;
	for(float i =angle; i<Pi+angle; i+=Pi/4.0f){
		rot.setupRotateY(i);
		mt.world = fliptransx*scale*rot;
		mt.world.Transpose();
		MatrixBuffer.Update(&mt);
		Render_VS.SetConstantBuffer(MatrixBuffer);
		Graphics::DrawIndexed(0, Num_Ind);
	}
	mt.texoffset_x.x=.5f;
	for(float i =angle; i<Pi+angle; i+=Pi/4.0f){
		rot.setupRotateY(i);
		mt.world = scale*rot;
		mt.world.Transpose();
		MatrixBuffer.Update(&mt);
		Render_VS.SetConstantBuffer(MatrixBuffer);
		Graphics::DrawIndexed(0, Num_Ind);
	}
}
