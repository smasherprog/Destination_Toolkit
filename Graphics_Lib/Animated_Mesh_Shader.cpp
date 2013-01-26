#include "stdafx.h"
#include "Animated_Mesh_Shader.h"
#include <sstream>

Animated_Mesh_Shader::Animated_Mesh_Shader(){
	Quality =0 ;
	Matrix_Count= 60;
	Init();
}

Animated_Mesh_Shader::~Animated_Mesh_Shader(){
	DeInit();
}

void Animated_Mesh_Shader::Init(){
	Compile();
}
void Animated_Mesh_Shader::DeInit(){
	VS.Destroy();
	PS.Destroy();
}

void Animated_Mesh_Shader::Compile(){

	// convert the numbers to a string to pass to the shader
	std::ostringstream co;
	co<<Quality;
	std::string str0 = co.str();
	char* ptr0 = new char[str0.size() +1];// the extra 1 is for the null terminator
	memcpy(ptr0, str0.c_str(), str0.size() +1);
	co.str("");// clear the stream
	co<<Matrix_Count;
	str0 = co.str();
	char* ptr1 = new char[str0.size() +1];// the extra 1 is for the null terminator
	memcpy(ptr1, str0.c_str(), str0.size() +1);

	Graphics::Shader_Macro macro[] = {
		{"QUALITY", ptr0 }, 
		{"MATRIX_PALETTE_SIZE_DEFAULT ", ptr1}, 
		{NULL, NULL}
	};

	FormatDesc inputs[] = { 
		FormatDesc(TYPE_VERTEX, FORMAT_FLOAT, 3, 0),
		FormatDesc(TYPE_TEXCOORD, FORMAT_FLOAT, 2, 1),
		FormatDesc(TYPE_NORMAL, FORMAT_FLOAT, 3, 1),
		FormatDesc(TYPE_TANGENT, FORMAT_FLOAT, 3, 2),
		FormatDesc(TYPE_BONE, FORMAT_FLOAT, 4, 3),
		FormatDesc(TYPE_WEIGHT, FORMAT_FLOAT, 4, 3),
	};

	VS.CompileShaderFromFile("Mesh.fx", "VS", "vs_4_0", macro);// make sure to force a re-compile
	VS.CreateInputLayout(inputs, 6);
	PS.CompileShaderFromFile("Mesh.fx", "PS", "ps_4_0", macro);// make sure to force a re-compile
	delete [] ptr0;
	delete [] ptr1;
}