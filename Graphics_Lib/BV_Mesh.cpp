#include "stdafx.h"
#include "BV_Mesh.h"
#include "Shaders.h"
#include "Base_Camera.h"
#include "Batch.h"

bool BV_Mesh::Init(){
	Vertices.push_back(vec3(.5f, .5f, .5f));
	Vertices.push_back(vec3(.5f,-.5f,.5f));
	Vertices.push_back(vec3(.5f, -.5f, -.5f));
	Vertices.push_back(vec3(.5f, .5f, -.5f));

	Vertices.push_back(vec3(-.5f, .5f, .5f));
	Vertices.push_back(vec3(-.5f,-.5f, .5f));
	Vertices.push_back(vec3(-.5f, -.5f, -.5f));
	Vertices.push_back(vec3(-.5f, .5f, -.5f));
	Vertices.push_back(vec3(-.5f, .5f, -.5f));
	
	uint16_t ind[] = {0,1,2,3,0,4,5,6, 7,4, 0, 5, 1, 6, 2, 7, 3,4};
	for(uint16_t i=0; i< sizeof(ind)/sizeof(uint16_t); i++) Indices.push_back(ind[i]);

	VB[0].Create(Vertices.size(), sizeof(vec3), BufferType::VERTEX_BUFFER, DEFAULT, CPU_NONE, &Vertices[0]);
	IB.Create(Indices.size(), sizeof(uint16_t), BufferType::INDEX_BUFFER, DEFAULT, CPU_NONE, &Indices[0]);
	CBuffer0.Create(1, sizeof(mat4) + sizeof(vec4), CONSTANT_BUFFER);
	Batch* b = new Batch();
	FormatDesc layers[1] = { FormatDesc(TYPE_VERTEX, FORMAT_FLOAT, 4, 0) };
	b->GetVS()->CompileShaderFromMemory(Graphics::Shader_Defines::BV_VS);
	b->GetVS()->CreateInputLayout(layers, 1);
	b->GetPS()->CompileShaderFromMemory(Graphics::Shader_Defines::BV_PS);
	b->NumIndices = Indices.size();
	b->StartIndex =0;
	b->NumVerts = Vertices.size();
	Batches.push_back(b);
	return true;
}

void BV_Mesh::Draw(const Base_Camera* camera, float dt){

	Graphics::SetTopology(PRIM_LINE_STRIP);
	Graphics::DepthStates::DepthTest.Bind();
	Graphics::RasterizerStates::CullNone.Bind();
	Graphics::BlendStates::No_Blend.Bind();

	struct tempstruct{
		mat4 vp;
		vec4 color;
	};
	tempstruct t;
	t.vp = GetWorld()*camera->VP;// we have to move the BV that was pregenerated into the correct position and scale it 
	t.vp.Transpose();
	t.color = vec4(1.0f, 0, 0, 1);//red
	CBuffer0.Update(&t);

	VB[0].BindAsVertexBuffer();
	IB.BindAsIndexBuffer();

	for(auto beg = Batches.begin(); beg != Batches.end(); beg++){
		Batch& batch = **beg;
		batch.GetVS()->Bind();
		batch.GetVS()->SetConstantBuffer(CBuffer0, 0);

		batch.GetPS()->Bind();

		Graphics::DrawIndexed(batch.StartIndex, batch.NumIndices);
	}

}