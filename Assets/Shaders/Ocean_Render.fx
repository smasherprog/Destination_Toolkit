
#include "Common_Ocean.fx"

float4 PSOcean(VSQuadOutMax inp) : SV_Target0 {
	
    float3 V = normalize(CameraPosition.xyz - inp.vWorldPos.xyz).xzy;

	float2 slopes =float2(0, 0);
	slopes += texture2.Sample(sampler0, float3(inp.texcoord/ GRID_SIZES.x, 1.0)).xy;
    slopes += texture2.Sample(sampler0, float3(inp.texcoord/ GRID_SIZES.y, 1.0)).zw;
    slopes += texture2.Sample(sampler0, float3(inp.texcoord/ GRID_SIZES.z, 2.0)).xy;
    slopes += texture2.Sample(sampler0, float3(inp.texcoord/ GRID_SIZES.w, 2.0)).zw;
	
    float3 N = normalize(float3(-slopes.x , -slopes.y, 1));
	if (dot(V, N) < 0.0) {
       N = reflect(N, V); // reflects backfacing normals
   }
	
    float Jxx = ddx(inp.texcoord.x);
    float Jxy = ddy(inp.texcoord.x);
    float Jyx = ddx(inp.texcoord.y);
    float Jyy = ddy(inp.texcoord.y);
    float A = Jxx * Jxx + Jyx * Jyx;
    float B = Jxx * Jxy + Jyx * Jyy;
    float C = Jxy * Jxy + Jyy * Jyy;

    float ua = pow(A / 10.0f, 0.25);
    float ub = 0.5 + 0.5 * B / sqrt(A * C);
    float uc = pow(C / 10.0f, 0.25);

    float2 sigmaSq = texture3.Sample(sampler1, float3(ua, ub, uc)).xy;	
    sigmaSq = max(sigmaSq, 2e-5);
	
    float3 Ty = normalize(float3(0.0, N.z, -N.y));	
    float3 Tx = cross(Ty, N);
	
	float fresnel = 0.02 + 0.98 * meanFresnel(V, N, sigmaSq);

    float3 Lsun, Esky, gl_FragColor;
	gl_FragColor = float3(0, 0, 0);
	
	float3 worldP = CameraPosition.xyz + earthPos;
	float3 worldV = normalize(worldP.xzy); // vertical floattor
    float r = length(worldP);
    float muS = dot(worldV, SunNormal.xzy);
    Lsun = sunRadiance(r, muS);
    Esky = skyIrradiance(r, muS);

	gl_FragColor.rgb += reflectedSunRadiance(SunNormal.xzy, V, N, Tx, Ty, sigmaSq) * Lsun;
	
	gl_FragColor.rgb += fresnel * meanSkyRadiance(V, N, Tx, Ty, sigmaSq);

	float3 Lsea = seaColor * Esky / M_PI;
    gl_FragColor.rgb += (1.0 - fresnel) * Lsea;

	return float4(hdr(gl_FragColor.rgb), 1);
}

