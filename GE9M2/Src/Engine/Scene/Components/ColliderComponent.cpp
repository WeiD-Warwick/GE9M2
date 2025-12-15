#include "ColliderComponent.h"
#include "../Component.h"
#include "../../Foundation/Maths.h"
#include "../../Foundation/Transform.h"

Vec3 ColliderComponent::worldMin() const {
    const Transform& t = transform();
    Vec3 center = t.position + _offset;
    return center - _halfExtents;
}

Vec3 ColliderComponent::worldMax() const {
    const Transform& t = transform();
    Vec3 center = t.position + _offset;
    return center + _halfExtents;
}

bool ColliderComponent::intersect(const ColliderComponent* other) {
    Vec3 selfMin = worldMin();
    Vec3 selfMax = worldMax();
    Vec3 otherMin = other->worldMin();
    Vec3 otherMax = other->worldMax();

    return
        selfMin.x <= otherMax.x && selfMax.x >= otherMin.x &&
        selfMin.y <= otherMax.y && selfMax.y >= otherMin.y &&
        selfMin.z <= otherMax.z && selfMax.z >= otherMin.z;
}

