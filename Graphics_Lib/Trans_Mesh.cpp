#include "stdafx.h"
#include "Trans_Mesh.h"
#include "Shaders.h"

bool Trans_Mesh::Init(){

	Vertices.push_back(vec3(1.0f, 0.0f, 0.0f));
	const float split=16.0f;
	//note: all of these shapes will have to be scaled to correctly in the draw function, but its not something the user will do. that will be done in the draw function below
	// first create the cone pointer
	uint16_t index=1;
	for(float i=0.0f; i<2*Pi; i+=Pi/split){
		vec3 p;
		p.x=0.0f;
		p.y=sinf(i);
		p.z=cosf(i);
		Vertices.push_back(p);
	}

	index=0;
	for(float i=0.0f; i<2*Pi; i+=Pi/split){
		Indices.push_back(0);
		Indices.push_back(index+2);
		Indices.push_back(index+1);
		index+=1;
	}
	Indices[Indices.size()-2]=Vertices.size()-1;
	Indices.push_back(0);
	Indices.push_back(1);
	Indices.push_back(Vertices.size()-1);

	FormatDesc layers[1] = { FormatDesc(TYPE_VERTEX, FORMAT_FLOAT, 4, 0) };
	Batch* b = new Batch();
	b->GetVS()->CompileShaderFromMemory(Graphics::Shader_Defines::BV_VS);
	b->GetVS()->CreateInputLayout(layers, 1);
	b->GetPS()->CompileShaderFromMemory(Graphics::Shader_Defines::BV_PS);
	b->NumIndices = Indices.size();
	b->StartIndex =0;
	b->NumVerts = Vertices.size();
	Batches.push_back(b);

	// now create the rod to connect to it 
	// a long triangle looks the same as a rod
	Vertices.push_back(vec3(0.0f, 1.0f, 0.0f));//0
	Vertices.push_back(vec3(1.0f, 0.0f, 0.0f));//1
	Vertices.push_back(vec3(0.0f, 0.0f, 1.0f));//2

	Vertices.push_back(vec3(0.0f, -1.0f, 0.0f));//3
	Vertices.push_back(vec3(-1.0f, 0.0f, 0.0f));//4
	Vertices.push_back(vec3(0.0f, 0.0f, -1.0f));//5

	index=Indices.size()-1;// get the index
	Indices.push_back(index+0);
	Indices.push_back(index+3);
	Indices.push_back(index+1);

	Indices.push_back(index+1);
	Indices.push_back(index+4);
	Indices.push_back(index+3);

	Indices.push_back(index+1);
	Indices.push_back(index+4);
	Indices.push_back(index+2);

	Indices.push_back(index+2);
	Indices.push_back(index+5);
	Indices.push_back(index+4);

	Indices.push_back(index+2);
	Indices.push_back(index+5);
	Indices.push_back(index+0);

	Indices.push_back(index+0);
	Indices.push_back(index+5);
	Indices.push_back(index+3);

	CBuffer0.Create(1, sizeof(mat4) + sizeof(vec4), CONSTANT_BUFFER);
	VB[0].Create(Vertices.size(), sizeof(vec3), BufferType::VERTEX_BUFFER, DEFAULT, CPU_NONE, &Vertices[0]);
	IB.Create(Indices.size(), sizeof(uint16_t), BufferType::INDEX_BUFFER, DEFAULT, CPU_NONE, &Indices[0]);
	return true;
}

void Trans_Mesh::Draw(const mat4& view, const mat4& proj){

	Graphics::SetTopology(PRIM_TRIANGLELIST);
	Graphics::DepthStates::NoDepthTest.Bind();
	Graphics::RasterizerStates::CullNone.Bind();
	Graphics::BlendStates::No_Blend.Bind();

	VB[0].BindAsVertexBuffer();
	IB.BindAsIndexBuffer();

	// first axis
	Batch* batch = Batches[0];

	struct tempstruct{
		mat4 vp;
		vec4 color;
	};
	tempstruct t;
	mat4 bvscale, bvtrans, bvaxis;
	vec3 size_of_each_axis = GetScaling();
	float maxaxis = max(max(size_of_each_axis.x/20.0f, size_of_each_axis.y/20.0f), size_of_each_axis.z/20.0f);
	bvscale.setupScale(maxaxis);
	bvtrans.setupTranslation(GetPosition() + vec3(size_of_each_axis.x/2.0f, 0, 0));
	bvaxis.setupTranslation(vec3(-1.0f, 0, 0));

	t.vp = bvaxis*bvscale*bvtrans*view*proj;// we have to move the BV that was pregenerated into the correct position and scale it 
	t.vp.Transpose();
	t.color = vec4(1.0f, 0, 0, 1);//red  ==x
	CBuffer0.Update(&t);

	batch->GetVS()->Bind();
	batch->GetVS()->SetConstantBuffer(CBuffer0, 0);
	batch->GetPS()->Bind();

	Graphics::DrawIndexed(batch->StartIndex, batch->NumIndices);

	// second axis
	mat4 rot;
	rot.setupRotateY(-Pi/2.0f);
	bvaxis.setupTranslation( 0.0f,  0.0f, -1.0f);
	bvtrans.setupTranslation(GetPosition() + vec3(0.0f, 0.0f, size_of_each_axis.z/2.0f));

	t.vp = bvaxis*bvscale*rot*bvtrans*view*proj;
	t.vp.Transpose();
	t.color = vec4(0, 1.0f, 0, 1);//green  = z
	CBuffer0.Update(&t);
	batch->GetVS()->SetConstantBuffer(CBuffer0, 0);

	Graphics::DrawIndexed(batch->StartIndex, batch->NumIndices);
	/// last axis
	rot.setupRotateZ(Pi/2.0f);
	bvtrans.setupTranslation(GetPosition() + vec3(0.0f, size_of_each_axis.y/2.0f, 0.0f));
	bvaxis.setupTranslation(0.0f, -1.0f, 0.0f);

	t.vp = bvaxis*bvscale*rot*bvtrans*view*proj;
	t.vp.Transpose();
	t.color = vec4(0, 0, 1.0f, 1);//blue   = y
	CBuffer0.Update(&t);
	batch->GetVS()->SetConstantBuffer(CBuffer0, 0);

	Graphics::DrawIndexed(batch->StartIndex, batch->NumIndices);
}