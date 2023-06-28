#include "HighLumi.hlsli"

Texture2D<float4> tex0 : register(t0);
SamplerState smp : register(s0);

float4 main(VSOutput input) : SV_TARGET
{
    float4 col = tex0.Sample(smp, input.uv);
    float grayScale = col.r * 0.299 + col.g * 0.587 + col.b * 0.114;
    float extract = smoothstep(0.6, 0.9, grayScale);
    return col * extract;

    return float4((col * extract).rgb, 1);
}
