#include "Model.hlsli"

Texture2D<float4> tex : register(t0);
SamplerState smp : register(s0);

float4 main(VSOutput input) : SV_TARGET
{
    // �e�N�X�`���}�b�s���O
    float4 texcolor = tex.Sample(smp, input.uv_);
    // ����x
    const float shininess = 4.0f;
    // ���_���王�_�ւ̕����x�N�g��
    float3 eyedir = normalize(cameraPos_ - input.wpos_.xyz);
    
    // �����ˌ�
    float3 ambient = mAmbient;
    
    // �V�F�[�f�B���O�ɂ��F
    float4 shadecolor = float4(ambientColor * ambient, mAlpha);

    // ���s����
    for (uint i = 0; i < kDirLightNum; i++)
    {
        if (dirLights[i].isActive)
        {
            // ���C�g�Ɍ������x�N�g���Ɩ@���̓���
            float3 dotlightnormal = dot(dirLights[i].lightv, input.normal_);
            // ���ˌ��x�N�g��
            float3 reflect = normalize(-dirLights[i].lightv + 2 * dotlightnormal * input.normal_);
            // �g�U���ˌ�
            float3 diffuse = dotlightnormal * mDiffuse;
            // ���ʔ��ˌ�
            float3 specular = pow(saturate(dot(reflect, eyedir)), shininess) * mSpecular;
            
            // �S�ĉ��Z����
            shadecolor.rgb += (diffuse + specular) * dirLights[i].lightcolor;
        }
    }
    
    // �_����
    for (uint j = 0; j < kPointLightNum; j++)
    {
        if (pointLights[j].isActive)
        {
            // ���C�g�ւ̃x�N�g��
            float3 lightv = pointLights[j].lightpos - input.wpos_.xyz;
            // �x�N�g���̒���
            float d = length(lightv);
            // ���K��
            lightv = normalize(lightv);
            // ���������W��
            float atten = 1.0f / (pointLights[j].lightatten.x + pointLights[j].lightatten.y * d + pointLights[j].lightatten.z * d * d);
            // ���C�g�Ɍ������x�N�g���Ɩ@���̓���
            float3 dotlightnormal = dot(lightv, input.normal_);
            // ���ˌ��x�N�g��
            float3 reflect = normalize(-lightv + 2 * dotlightnormal * input.normal_);
            // �g�U���ˌ�
            float3 diffuse = dotlightnormal * mDiffuse;
            // ���ʔ��ˌ�
            float3 specular = pow(saturate(dot(reflect, eyedir)), shininess) * mSpecular;
            
            // �S�ĉ��Z����
            shadecolor.rgb += atten * (diffuse + specular) * pointLights[j].lightcolor;
        }
    }
    
    // �V�F�[�f�B���O�ɂ��F�ŕ`��
    return shadecolor * texcolor;
}