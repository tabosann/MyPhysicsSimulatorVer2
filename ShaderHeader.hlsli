struct Output
{
    float4 svpos    : SV_POSITION;
    float4 pos      : POSITION;
    float4 normal   : NORMAL;
    float3 color    : COLOR;
    float3 ray      : VECTOR;
};

cbuffer SceneData : register(b0)
{
    matrix view;
    matrix proj;
    float3 eyePos;
    float4 lightDir;
};

cbuffer Transform : register(b1)
{
    matrix world;
    float4 objColor;
};
