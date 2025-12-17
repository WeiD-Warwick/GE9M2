#define MAX_POINT_LIGHTS 10

Texture2D albedoTex : register(t0);
Texture2D normalTex : register(t1);

SamplerState samplerLinear : register(s0);

cbuffer staticMeshBuffer : register(b0){
    float4x4 W;
    float4x4 V;
    float4x4 P;
    float2 uvScale;
    int useNormalMap;
    float1 _padding;
};


cbuffer lightCB : register(b1) {
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
}

struct PS_INPUT {
    float4 Pos : SV_POSITION;
    float3 PosWS : TEXCOORD0;
    float3 NormalWS : TEXCOORD1;
    float3 TangentWS : TEXCOORD2;
    float2 TexCoords : TEXCOORD3;
};

// Attenuation
float Attenuation(float d, float range) {
    float x = saturate(1.0 - d / max(range, 1e-4));
    return x * x;
}

float4 PS(PS_INPUT input) : SV_Target0 {
    
    // --- Albedo Map ---
    float3 albedoColor = albedoTex.Sample(samplerLinear, input.TexCoords * uvScale).rgb;
    return float4(albedoColor, 1);
    
    float3 N = normalize(input.NormalWS);
    
    // ---- Light ----
    float3 diffuseLighting = float3(0, 0, 0);
    
    for (int i = 0; i < pointLightCount; ++i) {
        float3 dirVector = input.PosWS - lightPosWS[i];
        
        float dist = length(dirVector);
        
        float3 normalLightDir = dirVector / max(dist, 1e-4);

        // Lambert
        float NdotL = saturate(dot(N, normalLightDir));
        
        // Attenuation
        float atten = Attenuation(dist, lightRange[i]);

        diffuseLighting += lightColor[i] * (lightIntensity[i] * NdotL * atten);
    }

    float3 ambient = albedoColor * skyLightColor * skyLightIntensity;
    return float4(ambient + albedoColor * diffuseLighting, 1.0);
}