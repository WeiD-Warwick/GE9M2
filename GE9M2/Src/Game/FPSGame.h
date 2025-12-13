#pragma once
#include "Game.h"

class FPSGame : public Game {
public:
    void onInit(Engine& engine, Scene& scene) override;
    void createPlayer(Engine& engine, Scene& scene);
    void createGround(Engine& engine, Scene& scene);
    void createDinosaur(Engine& engine, Scene& scene);
    void createSky(Engine& engine, Scene& scene);
};

