#include "Sprite.hlsli"

Texture2D<float4> tex : register(t0);

SamplerState smp : register(s0);

float4 main(VSOutput input) : SV_TARGET
{
    // gpuにif文させるか、無駄なuvを打ち込むか。。。
    //if (color_.a <= 0.1f)
    //    discard;
    
    return float4(tex.Sample(smp, input.uv_)) * color_;
}