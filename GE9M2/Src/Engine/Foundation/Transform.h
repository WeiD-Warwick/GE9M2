#pragma once
#include "Maths.h"

class Transform {
private:
    Vec3 _position = Vec3(0, 0, 0);
    Quaternion _rotation = Quaternion::Identity();
    Vec3 _scale = Vec3(1, 1, 1);
    Transform* _parent = nullptr;

public:
    Transform() = default;

    Matrix localMatrix() const {
        Matrix T = Matrix::translation(_position);
        Matrix R = _rotation.toMatrix();
        Matrix S = Matrix::scale(_scale);
        return T * R * S;
    }

    Matrix worldMatrix() const {
        if (_parent) {
            return _parent->worldMatrix() * localMatrix();
        }
        return localMatrix();
    }

    void setPosition(const Vec3& position) { _position = position; }
    void setRotation(const Quaternion& rotation) { _rotation = rotation; }
    void setScale(const Vec3& scale) { _scale = scale; }
    void setParent(Transform* parent) { _parent = parent; }

    Vec3 getPosition() const { return _position; }
    Quaternion getRotation() const { return _rotation; }
    Vec3 getScale() const { return _scale; }
    Transform* getParent() const { return _parent; }

    // get forward direction
    Vec3 forward() const {
        return _rotation.rotate(Vec3(0, 0, 1));
    }

    // get right direction
    Vec3 right() const {
        return _rotation.rotate(Vec3(1, 0, 0));
    }

    // get up direwction
    Vec3 up() const {
        return _rotation.rotate(Vec3(0, 1, 0));
    }

};