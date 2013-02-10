#include "stdafx.h"
#include "Trans_Mesh.h"
#include "Shaders.h"
#include "Base_Camera.h"

bool Trans_Mesh::Init(){
	XAxis_Color=vec3(1, 0, 0);
	YAxis_Color=vec3(0, 0, 1);
	ZAxis_Color=vec3(0, 1, 0);
	Hit_Axis_Color = vec3(1, 1, 1);

	Vertices.push_back(vec3(1.0f, 0.0f, 0.0f));
	const float split=16.0f;
	//note: all of these shapes will have to be scaled to correctly in the draw function, but its not something the user will do. that will be done in the draw function below
	// first create the cone pointer
	uint16_t index=1;
	for(float i=0.0f; i<2*Pi; i+=Pi/split){
		vec3 p;
		p.x=-1.0f;
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

	index=Vertices.size();

	// now create the rod to connect to it 
	// a long triangle looks the same as a rod
	Vertices.push_back(vec3(1.0f, 1.0f, 0.0f));//0
	Vertices.push_back(vec3(1.0f, -1.0f, 1.0f));//1
	Vertices.push_back(vec3(1.0f, -1.0f, -1.0f));//2

	Vertices.push_back(vec3(-1.0f, 1.0f, 0.0f));//3
	Vertices.push_back(vec3(-1.0f, -1.0f, 1.0f));//4
	Vertices.push_back(vec3(-1.0f, -1.0f, -1.0f));//5

	Vertices.push_back(vec3(-1.0f, -1.0f, -1.0f));//extra vert is needed for alignment reasons.. .  i guess

	Batch* b1 = new Batch();
	b1->GetVS()->CompileShaderFromMemory(Graphics::Shader_Defines::BV_VS);
	b1->GetVS()->CreateInputLayout(layers, 1);
	b1->GetPS()->CompileShaderFromMemory(Graphics::Shader_Defines::BV_PS);
	b1->StartIndex =Indices.size();
	b1->NumVerts = 6;
	Batches.push_back(b1);

	Indices.push_back(index+0);
	Indices.push_back(index+4);
	Indices.push_back(index+3);

	Indices.push_back(index+1);
	Indices.push_back(index+4);
	Indices.push_back(index+0);

	Indices.push_back(index+1);
	Indices.push_back(index+4);
	Indices.push_back(index+2);

	Indices.push_back(index+2);
	Indices.push_back(index+4);
	Indices.push_back(index+5);

	Indices.push_back(index+5);
	Indices.push_back(index+2);
	Indices.push_back(index+3);

	Indices.push_back(index+3);
	Indices.push_back(index+2);
	Indices.push_back(index+0);

	b1->NumIndices = Indices.size()-b->NumIndices;


	CBuffer0.Create(1, sizeof(mat4) + sizeof(vec4), CONSTANT_BUFFER);
	VB[0].Create(Vertices.size(), sizeof(vec3), BufferType::VERTEX_BUFFER, DEFAULT, CPU_NONE, &Vertices[0]);
	IB.Create(Indices.size(), sizeof(uint16_t), BufferType::INDEX_BUFFER, DEFAULT, CPU_NONE, &Indices[0]);

	return true;
}
float Trans_Mesh::HighLight_Hit_Axis(const vec3& rayorig, const vec3& raydir){
	int axishit=0;
	float dist = Ray_Tri_Intersect(rayorig, raydir,axishit);
	if(dist!=INFINITY){
		if(axishit==0) XAxis_Color = Hit_Axis_Color;
		else if(axishit==1) ZAxis_Color = Hit_Axis_Color;
		else if(axishit==2) YAxis_Color = Hit_Axis_Color;
	} else{
		XAxis_Color=vec3(1, 0, 0);
		YAxis_Color=vec3(0, 0, 1);
		ZAxis_Color=vec3(0, 1, 0);
	}
	return dist;
}


float Trans_Mesh::Ray_Tri_Intersect(const vec3& rayorig, const vec3& raydir, int& axishit) const {

	Batch* cone_batch = Batches[0];
	Batch* rod_batch = Batches[1];

	mat4 bvtrans, bvaxis, rodscale, rodtrans, world;
	const float offaxis =10.0f;
	rodscale.setupScale(offaxis/3.0f, .1f, .1f);
	const float distaway = 120.0f;
	vec3 transpost =  GetPosition()-LastCamPosition;
	transpost.normalize();

	//FIRST AXIS.. RED AND X
	bvtrans.setupTranslation((transpost*distaway) + LastCamPosition+vec3(offaxis, 0.0f, 0.0f));
	bvaxis.setupTranslation(vec3(-1.0f, 0, 0));
	world = bvaxis*bvtrans;
	float dist = Ray_Tri_Intersect(rayorig, raydir, world, cone_batch->StartIndex, cone_batch->NumIndices);
	if(dist != INFINITY) {
		axishit=0;
		return dist;
	}
	bvtrans.setupTranslation((transpost*distaway) + LastCamPosition);
	rodtrans.setupTranslation(vec3(1.3f, 0, 0));
	world = rodtrans*rodscale*bvtrans;
	dist = Ray_Tri_Intersect(rayorig, raydir, world, rod_batch->StartIndex, rod_batch->NumIndices);
	if(dist != INFINITY) {
		axishit=0;
		return dist;
	}

	//SECOND AXIS. .. GREEN AND Z
	mat4 rot;
	rot.setupRotateY(-Pi/2.0f);

	bvaxis.setupTranslation( 0.0f,  0.0f, -1.0f);
	bvtrans.setupTranslation((transpost*distaway) + LastCamPosition+ vec3(0.0f, 0.0f, offaxis));

	world = bvaxis*rot*bvtrans;
	dist = Ray_Tri_Intersect(rayorig, raydir, world, cone_batch->StartIndex, cone_batch->NumIndices);
	if(dist != INFINITY) {
		axishit=1;
		return dist;
	}
	bvtrans.setupTranslation((transpost*distaway) + LastCamPosition+ vec3(1.0f, 0, offaxis/2.0f));
	world  = rodscale*rot*bvtrans;
	dist = Ray_Tri_Intersect(rayorig, raydir, world, rod_batch->StartIndex, rod_batch->NumIndices);
	if(dist != INFINITY) {
		axishit=1;
		return dist;
	}

	//THIRD AXIS . . . . BLUE AND Y
	rot.setupRotateZ(Pi/2.0f);
	bvtrans.setupTranslation((transpost*distaway) + LastCamPosition + vec3(0.0f, offaxis, 0.0f));
	bvaxis.setupTranslation(0.0f, -1.0f, 0.0f);

	world= bvaxis*rot*bvtrans;
	dist = Ray_Tri_Intersect(rayorig, raydir, world, cone_batch->StartIndex, cone_batch->NumIndices);
	if(dist != INFINITY) {
		axishit=2;
		return dist;
	}
	bvtrans.setupTranslation((transpost*distaway) + LastCamPosition+ vec3(1.0f, offaxis/2.0f, 0.0f));
	rodtrans.setupTranslation(vec3(0,  1.3f, 0));
	world = rodtrans*rodscale*rot*bvtrans;
	dist = Ray_Tri_Intersect(rayorig, raydir, world, cone_batch->StartIndex, cone_batch->NumIndices);
	if(dist != INFINITY) {
		axishit=2;
		return dist;
	}
	return dist;
}
float Trans_Mesh::Ray_Tri_Intersect(const vec3& rayorig, const vec3& raydir, mat4& world, uint16_t startindex, uint16_t numindices) const{
	world.inverse();
	vec3 org(rayorig*world), di(raydir);// transform these to the mesh's space so the checks are in object space, not world space
	TransformNormal(di, world);
	// do all checks in OBJECT SPACE!!!
	di*=20000.0f;//make sure the ray is long enough
	return RayTriangleIntersect(org, di, &Vertices[0], &Indices[startindex],numindices);
}
void Trans_Mesh::Draw(const Base_Camera* camera){

	Graphics::SetTopology(PRIM_TRIANGLELIST);
	Graphics::DepthStates::NoDepthTest.Bind();
	Graphics::RasterizerStates::CullNone.Bind();
	Graphics::BlendStates::No_Blend.Bind();

	VB[0].BindAsVertexBuffer();
	IB.BindAsIndexBuffer();

	// first axis
	Batch* cone_batch = Batches[0];
	Batch* rod_batch = Batches[1];

	struct tempstruct{
		mat4 vp;
		vec4 color;
	};
	tempstruct t;
	mat4 bvtrans, bvaxis, rodscale, rodtrans;

	const float offaxis =10.0f;
	rodscale.setupScale(offaxis/3.0f, .1f, .1f);
	const float distaway = 120.0f;
	LastCamPosition = camera->Position;

	vec3 transpost =  GetPosition()-LastCamPosition;
	transpost.normalize();

	//FIRST AXIS.. RED AND X
	bvtrans.setupTranslation((transpost*distaway) + LastCamPosition+vec3(offaxis, 0.0f, 0.0f));
	bvaxis.setupTranslation(vec3(-1.0f, 0, 0));
	t.vp = bvaxis*bvtrans*camera->VP;// we have to move the BV that was pregenerated into the correct position and scale it 
	t.vp.Transpose();
	t.color = vec4(XAxis_Color, 1);//red  ==x
	CBuffer0.Update(&t);

	cone_batch->GetVS()->Bind();
	cone_batch->GetVS()->SetConstantBuffer(CBuffer0, 0);
	cone_batch->GetPS()->Bind();

	Graphics::DrawIndexed(cone_batch->StartIndex, cone_batch->NumIndices);
	//now draw the rod
	bvtrans.setupTranslation((transpost*distaway) + LastCamPosition);
	rodtrans.setupTranslation(vec3(1.3f, 0, 0));
	t.vp = rodtrans*rodscale*bvtrans*camera->VP;// we have to move the BV that was pregenerated into the correct position and scale it 
	t.vp.Transpose();
	CBuffer0.Update(&t);

	cone_batch->GetVS()->SetConstantBuffer(CBuffer0, 0);
	Graphics::DrawIndexed(rod_batch->StartIndex, rod_batch->NumIndices);


	//SECOND AXIS. .. GREEN AND Z
	mat4 rot;
	rot.setupRotateY(-Pi/2.0f);
	bvaxis.setupTranslation( 0.0f,  0.0f, -1.0f);
	bvtrans.setupTranslation((transpost*distaway) + LastCamPosition+ vec3(0.0f, 0.0f, offaxis));

	t.vp = bvaxis*rot*bvtrans*camera->VP;
	t.vp.Transpose();
	t.color = vec4(ZAxis_Color, 1);//green  = z
	CBuffer0.Update(&t);
	cone_batch->GetVS()->SetConstantBuffer(CBuffer0, 0);

	Graphics::DrawIndexed(cone_batch->StartIndex, cone_batch->NumIndices);

	//now draw the rod
	bvtrans.setupTranslation((transpost*distaway) + LastCamPosition+ vec3(1.0f, 0, offaxis/2.0f));
	t.vp = rodscale*rot*bvtrans*camera->VP;// we have to move the BV that was pregenerated into the correct position and scale it 
	t.vp.Transpose();
	CBuffer0.Update(&t);

	cone_batch->GetVS()->SetConstantBuffer(CBuffer0, 0);
	Graphics::DrawIndexed(rod_batch->StartIndex, rod_batch->NumIndices);

	//THIRD AXIS . . . . BLUE AND Y
	rot.setupRotateZ(Pi/2.0f);
	bvtrans.setupTranslation((transpost*distaway) + LastCamPosition + vec3(0.0f, offaxis, 0.0f));
	bvaxis.setupTranslation(0.0f, -1.0f, 0.0f);

	t.vp = bvaxis*rot*bvtrans*camera->VP;
	t.vp.Transpose();
	t.color = vec4(YAxis_Color, 1);//blue   = y
	CBuffer0.Update(&t);
	cone_batch->GetVS()->SetConstantBuffer(CBuffer0, 0);

	Graphics::DrawIndexed(cone_batch->StartIndex, cone_batch->NumIndices);

	//now draw the rod
	bvtrans.setupTranslation((transpost*distaway) + LastCamPosition+ vec3(1.0f, offaxis/2.0f, 0.0f));
	rodtrans.setupTranslation(vec3(0,  1.3f, 0));
	t.vp = rodtrans*rodscale*rot*bvtrans*camera->VP;// we have to move the BV that was pregenerated into the correct position and scale it 
	t.vp.Transpose();
	CBuffer0.Update(&t);

	cone_batch->GetVS()->SetConstantBuffer(CBuffer0, 0);
	Graphics::DrawIndexed(rod_batch->StartIndex, rod_batch->NumIndices);
}