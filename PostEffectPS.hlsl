#include "PostEffect.hlsli"

Texture2D<float4> tex0 : register(t0);
Texture2D<float4> tex1 : register(t1);
SamplerState smp : register(s0);

float Gaussian(float2 drawUV, float2 pickUV, float sigma)
{
    float d = distance(drawUV, pickUV);
    return exp(-(d * d) / (2 * sigma * sigma));
}

float4 main(VSOutput input) : SV_TARGET
{
    float4 texcolor0 = float4(1 - tex0.Sample(smp, input.uv_).rgb, 1);
    float totalWeight = 0;
    float _Sigma = 0.005;
    float _StepWidth = 0.001;

    float4 texcolor1 = float4(0, 0, 0, 0);

    for (float py = -_Sigma * 2; py <= _Sigma * 2; py += _StepWidth)
    {
        for (float px = -_Sigma * 2; px <= _Sigma * 2; px += _StepWidth)
        {
            float2 pickUV = input.uv_ + float2(px, py);
            float weight = Gaussian(input.uv_, pickUV, _Sigma);
            texcolor1 += tex1.Sample(smp, pickUV) * weight;
            totalWeight += weight;
        }
    }
    texcolor1.rgb = texcolor1.rgb / totalWeight;
    texcolor1.a = 1;

    float4 color = texcolor0;
    if (fmod(input.uv_.y, 0.1f) < 0.05f)
    {
        color = texcolor1;
    }
    
    return float4(color.rgb, 1);
}

//Texture2D<float4> tex : register(t0);
//SamplerState smp : register(s0);

//float4 main(VSOutput input) : SV_TARGET
//{
//    float4 texcolor = tex.Sample(smp, input.uv_);
//    return float4(texcolor.rgb, 1);
//}