#include "Common_Ocean.fx"

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