#pragma once
#include <string>
#include "../Component.h"
#include "../../Graphics/Animation/AnimationController.h"

class ModelData;
class AnimationData;
class AnimationController;
class Material;

enum class WeaponAnimState {
    Idle,
    Fire,
    Reload
};

enum class WeaponIntent {
    None,
    Fire,
    Reload
};

class FPSRenderComponent : public RenderComponent {
private:
    ModelData*           _data                    = nullptr;
    std::vector<Material*> _materials;
    AnimationController  _animationController;
    std::string          _currentAnimation        = "04 idle";

    WeaponAnimState      _state                   = WeaponAnimState::Idle;
    WeaponIntent         _intent                  = WeaponIntent::None;

    Vec3                 _baseOffset              = Vec3(0.0f, 0.0f, 2.0f);

public:
    FPSRenderComponent(ModelData* data);
    ~FPSRenderComponent() = default;

    void playAnimation(const std::string& name);

    void onStart() override;

    void onUpdate(float dt) override;

    void onRender(RenderContext& renderContext) override;
    RenderLayer layer() const override { return RenderLayer::FPS; }

    void setIntent(WeaponIntent intent);

    static std::string Name() { return "FPSRenderComponent"; }
};
