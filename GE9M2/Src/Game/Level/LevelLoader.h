#pragma once
#include <string>

class Scene;
class Engine;
class GameObject;
class Material;

enum class Section {
    None,
    Texture,
    Shader,
    PSO,
    Material,
    Light,
    Scene
};

class LevelLoader {
private:
    GameObject* _currentObject = nullptr;
    Material* _currentMaterial = nullptr;

    std::string commentFlag = "//";
    std::string textureSectionFlag = "=== TEXTURE ===";
    std::string shaderSectionFlag = "=== SHADER ===";
    std::string psoSectionFlag = "=== PSO ===";
    std::string materialSectionFlag = "=== MATERIAL ===";
    std::string lightSectionFlag = "=== LIGHT ===";
    std::string sceneSectionFlag = "=== SCENE ===";

    std::string newObjectFlag = "#";
    std::string materialPropFlag = ":";
    std::string transformFlag = "transform";

    void parseTexture(Engine& engine, const std::string& line);
    void parseShader(Engine& engine, const std::string& line);
    void parsePSO(Engine& engine, const std::string& line);
    void parseMaterialLine(Engine& engine, const std::string& line);
    void parseLight(Engine& engine, const std::string& line);
    void parseSceneLine(Engine& engine, Scene& scene, const std::string& line);
    std::string decodeArg(const std::string& in);
public:
    void loadLevel(Engine& engine, Scene& scene, const std::string& levelPath);

};
