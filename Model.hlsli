

cbuffer CBData3d_t : register(b0)
{
    matrix matWorld_;
}

cbuffer CBMatViewPerse_t : register(b1)
{
    matrix matView_;
    matrix matPerspective_;
}

// マテリアル
cbuffer CBMaterial_t : register(b2)
{
	float3 mAmbient  : packoffset(c0);   // アンビエント係数
	float3 mDiffuse  : packoffset(c1);   // ディフューズ係数
	float3 mSpecular : packoffset(c2);   // スペキュラー係数
	float  mAlpha    : packoffset(c2.w); // アルファ
}

// 頂点シェーダの出力構造体
// (頂点シェーダからピクセルシェーダへのやり取りに使用する)
struct VSOutput
{
    float4 svpos_ : SV_POSITION; // システム用頂点座標
    float3 normal_ : NORMAL; // 法線ベクトル
    float2 uv_ : TEXCOORD; // uv値
};
