struct PSPTNTIn {
	float4 position : SV_Position;
	float2 texcoord : TexCoord0;
	float3 normal   : Normal;
#if NORMALMAP ==1
	float3 tangent  : Tangent;
	float3 bitangent  : Bitangent;
#endif
};

cbuffer CBuffer0{
    float4x4 ObjectToWorld, ViewProjection, InvTransposeObjectToWorld;
};
cbuffer CBuffer1 {
	float4 Specular, Diffuse, Ambient, Emissive;
	float Power;
	float padd[3];
};
Texture2D Slot0, Slot1;
SamplerState sampler0;

PSPTNTIn VS(float3 position : Position, float2 texcoord : TexCoord, float3 normal : Normal, float3 tangent  : Tangent){
	PSPTNTIn Out;
	Out.texcoord = texcoord;
	Out.normal  = mul(float4(normal, 0.0f), InvTransposeObjectToWorld).xyz;
#if NORMALMAP ==1
	Out.tangent  = mul(float4(-tangent.xyz, 0.0f), InvTransposeObjectToWorld).xyz;
	Out.bitangent =  normalize(cross(normal, -tangent));
#endif
	Out.position = mul(mul(float4(position, 1.0f), ObjectToWorld), ViewProjection);
	return Out;
}

float4 PS(PSPTNTIn input): SV_Target0{
	float4 outcolor = Slot0.Sample(sampler0, input.texcoord);
	float3 Normal  = normalize(input.normal);
#if NORMALMAP ==1
	float3 Tangent = normalize(input.tangent);
	float3 Bitangent =normalize(input.bitangent); 
	float3 normal = Slot1.Sample(sampler0, input.texcoord).xyz*2.0-1;
	//Out.normal = float4(normalize(Tangent * normal.x + Bitangent * normal.y + Normal * normal.z), 1);
#else 
	//Out.normal = Normal;
#endif
	return outcolor;
}


