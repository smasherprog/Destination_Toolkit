#include "stdafx.h"
#include "../Graphics_Lib/cCamera.h"
#include <string>
#include "../Graphics_Lib/MeshTypes.h"
#if defined(USE_WOODY3D)
#include "cWoodyLoader.h"
#include "cWoodyTree.h"
struct buffer0{
	vec4						ra_vector;
	vec4						dul_vector;
	vec4						xzip_vector;
	vec4						scale_vector;
	vec4						center_of_leaves;
};
struct buffer1{
	mat4 ObjectToWorld, CameraViewProjection, InvTransposeObjectToWorld;
	vec4 CameraPosition;
};
struct buffer2{
	float						normal_multiplier;
	float pad[3];
	bool						is_translucent;
};

Graphics::Buffer cWoodyTree::CBuffer0, cWoodyTree::CBuffer1, cWoodyTree::CBuffer2;

cWoodyTree::cWoodyTree(): tree_lod(1.0f), tree_y_rotation(0.0f){// Seed wind emitter
	wind_emitter.seed(timeGetTime());
	wind_scale_array[0].level				= 2;
	wind_scale_array[0].weight				= 0.8f;
	wind_scale_array[0].duration_min_in_ms	= 2500;
	wind_scale_array[0].duration_max_in_ms	= 4500;
	wind_scale_array[1].level				= 3;
	wind_scale_array[1].weight				= 0.8f;
	wind_scale_array[1].duration_min_in_ms	= 2500;
	wind_scale_array[1].duration_max_in_ms	= 4500;
	wind_scale_array[2].level				= 4;
	wind_scale_array[2].weight				= 0.4f;
	wind_scale_array[2].duration_min_in_ms	= 1000;
	wind_scale_array[2].duration_max_in_ms	= 2000;
	wind_emitter.set_wind_scale_levels(wind_scale_array, 3);
	s_last_time_ms = timeGetTime();
	elapsed_time_ms=0;
	// Set wind direction
	wind_emitter.set_wind_direction(wd_vector2(-1.0f, 0.0f), wd_degree_to_radian(1.0f));
}
cWoodyTree::~cWoodyTree(){ 
	woody_tree.freee(); // this really should be in the objects desctructor, but its not
	cBaseMesh::clear();
}
bool cWoodyTree::Load(const std::string& file){ return cWoodyLoader::Load(file, this); }
float cWoodyTree::RayIntersect(const vec3& rayorig, const vec3& ray){// the rays are in world space
	mat4 inverseW(World);
	inverseW.inverse();
	vec3 org(rayorig*inverseW), di(ray);
	TransformNormal(di, inverseW);
	// do all checks in OBJECT SPACE!!!
	di*=20000.0f;
	if(ObjectSpace_BV.RayIntersect(org, di) == INFINITY) return INFINITY;// did not hit the aabb, therefore, we did not hit the object
	
	for(size_t i(0); i != Lod1.Batches.size(); i++){
		float t(RayTriangleIntersect(org, di, &Vertices[0], &Lod1.Indices[Lod1.Batches[i]->StartIndex], Lod1.Batches[i]->numIndices));
		if( t != INFINITY) {
			return t;
		}
	}
	return INFINITY;
}
/*

These Functions could be simplified into a single function and passing the LOD to it instead.  But, there might be differences in the way these should be remdered 
so, I left it like this.

*/
void cWoodyTree::DrawLod1(void* param1){// this should bind, pos, tex, normal, tang


	unsigned int now_time_ms = timeGetTime();
	
	elapsed_time_ms = now_time_ms - s_last_time_ms;// Store elapsed time
	s_last_time_ms = now_time_ms;// Store last time
	wind_emitter.update(elapsed_time_ms);// Update the wind emitter with elapsed time
	wd_real_32 wind_strength = wind_emitter.get_wind_intensity();// Get current  wind strength
	wd_vector3 wind_direction;
	wind_emitter.get_wind_direction(wind_direction);// Get current wind direction

	//set the lod data
	float LOD = 1.0f;
	const float farplane = 500.0f*500.0f;// use squared distance
	const float nearplane = 60.0f*60.0f;
	vec3 pos;
	GetPosition(pos.x, pos.y, pos.z);
	float distance_from_camera = (Camera.Position - pos).LengthSq();
	if(distance_from_camera >= nearplane) {
		if(distance_from_camera <= farplane){	
			LOD = 1.0f - wd_percentage(distance_from_camera, nearplane, farplane);
		} else LOD =0.0f;// the furthest away set to lowest lod
	}
	
	mat4 world_inverse_transpose_matrix(World);
	world_inverse_transpose_matrix.inverse();
	world_inverse_transpose_matrix.Transpose();
	vec3 rot;
	GetRotation(rot.x, rot.y, rot.z);
	woody_tree.set_lod(LOD); 
	wd_vector4 xzip_vect;
	wd_vector3 dul_vect, center_of_leave, tree_rotation(0, 0, 0);
	wd_vector2 ra_vect, leaf_scale_vector;
	// Get the woody shader constants
	woody_tree.get_shader_constants(xzip_vect, dul_vect, ra_vect);
	woody_tree.update_wind(elapsed_time_ms, tree_rotation, wind_direction, wind_strength);	

	Lod1.VS_Shader.Bind();
	Lod1.PS_Shader.Bind();

	buffer1 b1;
	b1.ObjectToWorld = World;
	b1.ObjectToWorld.Transpose();
	b1.CameraViewProjection = Camera.Trans_VP;
	b1.InvTransposeObjectToWorld = world_inverse_transpose_matrix;
	b1.InvTransposeObjectToWorld.Transpose();
	b1.CameraPosition = vec4(Camera.Position, 0.0f);
	CBuffer1.Update(&b1);

	Lod1.IB.BindAsIndexBuffer();
	Lod1.VB[0].BindAsVertexBuffer();// only 1 VB
	Lod1.VS_Shader.SetConstantBuffer(CBuffer1, 1);

/////////////SETUP constants for the entire tree here
		// Get center of leaves
	woody_tree.get_center_of_leaves(center_of_leave);
	// Create an XY scale std::vector for leaves
	leaf_scale_vector = wd_vector2(1, 1);
	buffer0 b0;
	b0.center_of_leaves = vec4(*reinterpret_cast<vec3*>(&center_of_leave), 0.0f);
	b0.dul_vector = vec4(*reinterpret_cast<vec3*>(&dul_vect), 0.0f);
	b0.ra_vector = vec4(*reinterpret_cast<vec2*>(&ra_vect), 0.0f, 0.0f);
	b0.scale_vector = vec4(*reinterpret_cast<vec2*>(&leaf_scale_vector), 0.0f, 0.0f);
	b0.xzip_vector = *reinterpret_cast<vec4*>(&xzip_vect.x) ;
	CBuffer0.Update(&b0);



	// If standard branches

	buffer2 b2;
	b2.is_translucent = false;
	b2.normal_multiplier = 1.0f;

	if(woody_tree.get_branch_index_count() > 0) {
		Lod1.PS_Shader.SetResource(&Lod1.Batches[0]->texture, 0);
		Lod1.PS_Shader.SetResource(&Lod1.Batches[0]->normal, 1);
		Lod1.PS_Shader.SetSampler(Graphics::Samplers::Linear, 0);
		CBuffer2.Update(&b2);
		Lod1.PS_Shader.SetConstantBuffer(CBuffer2);
		
		Graphics::DrawIndexed(Lod1.Batches[0]->StartIndex, woody_tree.get_branch_index_count());
	}

	// If imposter branches
	
	if(woody_tree.get_imposter_branch_index_count() > 0){

		Lod2.VS_Shader.Bind();
		Lod2.PS_Shader.Bind();
		Lod2.IB.BindAsIndexBuffer();
		Lod2.VB[0].BindAsVertexBuffer();// only 1 VB
		
		Lod2.PS_Shader.SetResource(&Lod2.Batches[0]->texture, 0);
		Lod2.PS_Shader.SetResource(&Lod2.Batches[0]->normal, 1);
		Lod2.PS_Shader.SetSampler(Graphics::Samplers::Linear, 0);
		Lod2.PS_Shader.SetConstantBuffer(CBuffer2);

		Graphics::DrawIndexed(Lod2.Batches[0]->StartIndex, Lod2.Batches[0]->numIndices);
		b2.normal_multiplier = -1.0f;
		// Invert normals and render back faces
		CBuffer2.Update(&b2);
		Lod2.PS_Shader.SetConstantBuffer(CBuffer2);
		Graphics::RasterizerStates::CullFront.Bind();

		Graphics::DrawIndexed(Lod2.Batches[0]->StartIndex, Lod2.Batches[0]->numIndices);
		Graphics::RasterizerStates::CullBack.Bind();

	}

	// If leaves
	if(woody_tree.get_leaf_index_count() > 0){
		Lod3.VS_Shader.Bind();
		Lod3.PS_Shader.Bind();

		b2.normal_multiplier = 1.0f;
		b2.is_translucent = true;
		// Invert normals and render back faces
		CBuffer2.Update(&b2);
		Lod3.PS_Shader.SetConstantBuffer(CBuffer2);

		Lod3.IB.BindAsIndexBuffer();
		Lod3.VB[0].BindAsVertexBuffer();

		Lod3.PS_Shader.SetResource(&Lod3.Batches[0]->texture, 0);
		Lod3.PS_Shader.SetResource(&Lod3.Batches[0]->normal, 1);
		Lod3.PS_Shader.SetSampler(Graphics::Samplers::Linear, 0);

		Graphics::DrawIndexed(Lod3.Batches[0]->StartIndex, woody_tree.get_leaf_index_count());

	}	

}

