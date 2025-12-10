#pragma once
#include "Maths.h"

class Transform {

public:
    Vec3 position = Vec3(0, 0, 0);
    Quaternion rotation = Quaternion::Identity();
    Vec3 scale = Vec3(1, 1, 1);

    Transform() = default;

    Matrix localMatrix() const {
        Matrix T = Matrix::translation(position);
        Matrix R = rotation.toMatrix();
        Matrix S = Matrix::scale(scale);
        return T * R * S;
    }

    Matrix worldMatrix() const {
        return localMatrix();
    }

    // get forward direction
    Vec3 forward() const {
        return rotation.rotate(Vec3(0, 0, 1));
    }

    // get right direction
    Vec3 right() const {
        return rotation.rotate(Vec3(1, 0, 0));
    }

    // get up direwction
    Vec3 up() const {
        return rotation.rotate(Vec3(0, 1, 0));
    }

};