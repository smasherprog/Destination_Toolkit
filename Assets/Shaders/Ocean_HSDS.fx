
#include "Common_Ocean.fx"

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
DUMMY PatchHS( InputPatch<HSIn_Heightfield, 1> inputPatch )
{
    return (DUMMY)0;
}




[domain("quad")]
VSQuadOutMax HeightFieldPatchDS(PatchData input, float2 uv : SV_DomainLocation, OutputPatch<DUMMY, 1> inputPatch )
{
    VSQuadOutMax output;
	float4 Pos = float4(0, 0, 0, 1);
	Pos.xz = input.origin + uv * input.size;
	output.texcoord = (input.origin + uv * input.size);

    float2 ux = output.texcoord;
    float2 uy = output.texcoord;

    float2 dux = abs(ux - output.texcoord) * 2.0;
    float2 duy = abs(uy - output.texcoord) * 2.0;
    
	Pos.y += texture2.SampleGrad(sampler0, float3(output.texcoord/ GRID_SIZES.x, 0.0), dux / GRID_SIZES.x, duy / GRID_SIZES.x).x;
	Pos.y += texture2.SampleGrad(sampler0, float3(output.texcoord/ GRID_SIZES.y, 0.0), dux / GRID_SIZES.y, duy / GRID_SIZES.y).y;
	Pos.y += texture2.SampleGrad(sampler0, float3(output.texcoord/ GRID_SIZES.z, 0.0), dux / GRID_SIZES.z, duy / GRID_SIZES.z).z;
	Pos.y += texture2.SampleGrad(sampler0, float3(output.texcoord/ GRID_SIZES.w, 0.0), dux / GRID_SIZES.w, duy / GRID_SIZES.w).w;
		Pos.xz += texture2.SampleGrad( sampler0, float3(output.texcoord / GRID_SIZES.x, 3.0), dux / GRID_SIZES.x, duy / GRID_SIZES.x).xy;
		Pos.xz += texture2.SampleGrad( sampler0, float3(output.texcoord / GRID_SIZES.y, 3.0), dux / GRID_SIZES.y, duy / GRID_SIZES.y).zw;
		Pos.xz += texture2.SampleGrad( sampler0, float3(output.texcoord / GRID_SIZES.z, 4.0), dux / GRID_SIZES.z, duy / GRID_SIZES.z).xy;
		Pos.xz += texture2.SampleGrad( sampler0, float3(output.texcoord / GRID_SIZES.w, 4.0), dux / GRID_SIZES.w, duy / GRID_SIZES.w).zw;

	output.vWorldPos = Pos;
	output.position = mul( output.vWorldPos , CameraViewProjection);

    return output;
}