/*

These Functions could be simplified into a single shadow function and passing the LOD to it instead.  But, there might be differences in the way these should be remdered 
so, I left it like this.

*/
void cWoodyTree::DrawShadowLod1(void* param1){
	return;
	/*
	Renderer.SetObjectToWorldShadow(World);
	Renderer.SetIndexBuffer(Lod1.IB); 
	Renderer.SetVertexBuffer(0, Lod1.VB[0]);
	for(size_t i(0); i != Lod1.Batches.size(); i++){
		Renderer.Apply();
		Renderer.DrawIndexed(Lod1.Batches[i].StartIndex, Lod1.Batches[i].numIndices);
	}
	*/
}
void cWoodyTree::DrawShadowLod2(void* param1){
	return;
	/*
	Renderer.SetObjectToWorldShadow(World);
	Renderer.SetIndexBuffer(Lod2.IB); 
	Renderer.SetVertexBuffer(0, Lod2.VB[0]);
	for(size_t i(0); i != Lod2.Batches.size(); i++){
		Renderer.Apply();
		Renderer.DrawIndexed(Lod2.Batches[i].StartIndex, Lod2.Batches[i].numIndices);
	}
	*/
}
void cWoodyTree::DrawShadowLod3(void* param1){
	return;
	/*
	Renderer.SetObjectToWorldShadow(World);
	Renderer.SetIndexBuffer(Lod3.IB); 
	Renderer.SetVertexBuffer(0, Lod3.VB[0]);
	for(size_t i(0); i != Lod3.Batches.size(); i++){
		Renderer.Apply();
		Renderer.DrawIndexed(Lod3.Batches[i].StartIndex, Lod3.Batches[i].numIndices);
	}
	*/
}

#endif