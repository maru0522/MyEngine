#include "Model.hlsli"

Texture2D<float4> tex : register(t0);
SamplerState smp : register(s0);

PSOutput main(VSOutput input)
{
    PSOutput output;
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
            float3 dotlightnormal = dot(dirLights[i].lightV, input.normal_);
            // ���ˌ��x�N�g��
            float3 reflect = normalize(-dirLights[i].lightV + 2 * dotlightnormal * input.normal_);
            // �g�U���ˌ�
            float3 diffuse = dotlightnormal * mDiffuse;
            // ���ʔ��ˌ�
            float3 specular = pow(saturate(dot(reflect, eyedir)), shininess) * mSpecular;
            
            // �S�ĉ��Z����
            shadecolor.rgb += (diffuse + specular) * dirLights[i].lightColor;
        }
    }
    
    // �_����
    for (uint j = 0; j < kPointLightNum; j++)
    {
        if (pointLights[j].isActive)
        {
            // ���C�g�ւ̃x�N�g��
            float3 lightv = pointLights[j].lightPos - input.wpos_.xyz;
            // �x�N�g���̒���
            float d = length(lightv);
            // ���K��
            lightv = normalize(lightv);
            // ���������W��
            float atten = 1.0f / (pointLights[j].lightAtten.x + pointLights[j].lightAtten.y * d + pointLights[j].lightAtten.z * d * d);
            // ���C�g�Ɍ������x�N�g���Ɩ@���̓���
            float3 dotlightnormal = dot(lightv, input.normal_);
            // ���ˌ��x�N�g��
            float3 reflect = normalize(-lightv + 2 * dotlightnormal * input.normal_);
            // �g�U���ˌ�
            float3 diffuse = dotlightnormal * mDiffuse;
            // ���ʔ��ˌ�
            float3 specular = pow(saturate(dot(reflect, eyedir)), shininess) * mSpecular;
            
            // �S�ĉ��Z����
            shadecolor.rgb += atten * (diffuse + specular) * pointLights[j].lightColor;
        }
    }
    
    // �X�|�b�g���C�g
    for (uint k = 0; k < kPointLightNum; k++)
    {
        if (spotLights[k].isActive)
        {
            // ���C�g�ւ̃x�N�g��
            float3 lightv = spotLights[k].lightPos - input.wpos_.xyz;
            // �x�N�g���̒���
            float d = length(lightv);
            // ���K��
            lightv = normalize(lightv);
            // ���������W��
            float atten = saturate(1.0f / (spotLights[k].lightAtten.x + spotLights[k].lightAtten.y * d + spotLights[k].lightAtten.z * d * d));
            // �p�x����
            float cos = dot(lightv, spotLights[k].lightV);
            // �J�n�p�x(����:1�{�P�x) ~ �I���p�x(����:0�{�P�x)�ɂ����Č���
            float angleatten = smoothstep(spotLights[k].lightFactorAngleCos.y, spotLights[k].lightFactorAngleCos.x, cos);
            // �p�x��������Z
            atten *= angleatten;
            // ���C�g�Ɍ������x�N�g���Ɩ@���̓���
            float3 dotlightnormal = dot(lightv, input.normal_);
            // ���ˌ��x�N�g��
            float3 reflect = normalize(-lightv + 2 * dotlightnormal * input.normal_);
            // �g�U���ˌ�
            float3 diffuse = dotlightnormal * mDiffuse;
            // ���ʔ��ˌ�
            float3 specular = pow(saturate(dot(reflect, eyedir)), shininess) * mSpecular;
            
            // �S�ĉ��Z����
            shadecolor.rgb += atten * (diffuse + specular) * spotLights[k].lightColor;
        }
    }
    
    // �ۉe
    for (uint l = 0; l < kPointLightNum; l++)
    {
        if (circleShadows[l].isActive)
        {
            // �L���X�^�[�ւ̃x�N�g��
            float3 casterv = circleShadows[l].casterPos - input.wpos_.xyz;
            // ���e�����ł̋���
            float d = dot(casterv, circleShadows[l].dir);
            // ���������W��
            float atten = saturate(1.0f / (circleShadows[l].atten.x + circleShadows[l].atten.y * d + circleShadows[l].atten.z * d * d));
            // �������}�C�i�X�Ȃ�0
            atten *= step(0, d);
            // ���z���C�g�̍��W
            float3 lightpos = circleShadows[l].casterPos + circleShadows[l].dir * circleShadows[l].distanceCasterLight;
            // �I�u�W�F�N�g�\�ʂ��烉�C�g�ւ̃x�N�g��
            float3 lightv = normalize(lightpos - input.wpos_.xyz);
            // �p�x����
            float cos = dot(lightv, circleShadows[l].dir);
            // �J�n�p�x(����:1�{�P�x) ~ �I���p�x(����:0�{�P�x)�ɂ����Č���
            float angleatten = smoothstep(circleShadows[l].factorAngleCos.y, circleShadows[l].factorAngleCos.x, cos);
            // �p�x��������Z
            atten *= angleatten;

            // �S�Č��Z����
            shadecolor.rgb -=atten;
        }
    }
    
    // �V�F�[�f�B���O�ɂ��F�ŕ`��
    output.target0 = shadecolor * texcolor;
    output.target1 = shadecolor * texcolor;
    return output;
}