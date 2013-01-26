/**
 * Real-time Realistic Ocean Lighting using Seamless Transitions from Geometry to BRDF
 * Copyright (c) 2009 INRIA
 * All rights reserved.
 *
 * ReLengthribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. ReLengthributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. ReLengthributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the Lengthribution.
 * 3. Neither the name of the copyright holders nor the names of its
 *    contributors may be used to endorse or promote products derived from
 *    this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 */

/**
 * Author: Eric Bruneton
 */
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
PatchData PatchConstantHS( InputPatch<HSIn_Heightfield, 1> inputPatch )
{    
    PatchData output;

	float distance_to_camera;
	float tesselation_factor;
	float inside_tessellation_factor=0;
	float in_frustum=0;

	output.origin = inputPatch[0].origin;
	output.size = inputPatch[0].size;

		distance_to_camera=length(CameraPosition.xz-inputPatch[0].origin-float2(0,inputPatch[0].size.y*0.5));
		tesselation_factor=CalculateTessellationFactor(distance_to_camera);
		output.Edges[0] =  tesselation_factor;
		inside_tessellation_factor+=tesselation_factor;


		distance_to_camera=length(CameraPosition.xz-inputPatch[0].origin-float2(inputPatch[0].size.x*0.5,0));
		tesselation_factor=CalculateTessellationFactor(distance_to_camera);
		output.Edges[1] =  tesselation_factor;
		inside_tessellation_factor+=tesselation_factor;

		distance_to_camera=length(CameraPosition.xz-inputPatch[0].origin-float2(inputPatch[0].size.x,inputPatch[0].size.y*0.5));
		tesselation_factor=CalculateTessellationFactor(distance_to_camera);
		output.Edges[2] =  tesselation_factor;
		inside_tessellation_factor+=tesselation_factor;

		distance_to_camera=length(CameraPosition.xz-inputPatch[0].origin-float2(inputPatch[0].size.x*0.5,inputPatch[0].size.y));
		tesselation_factor=CalculateTessellationFactor(distance_to_camera);
		output.Edges[3] =  tesselation_factor;
		inside_tessellation_factor+=tesselation_factor;
		output.Inside[0] = output.Inside[1] = inside_tessellation_factor*0.25;

    return output;
}

[domain("quad")]
[partitioning("fractional_odd")]
[outputtopology("triangle_ccw")]
[outputcontrolpoints(1)]
[patchconstantfunc("PatchConstantHS")]
DUMMY PatchHS( InputPatch<HSIn_Heightfield, 1> inputPatch )
{
    return (DUMMY)0;
}


// first two are x and z offset is an offset
VSQuadOutMax VSOcean(float2 Pos : Position0, float4 offset: Position1) {
	VSQuadOutMax O;
	O.texcoord = (Pos.xy + offset.xz);

    float2 ux = O.texcoord + float2(.25, 0.0);
    float2 uy = O.texcoord + float2(0.0, .25);

    float2 dux = abs(ux - O.texcoord) * 2.0;
    float2 duy = abs(uy - O.texcoord) * 2.0;
    float3 dP = float3(0, 0, 0);
	dP.y += texture2.SampleGrad(sampler0, float3(O.texcoord/ GRID_SIZES.x, 0.0), dux / GRID_SIZES.x, duy / GRID_SIZES.x).x;
    //dP.y += texture2.SampleGrad(sampler0, float3(O.texcoord/ GRID_SIZES.y, 0.0), dux / GRID_SIZES.y, duy / GRID_SIZES.y).y;
	//dP.y += texture2.SampleGrad(sampler0, float3(O.texcoord/ GRID_SIZES.z, 0.0), dux / GRID_SIZES.z, duy / GRID_SIZES.z).z;
	//dP.y += texture2.SampleGrad(sampler0, float3(O.texcoord/ GRID_SIZES.w, 0.0), dux / GRID_SIZES.w, duy / GRID_SIZES.w).w;
	
	//if (choppy ) {
		dP.xz += texture2.SampleGrad( sampler0, float3(O.texcoord / GRID_SIZES.x, 3.0), dux / GRID_SIZES.x, duy / GRID_SIZES.x).xy;
		dP.xz += texture2.SampleGrad( sampler0, float3(O.texcoord / GRID_SIZES.y, 3.0), dux / GRID_SIZES.y, duy / GRID_SIZES.y).zw;
		dP.xz += texture2.SampleGrad( sampler0, float3(O.texcoord / GRID_SIZES.z, 4.0), dux / GRID_SIZES.z, duy / GRID_SIZES.z).xy;
		dP.xz += texture2.SampleGrad( sampler0, float3(O.texcoord / GRID_SIZES.w, 4.0), dux / GRID_SIZES.w, duy / GRID_SIZES.w).zw;
	//}
	dP.x =0;
	dP.z=0;
	O.vWorldPos = float4(Pos.x, 0, Pos.y, 1) + float4(dP, 0.0f) + offset;
	O.position = mul( O.vWorldPos , CameraViewProjection);
	return O;
}


