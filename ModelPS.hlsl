#include "Model.hlsli"

Texture2D<float4> tex : register(t0);
SamplerState smp : register(s0);

float4 main(VSOutput input) : SV_TARGET
{
    // テクスチャマッピング
    float4 texcolor = tex.Sample(smp, input.uv_);
    // 光沢度
    const float shininess = 4.0f;
    // 頂点から視点への方向ベクトル
    float3 eyedir = normalize(cameraPos_ - input.wpos_.xyz);
    
    // 環境反射光
    float3 ambient = mAmbient;
    
    // シェーディングによる色
    float4 shadecolor = float4(ambientColor * ambient, mAlpha);

    for (uint i = 0; i < kDirLightNum; i++)
    {
        if (dirLights[i].isActive)
        {
            // ライトに向かうベクトルと法線の内積
            float3 dotlightnormal = dot(dirLights[i].lightv, input.normal_);
            // 反射光ベクトル
            float3 reflect = normalize(-dirLights[i].lightv + 2 * dotlightnormal * input.normal_);
            // 拡散反射光
            float3 diffuse = dotlightnormal * mDiffuse;
            // 鏡面反射光
            float3 specular = pow(saturate(dot(reflect, eyedir)), shininess) * mSpecular;
            
            // 全て加算する
            shadecolor.rgb += (diffuse + specular) * dirLights[i].lightcolor;
        }
    }
    
    // シェーディングによる色で描画
    return shadecolor * texcolor;
}