#ifndef PARTICLE_GROUP_H
#define PARTICLE_GROUP_H
#include "../Graphics_Lib/GraphicsCore.h"

class mat4;

class Particle_Group {
public:

	Particle_Group() {
		From=&Particle_In;
		To =&Particle_Out;
	}
	~Particle_Group(){ DeInit(); }

	void Init();
	void DeInit();
	void Draw(mat4& view, mat4& proj, float dt);
	
	void Update(float dt);
	void ReloadShaders();

protected:
	void _Update(float dt);
	Graphics::Buffer Particle_In, Particle_Out, CBuffer;
	Graphics::Buffer *From, *To;

	Graphics::Texture Random_Tex, Particle_Tex;

	Graphics::VertexShader VS_Passthrough, VS_Main;
	Graphics::GeometryShader GS_StreamOut, GS_Main;
	Graphics::PixelShader PS_Main;
	bool First_Run;
};

#endif