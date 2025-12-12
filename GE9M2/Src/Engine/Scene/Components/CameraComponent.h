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
        const Transform& ownerTransform = owner->transform;

        Vec3 ownerPosition = ownerTransform.position;
		// -Z is forward
        Vec3 ownerForward = ownerTransform.forward();
        Vec3 ownerUp = ownerTransform.up();

        view = Matrix::lookAt(ownerPosition, ownerPosition + ownerForward, ownerUp);
    }

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
