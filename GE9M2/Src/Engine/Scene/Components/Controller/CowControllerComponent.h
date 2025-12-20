#pragma once
#include "../Component.h"

struct CowAnimConfig {
public:
    std::string idle;
    std::string hit;
    std::string death;
};

class CowControllerComponent : public Component {
public:
    enum class State { Idle, Hitting, Death };

    enum class Intent { None, Hitted, Death };

    State _state = State::Idle;
    Intent _intent = Intent::None;

    bool _idleInitialized = false;

    CowAnimConfig _animConfig;

public:
    CowControllerComponent(const CowAnimConfig& config);

    void setIntent(Intent intent);

    void onUpdate(float dt) override;

    static std::string Name() { return "CowControllerComponent"; }
};
