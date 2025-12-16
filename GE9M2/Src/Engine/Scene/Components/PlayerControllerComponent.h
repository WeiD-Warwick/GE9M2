#pragma once
#include "../Component.h"
#include "../../Foundation/Base/Maths.h"

class PlayerControllerComponent : public Component {
private:
    float _moveSpeed = 20.0f;
    float _mouseSensitivity = 0.002f;

    float _pitch = 0.0f;
    float _yaw = 0.0f;

    Quaternion _fullLookRotation;

    bool _lastMouseLeft = false;
    bool _lastReloadKey = false;

    bool _lastEsc = false;

public:

    void onStart() override;

    void onUpdate(float dt) override;

    const Quaternion& getFullLookRotation() const { return _fullLookRotation; }

    static std::string Name() { return "PlayerControllerComponent"; }
};
