#ifndef MESH_SHADER_H
#define MESH_SHADER_H
#include "GraphicsCore.h"


class Animated_Mesh_Shader{
public:

	Animated_Mesh_Shader();
	~Animated_Mesh_Shader();

	void Init();
	void DeInit();
	void Compile();

	Graphics::VertexShader VS;
	Graphics::PixelShader PS;

	unsigned int Quality;// 0 = low, 1 = mid, 2 = high
	unsigned int Matrix_Count;// default = 60
};



#endif