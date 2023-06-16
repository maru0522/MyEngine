#include "Model.hlsli"

Texture2D<float4> tex : register(t0);
SamplerState smp : register(s0);

float4 main(VSOutput input) : SV_TARGET
{
    //float3 light = normalize(float3(1, -1, 1)); 
    //float light_diffuse = saturate(dot(-light, input.normal_));
    //float3 shade_color;
    //shade_color = mAmbient; // ambient
    //shade_color += mDiffuse * light_diffuse; // diffuse
    //float4 texcolor = float4(tex.Sample(smp, input.uv_));
    //return float4(texcolor.rgb * shade_color, texcolor.a * mAlpha);
    
    // �e�N�X�`���}�b�s���O
    float4 texcolor = tex.Sample(smp, input.uv_);
    
    // �V�F�[�f�B���O�ɂ��F
    float4 shadecolor;
    // ����x
    const float shininess = 4.0f;
    // ���_���王�_�ւ̕����x�N�g��
    float3 eyedir = normalize(cameraPos_ - input.wpos_.xyz);
    // ���C�g�Ɍ������x�N�g���Ɩ@���̓���
    float3 dotlightnormal = dot(lightv, input.normal_);
    // ���ˌ��x�N�g��
    float3 reflect = normalize(-lightv + 2 * dotlightnormal * input.normal_);
    
    // �����ˌ�
    float3 ambient = mAmbient;
    // �g�U���ˌ�
    float3 diffuse = dotlightnormal * mDiffuse;
    // ���ʔ��ˌ�
    float3 specular = pow(saturate(dot(reflect, eyedir)), shininess) * mSpecular;
    // �S�ĉ��Z����
    shadecolor.rgb = (ambient + diffuse + specular) * lightcolor;
    shadecolor.a = mAlpha;
    
    // �V�F�[�f�B���O�ɂ��F�ŕ`��
    return shadecolor * texcolor;
}