#pragma once
#include <string>
#include "../Component.h"
#include "../../Foundation/Maths.h"

class ColliderComponent : public Component {

private:
    Vec3 size;
    bool _enabled = true;

public:

    ColliderComponent(const Vec3& size) : size(size) {}

    Vec3 halfSize() const { return size * 0.5f; }

    Vec3 center() const {
        return transform().position + Vec3(0, halfSize().y, 0);
    }

    bool enabled() const { return _enabled; }
    void setEnabled(bool v) { _enabled = v; }

    Vec3 worldMin() const {
        return center() - halfSize();
    }

    Vec3 worldMax() const {
        return center() + halfSize();
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
