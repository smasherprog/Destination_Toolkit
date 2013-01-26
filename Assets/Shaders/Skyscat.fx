#include "Common.fx"
#define  PI 3.141592653
#define  fExposure -2.0
#define  InnerRadius 6356.7523142
#define  OuterRadius = 6356.7523142 * 1.015
#define  fScale 1.0 / (6452.103598913 - 6356.7523142)
#define  KrESun 0.0025 * 20.0
#define  KmESun 0.0010 * 20.0
#define  Kr4PI 0.0025 * 4.0 * PI
#define  Km4PI 0.0010 * 4.0 * PI
#define  tNumSamples  50
#define  invtNumSamples  1.0f/50
#define  iNumSamples  20
#define  inviNumSamples 1.0f/20.0f
#define  v2dRayleighMieScaleHeight float2( 0.25, 0.1 )
#define  invv2dRayleighMieScaleHeight float2( 1.0f/0.25, 1.0f/0.1 )
#define  WavelengthMie  float3( pow( 0.650, -0.84 ), pow( 0.570, -0.84 ), pow( 0.475, -0.84 ) )
#define  InvOpticalDepthN 1.0 / 256.0
#define  v3HG float3( 1.5f * ( (1.0f - (-0.995*-0.995)) / (2.0f + (-0.995*-0.995)) ), 1.0f + (-0.995*-0.995), 2.0f * -0.995 )
#define  InvOpticalDepthNLessOne 1.0 / ( 256.0 - 1.0 )
#define  InvRayleighMieN  float2( 1.0 / 256.0, 1.0 / 128.0 )
#define  InvRayleighMieNLessOne  float2( 1.0 / (256.0 - 1.0), 1.0 / (128.0 - 1.0) )
#define  HalfTexelOpticalDepthN 0.5 / 256.0
#define  InvWavelength4 float3( 1.0 / pow( 0.650, 4 ), 1.0 / pow( 0.570, 4 ), 1.0 / pow( 0.475, 4 ) )
#define  OuterRadius2 6356.7523142 * 1.015 * 6356.7523142 * 1.015
#define  inv180 1.0f/180.0f

Texture2D Slot0, Slot1;

SamplerState sampler0;

cbuffer buffer0{
	float4x4 ObjectToWorld;
	float4 SunNormal;
};
cbuffer buffer1{
	float4x4 ViewCMProj[6];
};
struct PS_INPUT{
    float4 Pos : SV_POSITION;
    float2 Tex0 : TEXCOORD0;
	float3 Tex1 : TEXCOORD1;
};


struct PS_INPUT_UPDATE{
    float4 Pos : SV_POSITION;
};

struct PS_OUTPUT_UPDATE{
    float4 RayLeigh : SV_TARGET0;
	float4 Mie : SV_TARGET1;
};

float3 HDR( float3 LDR){
	return 1.0f - exp( fExposure * LDR );
}

float3 ToneMap( float3 HDR){
	return (HDR / (HDR + 1.0f));
}

float getMiePhase(float fCos, float fCos2){
	return v3HG.x * (1.0 + fCos2) / pow(abs(v3HG.y - v3HG.z * fCos), 1.5);
}

float2 GetDensityRatio( float fHeight ){
	const float fAltitude = (fHeight - InnerRadius) * fScale;
	return exp( -fAltitude * invv2dRayleighMieScaleHeight );
}
float HitOuterSphere( float3 O, float3 Dir ) {
	float B = dot( -O, Dir );
	float C = dot( O, O );
	return B + sqrt( OuterRadius2 - (C - B * B) );
}

float2 t( float3 P, float3 Px ){
	float3 v3Vector =  Px - P;
	float fFar = length( v3Vector );
	float3 v3Dir = v3Vector / fFar;		
	float fSampleLength = fFar * invtNumSamples;
	float fScaledLength = fSampleLength * fScale;
	float3 v3SampleRay = v3Dir * fSampleLength;
	P += v3SampleRay * 0.5f;		
	float2 OpticalDepth = 0;
	for(int i = 0; i < tNumSamples; i++){
		float fHeight = length( P );
		OpticalDepth += GetDensityRatio( fHeight );
		P += v3SampleRay;
	}		
	OpticalDepth *= fScaledLength;
	return OpticalDepth;
}
struct GS_INPUT
{
    float3 Pos : POSITION;
    float2 Tex0 : TEXCOORD0;
};
GS_INPUT VS_CUBEMAP( float3 Pos : POSITION, float2 Tex0 : TEXCOORD0)
{
    GS_INPUT output;
	output.Pos =  Pos;
    output.Tex0 = Tex0; 
    return output;
}
struct PS_CUBEMAP_INPUT
{
    float4 Pos : SV_POSITION;
    float2 Tex0 : TEXCOORD0;
	float3 Tex1 : TEXCOORD1;
	uint RTIndex : SV_RenderTargetArrayIndex;
};
[maxvertexcount(18)]
void GS( triangle GS_INPUT input[3], inout TriangleStream<PS_CUBEMAP_INPUT> CubeMapStream )
{
    for( uint f = 0; f < 6; ++f )
    {
        // Compute screen coordinates
        PS_CUBEMAP_INPUT output;
        output.RTIndex = f;
        for( uint v = 0; v < 3; v++ )
        {
            output.Pos = mul( ViewCMProj[f], float4( input[v].Pos.xyz, 1) );
            output.Tex0 = input[v].Tex0;
			output.Tex1 = - input[v].Pos;
            CubeMapStream.Append( output );
        }
        CubeMapStream.RestartStrip();
    }
}
PS_INPUT VS( float4 Pos : POSITION, float2 Tex0 : TEXCOORD0){
    PS_INPUT output = (PS_INPUT)0;
	output.Pos =  mul( float4( Pos.xyz, 1), ObjectToWorld ); //ObjectToWorld is not actually what the name implies.. this is the full world to view projection
    output.Tex0 = Tex0; 
    output.Tex1 = - Pos.xyz;
    return output;
}
float4 PS( PS_INPUT input) : SV_Target0{
	float fCos = dot( SunNormal.xyz, input.Tex1 ) / length( input.Tex1 );
	float fCos2 = fCos * fCos;
	float3 v3RayleighSamples = Slot1.Sample( sampler0, input.Tex0.xy ).xyz;
	float3 v3MieSamples = Slot0.Sample( sampler0, input.Tex0.xy ).xyz;
	float3 Color = (0.75 * (1.0 + fCos2)) * v3RayleighSamples.rgb + getMiePhase(fCos, fCos2) * v3MieSamples.rgb;
	Color.rgb = HDR( Color.rgb );
	return float4( Color.rgb, 1 );
}

