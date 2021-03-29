#include"ShaderHeader.hlsli"

Output VS( 
	//�C���v�b�g���C�A�E�g�ɍ��킹��
	float4 pos		: POSITION,
	float4 normal	: NORMAL,
	float3 color	: COLOR
) {
	Output output;
    pos = mul(world, pos) - float4(eyePos, 0.f);
	output.svpos = mul(mul(proj, view), pos);
	output.pos = mul(view, pos);
    output.color = objColor;
	normal.w = 0; //�@���̌���������ύX�������̂ŁA���s�ړ������𖳌���
    output.normal = mul(world, normal);
    output.ray = normalize(pos.xyz - mul(view, eyePos));
	
	//output.normal = mul(world, normal); //�@���̌�����ύX
	//output.vnormal = mul(view, output.normal); //���݂̃J�����̌������l��
	return output;
}