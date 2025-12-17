#define MAX_POINT_LIGHTS 10

cbuffer staticMeshBuffer : register(b0) {
    float4x4 W;
    float4x4 V;
    float4x4 P;
    float2 uvScale;
    int useNormalMap;
    float1 _pad0;
};


cbuffer lightCB : register(b1) {
    // ---------- SkyLight ----------
    float3 skyLightColor;
    float skyLightIntensity;
    
    // ---------- PointLights ----------
    int pointLightCount;
    float3 lightPositions[MAX_POINT_LIGHTS];
    float lightRanges[MAX_POINT_LIGHTS];
    float3 lightColors[MAX_POINT_LIGHTS];
    float lightIntensities[MAX_POINT_LIGHTS];
    float3 _pad1;
}

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
    float4 pos = float4(input.Pos, 1.0f);
    float4 posW = mul(pos, W);
    float4 cameraSpace = mul(posW, V);
    float4 clipSpace = mul(cameraSpace, P);
    
    output.Pos = clipSpace;
    output.PosWS = posW.xyz;
    output.NormalWS = normalize(mul(input.Normal, (float3x3) W));
    output.TangentWS = normalize(mul(input.Tangent, (float3x3) W));
    output.TexCoords = input.TexCoords;

    return output;
}