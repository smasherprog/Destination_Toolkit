
cbuffer perfr{
	float4	 g_vPositionOffset;
	float4   g_vVelocity;
	float4   g_vAlpha;
	float4   g_vForward;
	float4x4 CameraViewProjectionPrev, CameraViewProjection;
	float4	CameraPosition;
	float2   g_vSizeScale;
	
};
#define g_vLighting float4(1, 1, 1, 1)

Texture2D Slot0;
SamplerState sampler0;

struct WeatherVS_OUTPUT{
    float4 position		: SV_Position;		//Pixel position (world-view-proj space)
    float2 uv			: TEXCOORD0;
    float4 colour		: COLOR0;		//Diffuse texture
	float3 worldPosition : TEXCOORD1;
};

#define PARTICLE_BOX 30.0f

WeatherVS_OUTPUT VS(float3 pos : Position, float2 uv : Texcoord){
	WeatherVS_OUTPUT output;
	
	output.uv = uv;
	float3 position = pos;
		
	// subtract the offset then fmod into (0.0f, PARTICLE_BOX)
	position.xyz -= g_vPositionOffset.xyz;
	position.xyz = fmod(position.xyz, PARTICLE_BOX);
	
	// centre box on origin
	position.xyz -= (PARTICLE_BOX/2);
	// push along view std::vector so box is positioned more infront of camera
	position.xyz += g_vForward.xyz;
	
	// calculate colour based on Lengthance from origin
	float subtractColour = length(position.xyz) / PARTICLE_BOX;
	float LengthanceColourScale = 1.0f - subtractColour * subtractColour;
	
	// get world position
	float4 worldPositionBottom = float4(position.xyz + CameraPosition.xyz, 1.0f);
	float4 worldPositionTop = float4(worldPositionBottom.xyz + g_vVelocity.xyz * g_vSizeScale.y, 1.0f);
		
	// get projected positions of top and bottom of particle, and top of particle in previous frame
	float4 bottom = mul(worldPositionBottom, CameraViewProjection);	

	float4 top = mul(worldPositionTop, CameraViewProjection);
	float4 topPrev = mul(worldPositionTop, CameraViewProjectionPrev);
	
	// get 2d std::vector in screenspace between top and bottom of particle
	float2 dir = (top.xy/top.w) - (bottom.xy/bottom.w);
	float2 dirPrev = (topPrev.xy/topPrev.w) - (bottom.xy/bottom.w);
	
	// calculate lengths
	float len = length(dir);
	float lenPrev = length(dirPrev);
	
	// scale colour depending on length
	float lenColourScale = saturate(len/lenPrev);
	
	// get 2d std::vector perpendicular to velocity
	float2 dirPerp = normalize(float2(-dirPrev.y, dirPrev.x));

	// choose either the top or bottom projected position using uv.y
	output.position = lerp(topPrev, bottom, output.uv.y);
	
	// offset the position of each side of the particle using uv.x 
	output.position.xy += (0.5f - output.uv.x) * dirPerp * g_vSizeScale.x;

	// get the final colour including the lighting, Lengthance and length scales, and per instance alpha
	output.colour = g_vLighting * LengthanceColourScale * lenColourScale * g_vAlpha.x;
	
	output.worldPosition = worldPositionBottom.xyz;
	
	return output;
}


//float4x4 g_mOcclusionMatrix;

//sampler2D g_tOcclusionHeightMap : register(s1);

float4 PS(WeatherVS_OUTPUT input) : SV_Target0{
	float4 colour = Slot0.Sample(sampler0, input.uv) * input.colour;
	#ifdef OCCLUSION
	// run the world position through the world to box space matrix
	float3 uv = mul(float4(input.worldPosition, 1.0f), g_mOcclusionMatrix).xyz;
	float height = tex2D(g_tOcclusionHeightMap, uv.xz).a;
	
	// clamp the uvs
	float3 uvclamp = saturate(uv);
	if(uvclamp.x == uv.x && uvclamp.z == uv.z && uv.y < height)
		colour = 0.0f;
	#endif
	return colour;
}




