#define MAX_POINT_LIGHTS 10

cbuffer staticMeshBuffer : register(b0) {
    
    // ---------- Mesh ----------
    float4x4 W;
    float4x4 V;
    float4x4 P;
    float2 uvScale;
    float2 _padding;
    
    // ---------- SkyLight ----------
    float3 skyLightColor;
    float skyLightIntensity;
    

    // ---------- PointLights ----------
    int pointLightCount;
    float3 _pad0;

    float3 lightPosWS[MAX_POINT_LIGHTS];
    float lightRange[MAX_POINT_LIGHTS];
    float3 lightColor[MAX_POINT_LIGHTS];
    float lightIntensity[MAX_POINT_LIGHTS];
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
    float3 Normal : TEXCOORD1;
    float2 TexCoords : TEXCOORD2;
};

PS_INPUT VS(VS_INPUT input) {
    PS_INPUT output;
    float4 pos = float4(input.Pos, 1.0f);
    pos = mul(pos, W);
    output.PosWS = pos.xyz;
    
    pos = mul(pos, V);
    pos = mul(pos, P);
    output.Pos = pos;
    output.Normal = mul(input.Normal, (float3x3)W);
    output.TexCoords = input.TexCoords;
    return output;
}