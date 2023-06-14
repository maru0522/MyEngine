#include "Model.hlsli"

VSOutput main(float4 pos : POSITION, float3 normal : NORMAL, float2 uv : TEXCOORD)
{
    
    VSOutput output; // �s�N�Z���V�F�[�_�[�ɓn���l
    output.svpos_ = mul(matWorld_, pos); // ���W�ɍs�����Z
    float3 eyedir = normalize(cameraPos_ - output.svpos_.xyz);
    output.svpos_ = mul(matView_,output.svpos_); // ����Ƀr���[�s�����Z
    output.svpos_ = mul(matPerspective_,output.svpos_); // ����Ɏˉe�ϊ��s�����Z
    //output.normal_ = normal;
    
    // ---
    float4 wnormal = normalize(mul(matWorld_, float4(normal, 0)));
    
    float3 ambient = mAmbient;
    float3 diffuse = dot(lightv, wnormal.xyz) * mDiffuse;
    const float shininess = 4.0f;
    float3 reflect = normalize(-lightv + 2 * dot(lightv, wnormal.xyz) * wnormal.xyz);
    float3 specular = pow(saturate(dot(reflect, eyedir)), shininess) * mSpecular;
    // ---

    output.color_.rgb = (ambient + diffuse + specular) * lightcolor; // �S�ĉ��Z
    //output.color_.rgb = dot(-lightdir, normal) * mDiffuse * lightcolor;
    output.color_.a = mAlpha;
    output.uv_ = uv;
    return output;
}