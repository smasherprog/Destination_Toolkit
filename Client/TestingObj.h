#ifndef TESTINGOBJ_H
#define TESTINGOBJ_H

#include "../Graphics_Lib/GraphicsCore.h"

class Testing_OBJ{
public:


	Graphics::VertexShader VS;
	Graphics::HullShader HS;
	Graphics::DomainShader DS;
	Graphics::PixelShader PS;

	Graphics::Buffer Cbuffer0, VertexBuffer, IndexBuffer;

	Testing_OBJ() {}
	~Testing_OBJ() {DeInit();}
	void Init(){	

	}
	void DeInit(){

	}


};


#endif