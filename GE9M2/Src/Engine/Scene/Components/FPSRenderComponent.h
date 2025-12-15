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
    Material*            _material                = nullptr;
    AnimationController  _animationController;
    std::string          _psoName                 = "fpsPSO";
    std::string          _currentAnimation        = "04 idle";

    WeaponAnimState      _state                   = WeaponAnimState::Idle;
    WeaponIntent         _intent                  = WeaponIntent::None;

    // recoil
    // Base offset when load model
    Vec3                 _baseOffset              = Vec3(0.0f, 0.0f, 2.0f);
    float                _recoilZ                 = 0.0f;
    float                _recoilVel               = 0.0f;


public:
    FPSRenderComponent(ModelData* data, Material* material);
    ~FPSRenderComponent();

    void playAnimation(const std::string& name);

    void onUpdate(float dt) override;

    void onRender(RenderContext& renderContext) override;
    RenderLayer layer() const override { return RenderLayer::FPS; }

    void setIntent(WeaponIntent intent);

    static std::string Name() { return "FPSRenderComponent"; }
};
