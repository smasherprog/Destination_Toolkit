//=============================================================================
// lighthelper.fx by Frank Luna (C) 2008 All Rights Reserved.
//
// Structures and functions for lighting calculations.
//=============================================================================


struct Light
{
	float3 pos;
	float3 dir;
	float4 ambient;
	float4 diffuse;
	float4 spec;
	float3 att;
	float  spotPower;
	float  range;
};

struct SurfaceInfo
{
	float3 pos;
    float3 normal;
    float3 diffuse;
    float spec;
};
float3 ParallelLight(SurfaceInfo v, Light L, float3 eyePos, float shadowFactor){
	float3 litColor = float3(0.0f, 0.0f, 0.0f);
 
	// The light vector aims opposite the direction the light rays travel.
	float3 lightVec = -L.dir;
	
	// Add the ambient term.
	litColor += v.diffuse * L.ambient;	
	
	// Add diffuse and specular term, provided the surface is in 
	// the line of site of the light.
	
	float diffuseFactor = dot(lightVec, v.normal);
	[branch]
	if( diffuseFactor > 0.0f ){
		float specPower   = max(v.spec, 1.0f);
		float3 toEye      = normalize(eyePos - v.pos);
		float3 reflectVec = reflect(-lightVec, v.normal);
		float specFactor  = pow(max(dot(reflectVec, toEye), 0.0f), specPower);
					
		// diffuse and specular terms
		litColor += shadowFactor * diffuseFactor * v.diffuse * L.diffuse;
		litColor += shadowFactor * specFactor * L.spec;
	}
	
	return litColor;
}
