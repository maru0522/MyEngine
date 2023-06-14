#include "Model.hlsli"

VSOutput main(float4 pos : POSITION, float3 normal : NORMAL, float2 uv : TEXCOORD)
{
    
    VSOutput output; // ピクセルシェーダーに渡す値
    output.svpos_ = mul(matWorld_, pos); // 座標に行列を乗算
    float3 eyedir = normalize(cameraPos_ - output.svpos_.xyz);
    output.svpos_ = mul(matView_,output.svpos_); // さらにビュー行列を乗算
    output.svpos_ = mul(matPerspective_,output.svpos_); // さらに射影変換行列を乗算
    //output.normal_ = normal;
    
    // ---
    float4 wnormal = normalize(mul(matWorld_, float4(normal, 0)));
    
    float3 ambient = mAmbient;
    float3 diffuse = dot(lightv, wnormal.xyz) * mDiffuse;
    const float shininess = 4.0f;
    float3 reflect = normalize(-lightv + 2 * dot(lightv, wnormal.xyz) * wnormal.xyz);
    float3 specular = pow(saturate(dot(reflect, eyedir)), shininess) * mSpecular;
    // ---

    output.color_.rgb = (ambient + diffuse + specular) * lightcolor; // 全て加算
    //output.color_.rgb = dot(-lightdir, normal) * mDiffuse * lightcolor;
    output.color_.a = mAlpha;
    output.uv_ = uv;
    return output;
}