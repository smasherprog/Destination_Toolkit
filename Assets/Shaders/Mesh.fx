
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
struct PSPIn{
    float4 position : SV_Position;
	float fDepth : fDepth;
};
cbuffer CBuffer0{
    float4x4 ObjectToWorld, CameraViewProjection, InvTransposeObjectToWorld;
};

#ifndef MATRIX_PALETTE_SIZE_DEFAULT 
    #define MATRIX_PALETTE_SIZE_DEFAULT 60
#endif

cbuffer cbuffer1{
	float4x4    g_matrices[MATRIX_PALETTE_SIZE_DEFAULT];
}

struct PSBSOut {
	float4 base   : SV_Target0;
	float4 normal : SV_Target1;
	float depth   : SV_Target2;
};

Texture2D Slot0, Slot1;
SamplerState sampler0;
#if (QUALITY == 0)
PSPTNIn VS(float3 vPos : POSITION, float2 vTexCoord0 : TEXCOORD0, float3 vNormal : NORMAL, float3 vTangent : TANGENT, float4 vBones : BONE, float4 vWeights : BLENDWEIGHT){
    PSPTNIn output;
    float3 vNormalWorldSpace;
                
    matrix finalMatrix;
    finalMatrix = vWeights.x * g_matrices[vBones.x];
    finalMatrix += vWeights.y * g_matrices[vBones.y];
    finalMatrix += vWeights.z * g_matrices[vBones.z];
    finalMatrix += vWeights.w * g_matrices[vBones.w];    
    
    float4 vAnimatedPos = mul(float4(vPos.xyz,1),finalMatrix);
    float4 vAnimatedNormal = mul(float4(vNormal.xyz,0),finalMatrix);
        
    // Transform the position from object space to homogeneous projection space
	output.position = mul(mul(vAnimatedPos,ObjectToWorld), CameraViewProjection);

    // Transform the normal from object space to world space    
	output.normal = mul(vAnimatedNormal.xyz, (float3x3)InvTransposeObjectToWorld); // normal to world space

	output.texcoord.xy = vTexCoord0 ;
    return output;    
}
PSBSOut PS( PSPTNIn input){
	PSBSOut Out;
    	Out.base = Slot0.Sample(sampler0,input.texcoord.xy);
	Out.base.a = 10;// put the specular in
	Out.normal = float4(input.normal, 1);
	Out.depth = input.position.z;
	return Out;
}
#elif (QUALITY == 1)
PSPTNTIn VS(float3 vPos : POSITION, float2 vTexCoord0 : TEXCOORD0, float3 vNormal : NORMAL, float3 vTangent : TANGENT, float4 vBones : BONE, float4 vWeights : BLENDWEIGHT){
    PSPTNTIn output;
    float3 vNormalWorldSpace;
                
    matrix finalMatrix;
    finalMatrix = vWeights.x * g_matrices[vBones.x];
    finalMatrix += vWeights.y * g_matrices[vBones.y];
    finalMatrix += vWeights.z * g_matrices[vBones.z];
    finalMatrix += vWeights.w * g_matrices[vBones.w];    
    
    float4 vAnimatedPos = mul(float4(vPos.xyz,1),finalMatrix);
    float4 vAnimatedNormal = mul(float4(vNormal.xyz,0),finalMatrix);
    float4 vAnimatedTangent = mul(float4(vTangent.xyz,0),finalMatrix);
        
    // Transform the position from object space to homogeneous projection space
	output.position = mul(mul(vAnimatedPos,ObjectToWorld), CameraViewProjection);

    // Transform the normal from object space to world space    
	output.normal = mul(vAnimatedNormal.xyz, (float3x3)InvTransposeObjectToWorld); // normal to world space
    output.tangent = mul(vAnimatedTangent.xyz, (float3x3)InvTransposeObjectToWorld);
	output.bitangent = cross(output.normal ,output.tangent);
    
	output.texcoord.xy = vTexCoord0 ;

    return output;    
}
#else 
PSPTNTIn VS(float3 vPos : POSITION, float2 vTexCoord0 : TEXCOORD0, float3 vNormal : NORMAL, float3 vTangent : TANGENT, float4 vBones : BONE, float4 vWeights : BLENDWEIGHT){
    PSPTNTIn output;
    float3 vNormalWorldSpace;
                
    float2x4 dual = (float2x4)0;
    float2x4 m = g_dualquat[vBones.x];
    float4 dq0 = (float1x4)m ;     
    
    dual = vWeights.x * m ;

    m = g_dualquat[vBones.y] ;
    float4 dq = (float1x4)m ;   
    if (dot( dq0, dq ) < 0)        
      dual -= vWeights.y * m;
    else 
    dual += vWeights.y * m;        

        
    m = g_dualquat[vBones.z] ;
    dq = (float1x4)m ;          
    if (dot( dq0, dq ) < 0)        
      dual -= vWeights.z * m;
    else 
    dual += vWeights.z * m ;
            
            
    m = g_dualquat[vBones.w] ;
    dq = (float1x4)m ;              
    if (dot( dq0, dq ) < 0)        
      dual -= vWeights.w * m;
    else                
    dual += vWeights.w * m;    
   
    float4 Pos;
    float3 Norm, Tan, position, translation ; 
	
    // fast dqs 
    float length = sqrt(dual[0].w * dual[0].w + dual[0].x * dual[0].x + dual[0].y * dual[0].y + dual[0].z * dual[0].z) ;
    dual = dual / length ; 
    position = vPos.xyz + 2.0 * cross(dual[0].xyz, cross(dual[0].xyz, vPos.xyz) + dual[0].w * vPos.xyz) ;
    translation = 2.0 * (dual[0].w * dual[1].xyz - dual[1].w * dual[0].xyz + cross(dual[0].xyz, dual[1].xyz)) ; 
    position += translation ; 
    
    Pos = float4(position,1);
    Norm = vNormal.xyz + 2.0 * cross(dual[0].xyz, cross(dual[0].xyz,vNormal.xyz) + dual[0].w * vNormal.xyz) ; 
    Tan = vTangent.xyz + 2.0 * cross(dual[0].xyz, cross(dual[0].xyz,vTangent.xyz) + dual[0].w * vTangent.xyz) ;
    float4 vAnimatedPos = Pos ; 
    float4 vAnimatedNormal = float4(Norm, 0) ; 
    float4 vAnimatedTangent = float4(Tan, 0) ;  
       
    // Transform the position from object space to homogeneous projection space
    output.position = mul(mul(vAnimatedPos,ObjectToWorld), CameraViewProjection);

    // Transform the normal from object space to world space    
	output.normal = mul(vAnimatedNormal.xyz, (float3x3)InvTransposeObjectToWorld); // normal to world space
    output.tangent = mul(vAnimatedTangent.xyz, (float3x3)InvTransposeObjectToWorld);
	output.bitangent = cross(output.normal ,output.tangent);
 
	output.texcoord = vTexCoord0;   
    return output; 
}
#endif
#if (QUALITY != 0)
PSBSOut PS( PSPTNTIn input){
	PSBSOut Out;
	float4 bump = Slot1.Sample(sampler0,input.texcoord.xy)*2.0-1; 
	Out.base = Slot0.Sample(sampler0,input.texcoord.xy);
	Out.base.a = 10;// put the specular in
	//Out.normal = float4(normalize(input.tangent * bump.x + input.bitangent * bump.y + input.normal * bump.z), 1);
	Out.normal = float4(normalize(input.normal), 1);
	Out.depth = input.position.z;
	return Out;
}
#endif

