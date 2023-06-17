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
    
    // �V�F�[�f�B���O�ɂ��F�ŕ`��
    return shadecolor * texcolor;
}