PS_OUTPUT_UPDATE PS_UPDATE_FAST( VSQuadOut input){
	PS_OUTPUT_UPDATE output = (PS_OUTPUT_UPDATE)0;
	float2 Tex0 = (input.position.xy - 0.5) * InvRayleighMieNLessOne.xy; // [0..1]
	float2 Tex1 = (input.position.xy - 0.5) * InvRayleighMieN.xy + HalfTexelOpticalDepthN; // [0.5/Texsize .. (Texsize-0.5)/Texsize]
	const float3 v3PointPv = float3( 0, InnerRadius + 1e-3, 0 );
	const float AngleY = 100.0 * Tex0.x * PI *inv180;// [0..100]
	const float AngleXZ = PI * Tex0.y;// [0..PI]	
	float3 v3Dir;
	float say = sin( AngleY );
	v3Dir.x = say* cos( AngleXZ  );
	v3Dir.y = cos( AngleY );
	v3Dir.z = say * sin( AngleXZ  );
	v3Dir = normalize( v3Dir );
	float fFarPvPa = HitOuterSphere( v3PointPv , v3Dir );
	float3 v3Ray = v3Dir;
	float3 v3PointP = v3PointPv;
	float fSampleLength = fFarPvPa * inviNumSamples;
	float fScaledLength = fSampleLength * fScale;
	float3 v3SampleRay = v3Ray * fSampleLength;
	v3PointP += v3SampleRay * 0.5f;			
	float3 v3RayleighSum = 0;
	float3 v3MieSum = 0;
	float SampleU = HalfTexelOpticalDepthN;
	float2 Mies = 0;
	for( int k = 0; k < iNumSamples; k++ ){
		float PointPHeight = length( v3PointP );
		float2 DensityRatio = GetDensityRatio( PointPHeight );
		DensityRatio *= fScaledLength;
		float2 ViewerOpticalDepth = Slot0.Sample( sampler0, float2( Tex1.x, SampleU ) ).rg;// sampler0 == linearclamp
		float fAngle = dot(v3PointP, SunNormal.xyz) / length(v3PointP);
		float index = ((1.0 - fAngle) * 9.0/ 10.0) * 127.0/128.0;
		float2 SunOpticalDepth = Slot0.Sample( sampler0, float2( index, SampleU ) ).ba;
		float2 OpticalDepthP = SunOpticalDepth.xy + ViewerOpticalDepth.xy;
		float3 v3Attenuation = exp( - Kr4PI * InvWavelength4 * OpticalDepthP.x - Km4PI * OpticalDepthP.y );
		v3RayleighSum += DensityRatio.x * v3Attenuation;
		v3MieSum += DensityRatio.y * v3Attenuation;
		v3PointP += v3SampleRay;
		SampleU += inviNumSamples;
	}
	float3 RayLeigh = v3RayleighSum * KrESun;
	float3 Mie = v3MieSum * KmESun;
	RayLeigh *= InvWavelength4;
	Mie *= WavelengthMie;	
	output.RayLeigh = float4( RayLeigh, 1 );
	output.Mie = float4( Mie, 1 );
	return output;
}


float4 PS_CREATE_OPTICALDEPTH( VSQuadOut input) : SV_Target0{
	float2 Tex0 = (input.position.xy - 0.5) * float2( InvOpticalDepthNLessOne, InvOpticalDepthN ); // [0..1] x [0..127.0/128.0]
	float3 v3PointPv = float3( 0, InnerRadius + 1e-3, 0 );
	float AngleY = 100.0 * Tex0.x * PI / 180.0; // [0..rad(100)]
	float3 v3Ray;
	v3Ray.x = sin( AngleY );
	v3Ray.y = cos( AngleY );
	v3Ray.z = 0;
	float fFarPvPa = HitOuterSphere( v3PointPv , v3Ray );
	float3 v3PointP = v3PointPv;
	float fSampleLength = fFarPvPa / iNumSamples;
	float3 v3SampleRay = v3Ray * fSampleLength;
	v3PointP += v3SampleRay * 0.5f;
	v3PointP += v3SampleRay * ( Tex0.y * iNumSamples );// [0.5 * SampleRay .. iNumSample * SampleRay]
	float2 ViewerOpticalDepth = t( v3PointP, v3PointPv );
    float2 SunOpticalDepth = t( v3PointP, v3PointPv + v3Ray * fFarPvPa ) ;
	return float4( ViewerOpticalDepth.xy, SunOpticalDepth.xy );
}
