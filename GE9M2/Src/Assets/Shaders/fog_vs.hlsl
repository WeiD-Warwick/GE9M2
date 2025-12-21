cbuffer fogCB : register(b0)
{
    float4x4 VP;
    float3 cameraRight;
    float _pad0;
    float3 cameraUp;
    float _pad1;
    float time;
    float fogDensity;
    float fogNoiseScale;
    float fogOpacity;
    float3 fogColor;
    float _pad2;
};

struct VS_INPUT
{
    float3 Pos : POSITION;
    float3 Normal : NORMAL;
    float3 Tangent : TANGENT;
    float2 TexCoords : TEXCOORD;

    float4x4 World : WORLD;
};

struct PS_INPUT
{
    float4 Pos : SV_POSITION;
    float2 TexCoords : TEXCOORD0;
    float3 WorldPos : TEXCOORD1;
};

PS_INPUT VS(VS_INPUT input)
{
    PS_INPUT output;

    // Quad corners (-0.5~0.5) come from the static plane mesh: use X/Z as quad axes
    float2 quad = float2(input.Pos.x, input.Pos.z);
    float3 center = float3(input.World._14, input.World._24, input.World._34);

    float scaleX = length(input.World[0].xyz);
    float scaleY = length(input.World[1].xyz);

    float3 worldPos = center
        + cameraRight * quad.x * scaleX
        + cameraUp * quad.y * scaleY;

    output.Pos = mul(float4(worldPos, 1.0f), VP);
    output.TexCoords = input.TexCoords;
    output.WorldPos = worldPos;
    return output;
}