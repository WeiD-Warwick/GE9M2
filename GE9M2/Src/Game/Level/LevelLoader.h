#pragma once
#include <string>

class Scene;
class Engine;

class LevelLoader {
public:
    void loadLevel(Engine& engine, Scene& scene, const std::string& levelPath);
};
