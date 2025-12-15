#pragma once
#include <string>
#include "../Component.h"
#include "../../Foundation/Maths.h"

class ColliderComponent : public Component {

private:
    Vec3 _size;
    bool _enabled = true;

public:

    ColliderComponent(const Vec3& size) : _size(size) {}

    bool enabled() const { return _enabled; }
    void setEnabled(bool v) { _enabled = v; }

    Vec3 size() { return _size; }

    Vec3 worldMin() const {
        return Vec3(
            transform().position.x - _size.x * 0.5f,
            transform().position.y,
            transform().position.z - _size.z * 0.5f
        );
    }

    Vec3 worldMax() const {
        return Vec3(
            transform().position.x + _size.x * 0.5f,
            transform().position.y + _size.y,
            transform().position.z + _size.z * 0.5f
        );
    }

    bool intersect(const ColliderComponent* other) const {
        Vec3 aMin = worldMin();
        Vec3 aMax = worldMax();
        Vec3 bMin = other->worldMin();
        Vec3 bMax = other->worldMax();

        return !(aMax.x < bMin.x || aMin.x > bMax.x ||
            aMax.y < bMin.y || aMin.y > bMax.y ||
            aMax.z < bMin.z || aMin.z > bMax.z);
    }

    static std::string Name() { return "ColliderComponent"; }
};
