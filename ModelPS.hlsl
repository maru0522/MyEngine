#include "Model.hlsli"

Texture2D<float4> tex : register(t0);
SamplerState smp : register(s0);

float4 main(VSOutput input) : SV_TARGET
{
    //float3 light = normalize(float3(1, -1, 1)); 
    //float light_diffuse = saturate(dot(-light, input.normal_));
    //float3 shade_color;
    //shade_color = mAmbient; // ambient
    //shade_color += mDiffuse * light_diffuse; // diffuse
    //float4 texcolor = float4(tex.Sample(smp, input.uv_));
    //return float4(texcolor.rgb * shade_color, texcolor.a * mAlpha);
    
    // テクスチャマッピング
    float4 texcolor = tex.Sample(smp, input.uv_);
    return input.color_;
}