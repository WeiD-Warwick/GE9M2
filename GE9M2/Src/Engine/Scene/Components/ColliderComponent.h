#pragma once
#include <string>
#include "../Component.h"
#include "../../Foundation/Maths.h"

class ColliderComponent : public Component {

private:
    Vec3 _halfExtents;
    Vec3 _offset;
    bool _enabled = true;

public:

    ColliderComponent(const Vec3& halfExtents, const Vec3& offset = Vec3(0, 0, 0), bool enabled = true)
        : _halfExtents(halfExtents)
        , _offset(offset)
        , _enabled(enabled) {
    }

    bool enabled() const { return _enabled; }
    void setEnabled(bool v) { _enabled = v; }

    Vec3 worldMin() const;
    Vec3 worldMax() const;

    bool intersect(const ColliderComponent* other);

    static std::string Name() { return "ColliderComponent"; }
};
