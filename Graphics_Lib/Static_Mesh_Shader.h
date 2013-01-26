#ifndef MESH_SHADER_H
#define MESH_SHADER_H
#include "GraphicsCore.h"


class Static_Mesh_Shader{
public:


	Static_Mesh_Shader();
	~Static_Mesh_Shader();

	void Init();
	void DeInit();
	void Compile();

	Graphics::VertexShader VS_Animated_LowQ, VS_Animated_MedQ, VS_Animated_HighQ;
	Graphics::PixelShader PS_Animated_LowQ, PS_Animated_MedQ, PS_Animated_HighQ;

};



#endif