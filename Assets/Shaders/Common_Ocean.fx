#include "Common.fx"
#define GRID1_SIZE  5488.0 // size in meters (i.e. in spatial domain) of the first grid
#define GRID2_SIZE 392.0 // size in meters (i.e. in spatial domain) of the second grid
#define GRID3_SIZE 28.0 // size in meters (i.e. in spatial domain) of the third grid
#define GRID4_SIZE  2.0 // size in meters (i.e. in spatial domain) of the fourth grid
#define GRID_SIZES  float4(GRID1_SIZE, GRID2_SIZE, GRID3_SIZE, GRID4_SIZE)
#define seaColor float3((10.0f/255)*.1f, (40.0f/255.0f)*.1f, (120.0f/255.0f)*.1f) // sea bottom color
#define N_SLOPE_VARIANCE 10
#define FFT_SIZE 256
#define M_PI 3.14159265
#define INVERSE_GRID_SIZES float4(2.0 * M_PI * FFT_SIZE / GRID1_SIZE, 2.0 * M_PI * FFT_SIZE / GRID2_SIZE, 2.0 * M_PI * FFT_SIZE / GRID3_SIZE, 2.0 * M_PI * FFT_SIZE / GRID4_SIZE)
#define earthPos  float3(0.0, 6360010.0, 0.0  )
#define choppy true
#define Rg  6360.0 * 1000.0
#define Rt 6420.0 * 1000.0
#define RL 6421.0 * 1000.0
#define SUN_INTENSITY 100.0

Texture2D texture0, texture1;
Texture2DArray texture2;
Texture3D texture3;
Texture2D texture4;
SamplerState sampler0, sampler1;

cbuffer CBuffer1{// VS_OCEAN and PS_OCEAN USE THIS
	float4x4 CameraViewProjection;
	float4 CameraPosition;
	float4 SunNormal;//stored in the w float hdrExposure;
};
cbuffer CBuffer2{
	float4 slopeVarianceDelta;
	float4 Slice;
};
cbuffer CBuffer_Init{
	float4 lastime;
};
cbuffer CBuffer_GS_FFT{
	int4 Layers;
	float4 Passes;
};

struct DUMMY
{
	float Dummmy : DUMMY;
};

struct HSIn_Heightfield
{
    float2 origin   : ORIGIN;
    float2 size     : SIZE;
};
struct PatchData
{
    float Edges[4]  : SV_TessFactor;
    float Inside[2]	: SV_InsideTessFactor;

