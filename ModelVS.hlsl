#include "Model.hlsli"

VSOutput main(float4 pos : POSITION, float3 normal : NORMAL, float2 uv : TEXCOORD)
{
    VSOutput output; // ピクセルシェーダーに渡す値
    output.svpos_ = mul(matWorld_, pos); // 座標に行列を乗算
    output.svpos_ = mul(matView_,output.svpos_); // さらにビュー行列を乗算
    output.svpos_ = mul(matPerspective_,output.svpos_); // さらに射影変換行列を乗算
    output.normal_ = normal;
    output.uv_ = uv;
    return output;
}