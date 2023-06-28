#include "RadialBlur.hlsli"

Texture2D<float4> tex : register(t0);
SamplerState smp : register(s0);

float Gaussian(float2 drawUV, float2 pickUV, float sigma)
{
    float d = distance(drawUV, pickUV);
    return exp(-(d * d) / (2 * sigma * sigma));
}

float4 main(VSOutput input) : SV_TARGET
{
    const int _RadialCount = 10;
    float _Strength = 0.2;
    
    float4 col = 0;
    float2 symmetryUV = input.uv_ - 0.5;
    float distance = length(symmetryUV);
    for (int j = 0; j < _RadialCount; j++)
    {
        float uvOffset = 1 - _Strength * j / _RadialCount * distance;
        col += tex.Sample(smp, symmetryUV * uvOffset + 0.5);
    }
    col /= _RadialCount;
    return col;
}