// 通常の2D描画
cbuffer CBData2d_t : register(b0)
{
    matrix matWorld_;   // 3D変換行列
    float4 color_; // 色(RGBA)
}

cbuffer CBMatOrthoGraphic_t : register(b1)
{
    matrix matOrthoGraphic_;
}

// 頂点シェーダの出力構造体
// (頂点シェーダからピクセルシェーダへのやり取りに使用する)
struct VSOutput
{
	// システム用頂点座標
    float4 svpos_ : SV_POSITION;
	// uv値
    float2 uv_ : TEXCOORD;
};