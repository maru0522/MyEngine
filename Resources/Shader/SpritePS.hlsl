#include "Sprite.hlsli"

Texture2D<float4> tex : register(t0);

SamplerState smp : register(s0);

float4 main(VSOutput input) : SV_TARGET
{
    // gpu��if�������邩�A���ʂ�uv��ł����ނ��B�B�B
    //if (color_.a <= 0.1f)
    //    discard;
    
    return float4(tex.Sample(smp, input.uv_)) * color_;
}