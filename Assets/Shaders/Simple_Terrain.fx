#include "Common.fx"

#define TEXSCALE 30.0f
#define TEXSIZE 2048
#define WORLDSIZE TEXSIZE*3

cbuffer buff0{
	float4x4 ViewProjection;
	float4 CameraPosition;
};
cbuffer buff1{
	int4 EditorTex;
};

Texture2DArray Heights, Normals, Tangents, Uvs;

struct VS_to_PS_TerrainLod0{
    float4 pos      : SV_Position;
    float2 texcoords : TEXCOORD0;
	float2 texalphas : TEXCOORD1;
	uint2 texindicies : BLENDINDICES;
    float3 norm     : NORMAL;
    float3 tangent  : TANGENT;
    float3 binorm   : BINORMAL;
};

Texture2DArray ArraySlot0, ArraySlot1;
SamplerState sampler0;


struct DUMMY
{
	float Dummmy : DUMMY;
};

struct HSIn_Heightfield
{
    float2 origin   : ORIGIN;
    float2 size     : SIZE;
};
struct PatchData
{
    float Edges[4]  : SV_TessFactor;
    float Inside[2]	: SV_InsideTessFactor;

	float2 origin   : ORIGIN;
    float2 size     : SIZE;
};
float CalculateTessellationFactor(float distance)
{
	return 50*(1/(0.015*distance));
}
float CalculateMIPLevelForDisplacementTextures(float distance)
{
	return log2(128/CalculateTessellationFactor(distance));
}
PatchData PatchConstantHS( InputPatch<HSIn_Heightfield, 1> inputPatch )
{    
    PatchData output;

	float distance_to_camera;
	float tesselation_factor;
	float inside_tessellation_factor=0;
	float in_frustum=0;

	output.origin = inputPatch[0].origin;
	output.size = inputPatch[0].size;

		distance_to_camera=length(CameraPosition.xz-inputPatch[0].origin-float2(0,inputPatch[0].size.y*0.5));
		tesselation_factor=CalculateTessellationFactor(distance_to_camera);
		output.Edges[0] =  tesselation_factor;
		inside_tessellation_factor+=tesselation_factor;


		distance_to_camera=length(CameraPosition.xz-inputPatch[0].origin-float2(inputPatch[0].size.x*0.5,0));
		tesselation_factor=CalculateTessellationFactor(distance_to_camera);
		output.Edges[1] =  tesselation_factor;
		inside_tessellation_factor+=tesselation_factor;

		distance_to_camera=length(CameraPosition.xz-inputPatch[0].origin-float2(inputPatch[0].size.x,inputPatch[0].size.y*0.5));
		tesselation_factor=CalculateTessellationFactor(distance_to_camera);
		output.Edges[2] =  tesselation_factor;
		inside_tessellation_factor+=tesselation_factor;

		distance_to_camera=length(CameraPosition.xz-inputPatch[0].origin-float2(inputPatch[0].size.x*0.5,inputPatch[0].size.y));
		tesselation_factor=CalculateTessellationFactor(distance_to_camera);
		output.Edges[3] =  tesselation_factor;
		inside_tessellation_factor+=tesselation_factor;
		output.Inside[0] = output.Inside[1] = inside_tessellation_factor*0.25;

    return output;
}

[domain("quad")]
[partitioning("fractional_odd")]
[outputtopology("triangle_ccw")]
[outputcontrolpoints(1)]
[patchconstantfunc("PatchConstantHS")]
DUMMY HS( InputPatch<HSIn_Heightfield, 1> inputPatch )
{
    return (DUMMY)0;
}

