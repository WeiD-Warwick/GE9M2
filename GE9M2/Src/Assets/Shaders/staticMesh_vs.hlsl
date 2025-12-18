#define MAX_POINT_LIGHTS 10

cbuffer staticMeshBuffer : register(b0) {
    float4x4 W;
    float4x4 V;
    float4x4 P;
};

struct VS_INPUT {
    float3 Pos : POSITION;
    float3 Normal : NORMAL;
    float3 Tangent : TANGENT;
    float2 TexCoords : TEXCOORD;
};

struct PS_INPUT {
    float4 Pos : SV_POSITION;
    float3 PosWS : TEXCOORD0;
    float3 NormalWS : TEXCOORD1;
    float3 TangentWS : TEXCOORD2;
    float2 TexCoords : TEXCOORD3;
};

PS_INPUT VS(VS_INPUT input) {
    PS_INPUT output;
    // --- Position ---
    float4 pos = float4(input.Pos, 1.0f);
    pos = mul(pos, W);
    
    output.PosWS = pos.xyz;
    output.Pos = mul(mul(pos, V), P);
    
    // --- NormalWS ---
    output.NormalWS = normalize(mul(input.Normal, (float3x3) W));
    
    // --- TangentWS ---
    output.TangentWS = normalize(mul(input.Tangent, (float3x3) W));
    
    // --- TexCoords ---
    output.TexCoords = input.TexCoords;

    return output;
}