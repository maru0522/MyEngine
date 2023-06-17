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

    // 平行光源
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
    
    // 点光源
    for (uint j = 0; j < kPointLightNum; j++)
    {
        if (pointLights[j].isActive)
        {
            // ライトへのベクトル
            float3 lightv = pointLights[j].lightpos - input.wpos_.xyz;
            // ベクトルの長さ
            float d = length(lightv);
            // 正規化
            lightv = normalize(lightv);
            // 距離減衰係数
            float atten = 1.0f / (pointLights[j].lightatten.x + pointLights[j].lightatten.y * d + pointLights[j].lightatten.z * d * d);
            // ライトに向かうベクトルと法線の内積
            float3 dotlightnormal = dot(lightv, input.normal_);
            // 反射光ベクトル
            float3 reflect = normalize(-lightv + 2 * dotlightnormal * input.normal_);
            // 拡散反射光
            float3 diffuse = dotlightnormal * mDiffuse;
            // 鏡面反射光
            float3 specular = pow(saturate(dot(reflect, eyedir)), shininess) * mSpecular;
            
            // 全て加算する
            shadecolor.rgb += atten * (diffuse + specular) * pointLights[j].lightcolor;
        }
    }
    
    // シェーディングによる色で描画
    return shadecolor * texcolor;
}