[domain("quad")]
VS_to_PS_TerrainLod0 DS(PatchData input, float2 uv : SV_DomainLocation, OutputPatch<DUMMY, 1> inputPatch )
{
   VS_to_PS_TerrainLod0 Out;
	float3 Pos = float3(0, 0, 0);
	Pos.xz = input.origin + uv * input.size;
	float disttocamera = length(Pos - CameraPosition.xyz);
	float miplevel = CalculateMIPLevelForDisplacementTextures(disttocamera);

	Out.texcoords= (input.origin + uv * input.size)/WORLDSIZE;
	int2 indicies = Pos.xz*(1.0f/TEXSIZE);// get which textures to index to

	uint2 info = Uvs.Load(int4(Out.texcoords, indicies.x + indicies.y*3, 0)).xy;// no filtering
	Out.texalphas.x=info.y*(1.0f/255.0f);
	Out.texalphas.y = 1.0f - Out.texalphas.x;
	Out.texindicies= uint2(info.x&(0x000000f0), info.x&(0x0000000f));
	Pos.y = Heights.Load(int4(Out.texcoords, indicies.x + indicies.y*3, 0)).x;// no filtering
	Out.norm = Normals.Load(int4(Out.texcoords, indicies.x + indicies.y*3, 0)).xyz;// no filtering
	Out.tangent  = Tangents.Load(int4(Out.texcoords, indicies.x + indicies.y*3, 0)).xyz;// no filtering
	Out.binorm =  normalize(cross(Out.norm, Out.tangent));
	Out.pos = mul(float4(Pos, 1.0f), ViewProjection);
	return Out;
}


[domain("quad")]
VS_to_PS_TerrainLod0 DSEditor(PatchData input, float2 uv : SV_DomainLocation, OutputPatch<DUMMY, 1> inputPatch )
{
   VS_to_PS_TerrainLod0 Out;
	float3 Pos = float3(0, 0, 0);
	Pos.xz = input.origin + uv * input.size;
	float disttocamera = length(Pos - CameraPosition.xyz);
	float miplevel = CalculateMIPLevelForDisplacementTextures(disttocamera);

	Out.texcoords= (input.origin + uv * input.size)/WORLDSIZE;
	int2 indicies = Pos.xz*(1.0f/TEXSIZE);// get which textures to index to

	uint2 info = Uvs.Load(int4(Out.texcoords, indicies.x + indicies.y*3, 0)).xy;// no filtering
	Out.texalphas.x=1;
	Out.texalphas.y = 0;
	Out.texindicies= uint2(EditorTex.x, 0);
	Pos.y = Heights.Load(int4(Out.texcoords, indicies.x + indicies.y*3, 0)).x + 3.0f;// no filtering
	Out.norm = Normals.Load(int4(Out.texcoords, indicies.x + indicies.y*3, 0)).xyz;// no filtering
	Out.tangent  = Tangents.Load(int4(Out.texcoords, indicies.x + indicies.y*3, 0)).xyz;// no filtering
	Out.binorm =  normalize(cross(Out.norm, Out.tangent));
	Out.pos = mul(float4(Pos, 1.0f), ViewProjection);
	return Out;
}


///////////////////////////////////////////////////////////////pixel shader definitions

float4 PS(VS_to_PS_TerrainLod0 In): SV_Target0{//AnisotropicFilter

	float4 bumps0 = (ArraySlot1.Sample(sampler0, float3(In.texcoords.xy, In.texindicies.x) )*2.0-1.0)*In.texalphas.x;
    float4 bumps1 = (ArraySlot1.Sample(sampler0, float3(In.texcoords.xy, In.texindicies.y) )*2.0-1.0)*In.texalphas.y;
	float4 albedo0 = (ArraySlot0.Sample(sampler0, float3(In.texcoords.xy, In.texindicies.x)))*In.texalphas.x;
    float4 albedo1 = (ArraySlot0.Sample(sampler0, float3(In.texcoords.xy, In.texindicies.y)))*In.texalphas.y;
    // combine using sampled alphas
    float3 finalAlbedo = albedo0.xyz + albedo1.xyz;
    float3 finalBumps = bumps0.xyz + bumps1.xyz;

    finalBumps = normalize(finalBumps);
    
    // lighting
    float3 Normal = normalize(In.norm);
    float3 Tangent = normalize(In.tangent);
    float3 Bitangent = normalize(In.binorm);
	/*
	float rad = acos(SunNormal.x)/3.14;
	float shadowterm =1;
	if(rad < In.shadow.x) shadowterm =0;
	if(rad < In.shadow.y) shadowterm =0;
	Out.base = float4(In.shadow.y,0, 0, 30);
	*/
	//Out.base = float4(finalAlbedo, 30);
	//Out.normal = float4(normalize(finalBumps.x*Tangent.xyz+finalBumps.y*Bitangent.xyz+finalBumps.z*Normal.xyz), 1);
	
	return float4(finalAlbedo, 0);
}