	float2 origin   : ORIGIN;
    float2 size     : SIZE;
};
float CalculateTessellationFactor(float distance)
{
	return CameraPosition.w*(1/(0.015*distance));
}
float CalculateMIPLevelForDisplacementTextures(float distance)
{
	return log2(128/CalculateTessellationFactor(distance));
}
float erfc(float x) {
	return 2.0 * exp(-x * x) / (2.319 * x + sqrt(4.0 + 1.52 * x * x));
}
float Lambda(float cosTheta, float sigmaSq) {
	float v = cosTheta / sqrt((1.0 - cosTheta * cosTheta) * (2.0 * sigmaSq));
    return max(0.0, (exp(-v * v) - v * sqrt(M_PI) * erfc(v)) / (2.0 * v * sqrt(M_PI)));
	//return (exp(-v * v)) / (2.0 * v * sqrt(M_PI)); // approximate, faster formula
}
float reflectedSunRadiance(float3 L, float3 V, float3 N, float3 Tx, float3 Ty, float2 sigmaSq) {// L, V, N, Tx, Ty in world space
    float3 H = normalize(L + V);
    float zetax = dot(H, Tx) / dot(H, N);
    float zetay = dot(H, Ty) / dot(H, N);
    float zL = dot(L, N); // cos of source zenith angle
    float zV = dot(V, N); // cos of receiver zenith angle
    float zH = dot(H, N); // cos of facet normal zenith angle
    float zH2 = zH * zH;
    float p = exp(-0.5 * (zetax * zetax / sigmaSq.x + zetay * zetay / sigmaSq.y)) / (2.0 * M_PI * sqrt(sigmaSq.x * sigmaSq.y));
    float tanV = atan2(dot(V, Ty), dot(V, Tx));
    float cosV2 = 1.0 / (1.0 + tanV * tanV);
    float sigmaV2 = sigmaSq.x * cosV2 + sigmaSq.y * (1.0 - cosV2);
    float tanL = atan2(dot(L, Ty), dot(L, Tx));
    float cosL2 = 1.0 / (1.0 + tanL * tanL);
    float sigmaL2 = sigmaSq.x * cosL2 + sigmaSq.y * (1.0 - cosL2);
    float fresnel = 0.02 + 0.98 * pow(1.0 - dot(V, H), 5.0);
    zL = max(zL, 0.01);
    zV = max(zV, 0.01);
    return fresnel * p / ((1.0 + Lambda(zL, sigmaL2) + Lambda(zV, sigmaV2)) * zV * zH2 * zH2 * 4.0);
}
float2 U(float2 zeta, float3 V, float3 N, float3 Tx, float3 Ty) {// V, N, Tx, Ty in world space
    float3 f = normalize(float3(-zeta, 1.0)); // tangent space
    float3 F = f.x * Tx + f.y * Ty + f.z * N; // world space
    float3 R = 2.0 * dot(F, V) * F - V;
    return R.xy / (1.0 + R.z);
}
float meanFresnel(float cosThetaV, float sigmaV) {
	return pow(abs(1.0 - cosThetaV), 5.0 * exp(-2.69 * sigmaV)) / (1.0 + 22.7 * pow(sigmaV, 1.5));
}
float meanFresnel(float3 V, float3 N, float2 sigmaSq) {// V, N in world space
    float2 v = V.xy; // view direction in wind space
    float2 t = v * v / (1.0 - V.z * V.z); // cos^2 and sin^2 of view direction
    float sigmaV2 = dot(t, sigmaSq); // slope variance in view direction
    return meanFresnel(dot(V, N), sqrt(sigmaV2));
}

// V, N, Tx, Ty in world space;
float3 meanSkyRadiance(float3 V, float3 N, float3 Tx, float3 Ty, float2 sigmaSq) {
    float4 result = float4(0, 0, 0, 0);

    const float eps = 0.001;
    float2 u0 = U(float2(0, 0), V, N, Tx, Ty);
    float2 dux = 2.0 * (U(float2(eps, 0.0), V, N, Tx, Ty) - u0) / eps * sqrt(sigmaSq.x);
    float2 duy = 2.0 * (U(float2(0.0, eps), V, N, Tx, Ty) - u0) / eps * sqrt(sigmaSq.y);
	result.rgb = texture4.SampleGrad(sampler1, u0 * (0.5 / 1.1) + 0.5, dux * (0.5 / 1.1), duy * (0.5 / 1.1) ) .rgb;
    return result.rgb;
}

float3 transmittance(float r, float mu) {
    float uR, uMu;
    uR = sqrt((r - Rg) / (Rt - Rg));
    uMu = atan((mu + 0.15) / (1.0 + 0.15) * tan(1.5)) / 1.5;
    return texture0.Sample(sampler1, float2(uMu, uR)).rgb;
}

float3 sunRadiance(float r, float muS) {
	return muS < -sqrt(1.0 - (Rg / r) * (Rg / r)) ? float3(0, 0, 0) : transmittance(r, muS) * SUN_INTENSITY;
}

float3 skyIrradiance(float r, float muS) {
    float uR = (r - Rg) / (Rt - Rg);
    float uMuS = (muS + 0.2) / (1.0 + 0.2);
    return (texture1.Sample(sampler1, float2(uMuS, uR)).rgb)* SUN_INTENSITY;
}

float3 hdr(float3 L) {
    L = L * SunNormal.w;
    L.r = L.r < 1.413 ? pow(abs(L.r * 0.38317), 1.0 / 2.2) : 1.0 - exp(-L.r);
    L.g = L.g < 1.413 ? pow(abs(L.g * 0.38317), 1.0 / 2.2) : 1.0 - exp(-L.g);
    L.b = L.b < 1.413 ? pow(abs(L.b * 0.38317), 1.0 / 2.2) : 1.0 - exp(-L.b);
    return L;
}
float fast_fresnel(float3 I, float3 N)
{
  	float R0 = 0.02037;
	float OneLessR0 = 0.97963;
	
	float icosIN = saturate( 1.0 - dot(I, N) );
  	return R0 + OneLessR0 * pow(icosIN, 5.0);
}

