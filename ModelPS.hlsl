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
    
    // スポットライト
    for (uint k = 0; k < kPointLightNum; k++)
    {
        if (spotLights[k].isActive)
        {
            // ライトへのベクトル
            float3 lightv = spotLights[k].lightpos - input.wpos_.xyz;
            // ベクトルの長さ
            float d = length(lightv);
            // 正規化
            lightv = normalize(lightv);
            // 距離減衰係数
            float atten = saturate(1.0f / (spotLights[k].lightatten.x + spotLights[k].lightatten.y * d + spotLights[k].lightatten.z * d * d));
            // 角度減衰
            float cos = dot(lightv, spotLights[k].lightv);
            // 開始角度(内側:1倍輝度) ~ 終了角度(内側:0倍輝度)にかけて減衰
            float angleatten = smoothstep(spotLights[k].lightfactoranglecos.y, spotLights[k].lightfactoranglecos.x, cos);
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
            shadecolor.rgb += atten * (diffuse + specular) * spotLights[k].lightcolor;
        }
    }
    
    // 丸影
    for (uint l = 0; l < kPointLightNum; l++)
    {
        if (circleShadows[l].isActive)
        {
            // キャスターへのベクトル
            float3 casterv = circleShadows[l].casterpos - input.wpos_.xyz;
            // 投影方向での距離
            float d = dot(casterv, circleShadows[l].dir);
            // 距離減衰係数
            float atten = saturate(1.0f / (circleShadows[l].atten.x + circleShadows[l].atten.y * d + circleShadows[l].atten.z * d * d));
            // 距離がマイナスなら0
            atten *= step(0, d);
            // 仮想ライトの座標
            float3 lightpos = circleShadows[l].casterpos + circleShadows[l].dir * circleShadows[l].distanceCasterLight;
            // オブジェクト表面からライトへのベクトル
            float3 lightv = normalize(lightpos - input.wpos_.xyz);
            // 角度減衰
            float cos = dot(lightv, circleShadows[l].dir);
            // 開始角度(内側:1倍輝度) ~ 終了角度(内側:0倍輝度)にかけて減衰
            float angleatten = smoothstep(circleShadows[l].factoranglecos.y, circleShadows[l].factoranglecos.x, cos);
            // 角度減衰を乗算
            atten *= angleatten;

            // 全て減算する
            shadecolor.rgb -=atten;
        }
    }
    
    // シェーディングによる色で描画
    return shadecolor * texcolor;
}