[domain("quad")]
VSQuadOutMax HeightFieldPatchDS(PatchData input, float2 uv : SV_DomainLocation, OutputPatch<DUMMY, 1> inputPatch )
{
    VSQuadOutMax output;
	float3 Pos = float3(0, 0, 0);
	Pos.xz = input.origin + uv * input.size;
	output.texcoord = (input.origin + uv * input.size);

    float2 ux = output.texcoord + float2(.25, 0.0);
    float2 uy = output.texcoord + float2(0.0, .25);

    float2 dux = abs(ux - output.texcoord) * 2.0;
    float2 duy = abs(uy - output.texcoord) * 2.0;
    float3 dP = float3(0, 0, 0);
	dP.y += texture2.SampleGrad(sampler0, float3(output.texcoord/ GRID_SIZES.x, 0.0), dux / GRID_SIZES.x, duy / GRID_SIZES.x).x;
    dP.y += texture2.SampleGrad(sampler0, float3(output.texcoord/ GRID_SIZES.y, 0.0), dux / GRID_SIZES.y, duy / GRID_SIZES.y).y;
	dP.y += texture2.SampleGrad(sampler0, float3(output.texcoord/ GRID_SIZES.z, 0.0), dux / GRID_SIZES.z, duy / GRID_SIZES.z).z;
	dP.y += texture2.SampleGrad(sampler0, float3(output.texcoord/ GRID_SIZES.w, 0.0), dux / GRID_SIZES.w, duy / GRID_SIZES.w).w;
		dP.xz += texture2.SampleGrad( sampler0, float3(output.texcoord / GRID_SIZES.x, 3.0), dux / GRID_SIZES.x, duy / GRID_SIZES.x).xy;
		dP.xz += texture2.SampleGrad( sampler0, float3(output.texcoord / GRID_SIZES.y, 3.0), dux / GRID_SIZES.y, duy / GRID_SIZES.y).zw;
		dP.xz += texture2.SampleGrad( sampler0, float3(output.texcoord / GRID_SIZES.z, 4.0), dux / GRID_SIZES.z, duy / GRID_SIZES.z).xy;
		dP.xz += texture2.SampleGrad( sampler0, float3(output.texcoord / GRID_SIZES.w, 4.0), dux / GRID_SIZES.w, duy / GRID_SIZES.w).zw;

	output.vWorldPos = float4(Pos.x, 0, Pos.z, 1) + float4(dP, 0.0f);
	output.position = mul( output.vWorldPos , CameraViewProjection);

    return output;
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


float2 getSlopeVariances(float2 k, float A, float B, float C, float2 spectrumSample) {
    float w = 1.0 - exp(A * k.x * k.x + B * k.x * k.y + C * k.y * k.y);
    float2 kw = k * w;
    return kw * kw * dot(spectrumSample, spectrumSample) * 2.0;
}

float4 VariancePS(VSQuadOut inp): SV_Target {
    float a = floor(inp.texcoord.x * N_SLOPE_VARIANCE);
    float b = floor(inp.texcoord.y * N_SLOPE_VARIANCE);
    float A = pow(a / (N_SLOPE_VARIANCE - 1.0), 4.0) * 10.0f;
    float C = pow(Slice.x / (N_SLOPE_VARIANCE - 1.0), 4.0) * 10.0f;
    float B = (2.0 * b / (N_SLOPE_VARIANCE - 1.0) - 1.0) * sqrt(A * C);
    A = -0.5 * A;
    B = - B;
    C = -0.5 * C;

    float2 slopeVariances = float2(slopeVarianceDelta.x, slopeVarianceDelta.x);
    for (int y = 0; y < FFT_SIZE; ++y) {
        for (int x = 0; x < FFT_SIZE; ++x) {
            int i = x >= FFT_SIZE / 2 ? x - FFT_SIZE : x;
            int j = y >= FFT_SIZE / 2 ? y - FFT_SIZE : y;
            float2 k = 2.0 * M_PI * float2(i, j);

            float4 spectrum12 = texture0.Sample(sampler0, float2(float(x) + 0.5, float(y) + 0.5) / float(FFT_SIZE));
            float4 spectrum34 = texture1.Sample(sampler0, float2(float(x) + 0.5, float(y) + 0.5) / float(FFT_SIZE));

            slopeVariances += getSlopeVariances(k / GRID_SIZES.x, A, B, C, spectrum12.xy);
            slopeVariances += getSlopeVariances(k / GRID_SIZES.y, A, B, C, spectrum12.zw);
            slopeVariances += getSlopeVariances(k / GRID_SIZES.z, A, B, C, spectrum34.xy);
            slopeVariances += getSlopeVariances(k / GRID_SIZES.w, A, B, C, spectrum34.zw);
        }
    }
    return float4(slopeVariances.xy, 0, 1);
}
/*
technique10 RenderVariance {
    pass P0 {
        SetVertexShader( CompileShader( vs_4_0, VSQuad() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, VariancePS() ) );
    }
}
*/


float2 getSpectrum(float k, float2 s0, float2 s0c) {
    float w = sqrt(9.81 * k * (1.0 + k * k / (370.0 * 370.0)));
    float c = cos(w * lastime.x);
    float s = sin(w * lastime.x);
    return float2((s0.x + s0c.x) * c - (s0.y + s0c.y) * s, (s0.x - s0c.x) * s + (s0.y - s0c.y) * c);
}
float2 i(float2 z) { return float2(-z.y, z.x); }// returns i times z (complex number)
struct InitPSOut{
	float4 target0 : SV_Target0;
	float4 target1 : SV_Target1;
	float4 target2 : SV_Target2;
	float4 target3 : SV_Target3;
	float4 target4 : SV_Target4;

};
InitPSOut InitPS(VSQuadOut inp) {

    float2 st = floor(inp.texcoord * FFT_SIZE) / FFT_SIZE;
    float x = inp.texcoord.x > 0.5 ? st.x - 1.0 : st.x;
    float y = inp.texcoord.y > 0.5 ? st.y - 1.0 : st.y;

    float4 s12 = texture0.SampleLevel(sampler0, inp.texcoord, 0.0);
    float4 s34 = texture1.SampleLevel(sampler1, inp.texcoord, 0.0);
    float4 s12c = texture0.SampleLevel(sampler0, float2(1.0 + 0.5 / FFT_SIZE, 1.0 + 0.5 / FFT_SIZE) - st, 0.0);
    float4 s34c = texture1.SampleLevel(sampler1, float2(1.0 + 0.5 / FFT_SIZE, 1.0 + 0.5 / FFT_SIZE) - st, 0.0);

    float2 k1 = float2(x, y) * INVERSE_GRID_SIZES.x;
    float2 k2 = float2(x, y) * INVERSE_GRID_SIZES.y;
    float2 k3 = float2(x, y) * INVERSE_GRID_SIZES.z;
    float2 k4 = float2(x, y) * INVERSE_GRID_SIZES.w;

    float K1 = length(k1);
    float K2 = length(k2);
    float K3 = length(k3);
    float K4 = length(k4);

    float IK1 = K1 == 0.0 ? 0.0 : 1.0 / K1;
    float IK2 = K2 == 0.0 ? 0.0 : 1.0 / K2;
    float IK3 = K3 == 0.0 ? 0.0 : 1.0 / K3;
    float IK4 = K4 == 0.0 ? 0.0 : 1.0 / K4;

    float2 h1 = getSpectrum(K1, s12.xy, s12c.xy);
    float2 h2 = getSpectrum(K2, s12.zw, s12c.zw);
    float2 h3 = getSpectrum(K3, s34.xy, s34c.xy);
    float2 h4 = getSpectrum(K4, s34.zw, s34c.zw);
	InitPSOut o;
	o.target0 = float4(h1 + i(h2), h3 + i(h4));
	o.target1 = float4(i(k1.x * h1) - k1.y * h1, i(k2.x * h2) - k2.y * h2);
	o.target2 = float4(i(k3.x * h3) - k3.y * h3, i(k4.x * h4) - k4.y * h4);
    o.target3 = o.target1 * float4(IK1, IK1, IK2, IK2);
    o.target4 = o.target2 * float4(IK3, IK3, IK4, IK4);
	return o;
}
/*
technique10 RenderInit {
    pass P0 {
        SetVertexShader( CompileShader( vs_4_0, VSQuad() ) );
        SetGeometryShader( NULL);
        SetPixelShader( CompileShader( ps_4_0, InitPS() ) );
    }
}
*/
struct VSQuadOutWRT{
    float4 position : SV_Position;
    float2 texcoord: TexCoord;
	uint layer : SV_RenderTargetArrayIndex;
};
[maxvertexcount(2*5*3)]
void GSQuadWRT( triangle VSQuadOut In[3], inout TriangleStream<VSQuadOutWRT> TriStream ){
	for(uint i = 0; i < (uint)Layers.x; i++){
		VSQuadOutWRT Out;
		Out.layer = i; 
		[unroll] 
		for(int iVertex = 0; iVertex < 3; iVertex++){
			Out.position = In[iVertex].position;
			Out.texcoord = In[iVertex].texcoord;
			TriStream.Append(Out); // append vertex to stream
		} 
	}
}

// performs two FFTs on two inputs packed in a single texture
// returns two results packed in a single float4
float4 ffty2(int layer, float2 i, float2 w, float2 uv) {
    float4 input1 = texture2.SampleLevel(sampler0, float3(uv.x, i.x, layer),0);
    float4 input2 = texture2.SampleLevel(sampler0, float3(uv.x, i.y, layer),0);
    float res1x = w.x * input2.x - w.y * input2.y;
    float res1y = w.y * input2.x + w.x * input2.y;
    float res2x = w.x * input2.z - w.y * input2.w;
    float res2y = w.y * input2.z + w.x * input2.w;
    return input1 + float4(res1x, res1y, res2x, res2y);
}
float4 fftx2(int layer, float2 i, float2 w, float2 uv) {
    float4 input1 = texture2.SampleLevel(sampler0, float3(i.x, uv.y, layer),0);
    float4 input2 = texture2.SampleLevel(sampler0, float3(i.y, uv.y, layer),0);
    float res1x = w.x * input2.x - w.y * input2.y;
    float res1y = w.y * input2.x + w.x * input2.y;
    float res2x = w.x * input2.z - w.y * input2.w;
    float res2y = w.y * input2.z + w.x * input2.w;
    return input1 + float4(res1x, res1y, res2x, res2y);
}
float4 fftyPS(VSQuadOutWRT inp): SV_Target {
	float4 data = texture0.SampleLevel(sampler1, float2(inp.texcoord.y, Passes.x), 0.0);
    float2 i = data.xy;
    float2 w = data.zw;
    return ffty2(inp.layer, i, w, inp.texcoord);
}
float4 fftxPS(VSQuadOutWRT inp): SV_Target {
	float4 data = texture0.SampleLevel(sampler1, float2(inp.texcoord.x, Passes.x), 0.0);
    float2 i = data.xy;
    float2 w = data.zw;
	return fftx2(inp.layer, i, w, inp.texcoord);
}



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
float4 SkyMapPS(VSQuadOut inp) : SV_Target0 {
	float2 u = inp.texcoord;
    float l = dot(u, u);
    float4 result = float4(0, 0, 0, 0);
    if (l <= 1.02) {
        if (l > 1.0) {
            u = u / l;
            l = 1.0 / l;
        }
        // inverse stereographic projection, from skymap coordinates to world space directions
        float3 r = float3(2.0 * u, 1.0 - l) / (1.0 + l);
        result.rgb = skyRadiance(earthPos.xzy, r, SunNormal.xzy);
		
#ifdef CLOUDS
        float4 cloudL = cloudColorV(float3(0.0), r, sunDir);
        result.rgb = cloudL.rgb * cloudL.a + result.rgb * (1.0 - cloudL.a);
#endif
		
    } else {
        // below horizon:use average fresnel * average sky radiance to simulate multiple reflections on waves
        const float avgFresnel = 0.17;
        result.rgb = skyIrradiance(earthPos.y, SunNormal.y) / M_PI * avgFresnel;
    }
    return float4(result.rgb, 1);
}
/*
technique10 SkyMap {
    pass P0 {
        SetVertexShader( CompileShader( vs_4_0, VSQuadWone() ) );
        SetGeometryShader(NULL);
        SetPixelShader( CompileShader( ps_4_0, SkyMapPS() ) );
    }
}
*/