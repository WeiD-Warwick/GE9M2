#include "LevelLoader.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <functional>
#include "../../Engine/Engine.h"
#include "../../Engine/Scene/Scene.h"
#include "../../Engine/Scene/GameObject.h"
#include "../../Engine/Graphics/Material/ModelMaterial.h"
// Components
#include "../../Engine/Graphics/Assets/ModelData.h"
#include "../../Engine/Graphics/Assets/ModelLoader.h"
#include "../../Engine/Scene/ComponentFactory.h"


static bool parseTransform(
    std::istream& in,
    Vec3& position,
    Quaternion& rotation,
    Vec3& scale
) { 
    return (in >> position.x >> position.y >> position.z) 
        && (in >> rotation.x >> rotation.y >> rotation.z >> rotation.w) 
        && (in >> scale.x >> scale.y >> scale.z); 
}

void LevelLoader::loadLevel(Engine& engine, Scene& scene, const std::string& levelPath) {
    static bool registered = false;
    auto& componentFactory = ComponentFactory::shared();

    std::ifstream file(levelPath);

    std::string line;
    GameObject* currentObject = nullptr;

    while (std::getline(file, line)) {
        // Skip empty lines
        if (line.empty())
            continue;

        // Skip comments
        if (line.rfind("//", 0) == 0)
            continue;

        // ------------------------------------------------------------
        // New GameObject
        // ------------------------------------------------------------
        if (line[0] == '#') {
            currentObject = scene.createObject();
            currentObject->setName(line.substr(1));

            if (!std::getline(file, line)) { break; }

            std::stringstream transformLine(line);

            Vec3 position;
            Quaternion rotation;
            Vec3 scale;

            if (!parseTransform(transformLine, position, rotation, scale)) {
                currentObject = nullptr;
                continue;
            }

            currentObject->transform().position = position;
            currentObject->transform().rotation = rotation;
            currentObject->transform().scale = scale;

            continue;
        }

        // ------------------------------------------------------------
        // Component line (create components belong to this GameObject)
        // ------------------------------------------------------------
        if (!currentObject) { 
            continue;
        }

        std::stringstream componentLine(line);

        std::string componentName;
        componentLine >> componentName;

        if (componentName.empty()) {
            continue;
        }

        // read arguments
        std::vector<std::string> args;
        std::string arg;
        while (componentLine >> arg) {
            args.push_back(arg);
        }

        assert(componentFactory.create(componentName, currentObject, engine, args));
    }
}