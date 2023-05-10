

cbuffer CBData3d_t : register(b0)
{
    matrix matWorld_;
}

cbuffer CBMatViewPerse_t : register(b1)
{
    matrix matView_;
    matrix matPerspective_;
}

// �}�e���A��
cbuffer CBMaterial_t : register(b2)
{
	float3 mAmbient  : packoffset(c0);   // �A���r�G���g�W��
	float3 mDiffuse  : packoffset(c1);   // �f�B�t���[�Y�W��
	float3 mSpecular : packoffset(c2);   // �X�y�L�����[�W��
	float  mAlpha    : packoffset(c2.w); // �A���t�@
}

// ���_�V�F�[�_�̏o�͍\����
// (���_�V�F�[�_����s�N�Z���V�F�[�_�ւ̂����Ɏg�p����)
struct VSOutput
{
    float4 svpos_ : SV_POSITION; // �V�X�e���p���_���W
    float3 normal_ : NORMAL; // �@���x�N�g��
    float2 uv_ : TEXCOORD; // uv�l
};
