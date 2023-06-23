

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

// �}�e���A��
cbuffer CBMaterial_t : register(b2)
{
	float3 mAmbient  : packoffset(c0);   // �A���r�G���g�W��
	float3 mDiffuse  : packoffset(c1);   // �f�B�t���[�Y�W��
	float3 mSpecular : packoffset(c2);   // �X�y�L�����[�W��
	float  mAlpha    : packoffset(c2.w); // �A���t�@
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
    float3 lightAtten; // ���C�g���������W��
    uint isActive;
};

struct SpotLight
{
    float3 lightV;
    float3 lightPos;
    float3 lightColor;
    float3 lightAtten;
    float2 lightFactorAngleCos; // ���C�g�����p�x��cos
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

// ���_�V�F�[�_�̏o�͍\����
// (���_�V�F�[�_����s�N�Z���V�F�[�_�ւ̂����Ɏg�p����)
struct VSOutput
{
    float4 svpos_ : SV_POSITION; // �V�X�e���p���_���W
    float4 wpos_ : POSITION;
    float3 normal_ : NORMAL; // �@���x�N�g��
    float2 uv_ : TEXCOORD;    // uv�l
};

struct PSOutput
{
    float4 target0 : SV_TARGET0;
    float4 target1 : SV_TARGET1;
};

// �W�I���g���V�F�[�_����s�N�Z���V�F�[�_�ւ̏o��
//struct GSOutput
//{
//    float4 svpos_ : SV_POSITION;
//    //float3 normal_ : NORMAL;
//    float4 color_ : COLOR;     // �F
//    float2 uv_ : TEXCOORD;
//};
