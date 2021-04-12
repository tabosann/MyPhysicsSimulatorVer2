#include"ShaderHeader.hlsli"

Output VS( 
	//インプットレイアウトに合わせる
	float4 pos		: POSITION,
	float4 normal	: NORMAL,
	float3 color	: COLOR
) {
	Output output;
    pos = mul(world, pos) - float4(eyePos, 0.f);
	output.svpos = mul(mul(proj, view), pos);
	output.pos = mul(view, pos);
    output.color = objColor;
	normal.w = 0; //法線の向きだけを変更したいので、平行移動成分を無効化
    output.normal = mul(world, normal);
    output.ray = normalize(pos.xyz - mul(view, eyePos));
	
	//output.normal = mul(world, normal); //法線の向きを変更
	//output.vnormal = mul(view, output.normal); //現在のカメラの向きを考慮
	return output;
}