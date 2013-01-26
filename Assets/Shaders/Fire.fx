#define GRID_SIZE 24

cbuffer MatrixBuffer
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
	float2 texoffset_x;
};

cbuffer NoiseBuffer
{
	float frameTime;
	float3 scrollSpeeds;
	float3 scales;
	float pad;
};


//////////////
// TYPEDEFS //
//////////////
struct VertexInputType
{
	float3 position : POSITION;
};

struct PixelInputType
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
	float2 texCoords1 : TEXCOORD1;
	float2 texCoords2 : TEXCOORD2;
	float2 texCoords3 : TEXCOORD3;
};
struct RenPixelInputType
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
};
/////////////
// GLOBALS //
/////////////
Texture2D fireTexture;
Texture2D noiseTexture;
Texture2D alphaTexture;
SamplerState SampleType;
SamplerState SampleType2;

RenPixelInputType FireVertexShader(VertexInputType input)
{
	RenPixelInputType output;

	float4 pos = float4(input.position, 1);

	output.tex = float2(pos.x/2.0f +texoffset_x.x, -pos.y+1.0f);
	

	output.position = mul(pos, worldMatrix);
	output.position.xyz*=20;
	float3 fp = fireTexture.SampleLevel(SampleType, float2(output.tex.x, output.tex.y*frameTime/3), 0).xyz;
		fp.xz*=(output.tex.y + .2);
	output.position.xyz += fp;
	output.position = mul(output.position, viewMatrix);
	output.position = mul(output.position, projectionMatrix);
	
	return output;
}

float4 FirePixelShader(RenPixelInputType input) : SV_TARGET
{
	float4 f = fireTexture.Sample(SampleType, input.tex);

	if(input.tex.y>.9){
		f.a = min(f.a, (1.0f - input.tex.y)*10.0f);
	}
	f.a*=.9f;
	return f;
}

////////////////////////////////////////////////////////////////////////////////
// Vertex Shader
////////////////////////////////////////////////////////////////////////////////
PixelInputType Build_FireVertexShader(uint VertexID: SV_VertexID)
{
	PixelInputType output;

	output.tex = float2( (VertexID << 1) & 2, VertexID & 2 );
	output.position = float4( output.tex * float2( 2.0f, -2.0f ) + float2( -1.0f, 1.0f), 0.0f, 1.0f );


	// Compute texture coordinates for first noise texture using the first scale and upward scrolling speed values.
	output.texCoords1 = (output.tex * scales.x);
	output.texCoords1.y = output.texCoords1.y + (frameTime * scrollSpeeds.x);

	// Compute texture coordinates for second noise texture using the second scale and upward scrolling speed values.
	output.texCoords2 = (output.tex * scales.y);
	output.texCoords2.y = output.texCoords2.y + (frameTime * scrollSpeeds.y);

	// Compute texture coordinates for third noise texture using the third scale and upward scrolling speed values.
	output.texCoords3 = (output.tex * scales.z);
	output.texCoords3.y = output.texCoords3.y + (frameTime * scrollSpeeds.z);

	return output;
}


////////////////////////////////////////////////////////////////////////////////
// Filename: fire.ps
////////////////////////////////////////////////////////////////////////////////




cbuffer DistortionBuffer
{
	float2 distortion1;
	float2 distortion2;
	float2 distortion3;
	float distortionScale;
	float distortionBias;
};



////////////////////////////////////////////////////////////////////////////////
// Pixel Shader
////////////////////////////////////////////////////////////////////////////////
float4 Build_FirePixelShader(PixelInputType input) : SV_TARGET
{
	float4 noise1;
	float4 noise2;
	float4 noise3;
	float4 finalNoise;
	float perturb;
	float2 noiseCoords;
	float4 fireColor;
	float4 alphaColor;


	// Sample the same noise texture using the three different texture coordinates to get three different noise scales.
	noise1 = noiseTexture.Sample(SampleType, input.texCoords1);
	noise2 = noiseTexture.Sample(SampleType, input.texCoords2);
	noise3 = noiseTexture.Sample(SampleType, input.texCoords3);

	// Move the noise from the (0, 1) range to the (-1, +1) range.
	noise1 = (noise1 - 0.5f) * 2.0f;
	noise2 = (noise2 - 0.5f) * 2.0f;
	noise3 = (noise3 - 0.5f) * 2.0f;

	// Distort the three noise x and y coordinates by the three different distortion x and y values.
	noise1.xy = noise1.xy * distortion1.xy;
	noise2.xy = noise2.xy * distortion2.xy;
	noise3.xy = noise3.xy * distortion3.xy;

	// Combine all three distorted noise results into a single noise result.
	finalNoise = noise1 + noise2 + noise3;

	// Perturb the input texture Y coordinates by the distortion scale and bias values.  
	// The perturbation gets stronger as you move up the texture which creates the flame flickering at the top effect.
	perturb = ((1.0f - input.tex.y) * distortionScale) + distortionBias;

	// Now create the perturbed and distorted texture sampling coordinates that will be used to sample the fire color texture.
	noiseCoords.xy = (finalNoise.xy * perturb) + input.tex.xy;

	// Sample the color from the fire texture using the perturbed and distorted texture sampling coordinates.
	// Use the clamping sample state instead of the wrap sample state to prevent flames wrapping around.
	fireColor = fireTexture.Sample(SampleType2, noiseCoords.xy);

	// Sample the alpha value from the alpha texture using the perturbed and distorted texture sampling coordinates.
	// This will be used for transparency of the fire.
	// Use the clamping sample state instead of the wrap sample state to prevent flames wrapping around.
	alphaColor = alphaTexture.Sample(SampleType2, noiseCoords.xy);

	// Set the alpha blending of the fire to the perturbed and distored alpha texture value.
	fireColor.a = alphaColor.x;

	return fireColor;
}
