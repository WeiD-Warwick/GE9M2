#include "FogParticleComponent.h"
#include <random>
#include <cmath>
#include "../CameraComponent.h"
#include "../../GameObject.h"
#include "../../../Engine.h"
#include "../../../Graphics/RenderContext.h"
#include "../../../Graphics/Material/MaterialManager.h"
#include "../../../Graphics/Model/ModelLoader.h"
#include "../../../Graphics/Mesh/MeshLib.h"

FogParticleComponent::FogParticleComponent(
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
) :
    _materialKey(materialKey),
    _count(count),
    _radius(radius),
    _minHeight(minHeight),
    _maxHeight(maxHeight),
    _minScale(minScale),
    _maxScale(maxScale),
    _density(density),
    _opacity(opacity),
    _noiseScale(noiseScale) {
}

void FogParticleComponent::onStart() {
    auto& ctx = engine()->renderContext();
    auto* meshLib = engine()->loader().meshLib();

    // Build a single quad submesh using the requested material
    SubMesh quad;
    quad.mesh = &meshLib->plane;
    quad.materialKey = _materialKey;
    _subMeshes.push_back(quad);

    _materials.push_back(ctx.materialManager().createInstance(ctx, quad));

    // Generate particle data
    _particles.resize(_count);
    _instances.resize(_count);

    std::mt19937 rng(1337); // deterministic for reproducibility
    std::uniform_real_distribution<float> radius01(0.0f, 1.0f);
    std::uniform_real_distribution<float> angle(0.0f, 2.0f * M_PI);
    std::uniform_real_distribution<float> height(_minHeight, _maxHeight);
    std::uniform_real_distribution<float> scale(_minScale, _maxScale);
    std::uniform_real_distribution<float> speed(0.25f, 0.8f);
    std::uniform_real_distribution<float> phase(0.0f, 2.0f * M_PI);

    for (int i = 0; i < _count; ++i) {
        float r = std::sqrt(radius01(rng)) * _radius;
        float a = angle(rng);
        Vec3 pos(r * std::cos(a), height(rng), r * std::sin(a));

        _particles[i].basePos = pos;
        _particles[i].scale = scale(rng);
        _particles[i].swaySpeed = speed(rng);
        _particles[i].phase = phase(rng);
    }
}

void FogParticleComponent::onRender(RenderContext& renderContext) {
    auto* camera = mainCamera();
    if (!camera || _subMeshes.empty() || _materials.empty())
        return;

    float t = engine()->time();
    for (int i = 0; i < _count; ++i) {
        const Particle& p = _particles[i];
        float sway = std::sinf(t * p.swaySpeed + p.phase) * 0.4f;
        Vec3 pos = p.basePos + Vec3(0, sway, 0);

        Matrix S = Matrix::Scale(Vec3(p.scale, p.scale, p.scale));
        Matrix T = Matrix::Translation(pos);
        _instances[i].world = S * T;
    }

    renderContext.instanceBuffer().upload(_instances.data(), _count);

    MaterialParam param;
    param.W = Matrix::Identity();
    param.VP = camera->view * camera->projection;
    param.time = t;
    param.cameraRight = camera->transform().right();
    param.cameraUp = camera->transform().up();
    param.fogDensity = _density;
    param.fogOpacity = _opacity;
    param.fogNoiseScale = _noiseScale;

    auto* cmd = renderContext.renderer().commandList();

    for (int i = 0; i < _subMeshes.size(); ++i) {
        _materials[i]->apply(renderContext, param);
        _subMeshes[i].mesh->drawInstanced(cmd, renderContext.instanceBuffer().vertexBufferView(), _count);
    }
}