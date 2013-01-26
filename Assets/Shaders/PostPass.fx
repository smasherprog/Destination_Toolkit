#include "common.fx"

Texture2D Slot0, Slot1, Slot2, Slot3;
Texture2DArray ArraySlot0;

SamplerState sampler0;

#define NumSplits  4

cbuffer Buffer0{		
	float4x4 ShadowMapSplits[NumSplits], ShadowMapSplitsTex[NumSplits];
	float4 SplitPositions;
	float4x4 InvCameraViewProjectionFromScreen, CameraViewProjection;
	float4 SunNormal, SunPosition, CameraPosition;
	float4x4 CloudsToTex;
	float4 ShadowSettings;
	
};


cbuffer perminent{
    static const int SplitPowLookup[8] = {0, 1, 1, 2, 2, 2, 2, 3};
    static const float3 SplitColors[2][4] = { {1, 1, 1}, {1, 1, 1}, {1, 1, 1}, {1, 1, 1}, {1, 0, 0}, {0, 1, 0}, {0, 0, 1}, {1, 1, 0} };
}

float3 PSDay(VSQuadOut In ) : SV_Target {
	
	int3 texCoord = int3(int2(In.position.xy), 0);//rebuild World position.....
	float depth = Slot2.Load(texCoord).r;

	float4 cPos = float4(In.position.xy, depth, 1);
	// World-space position
	cPos = mul(cPos, InvCameraViewProjectionFromScreen);
	float3 pos = cPos.xyz / cPos.w;// pos is now the real world position

	// now convert the world position into the lights view frustum
	float4 base = Slot0.Load(texCoord);
	float spe = base.a;
	float3 normal = Slot1.Load(texCoord).xyz;
	
	float dfactor = dot( normal, SunNormal.xyz );
	
	float3 otherp = mul(float4(pos, 1.0f), CameraViewProjection).xyz;
	 /*
	 float receiver = length( SunPosition.xyz- pos);
	 int Split = dot(1, otherp.z > SplitPositions);
   
    // Ensure that every fragment in the quad choses the same split so that derivatives
    // will be meaningful for proper texture filtering and LOD selection.
    int SplitPow = 1 << Split;
    int SplitX = abs(ddx(SplitPow));
    int SplitY = abs(ddy(SplitPow));
    int SplitXY = abs(ddx(SplitY));
    int SplitMax = max(SplitXY, max(SplitX, SplitY));
    Split = SplitMax > 0 ? SplitPowLookup[SplitMax-1] : Split;
	float4 PosLight = mul(float4(pos, 1), ShadowMapSplitsTex[Split]);// no need for a divide by w, I use an orthog proj

	float occluder= ArraySlot0.Sample( sampler0, float3(PosLight.xy, Split) ).x;
	float	lit = exp(OverDark* ( occluder - receiver));

	PosLight = mul(float4(pos, 1), CloudsToTex);
	float2 LightTexCoord = PosLight.xy;///PosLight.w;
	float cloudslit = Slot3.Sample(sampler0, LightTexCoord).x;
	lit = min(lit, cloudslit);// select from the two, the smallest
	lit = saturate(lit);
	float3 litColor = base.rgb*ShadowSettings.x;
	*/
	float litColor = base.rgb;

	if( dfactor > 0.0f ){
		float specPower   = max(128, 1.0f);
		float3 toEye      = normalize(CameraPosition.xyz - pos);
		float3 reflectVec = reflect(-SunNormal.xyz, normal);
		float specFactor  = pow( max(dot(reflectVec, toEye), 0.0f), specPower);
		
		// diffuse and specular terms
		litColor += dfactor * base.rgb;
		litColor += specFactor * float3(1, 1, 1);

	} 
	return litColor;
	//return litColor * SplitColors[ShadowSettings.y][Split];
	
}


float3 PSNight(VSQuadOut In ) : SV_Target {
	
	int3 texCoord = int3(int2(In.position.xy), 0);//rebuild World position.....
	float depth = Slot2.Load(texCoord).r;

	float4 cPos = float4(In.position.xy, depth, 1);
	// World-space position
	cPos = mul(cPos, InvCameraViewProjectionFromScreen);
	float3 pos = cPos.xyz / cPos.w;// pos is now the real world position

	float3 otherp = mul(float4(pos, 1.0f), CameraViewProjection).xyz;

	int Split = dot(1, otherp.z > SplitPositions);
    // Ensure that every fragment in the quad choses the same split so that derivatives
    // will be meaningful for proper texture filtering and LOD selection.
    int SplitPow = 1 << Split;
    int SplitX = abs(ddx(SplitPow));
    int SplitY = abs(ddy(SplitPow));
    int SplitXY = abs(ddx(SplitY));
    int SplitMax = max(SplitXY, max(SplitX, SplitY));
    Split = SplitMax > 0 ? SplitPowLookup[SplitMax-1] : Split;


	// now convert the world position into the lights view frustum
	float4 base = Slot0.Load(texCoord);
	float3 litColor = base.rgb *ShadowSettings.x; //*float3(.9, .9, .3);
	return litColor * SplitColors[ShadowSettings.y][Split];
	
}
