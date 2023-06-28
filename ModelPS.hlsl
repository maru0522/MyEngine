#include "Model.hlsli"

Texture2D<float4> tex : register(t0);
SamplerState smp : register(s0);

PSOutput main(VSOutput input)
{
    PSOutput output;
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
            float3 dotlightnormal = dot(dirLights[i].lightV, input.normal_);
            // 反射光ベクトル
            float3 reflect = normalize(-dirLights[i].lightV + 2 * dotlightnormal * input.normal_);
            // 拡散反射光
            float3 diffuse = dotlightnormal * mDiffuse;
            // 鏡面反射光
            float3 specular = pow(saturate(dot(reflect, eyedir)), shininess) * mSpecular;
            
            // 全て加算する
            shadecolor.rgb += (diffuse + specular) * dirLights[i].lightColor;
        }
    }
    
    // 点光源
    for (uint j = 0; j < kPointLightNum; j++)
    {
        if (pointLights[j].isActive)
        {
            // ライトへのベクトル
            float3 lightv = pointLights[j].lightPos - input.wpos_.xyz;
            // ベクトルの長さ
            float d = length(lightv);
            // 正規化
            lightv = normalize(lightv);
            // 距離減衰係数
            float atten = 1.0f / (pointLights[j].lightAtten.x + pointLights[j].lightAtten.y * d + pointLights[j].lightAtten.z * d * d);
            // ライトに向かうベクトルと法線の内積
            float3 dotlightnormal = dot(lightv, input.normal_);
            // 反射光ベクトル
            float3 reflect = normalize(-lightv + 2 * dotlightnormal * input.normal_);
            // 拡散反射光
            float3 diffuse = dotlightnormal * mDiffuse;
            // 鏡面反射光
            float3 specular = pow(saturate(dot(reflect, eyedir)), shininess) * mSpecular;
            
            // 全て加算する
            shadecolor.rgb += atten * (diffuse + specular) * pointLights[j].lightColor;
        }
    }
    
    // スポットライト
    for (uint k = 0; k < kPointLightNum; k++)
    {
        if (spotLights[k].isActive)
        {
            // ライトへのベクトル
            float3 lightv = spotLights[k].lightPos - input.wpos_.xyz;
            // ベクトルの長さ
            float d = length(lightv);
            // 正規化
            lightv = normalize(lightv);
            // 距離減衰係数
            float atten = saturate(1.0f / (spotLights[k].lightAtten.x + spotLights[k].lightAtten.y * d + spotLights[k].lightAtten.z * d * d));
            // 角度減衰
            float cos = dot(lightv, spotLights[k].lightV);
            // 開始角度(内側:1倍輝度) ~ 終了角度(内側:0倍輝度)にかけて減衰
            float angleatten = smoothstep(spotLights[k].lightFactorAngleCos.y, spotLights[k].lightFactorAngleCos.x, cos);
            // 角度減衰を乗算
            atten *= angleatten;
            // ライトに向かうベクトルと法線の内積
            float3 dotlightnormal = dot(lightv, input.normal_);
            // 反射光ベクトル
            float3 reflect = normalize(-lightv + 2 * dotlightnormal * input.normal_);
            // 拡散反射光
            float3 diffuse = dotlightnormal * mDiffuse;
            // 鏡面反射光
            float3 specular = pow(saturate(dot(reflect, eyedir)), shininess) * mSpecular;
            
            // 全て加算する
            shadecolor.rgb += atten * (diffuse + specular) * spotLights[k].lightColor;
        }
    }
    
    // 丸影
    for (uint l = 0; l < kPointLightNum; l++)
    {
        if (circleShadows[l].isActive)
        {
            // キャスターへのベクトル
            float3 casterv = circleShadows[l].casterPos - input.wpos_.xyz;
            // 投影方向での距離
            float d = dot(casterv, circleShadows[l].dir);
            // 距離減衰係数
            float atten = saturate(1.0f / (circleShadows[l].atten.x + circleShadows[l].atten.y * d + circleShadows[l].atten.z * d * d));
            // 距離がマイナスなら0
            atten *= step(0, d);
            // 仮想ライトの座標
            float3 lightpos = circleShadows[l].casterPos + circleShadows[l].dir * circleShadows[l].distanceCasterLight;
            // オブジェクト表面からライトへのベクトル
            float3 lightv = normalize(lightpos - input.wpos_.xyz);
            // 角度減衰
            float cos = dot(lightv, circleShadows[l].dir);
            // 開始角度(内側:1倍輝度) ~ 終了角度(内側:0倍輝度)にかけて減衰
            float angleatten = smoothstep(circleShadows[l].factorAngleCos.y, circleShadows[l].factorAngleCos.x, cos);
            // 角度減衰を乗算
            atten *= angleatten;

            // 全て減算する
            shadecolor.rgb -=atten;
        }
    }
    
    // シェーディングによる色で描画
    output.target0 = shadecolor * texcolor;
    output.target1 = shadecolor * texcolor;
    return output;
}