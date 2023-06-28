#include "Bloom.hlsli"

Texture2D<float4> tex0 : register(t0);
Texture2D<float4> tex1 : register(t1);
SamplerState smp : register(s0);

float4 ExtractLuminance(float2 uv)
{
    float4 col = tex0.Sample(smp, uv);
    float grayScale = col.r * 0.299 + col.g * 0.587 + col.b * 0.114;
    float extract = smoothstep(0.6, 0.9, grayScale);
    return col * extract;
}

float Gaussian(float2 drawUV, float2 pickUV, float sigma)
{
    float d = distance(drawUV, pickUV);
    return exp(-(d * d) / (2 * sigma * sigma));
}

float4 Blur(float4 hiLumiColor,float2 uv)
{
    float totalWeight = 0, _Sigma = 0.01, _StepWidth = 0.001; // Bloom�̓u���[�����傰���ɂ�����
    float4 col = float4(0, 0, 0, 0);

    for (float py = -_Sigma * 2; py <= _Sigma * 2; py += _StepWidth) // xy��2�Ђ̕��ŐF���擾
    {
        for (float px = -_Sigma * 2; px <= _Sigma * 2; px += _StepWidth)
        {
            float2 pickUV = uv + float2(px, py);
            float4 weight = Gaussian(uv, pickUV, _Sigma);
            
            col += ExtractLuminance(pickUV) * weight; // Gaussian�Ŏ擾�����u�d�݁v��F�ɂ�����
            totalWeight += weight; // �������u�d�݁v�̍��v�l���T���Ă���
        }
    }
    col.rgb = col.rgb / totalWeight; // �������u�d�݁v���A���ʂ��犄��
    col.a = 1;
    return col;
}

float4 main(VSOutput input) : SV_TARGET
{
    float4 hiLumiTexColor = ExtractLuminance(input.uv_);
    float4 blurTexColor = Blur(hiLumiTexColor, input.uv_);
    float4 texcolor = tex1.Sample(smp, input.uv_);
    return texcolor + blurTexColor;
}