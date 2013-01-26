#include "stdafx.h"
#include "cMesh.h"
#include "../Utilities/My_Timer.h"
#include "cMeshLoader.h"
#include "../Utilities/MyMath.h"

/*
void cMesh::DrawLod1(void* param1){
	// USING LOD 2!!
	Lod2.VS_Shader.Bind();
	Lod2.PS_Shader.Bind();

	
	Renderer.SetObjectToWorld(World);
	mat4 invw(World);
	invw.inverse();
	invw.Transpose();
	Renderer.SetInvTransposeObjectToWorld(invw);
	Renderer.SetIndexBuffer(Lod1.IB);
	for(size_t j(0); j < Lod1.VB.size(); j++) 
		Renderer.SetVertexBuffer((uint32_t)j, Lod1.VB[j]);// set all the vertex buffers
	if(param1){
		AnimationTime+=cMyTime::DT;
		std::vector<mat4>& transforms = mAnimator.GetTransforms(AnimationTime);
		Renderer.SetFloat4x4Array(*reinterpret_cast<ShaderFloat4x4ArrayID*>(param1), &transforms[0], transforms.size());
		//Renderer.SetFloat4x4Array(*reinterpret_cast<ShaderFloat4x4ArrayID*>(param1), &mAnimator.QuatTransforms[0], mAnimator.QuatTransforms.size());
	}
	for(size_t i(0); i != Lod1.Batches.size(); i++){
		Renderer.SetSpecularColor(Lod1.Batches[i].Specular);
		Renderer.SetSpecularPower(Lod1.Batches[i].Power);
		Renderer.SetTextureSlot0(Lod1.Batches[i].texture);
		Renderer.SetTextureSlot1(Lod1.Batches[i].normal);
	
		Renderer.Apply();
		Renderer.DrawIndexed(Lod1.Batches[i].StartIndex, Lod1.Batches[i].numIndices);
	}

}
void cMesh::DrawLod2(void* param1){
	
	Renderer.SetTechnique(Lod2.Technique);
	Renderer.SetInputLayout(Lod2.InputLayout);
	Renderer.SetObjectToWorld(World);
	mat4 invw(World);
	invw.inverse();
	invw.Transpose();
	Renderer.SetInvTransposeObjectToWorld(invw);
	Renderer.SetIndexBuffer(Lod1.IB);
	for(size_t j(0); j < Lod1.VB.size(); j++) 
		Renderer.SetVertexBuffer((uint32_t)j, Lod1.VB[j]);// set all the vertex buffers

	if(param1){
		AnimationTime+=cMyTime::DT;
		std::vector<mat4>& transforms = mAnimator.GetTransforms(AnimationTime);
		Renderer.SetFloat4x4Array(*reinterpret_cast<ShaderFloat4x4ArrayID*>(param1), &transforms[0], transforms.size());
	}
	for(size_t i(0); i != Lod2.Batches.size(); i++){
		Renderer.SetSpecularColor(Lod2.Batches[i].Specular);
		Renderer.SetSpecularPower(Lod2.Batches[i].Power);
		Renderer.SetTextureSlot0(Lod2.Batches[i].texture);
		Renderer.SetTextureSlot1(Lod2.Batches[i].normal);
	
		Renderer.Apply();
		Renderer.DrawIndexed(Lod1.Batches[i].StartIndex, Lod1.Batches[i].numIndices);
	}

}
void cMesh::DrawLod3(void* param1){

	Renderer.SetTechnique(Lod3.Technique);
	Renderer.SetInputLayout(Lod3.InputLayout);
	Renderer.SetObjectToWorld(World);
	mat4 invw(World);
	invw.inverse();
	invw.Transpose();
	Renderer.SetInvTransposeObjectToWorld(invw);
	Renderer.SetIndexBuffer(Lod3.IB);
	for(size_t j(0); j < Lod3.VB.size(); j++) 
		Renderer.SetVertexBuffer((uint32_t)j, Lod3.VB[j]);// set all the vertex buffers

	if(param1){
		AnimationTime+=cMyTime::DT;
		std::vector<mat4>& transforms = mAnimator.GetTransforms(AnimationTime);
		Renderer.SetFloat4x4Array(*reinterpret_cast<ShaderFloat4x4ArrayID*>(param1), &transforms[0], transforms.size());
	}
	for(size_t i(0); i != Lod3.Batches.size(); i++){
		Renderer.SetSpecularColor(Lod3.Batches[i].Specular);
		Renderer.SetSpecularPower(Lod3.Batches[i].Power);
		Renderer.SetTextureSlot0(Lod3.Batches[i].texture);
		//Renderer.SetTextureSlot1(Lod3.Batches[i].normal);
	
		Renderer.Apply();
		Renderer.DrawIndexed(Lod3.Batches[i].StartIndex, Lod3.Batches[i].numIndices);
	}

}
	*/

