#include "Common_Ocean.fx"

float2 getSpectrum(float k, float2 s0, float2 s0c) {
    float w = sqrt(9.81 * k * (1.0 + k * k / (370.0 * 370.0)));
    float c = cos(w * lastime.x);
    float s = sin(w * lastime.x);
    return float2((s0.x + s0c.x) * c - (s0.y + s0c.y) * s, (s0.x - s0c.x) * s + (s0.y - s0c.y) * c);
}
float2 i(float2 z) { return float2(-z.y, z.x); }// returns i times z (complex number)

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