Texture2D tex : register(t0);
SamplerState samplerLinear : register(s0);
cbuffer staticMeshBuffer : register(b0)
{
    float4x4 W;
    float4x4 V;
    float4x4 P;
    float2 uvScale;
    float2 _padding;
};

struct PS_INPUT
{
    float4 Pos : SV_POSITION;
    float3 Normal : NORMAL;
    float3 Tangent : TANGENT;
    float2 TexCoords : TEXCOORD;
};

float4 PS(PS_INPUT input) : SV_Target0
{
    float2 tiledUV = input.TexCoords * uvScale;
    return tex.Sample(samplerLinear, tiledUV);
}