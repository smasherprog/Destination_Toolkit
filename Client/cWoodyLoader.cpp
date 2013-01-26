#include "stdafx.h"
#include <iostream>
#include <string>
#if defined(USE_WOODY3D)
#include "../Woody_Lib/woody3d.h"
#include "cWoodyLoader.h"
#include "cWoodyTree.h"


bool cWoodyLoader::Load(const std::string& xfile, cWoodyTree* mesh){
	// Local data	
	wd_tree_material_s	material;	
	// Shader animation vertex element descriptors
	// The branch vertex descriptor is used on both standard branches and imposter branches
	wd_ved shader_branch_ved[]	= { 
									wd_ved(WD_VED_POSITION, 3),		// Vertex position - 3 floats
									wd_ved(WD_VED_COLOR_ARGB, 1),	// Vertex color - 1 float
									wd_ved(WD_VED_TEXCOORD, 2),		// Vertex UV - 2 floats
									wd_ved(WD_VED_NORMAL, 3),		// Vertex Normal - 3 floats
									wd_ved(WD_VED_TANGENT, 4),		// Vertex Tangent - 4 floats
									wd_ved(WD_VED_FWOR, 4)			// Vertex FWOR std::vector - 4 floats (BRANCH FLEXIBILITY, BRANCH OSCILLATION WEIGHT, BRANCH OSCILLATION PHASE OFFSET, BRANCH OSCILLATION RANGE)
								  };	

	wd_ved shader_leaf_ved[]	= { 
									wd_ved(WD_VED_POSITION, 3),		// Vertex position - 3 floats
									wd_ved(WD_VED_COLOR_ARGB, 1),	// Vertex color - 1 float
									wd_ved(WD_VED_TEXCOORD, 2),		// Vertex UV - 2 floats
									wd_ved(WD_VED_LLOD, 1),			// Vertex LLOD - 1 float (LEAF LOD)
									wd_ved(WD_VED_FWOR, 4),			// Vertex FWOR std::vector - 4 floats (BRANCH FLEXIBILITY, BRANCH OSCILLATION WEIGHT, BRANCH OSCILLATION PHASE OFFSET, BRANCH OSCILLATION RANGE)
									wd_ved(WD_VED_XYOR, 4),			// Vertex XYOR std::vector - 4 floats (LEAF XY OFFSET, LEAF OSCILLATION PHASE OFFSET, LEAF ROTATION OSCILLATION RANGE)
									wd_ved(WD_VED_CUVA, 4)			// Vertex CUVA std::vector - 4 floats (LEAF ANIMATION COLUMN COUNT, LEAF ANIMATION UV FRAME SIZE, LEAF ANIMATION FRAME COUNT)
								  };


		// Load the mesh file for shader animation
	if(!mesh->woody_tree.load_mesh_from_file(xfile.c_str(), 0,		
										   TRUE,					// Create tree directory
										   shader_branch_ved, 6,	// Branch ved
										   shader_branch_ved, 6,	// Imposter branch ved
										   shader_leaf_ved, 7,		// Leaf ved
										   WD_TEXSPACE_DIRECT3D,	// Texture space origin
										   FALSE,					// Is create local animation (software animation)								
										   material))				// Tree material OUT
		{	
			cout<<"In function load_tree_mesh(): Failed to load Woody3D tree mesh from file."<<endl;
			return false;
		}
	// Set lod of tree to 1.0f to ensure buffers are large enough
	mesh->woody_tree.set_lod(1.0f);
	std::string pathtotree= GetPath(xfile);
	Batch* batch = mesh->Lod1.CreateBatch();
	/////////////////Create the textures
	std::string temp;
	if(strlen(material.branch_map_path_utf_8) > 0){	
		temp = pathtotree+ material.branch_map_path_utf_8;
		temp = (temp) + ".dds";
		if(!batch->texture.Create(temp)){		
			OUTPUT_DEBUG_MSG("Could not load material.branch_map_path: " + temp);
			return false;
		}
	
		// Load branch normal texture
		if(strlen(material.branch_normal_map_path_utf_8) > 0){	
			temp = pathtotree+ material.branch_normal_map_path_utf_8;
			temp = (temp) + ".dds";
			if(!batch->normal.Create(temp)){
				OUTPUT_DEBUG_MSG("Could not load material.branch_normal_map_path: " + temp);
				return false;
			}
		}
		
		batch->numIndices = mesh->woody_tree.get_branch_index_count();
		batch->StartIndex =0;
		batch->numVertices =mesh->woody_tree.get_branch_vertex_count();
	}
	
	// Load imposter branch texture
	if(strlen(material.imposter_branch_map_path_utf_8) > 0){
		batch = mesh->Lod2.CreateBatch();
		temp = pathtotree+ material.imposter_branch_map_path_utf_8;
		temp = (temp) + ".dds";
		if(batch->texture.Create(temp)){
			OUTPUT_DEBUG_MSG("Could not load material.imposter_branch_map_path_utf_8: " + temp);
			return false;
		}
	
		// Load bran#h normae texturm
		if(strlen(material.imposter_branch_normal_map_path_utf_8) > 0){	
			temp = pathtotree+ material.imposter_branch_normal_map_path_utf_8;
			temp = (temp) + ".dds";
			if(!batch->normal.Create(temp)){
				OUTPUT_DEBUG_MSG("Could not load material.imposter_branch_normal_map_path_utf_8: " + temp);
				return false;
			}
		}
		batch->numIndices = mesh->woody_tree.get_imposter_branch_index_count();
		batch->StartIndex =0;
		batch->numVertices =mesh->woody_tree.get_imposter_branch_vertex_count();
	}
	
	// Load leaf texture
	if(strlen(material.leaf_map_path_utf_8) > 0){	
		batch = mesh->Lod3.CreateBatch();
		temp = pathtotree+ material.leaf_map_path_utf_8;
		temp = (temp) + ".dds";
		if(batch->texture.Create(temp)){
			OUTPUT_DEBUG_MSG("Could not load material.leaf_map_path_utf_8: " + temp);
			return false;
		}

		// Load leaf normal texture
		if(strlen(material.leaf_normal_map_path_utf_8) > 0){
			temp = pathtotree+ material.leaf_normal_map_path_utf_8;
			temp = (temp) + ".dds";
			if(batch->normal.Create(temp)){
				OUTPUT_DEBUG_MSG("Could not load material.leaf_normal_map_path_utf_8: " + temp);
				return false;
			}
		}
		
		batch->numIndices = mesh->woody_tree.get_leaf_index_count();
		batch->StartIndex =0;
		batch->numVertices =mesh->woody_tree.get_leaf_vertex_count();
	}
	size_t count, stride;
	count= mesh->woody_tree.get_branch_vertex_count();
	stride =(mesh->woody_tree.get_branch_vertex_stride() * sizeof(wd_real_32));
	mesh->Lod1.VB[0].Create(count, stride, VERTEX_BUFFER, IMMUTABLE, CPU_NONE, mesh->woody_tree.get_branch_vertex_array() );
	mesh->Lod1.IB.Create(mesh->woody_tree.get_branch_index_count(), 2, INDEX_BUFFER, IMMUTABLE, CPU_NONE, mesh->woody_tree.get_branch_index_array() );

	mesh->Vertices.resize(mesh->woody_tree.get_branch_vertex_count());
	const float* arr = mesh->woody_tree.get_branch_vertex_array();
	for(size_t i(0); i< mesh->woody_tree.get_branch_vertex_count(); i++){
		mesh->Vertices[i].x = *arr;
		mesh->Vertices[i].y = *(arr+1);
		mesh->Vertices[i].z = *(arr+2);
		arr+=mesh->woody_tree.get_branch_vertex_stride();
	}
	mesh->Lod1.Indices.resize(mesh->woody_tree.get_branch_index_count());
	memcpy(&mesh->Lod1.Indices[0], mesh->woody_tree.get_branch_index_array(), sizeof(uint16_t)*mesh->woody_tree.get_branch_index_count());
	// Create imposter branch geometry buffers
	if(mesh->woody_tree.get_imposter_branch_vertex_count() > 0){

		count= mesh->woody_tree.get_imposter_branch_vertex_count();
		stride =(mesh->woody_tree.get_imposter_branch_vertex_stride() * sizeof(wd_real_32));

		mesh->Lod2.VB[0].Create(count, stride, VERTEX_BUFFER, IMMUTABLE, CPU_NONE, mesh->woody_tree.get_imposter_branch_vertex_array() );
		mesh->Lod2.IB.Create(mesh->woody_tree.get_imposter_branch_index_count(), 2, INDEX_BUFFER, IMMUTABLE, CPU_NONE, mesh->woody_tree.get_imposter_branch_index_array() );

		batch = mesh->Lod1.CreateBatch();
		batch->StartIndex = static_cast<uint32_t>(mesh->Lod1.Indices.size());
		batch->numIndices = mesh->woody_tree.get_imposter_branch_index_count();
		mesh->Lod1.Indices.resize(mesh->Lod1.Indices.size() + batch->numIndices);
		memcpy(&mesh->Lod1.Indices[batch->StartIndex], mesh->woody_tree.get_imposter_branch_index_array(), sizeof(uint16_t)*mesh->woody_tree.get_imposter_branch_index_count());
		
		count = mesh->Vertices.size();
		mesh->Vertices.resize(mesh->woody_tree.get_imposter_branch_vertex_count() + count);
		arr = mesh->woody_tree.get_imposter_branch_vertex_array();
		for(size_t i(0); i< mesh->woody_tree.get_imposter_branch_vertex_count(); i++){
			mesh->Vertices[i + count].x = *arr;
			mesh->Vertices[i + count].y = *(arr+1);
			mesh->Vertices[i + count].z = *(arr+2);
			arr+=mesh->woody_tree.get_imposter_branch_vertex_stride();
		}

	}
	// Create leaf geometry buffers
	if(mesh->woody_tree.get_leaf_vertex_count() > 0){
		count= mesh->woody_tree.get_leaf_vertex_count();
		stride =(mesh->woody_tree.get_leaf_vertex_stride() * sizeof(wd_real_32));

		mesh->Lod3.VB[0].Create(count, stride, VERTEX_BUFFER, IMMUTABLE, CPU_NONE, mesh->woody_tree.get_leaf_vertex_array() );
		mesh->Lod3.IB.Create(mesh->woody_tree.get_leaf_index_count(), 2, INDEX_BUFFER, IMMUTABLE, CPU_NONE, mesh->woody_tree.get_leaf_index_array() );

		batch = mesh->Lod1.CreateBatch();
		batch->StartIndex = static_cast<uint32_t>(mesh->Lod1.Indices.size());
		batch->numIndices = mesh->woody_tree.get_leaf_index_count();
		mesh->Lod1.Indices.resize(mesh->Lod1.Indices.size() + batch->numIndices);
		memcpy(&mesh->Lod1.Indices[batch->StartIndex], mesh->woody_tree.get_leaf_index_array(), sizeof(uint16_t)*mesh->woody_tree.get_leaf_index_count());

		count = mesh->Vertices.size();
		mesh->Vertices.resize(mesh->woody_tree.get_leaf_vertex_count() + count);
		arr = mesh->woody_tree.get_leaf_vertex_array();
		for(size_t i(0); i< mesh->woody_tree.get_leaf_vertex_count(); i++){
			mesh->Vertices[i + count].x = *arr;
			mesh->Vertices[i + count].y = *(arr+1);
			mesh->Vertices[i + count].z = *(arr+2);
			arr+=mesh->woody_tree.get_leaf_vertex_stride();
		}
	}

	// do not call free.... just these are fine
	mesh->woody_tree.free_directory();
	mesh->woody_tree.free_geometry();
	mesh->SetFileName(StripPath(xfile));
	return true;
}
#endif