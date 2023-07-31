#include "Model.hlsli"

VSOutput main(float4 pos : POSITION, float3 normal : NORMAL, float2 uv : TEXCOORD)
{
    
    VSOutput output; // ピクセルシェーダーに渡す値
    output.uv_ = uv;
    output.wpos_ = mul(matWorld_, pos); // 座標に行列を乗算
    output.normal_ = normalize(mul(matWorld_, float4(normal, 0))).xyz;

    output.svpos_ = mul(matView_,output.wpos_); // さらにビュー行列を乗算
    output.svpos_ = mul(matPerspective_,output.svpos_); // さらに射影変換行列を乗算
    
    return output;
}