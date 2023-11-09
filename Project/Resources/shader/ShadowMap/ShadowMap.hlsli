cbuffer SceneBuffer : register(b0)
{
    matrix view;            //�r���[
    matrix proj;            //�v���W�F�N�V����
    matrix shadow;          //�e�s��
    matrix lightCamera;
    float3 eye;             //���_
}

//���_�V�F�[�_�[����̏o�͍\����
struct VSOutput
{
    float4 svpos : SV_POSITION;
    float2 uv : TEXCOORD;
};