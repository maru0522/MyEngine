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
    
    // テクスチャマッピング
    float4 texcolor = tex.Sample(smp, input.uv_);
    
    // シェーディングによる色
    float4 shadecolor;
    // 光沢度
    const float shininess = 4.0f;
    // 頂点から視点への方向ベクトル
    float3 eyedir = normalize(cameraPos_ - input.wpos_.xyz);
    // ライトに向かうベクトルと法線の内積
    float3 dotlightnormal = dot(lightv, input.normal_);
    // 反射光ベクトル
    float3 reflect = normalize(-lightv + 2 * dotlightnormal * input.normal_);
    
    // 環境反射光
    float3 ambient = mAmbient;
    // 拡散反射光
    float3 diffuse = dotlightnormal * mDiffuse;
    // 鏡面反射光
    float3 specular = pow(saturate(dot(reflect, eyedir)), shininess) * mSpecular;
    // 全て加算する
    shadecolor.rgb = (ambient + diffuse + specular) * lightcolor;
    shadecolor.a = mAlpha;
    
    // シェーディングによる色で描画
    return shadecolor * texcolor;
}