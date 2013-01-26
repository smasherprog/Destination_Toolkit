#ifndef PARTICLE_GROUP_H
#define PARTICLE_GROUP_H
#include "../Graphics_Lib/GraphicsCore.h"

class mat4;
class vec3;
class Fire {
public:

	Fire() {}
	~Fire(){ DeInit(); }

	void Init(vec3* campos, uint32_t grid_size=24);
	void DeInit();
	void Draw(const mat4& view, const mat4& proj, float totaltime);
	
	void ReloadShaders();

protected:

	Graphics::Buffer MatrixBuffer, NoiseBuffer, DistortionBuffer, Vertex_Buffer, Index_Buffer;
	Graphics::Texture Noise_Tex, Alpha_Tex, Fire_Tex, RT_Fire, Rand_Tex;

	Graphics::VertexShader Build_VS, Render_VS;
	Graphics::PixelShader Build_PS, Render_PS;
	vec3* Cam_Pos;
	size_t Num_Verts, Num_Ind, Grid_Size;
};

#endif