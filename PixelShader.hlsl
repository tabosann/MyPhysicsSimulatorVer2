#include"ShaderHeader.hlsli"

float4 PS(Output input) : SV_TARGET
{
	//光の方向ベクトル(平行光線)
    float3 lightA = normalize(lightDir.xyz);
    float3 lightB = normalize(float3(5.f, -1.f, 4.f));

	//ライトのカラー(R, G, B)
    float3 lightColor = float3(1.f, 1.f, 1.f);

	//ディフューズ計算
    float diffuseA = dot(-lightA, input.normal.xyz);
    float diffuseB = dot(-lightB, input.normal.xyz);

	//光の反射ベクトル
   // float3 refLight = normalize(reflect(light, input.normal.xyz));
    
    float ambient = 0.2; //環境光の強さ
    
    //反射光と環境光の強さを比較して、強い方を出力する
    return max(saturate(diffuseA * float4(input.color.xyz * lightColor, 1)) +
               saturate(diffuseB * float4(input.color.xyz * lightColor, 1))
               ,
               saturate(ambient  * float4(input.color.xyz * lightColor, 1))
               );
}
