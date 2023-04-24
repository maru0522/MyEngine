#include "Sprite.hlsli"

VSOutput main(float4 pos : POSITION, float2 uv : TEXCOORD)
{
    VSOutput output; // ピクセルシェーダーに渡す値
    output.svpos_ = mul(mat_, pos); // 座標に行列を乗算
    output.svpos_ = mul(matOrthoGraphic_, output.svpos_); // 平行移動行列を乗算
    pos.z = 0.0f;
    output.uv_ = uv;
    return output;
}