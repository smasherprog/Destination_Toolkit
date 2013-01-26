#include "Common_Ocean.fx"

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

