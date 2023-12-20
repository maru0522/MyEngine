#include "Sprite.hlsli"

VSOutput main(float4 pos : POSITION, float2 uv : TEXCOORD)
{
    VSOutput output; // �s�N�Z���V�F�[�_�[�ɓn���l
    output.svpos_ = mul(matWorld_, pos); // ���W�ɍs�����Z
    output.svpos_ = mul(matOrthoGraphic_, output.svpos_); // ���s�ړ��s�����Z
    pos.z = 0.0f;
    output.uv_ = uv;
    return output;
}