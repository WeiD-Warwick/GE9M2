#define MAX_POINT_LIGHTS 10

Texture2D albedoTex : register(t0);
SamplerState samplerLinear : register(s0);

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

struct PS_INPUT {
    float4 Pos : SV_POSITION;
    float3 PosWS : TEXCOORD0;
    float3 Normal : TEXCOORD1;
    float2 TexCoords : TEXCOORD2;
};

float Attenuation(float d, float range) {
    float x = saturate(1.0 - d / max(range, 1e-4));
    return x * x;
}

float4 PS(PS_INPUT input) : SV_Target0 {
    float3 albedo = albedoTex.Sample(samplerLinear, input.TexCoords).rgb;
    
    float3 lighting = 0.0f;
    float3 N = normalize(input.Normal);

    for (int i = 0; i < pointLightCount; ++i) {
        float3 Lvec = lightPosWS[i] - input.PosWS;
        float dist = length(Lvec);
        float3 L = Lvec / max(dist, 1e-4);

        float NdotL = saturate(dot(N, L));
        float atten = Attenuation(dist, lightRange[i]);

        lighting += lightColor[i]
              * (lightIntensity[i] * NdotL * atten);
    }

    float3 ambient = albedo * skyLightColor * skyLightIntensity;
    return float4(ambient + albedo * lighting, 1.0);
}