struct PSPTNTIn {
	float4 position : SV_Position;
	float2 texcoord : TexCoord0;
	float3 normal   : Normal;

	float3 tangent  : Tangent;
	float3 bitangent  : Bitangent;

};
cbuffer CBuffer0{
    float4x4 ObjectToWorld, ViewProjection, InvTransposeObjectToWorld;
};
cbuffer CBuffer1 {
	float4 Specular, Diffuse, Ambient, Emissive;
	float Power;
	float padd[3];
};

cbuffer CBuffer2{
	float4x4 bone_matrices[MATRIX_PALETTE_SIZE_DEFAULT];
}

Texture2D Slot0, Slot1;
SamplerState sampler0;

PSPTNTIn VS(float3 vPos : POSITION, float2 vTexCoord0 : TEXCOORD0, float3 vNormal : NORMAL, float3 vTangent : TANGENT, float4 vBones : BONE, float4 vWeights : BLENDWEIGHT){
    PSPTNTIn output;
    float3 vNormalWorldSpace;
                
    float4x4 finalMatrix;
    finalMatrix = vWeights.x * bone_matrices[vBones.x];
    finalMatrix += vWeights.y * bone_matrices[vBones.y];
    finalMatrix += vWeights.z * bone_matrices[vBones.z];
    finalMatrix += vWeights.w * bone_matrices[vBones.w];    
    
    float4 vAnimatedPos = mul(float4(vPos.xyz,1),finalMatrix);
    float4 vAnimatedNormal = mul(float4(vNormal.xyz,0),finalMatrix);
    float4 vAnimatedTangent = mul(float4(vTangent.xyz,0),finalMatrix);
        
    // Transform the position from object space to homogeneous projection space
	output.position = mul(mul(vAnimatedPos,ObjectToWorld), ViewProjection);

    // Transform the normal from object space to world space    
	output.normal = mul(vAnimatedNormal.xyz, (float3x3)InvTransposeObjectToWorld); // normal to world space

    output.tangent = mul(vAnimatedTangent.xyz, (float3x3)InvTransposeObjectToWorld);
	output.bitangent = cross(output.normal ,output.tangent);

	output.texcoord.xy = vTexCoord0;

    return output;    
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
