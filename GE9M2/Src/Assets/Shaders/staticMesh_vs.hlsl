cbuffer staticMeshBuffer : register(b0) {
    float4x4 M;
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
    float3 Normal : NORMAL;
    float3 Tangent : TANGENT;
    float2 TexCoords : TEXCOORD;
};

PS_INPUT VS(VS_INPUT input) {
    PS_INPUT output;

    float4 posM = mul(M, float4(input.Pos, 1));
    float4 posMV = mul(V, posM);
    float4 posMVP = mul(P, posMV);

    output.Pos = posMVP;
    output.Normal = mul((float3x3) M, input.Normal);
    output.Tangent = mul((float3x3) M, input.Tangent);
    output.TexCoords = input.TexCoords;

    return output;
}
