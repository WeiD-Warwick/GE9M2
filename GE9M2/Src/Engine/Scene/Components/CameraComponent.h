#pragma once
#include "../Component.h"
#include "../GameObject.h"

class CameraComponent : public Component {

private:
    float _fov                   = 60.0f;         // degrees
    float _nearPlane             = 0.1f;
    float _farPlane              = 1000.0f;
    float _aspectRatio           = 1.0f;
    float _eyeHeight             = 1.8f;

    bool _needToUpdateProjetion  = true;

public:
    Matrix view;
    Matrix projection;

    void onUpdate(float dt) override {
        updateViewMatrix();
        updateProjectionIfNeeded();
    }

    void updateProjectionIfNeeded() {
        if (!_needToUpdateProjetion) return;

        projection = Matrix::perspective(_nearPlane, _farPlane, _aspectRatio, _fov);

        _needToUpdateProjetion = false;
    }

    void updateViewMatrix() {
        Transform& transform = owner->transform;

        Vec3 pos = transform.position;
        Vec3 forward = transform.forward();
        Vec3 up = transform.up();

        view = Matrix::lookAt(pos, pos + forward, up);
    }

    Vec3 forward() const { return owner->transform.forward(); }

    Vec3 right() const { return owner->transform.right(); }

    Vec3 up() const { return owner->transform.up(); }

public:

    CameraComponent(float aspectRatio) : _aspectRatio(aspectRatio) {}

    int getUpdateOrder() const override {
        return 40;
    }

    const std::string& getName() const override {
        static std::string name = "CameraComponent";
        return name;
    }
};
