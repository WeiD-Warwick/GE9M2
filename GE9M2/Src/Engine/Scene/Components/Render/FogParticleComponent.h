#pragma once
#include "../Component.h"
#include "../../../Graphics/Instancing/InstanceData.h"
#include "../../../Graphics/Material/Material.h"
#include "../../../Graphics/Model/ModelData.h"
#include <vector>
#include <string>

class FogParticleComponent : public RenderComponent {
private:
    struct Particle {
        Vec3 basePos;
        float scale = 1.0f;
        float swaySpeed = 1.0f;
        float phase = 0.0f;
    };

    std::vector<Particle> _particles;
    std::vector<InstanceData> _instances;
    std::vector<SubMesh> _subMeshes;
    std::vector<Material*> _materials;

    std::string _materialKey;
    int _count = 0;
    float _radius = 10.0f;
    float _minHeight = 0.5f;
    float _maxHeight = 2.5f;
    float _minScale = 1.0f;
    float _maxScale = 2.0f;
    float _density = 0.5f;
    float _opacity = 0.35f;
    float _noiseScale = 2.0f;

public:
    FogParticleComponent(
        const std::string& materialKey,
        int count,
        float radius,
        float minHeight,
        float maxHeight,
        float minScale,
        float maxScale,
        float density,
        float opacity,
        float noiseScale
    );

    void onStart() override;
    void onRender(RenderContext& renderContext) override;

    static std::string Name() { return "FogParticleComponent"; }
};