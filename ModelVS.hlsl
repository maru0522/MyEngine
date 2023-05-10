#include "Model.hlsli"

VSOutput main(float4 pos : POSITION, float3 normal : NORMAL, float2 uv : TEXCOORD)
{
    VSOutput output; // �s�N�Z���V�F�[�_�[�ɓn���l
    output.svpos_ = mul(matWorld_, pos); // ���W�ɍs�����Z
    output.svpos_ = mul(matView_,output.svpos_); // ����Ƀr���[�s�����Z
    output.svpos_ = mul(matPerspective_,output.svpos_); // ����Ɏˉe�ϊ��s�����Z
    output.normal_ = normal;
    output.uv_ = uv;
    return output;
}