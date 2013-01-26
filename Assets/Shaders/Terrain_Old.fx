
#include "Common.fx"
static const float InvRow = 1.0f/64.0f;

#define TEXSCALE 30.0f
cbuffer buff0{
	float4x4 InvTransposeObjectToWorld;
	float4x4 ViewProjection;
	float4 TexScale;
};

cbuffer buff1{
	int Split;
};
cbuffer buff2{
	float4 MeshColor;
};
cbuffer buff3{
	float4x4 ObjectToWorldShadow;
	float4x4 ShadowMapSplits[4];
	float4 SunPosition;
};
struct VS_to_PS_TerrainLod0{
    float4 pos      : SV_Position;
	float2 shadow	: TEXCOORD2;
    float2 texcoords : TEXCOORD0;
	float2 texalphas : TEXCOORD1;
	uint2 texindicies : BLENDINDICES;
    float3 norm     : NORMAL;
    float3 tangent  : TANGENT;
    float3 binorm   : BINORMAL;
};
struct VS_to_PS_TerrainLod1{
    float4 pos      : SV_Position;
    float2 texcoords     : TEXCOORD0;
	float2 texalphas : TEXCOORD1;
	float2 shadow	: TEXCOORD2;
	uint2 texindicies      : BLENDINDICES;
    float3 norm     : NORMAL;
};
Texture2DArray ArraySlot0, ArraySlot1;
SamplerState sampler0;
VS_to_PS_TerrainLod0 VSLod0(float2 pos : Position ,float4 normal : Normal, float4 tangent : Tangent, float4 shadowterm : TEXCOORD0, uint4  tex : BLENDINDICES0){
	VS_to_PS_TerrainLod0 Out;
	Out.texcoords=(pos*InvRow)*(TexScale*TEXSCALE);
	Out.texalphas=tex.xz * 1.0f/255.0f;//alphavalue
	Out.texindicies=tex.yw;// indecies of the textures
	Out.shadow = shadowterm.xy;
	Out.norm = mul(float4(normal.xyz, 0.0f), InvTransposeObjectToWorld).xyz;
	Out.tangent  = mul(float4(tangent.xyz, 0.0f), InvTransposeObjectToWorld).xyz;
	Out.binorm =  normalize(cross(Out.norm, Out.tangent));
	Out.pos = mul(float4(pos.x, normal.w, pos.y, 1.0f), ViewProjection);
	return Out;
}


///////////////////////////////////////////////////////////////pixel shader definitions

PSBSOut PSLod0(VS_to_PS_TerrainLod0 In){//AnisotropicFilter
	PSBSOut Out;

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
	Out.base = float4(finalAlbedo, 30);
	Out.normal = float4(normalize(finalBumps.x*Tangent.xyz+finalBumps.y*Bitangent.xyz+finalBumps.z*Normal.xyz), 1);
	Out.depth = In.pos.z;
	return Out;
}

VS_to_PS_TerrainLod1 VSLod1(float2 pos : Position, float4 normal : Normal, float4 tangent : Tangent , float4 shadowterm : TEXCOORD0, uint4  tex : BLENDINDICES){
	VS_to_PS_TerrainLod1 Out;
	Out.texcoords=(pos*InvRow)*(TexScale*TEXSCALE);
	Out.texalphas=tex.xz*1.0f/255.0f;
	Out.texindicies=tex.yw;
	Out.shadow = shadowterm.xy;
	Out.norm = mul(float4(normal.xyz, 0.0f), InvTransposeObjectToWorld).xyz;
	Out.pos = mul(float4(pos.x, normal.w, pos.y, 1.0f),ViewProjection);
	return Out;
}

PSBSOut PSLod1(VS_to_PS_TerrainLod1 In){
	PSBSOut Out;
	float4 albedo0 = ArraySlot0.Sample(sampler0,float3(In.texcoords.xy, In.texindicies.x) )*In.texalphas.x;
    float4 albedo1 = ArraySlot0.Sample(sampler0,float3(In.texcoords.xy, In.texindicies.y) )*In.texalphas.y;
	float3 finalAlbedo = albedo0.xyz+ albedo1.xyz;
	Out.base = float4(finalAlbedo, 30);
	Out.normal = float4(normalize(In.norm), 30);
	Out.depth = In.pos.z;
	return Out;
}

PSPIn VSShadow(float2 pos : Position, float4 normal : Normal, float4 tangent : Tangent){
	PSPIn output;
    float4 worldPos = mul( float4(pos.x, normal.w, pos.y, 1.0f), ObjectToWorldShadow );
    output.position = mul(worldPos, ShadowMapSplits[Split]);
    output.fDepth = length(SunPosition.xyz - worldPos.xyz );
    return output;
}
float4 PSShadow( PSPIn input ) : SV_Target{
    return float4(input.fDepth, 0, 0, 1);
}
cbuffer TextureIndex{
	float TexIndex;
};
/*
VSQuadOut VSEditor(float4 position : Position0, float height : Position1){
	VSQuadOut output;
	output.position = mul(float4(position.x, 0, position.y, 1), ObjectToWorld);
	output.position.y=height;
	output.position = mul(output.position  , CameraViewProjection);
	output.texcoord = position.zw;
    return output;
}
float4 PSEditor( VSQuadOut input ) : SV_Target0{
	return ArraySlot0.Sample(sampler0, float3(input.texcoord, TexIndex));
}
float4 PSEditorwotexture( VSQuadOut input ) : SV_Target0{
	return float4(MeshColor.rgb, 1);
}

technique10 EditorwTexture {
    pass P0 {
        SetVertexShader( CompileShader( vs_4_0, VSEditor() ) );
        SetGeometryShader(NULL);
        SetPixelShader( CompileShader( ps_4_0, PSEditor() ) );
	
     }
}
technique10 EditorwoTexture {
    pass P0 {
        SetVertexShader( CompileShader( vs_4_0, VSEditor() ) );
        SetGeometryShader(NULL);
        SetPixelShader( CompileShader( ps_4_0, PSEditorwotexture() ) );
	
     }
}
technique10 BuildShadowMap {
    pass P0 {
        SetVertexShader( CompileShader( vs_4_0, VSShadow() ) );
        SetGeometryShader(NULL);
        SetPixelShader( CompileShader( ps_4_0, PSShadow() ) );
	
     }
}
technique10 Lod0 {
    pass P0 {
        SetVertexShader( CompileShader( vs_4_0, VSLod0() ) );
        SetGeometryShader(NULL);
        SetPixelShader( CompileShader( ps_4_0, PSLod0() ) );
	
     }
}
technique10 Lod1 {
    pass P0 {
        SetVertexShader( CompileShader( vs_4_0, VSLod1() ) );
        SetGeometryShader(NULL);
        SetPixelShader( CompileShader( ps_4_0, PSLod1() ) );
     }
}

*/