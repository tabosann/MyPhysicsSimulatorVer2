#include"ShaderHeader.hlsli"

float4 PS(Output input) : SV_TARGET
{
	//���̕����x�N�g��(���s����)
    float3 lightA = normalize(lightDir.xyz);
    float3 lightB = normalize(float3(5.f, -1.f, 4.f));

	//���C�g�̃J���[(R, G, B)
    float3 lightColor = float3(1.f, 1.f, 1.f);

	//�f�B�t���[�Y�v�Z
    float diffuseA = dot(-lightA, input.normal.xyz);
    float diffuseB = dot(-lightB, input.normal.xyz);

	//���̔��˃x�N�g��
   // float3 refLight = normalize(reflect(light, input.normal.xyz));
    
    float ambient = 0.2; //�����̋���
    
    //���ˌ��Ɗ����̋������r���āA���������o�͂���
    return max(saturate(diffuseA * float4(input.color.xyz * lightColor, 1)) +
               saturate(diffuseB * float4(input.color.xyz * lightColor, 1))
               ,
               saturate(ambient  * float4(input.color.xyz * lightColor, 1))
               );
}
