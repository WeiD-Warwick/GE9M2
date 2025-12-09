#pragma once
#include "../Component.h"
#include "../GameObject.h"

class CameraComponent : public Component {

private:
    float fov                   = 60.0f;         // degrees
    float nearPlane             = 0.1f;
    float farPlane              = 1000.0f;
    float aspectRatio           = 1.0f;

    bool needToUpdateProjetion  = true;

public:
    // world space -> view space
    Matrix view;

    // view space -> clip space
    Matrix projection;

    const std::string& getName() const override {
        static std::string name = "CameraComponent";
        return name;
    }

    // If camera moves, update view matrix
    void onUpdate(float dt) override {
        updateViewMatrix();
        updateProjectionIfNeeded();
    }

    void setAspectRatio(float w, float h) {
        aspectRatio = w / h;
        needToUpdateProjetion = true;
    }

    void setFOV(float fovDeg) {
        fov = fovDeg;
        needToUpdateProjetion = true;
    }

    void updateProjectionIfNeeded() {
        if (!needToUpdateProjetion) return;

        projection = Matrix::perspective(nearPlane, farPlane, aspectRatio, fov);

        needToUpdateProjetion = false;
    }

    void updateViewMatrix() {
        Transform& transform = owner->transform;

        Vec3 pos = transform.getPosition();
        Vec3 forward = transform.forward();
        Vec3 up = transform.up();

        view = Matrix::lookAt(pos, pos + forward, up);
    }

    // rotate owner toward to target
    void orientTowards(const Vec3& target, const Vec3& up = Vec3(0, 1, 0)) {
        Transform& t = owner->transform;
        Vec3 pos = t.getPosition();

        Vec3 z = (target - pos).normalized();  // forward
        Vec3 x = up.cross(z).normalized();
        Vec3 y = z.cross(x);

        Matrix m = Matrix::Identity();
        m.at(0, 0) = x.x; m.at(1, 0) = x.y; m.at(2, 0) = x.z;
        m.at(0, 1) = y.x; m.at(1, 1) = y.y; m.at(2, 1) = y.z;
        m.at(0, 2) = z.x; m.at(1, 2) = z.y; m.at(2, 2) = z.z;

        Quaternion q = Quaternion::fromMatrix(m);
        t.setRotation(q);
    }

    Vec3 forward() const { return owner->transform.forward(); }

    Vec3 right() const { return owner->transform.right(); }

    Vec3 up() const { return owner->transform.up(); }
};
