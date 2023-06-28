#include "PostEffect.hlsli"

Texture2D<float4> tex0 : register(t0);
Texture2D<float4> tex1 : register(t1);
SamplerState smp : register(s0);

float4 main(VSOutput input) : SV_TARGET
{
    float4 texcolor0 = tex0.Sample(smp, input.uv_);
    float4 texcolor1 = tex1.Sample(smp, input.uv_);

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