/*

PSPIn sVSP(float3 position: Position){
	PSPIn Out;
	float3 pos = mul(float4(position, 1.0f), ObjectToWorldShadow).xyz;
	Out.position = mul(float4(pos, 1.0f), ShadowMapSplits[Split]);
    Out.fDepth = length( SunPosition - pos );
    return Out;
}
float4 sPSP(PSPIn In ) : SV_Target{
    return float4( In.fDepth, 0, 0.0f, 1.0f );
}
technique10 sPTech {// for shadowing
    pass P0 {
        SetVertexShader( CompileShader( vs_4_0, sVSP() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, sPSP() ) );
	 }
}
PSPTIn sVSPT(float3 position : Position, float2 texcoord : TexCoord){
	PSPTIn Out;
	float3 pos = mul(float4(position, 1.0f), ObjectToWorldShadow).xyz;
	Out.position = mul(float4(pos, 1.0f), ShadowMapSplits[Split] );
	Out.texcoord = texcoord;
    Out.fDepth = length( SunPosition - pos );
    return Out;
}
float4 sPSPT(PSPTIn In ) : SV_Target{
	float alpha = Slot0.Sample(PointFilter, In.texcoord).a;
	clip(alpha - .3);// clip transparent pixels
    return float4( In.fDepth, 0, 0.0f, 1.0f );
}
technique10 sPTTech {// for shadowing
    pass P0 {
        SetVertexShader( CompileShader( vs_4_0, sVSPT() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, sPSPT() ) );
	 }
}
PSPTNIn VSPT(float3 position : Position,  float2 texcoord : TexCoord, float3 normal : Normal){
	PSPTNIn Out;
	Out.position = mul(mul(float4(position, 1.0f), ObjectToWorld), CameraViewProjection);
	Out.texcoord = texcoord;
	Out.normal  = mul(float4(normal, 0.0f), InvTransposeObjectToWorld).xyz;
	return Out;
}
PSBSOut PSPT(PSPTNIn input){
	PSBSOut Out;
	Out.base = Slot0.Sample(LinearFilter, input.texcoord);
	//Out.base = float4(1, 1, 1, 1);
	clip(Out.base.a - .15);// clip transparent pixels
	Out.base.a = Power;// put the specular in
	Out.normal = float4(normalize(input.normal), 1);
	Out.depth = input.position.z;

	return Out;
}
technique10 PTNTech {// stands for pos tex rendering
    pass P0 {
        SetVertexShader( CompileShader( vs_4_0, VSPT() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, PSPT() ) );
    }
}PSPTNTIn VSPTN(float3 position : Position, float2 texcoord : TexCoord, float3 normal : Normal, float3 tangent  : Tangent){
	PSPTNTIn Out;
	Out.texcoord = texcoord;
	Out.normal  = mul(float4(normal, 0.0f), InvTransposeObjectToWorld).xyz;
	Out.tangent  = mul(float4(-tangent.xyz, 0.0f), InvTransposeObjectToWorld).xyz;
	Out.bitangent =  normalize(cross(normal, -tangent));
	Out.position = mul(mul(float4(position, 1.0f), ObjectToWorld), CameraViewProjection);
	return Out;
}
PSBSOut PSPTN(PSPTNTIn input){
	PSBSOut Out;
	Out.base = Slot0.Sample(LinearFilter, input.texcoord);
	//Out.base = float4(1, 1, 1, 1);
	clip(Out.base.a - .15);// clip transparent pixels
	Out.base.a = Power;// put the specular in
	float3 Normal  = normalize(input.normal);
	float3 Tangent = normalize(input.tangent);
	float3 Bitangent =normalize(input.bitangent); 
	float3 normal = Slot1.Sample(LinearFilter, input.texcoord).xyz*2.0-1;
	Out.normal = float4(normalize(Tangent * normal.x + Bitangent * normal.y + Normal * normal.z), 1);
	Out.depth = input.position.z;
	return Out;
}
technique10 PTNTTech {// stands for pos tex normal, normal rendering
    pass P0 {
        SetVertexShader( CompileShader( vs_4_0, VSPTN() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, PSPTN() ) );
    }
}



Vacuum 1.0 (min. index of refraction)
Air 1.000293
Ice 1.31
Water 1.333333
Ethyl Alcohol 1.36
Fluorite 1.43
Poppy Seed Oil 1.469
Olive Oil 1.47
Linseed Oil 1.478
Plexiglas 1.51
Immersion Oil 1.515
Crown Glass 1.52
Quartz 1.54
Salt 1.54
Light Flint Glass 1.58
Dense Flint Glass 1.66
Tourmaline 1.62
Garnet 1.73-1.89
Zircon 1.923
Cubic Zirconia 2.14-2.20
Diamond 2.417
Rutile 2.907
Gallium Phosphide 3.5 (max. index of refraction)
float fresnel(float3 light, float3 normal, float R0)
{
float const cosAngle = 1-saturate(dot3(light, normal));
float result = cosAngle * cosAngle;
result = result * result;
result = result * cosAngle;
result = saturate(mad(result, 1-saturate(R0), R0));
return result;
}
fragout main( vpconn IN,
uniform samplerCUBE EnvironmentMap,
uniform sampler2D NormalMap,
uniform sampler2D FresnelFunc)
{
fragout OUT;
float3 environColor = texCUBE(EnvironmentMap).xyz;
float4 normal = 2*(tex2D(NormalMap)-0.5);
float fresnelValue = saturate(tex2D_dp3x2(FresnelFunc,
IN.TexCoord2,
normal).w);
float3 litColor = IN.Color.xyz + IN.Specular.xyz;
OUT.col.xyz = lerp(litColor, environColor, fresnelValue);
return OUT;
}*/

