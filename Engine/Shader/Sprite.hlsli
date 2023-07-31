// �ʏ��2D�`��
cbuffer CBData2d_t : register(b0)
{
    matrix matWorld_;   // 3D�ϊ��s��
    float4 color_; // �F(RGBA)
}

cbuffer CBMatOrthoGraphic_t : register(b1)
{
    matrix matOrthoGraphic_;
}

// ���_�V�F�[�_�̏o�͍\����
// (���_�V�F�[�_����s�N�Z���V�F�[�_�ւ̂����Ɏg�p����)
struct VSOutput
{
	// �V�X�e���p���_���W
    float4 svpos_ : SV_POSITION;
	// uv�l
    float2 uv_ : TEXCOORD;
};