#include "shaderhelper.fx"
#include "Common.fx"

struct PsIn {
	float4 position : SV_Position;
	float4 Linfo    : info0;// x and y are z bounds, z is inv radius in world space, w is spec
	float4 Lpos     : info1;// first three are pos, the w is radius in world space
	float4 diffuse  : Color;
	float4 attenuation : info3;//  the w is radius in world space
	float4 specular : info4;//  the w is radius in world space
};


PsIn PointLightVS(float3 position : Position, float4 Linfo : TexCoord0, float4 Lpos  : TexCoord1, float4 diffuse  : Color, float4 atten  : TexCoord2, float4 spec  : TexCoord3){
	PsIn Out;
	float4 pos = float4(position.xyz, 1.0f);
	pos.xyz *= Lpos.w;// scale the light
	pos.xyz += Lpos.xyz;// translate the light
	Out.Linfo = Linfo;
	Out.Lpos = Lpos;
	Out.position = mul(pos, CameraViewProjection);
	Out.diffuse = diffuse;
	Out.attenuation = atten;
	Out.specular = spec;
	return Out;
}

float3 PointLightPS(PsIn In) : SV_Target {
    int3 texCoord = int3(int2(In.position.xy), 0);
    float depth = Slot2.Load(texCoord).x;
  	if (depth <= In.Linfo.x && depth >= In.Linfo.y){// x is the far plane ... y is the near plane
		float4 cPos = float4(In.position.xy, depth, 1);
		// World-space position
		cPos = mul(cPos, InvCameraViewProjectionFromScreen);
		float3 pos = cPos.xyz / cPos.w;
		float3 normal = normalize(Slot1.Load(texCoord).xyz); 
		float3 base = Slot0.Load(texCoord).rgb;
		float3 lightVec = (In.Lpos.xyz - pos);
		float d = length(lightVec);
		lightVec /=d;
		// Attenution that falls off to zero at light radius
		float dfactor = dot( lightVec, normal);
		float3 litColor = base;
		[branch]
		if(dfactor > 0.0f){
			float specPower   = max(In.Linfo.w, 1.0f);
			float3 toEye      = normalize(CameraPosition.xyz - pos);
			float3 reflectVec = reflect(-lightVec, normal);
			float specFactor  = pow( max(dot(reflectVec, toEye), 0.0f), specPower);
			// diffuse and specular terms			
			litColor += dfactor * base * In.diffuse.rgb;
			litColor += specFactor * In.specular.rgb;	
		}
		return float3(litColor / dot( In.attenuation.xyz, float3(1.0f, d, d*d)));
	} 
	return float4(0, 0, 0, 0);
}

float4 ShowPointLightPS(PsIn In) : SV_Target { 
	return float4(In.diffuse.rgb, 1);
}
technique10 PointTech{
    pass P0 {
        SetVertexShader( CompileShader( vs_4_0, PointLightVS() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, PointLightPS() ) );
    }
}
technique10 ShowPointTech{
    pass P0 {
        SetVertexShader( CompileShader( vs_4_0, PointLightVS() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, ShowPointLightPS() ) );
    }
}
