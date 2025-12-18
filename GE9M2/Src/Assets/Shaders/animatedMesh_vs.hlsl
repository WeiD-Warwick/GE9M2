#define MAX_POINT_LIGHTS 10

cbuffer animatedMeshBuffer
{
    // ---------- Mesh ----------
    float4x4 W;
    float4x4 V;
    float4x4 P;
    float4x4 bones[256];
};

struct VS_INPUT
{
    float3 Pos : POSITION;
    float3 Normal : NORMAL;
    float3 Tangent : TANGENT;
    float2 TexCoords : TEXCOORD;
    uint4 BoneIDs : BONEIDS;
    float4 BoneWeights : BONEWEIGHTS;
};

struct PS_INPUT
{
    float4 Pos : SV_POSITION;
    float3 PosWS : TEXCOORD0;
    float3 NormalWS : TEXCOORD1;
    float3 TangentWS : TEXCOORD2;
    float2 TexCoords : TEXCOORD3;
};

PS_INPUT VS(VS_INPUT input)
{
    PS_INPUT output;
    
    // --- Transform
    float4x4 transform = bones[input.BoneIDs[0]] * input.BoneWeights[0];
    transform += bones[input.BoneIDs[1]] * input.BoneWeights[1];
    transform += bones[input.BoneIDs[2]] * input.BoneWeights[2];
    transform += bones[input.BoneIDs[3]] * input.BoneWeights[3];
    
    // --- Position ---
    float4 pos = float4(input.Pos, 1.0f);
    pos = mul(mul(pos, transform), W);
    output.PosWS = pos.xyz;
    output.Pos = mul(mul(pos, V), P);
    
    // --- NormalWS ---
    output.NormalWS = normalize(mul(mul(input.Normal, (float3x3) transform), (float3x3) W));
    
    // --- TangentWS ---
    output.TangentWS = normalize(mul(input.Tangent, (float3x3) W));
    
    // --- TexCoords ---
    output.TexCoords = input.TexCoords;
    return output;
}