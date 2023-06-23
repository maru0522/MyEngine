

cbuffer CBData3d_t : register(b0)
{
    matrix matWorld_;
}

cbuffer CBMatViewPerse_t : register(b1)
{
    matrix matView_;
    matrix matPerspective_;
    float3 cameraPos_;
}

// マテリアル
cbuffer CBMaterial_t : register(b2)
{
	float3 mAmbient  : packoffset(c0);   // アンビエント係数
	float3 mDiffuse  : packoffset(c1);   // ディフューズ係数
	float3 mSpecular : packoffset(c2);   // スペキュラー係数
	float  mAlpha    : packoffset(c2.w); // アルファ
}

static const uint kDirLightNum = 3;
static const uint kPointLightNum = 3;
static const uint kSpotLightNum = 3;
static const uint kCircleShadowNum = 3;

struct DirLight
{
    float3 lightV;
    float3 lightColor;
    uint isActive;
};

struct PointLight
{
    float3 lightPos;
    float3 lightColor;
    float3 lightAtten; // ライト距離減衰係数
    uint isActive;
};

struct SpotLight
{
    float3 lightV;
    float3 lightPos;
    float3 lightColor;
    float3 lightAtten;
    float2 lightFactorAngleCos; // ライト減衰角度のcos
    uint isActive;
};

struct CircleShadow
{
    float3 dir;
    float3 casterPos;
    float distanceCasterLight;
    float3 atten;
    float2 factorAngleCos;
    uint isActive;
};

cbuffer CBLight_t : register(b3)
{
    float3 ambientColor;
    DirLight dirLights[kDirLightNum];
    PointLight pointLights[kPointLightNum];
    SpotLight spotLights[kSpotLightNum];
    CircleShadow circleShadows[kCircleShadowNum];
}

// 頂点シェーダの出力構造体
// (頂点シェーダからピクセルシェーダへのやり取りに使用する)
struct VSOutput
{
    float4 svpos_ : SV_POSITION; // システム用頂点座標
    float4 wpos_ : POSITION;
    float3 normal_ : NORMAL; // 法線ベクトル
    float2 uv_ : TEXCOORD;    // uv値
};

struct PSOutput
{
    float4 target0 : SV_TARGET0;
    float4 target1 : SV_TARGET1;
};

// ジオメトリシェーダからピクセルシェーダへの出力
//struct GSOutput
//{
//    float4 svpos_ : SV_POSITION;
//    //float3 normal_ : NORMAL;
//    float4 color_ : COLOR;     // 色
//    float2 uv_ : TEXCOORD;
//};
