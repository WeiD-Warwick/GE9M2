cbuffer staticMeshBuffer : register(b0)
{
    float4x4 W;
    float4x4 V;
    float4x4 P;
    float2 uvScale;
    float2 _padding;
};

struct VS_INPUT
{
    float3 Pos : POSITION;
    float3 Normal : NORMAL;
    float3 Tangent : TANGENT;
    float2 TexCoords : TEXCOORD;
};

struct PS_INPUT
{
    float4 Pos : SV_POSITION;
    float3 Normal : NORMAL;
    float3 Tangent : TANGENT;
    float2 TexCoords : TEXCOORD;
};

PS_INPUT VS(VS_INPUT input) {
    PS_INPUT output;
    float4 pos = float4(input.Pos, 1.0f);
    pos = mul(pos, W);
    pos = mul(pos, V);
    pos = mul(pos, P);
    output.Pos = pos;
    output.Normal = mul(input.Normal, (float3x3)W);
    output.Tangent = mul(input.Tangent, (float3x3)W);
    output.TexCoords = input.TexCoords;
    return output;
}