#include "Model.hlsli"

VSOutput main(float4 pos : POSITION, float3 normal : NORMAL, float2 uv : TEXCOORD)
{
    // ---
    float3 lightdir = float3(1, -1, 1); // ���C�g
    lightdir = normalize(lightdir);
    
    float3 lightcolor = float3(1, 1, 1); // ���C�g�̐F
    
    float3 ambient = mAmbient;
    float3 diffuse = dot(-lightdir, normal) * mDiffuse;
    const float3 eye = float3(0, 0, -20);
    const float shininess = 4.0f;
    float3 eyedir = normalize(eye - pos.xyz);
    float3 reflect = normalize(lightdir + 2 * dot(-lightdir, normal) * normal);
    float3 specular = pow(saturate(dot(reflect, eyedir)), shininess) * mSpecular;
    // ---
    
    VSOutput output; // �s�N�Z���V�F�[�_�[�ɓn���l
    output.svpos_ = mul(matWorld_, pos); // ���W�ɍs�����Z
    output.svpos_ = mul(matView_,output.svpos_); // ����Ƀr���[�s�����Z
    output.svpos_ = mul(matPerspective_,output.svpos_); // ����Ɏˉe�ϊ��s�����Z
    //output.normal_ = normal;
    
    output.color_.rgb = (ambient + diffuse + specular) * lightcolor; // �S�ĉ��Z
    output.color_.a = mAlpha;
    output.uv_ = uv;
    return output;
}