struct InitPSOut{
	float4 target0 : SV_Target0;
	float4 target1 : SV_Target1;
	float4 target2 : SV_Target2;
	float4 target3 : SV_Target3;
	float4 target4 : SV_Target4;

};

struct VSQuadOutWRT{
    float4 position : SV_Position;
    float2 texcoord: TexCoord;
	uint layer : SV_RenderTargetArrayIndex;
};



#define betaR (float3(5.8e-3, 1.35e-2, 3.31e-2) / 1000)
#define HR  8.0 * 1000
#define mieG  0.8

float3 getMie(float4 rayMie) { // rayMie.rgb=C*, rayMie.w=Cm,r
    return rayMie.rgb * rayMie.w / max(rayMie.r, 1e-4) * (betaR.r / betaR);
}
float phaseFunctionR(float mu) {
    return (3.0 / (16.0 * M_PI)) * (1.0 + mu * mu);
}
// Mie phase function
float phaseFunctionM(float mu) {
    return 1.5 * 1.0 / (4.0 * M_PI) * (1.0 - mieG*mieG) * pow(abs(1.0 + (mieG*mieG) - 2.0*mieG*mu), abs(-3.0/2.0)) * (1.0 + mu * mu) / (2.0 + mieG*mieG);
}
#define RES_R 32
#define RES_MU 128
#define RES_MU_S 32
#define RES_NU 8

float4 texture4D(Texture3D table, float r, float mu, float muS, float nu){
    float H = sqrt(Rt * Rt - Rg * Rg);
    float rho = sqrt(r * r - Rg * Rg);

    float rmu = r * mu;
    float delta = rmu * rmu - r * r + Rg * Rg;
    float4 cst = rmu < 0.0 && delta > 0.0 ? float4(1.0, 0.0, 0.0, 0.5 - 0.5 / float(RES_MU)) : float4(-1.0, H * H, H, 0.5 + 0.5 / float(RES_MU));
    float uR = 0.5 / float(RES_R) + rho / H * (1.0 - 1.0 / float(RES_R));
    float uMu = cst.w + (rmu * cst.x + sqrt(delta + cst.y)) / (rho + cst.z) * (0.5 - 1.0 / float(RES_MU));
    float uMuS = 0.5 / float(RES_MU_S) + (atan(max(muS, -0.1975) * tan(1.26 * 1.1)) / 1.1 + (1.0 - 0.26)) * 0.5 * (1.0 - 1.0 / float(RES_MU_S));

    float lerp = (nu + 1.0) / 2.0 * (float(RES_NU) - 1.0);
    float uNu = floor(lerp);
    lerp = lerp - uNu;
    return table.Sample(sampler0, float3((uNu + uMuS) / float(RES_NU), uMu, uR)) * (1.0 - lerp) +
           table.Sample(sampler0, float3((uNu + uMuS + 1.0) / float(RES_NU), uMu, uR)) * lerp;
}

float3 skyRadiance(float3 camera, float3 viewdir, float3 sundir){
    float3 result = float3(0, 0, 0);
    float r = length(camera);
    float rMu = dot(camera, viewdir);
    float mu = rMu / r;
    float r0 = r;
    float mu0 = mu;

    float deltaSq = sqrt(rMu * rMu - r * r + Rt*Rt);
    float din = max(-rMu - deltaSq, 0.0);
    if (din > 0.0) {
        camera += din * viewdir;
        rMu += din;
        mu = rMu / Rt;
        r = Rt;
    }
    if (r <= Rt) {
        float nu = dot(viewdir, sundir);
        float muS = dot(camera, sundir) / r;
		float4 inScatter = texture4D(texture3, r, rMu / r, muS, nu);
        float3 inScatterM = getMie(inScatter);
        float phase = phaseFunctionR(nu);
        float phaseM = phaseFunctionM(nu);
        result = inScatter.rgb * phase + inScatterM * phaseM;

    } 
    return result * SUN_INTENSITY;
}