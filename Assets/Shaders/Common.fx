// empty file... . 

struct PSPTIn{
    float4 position : SV_Position;
    float2 texcoord : TexCoord;
	float fDepth : fDepth;
};

struct PSPIn{
    float4 position : SV_Position;
	float fDepth : fDepth;
};

struct PSPTNIn {
	float4 position : SV_Position;
	float2 texcoord : TexCoord0;
	float3 normal   : Normal;
};
struct PSPTNTIn {
	float4 position : SV_Position;
	float2 texcoord : TexCoord0;
	float3 normal   : Normal;
	float3 tangent  : Tangent;
	float3 bitangent  : Bitangent;
};
struct PSPTNTWIn {
	float4 position : SV_Position;
	float2 texcoord : TexCoord0;
	float3 normal   : Normal;
	float3 tangent  : Tangent;
	float3 bitangent  : Bitangent;
	float3 worldpos  : Position;
};


struct PSBSOut {
	float4 base   : SV_Target0;
	float4 normal : SV_Target1;
	float depth   : SV_Target2;
};
struct GS_OUTPUT_SHADOWMAP{
  float4 vPos : SV_Position;
  float fDepth : fDepth;
  uint RTIndex : SV_RenderTargetArrayIndex;
};

struct VSQuadOut{
    float4 position : SV_Position;
    float2 texcoord: TexCoord;
};

struct VSQuadOutMax{
    float4 position : SV_Position;
    float2 texcoord: TexCoord;
	float4 vWorldPos: Other_Pos;
};

#define OverDark .08f
#define  PI 3.141592653
VSQuadOut VSQuad(uint VertexID: SV_VertexID){// ouputs a full screen quad with tex coords
	VSQuadOut Out;
    Out.texcoord = float2( (VertexID << 1) & 2, VertexID & 2 );
    Out.position = float4( Out.texcoord * float2( 2.0f, -2.0f ) + float2( -1.0f, 1.0f), 0.0f, 1.0f );
    return Out;
}
VSQuadOut VSQuadWone(uint VertexID: SV_VertexID){// ouputs a full screen quad with tex coords
	VSQuadOut Out;
    Out.texcoord = float2( (VertexID << 1) & 2, VertexID & 2 );
    Out.position = float4( Out.texcoord * float2( 2.0f, -2.0f ) + float2( -1.0f, 1.0f), 0.0f, 1.0f );
	Out.texcoord = Out.position.xy*1.1f;
    return Out;
}

// these are in radians
inline float3x3 GenRoty(const float sin, const float cos){
	return float3x3( 
		cos , 0.0f, -sin,
		0.0f, 1.0f, 0.0f,
		sin , 0.0f, cos);
}
inline float3x3 GenRotx(const float sin, const float cos){
	return float3x3( 
		1.0f, 0.0f, 0.0f,
		0.0f, cos , sin,  
		0.0f, -sin, cos);
}
inline float3x3 GenRotz(const float sin, const float cos){
	return float3x3( 
		cos, sin, 0.0f,
		-sin, cos, 0.0f,
		0.0f, 0.0f, 1.0f);
}
/*
cbuffer PerFrame{
	float4x4 CameraView;
	float4x4 InvCameraView;
	float4x4 CameraProjection;
	float4x4 InvCameraProjection;
	float4x4 CameraViewProjection;
	float4x4 InvCameraViewProjection;
	float4x4 InvCameraViewProjectionFromScreen;
	float3   CameraPosition;
	float3   SunPosition;
	float3   SunNormal;// this is the normalized vector pointing opposite of the way the sun is looking.
	float2	 NearAndFarPlanes;
};
cbuffer PerObject{
	float4x4 ObjectToWorld;
	float4x4 InvTransposeObjectToWorld;
};

cbuffer MeshProperties{
	float4 MeshColor;
	//float4 Ambient;
	float4 Specular;
	//float4 Emissive;
	float Power;
};
*/
