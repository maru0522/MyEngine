#include "Model.hlsli"

VSOutput main(float4 pos : POSITION, float3 normal : NORMAL, float2 uv : TEXCOORD)
{
    
    VSOutput output; // �s�N�Z���V�F�[�_�[�ɓn���l
    output.uv_ = uv;
    output.wpos_ = mul(matWorld_, pos); // ���W�ɍs�����Z
    output.normal_ = normalize(mul(matWorld_, float4(normal, 0))).xyz;

    output.svpos_ = mul(matView_,output.wpos_); // ����Ƀr���[�s�����Z
    output.svpos_ = mul(matPerspective_,output.svpos_); // ����Ɏˉe�ϊ��s�����Z
    
    return output;
}