#include "GaussianBlur.hlsli"

VSOutput main(float4 pos : POSITION, float2 uv : TEXCOORD)
{
    VSOutput output; // �s�N�Z���V�F�[�_�[�ɓn���l
    output.svpos_ = pos;
    output.uv_ = uv;
    return output;
}