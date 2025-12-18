#include "LevelLoader.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <functional>
#include "../../Engine/Engine.h"
#include "../../Engine/Scene/Scene.h"
#include "../../Engine/Scene/GameObject.h"
#include "../../Engine/Graphics/Material/Material.h"
#include "../../Engine/Graphics/Assets/ModelData.h"
#include "../../Engine/Graphics/Assets/ModelLoader.h"
#include "../../Engine/Scene/ComponentFactory.h"

void LevelLoader::loadLevel(Engine& engine, Scene& scene, const std::string& levelPath) {

    std::ifstream file(levelPath);

    std::string line;

    Section section = Section::None;

    while (std::getline(file, line)) {
        // Skip empty lines and  comments
        if (line.empty() || line.rfind(commentFlag, 0) == 0) continue;

        // Handle Scetion
        if (line == textureSectionFlag)  { section = Section::Texture;  continue; }
        if (line == shaderSectionFlag)   { section = Section::Shader;   continue; }
        if (line == psoSectionFlag)      { section = Section::PSO;      continue; }
        if (line == materialSectionFlag) { section = Section::Material; continue; }
        if (line == lightSectionFlag)    { section = Section::Light;    continue; }
        if (line == sceneSectionFlag)    { section = Section::Scene;    continue; };

        switch (section) {
        case Section::Texture:  parseTexture(engine, line);             break;
        case Section::Shader:   parseShader(engine, line);              break;
        case Section::PSO:      parsePSO(engine, line);                 break;
        case Section::Material: parseMaterialLine(engine, line);        break;
        case Section::Light:    parseLight(engine, line);               break;
        case Section::Scene:    parseSceneLine(engine, scene, line);    break;
        default: break;
        }
    }
};

void LevelLoader::parseTexture(Engine& engine, const std::string& line) {
    if (line.empty() || line.rfind(commentFlag, 0) == 0) return;

    std::stringstream textureLine(line);
    std::string key, path;
    textureLine >> key >> path;

    TextureUsage usgae = (key == "albedoTex")
        ? TextureUsage::Color
        : TextureUsage::Data;
    

    auto* texture = engine.renderContext().textureManager().loadTexture(
        engine.renderContext().device().dxDevice(),
        engine.renderContext().uploader(),
        engine.renderContext().srvHeap(),
        key,
        path,
        usgae
    );

    assert(texture);
}

void LevelLoader::parseShader(Engine& engine, const std::string& line) {
    if (line.empty() || line.rfind(commentFlag, 0) == 0) return;

    std::stringstream shaderLine(line);
    std::string name, cbuffer, vsPath, psPath;
    shaderLine >> name >> cbuffer >> vsPath >> psPath;

    auto* shader = engine.renderContext().shaderManager().load(
        engine.renderContext().device().dxDevice(),
        name,
        vsPath,
        psPath
    );

    assert(shader);
}

void LevelLoader::parsePSO(Engine& engine, const std::string& line) {
    if (line.empty() || line.rfind(commentFlag, 0) == 0) return;

    std::stringstream psoLine(line);

    std::string psoName, shaderName, layout, depthFunc, depthWrite;
    psoLine >> psoName >> shaderName >> layout >> depthFunc >> depthWrite;

    Shader* shader = engine.renderContext().shaderManager().find(shaderName);

    PSOParam param;
    param.psoName = psoName;
    param.vsBlob = shader->vs.Get();
    param.psBlob = shader->ps.Get();

    param.layout = (layout == "animated")
        ? DX12VertexLayoutCache::getAnimatedLayout()
        : DX12VertexLayoutCache::getStaticLayout();

    param.depthFunc =
        (depthFunc == "depth_less_equal") ? D3D12_COMPARISON_FUNC_LESS_EQUAL :
        (depthFunc == "depth_always") ? D3D12_COMPARISON_FUNC_ALWAYS :
        D3D12_COMPARISON_FUNC_LESS;

    param.depthWriteMask =
        (depthWrite == "nowrite") ? D3D12_DEPTH_WRITE_MASK_ZERO : D3D12_DEPTH_WRITE_MASK_ALL;

    engine.renderContext().psoManager().createPSO(
        engine.renderContext().device().dxDevice(),
        engine.renderContext().rootSignature().rootSignature(),
        param
    );
}

void LevelLoader::parseMaterialLine(Engine& engine, const std::string& line) {
    if (line.empty() || line.rfind(commentFlag, 0) == 0) return;

    // ------------------------------------
    // New material
    // ------------------------------------
    if (!line.starts_with(materialPropFlag)) {
        std::string materialName = line;
        _currentMaterial = new Material();
        engine.renderContext().materialManager().add(materialName, _currentMaterial);
        return;
    }

    if (!_currentMaterial) return;

    // ------------------------------------
    // Handle Props
    // ------------------------------------

    std::stringstream propLine(line);
    std::string flag;
    std::string key;
    propLine >> flag >> key;
    if (key == "shader") {
        std::string shaderName;
        std::string cbufferName;
        propLine >> shaderName >> cbufferName;
        _currentMaterial->setShader(shaderName, cbufferName);
        return;
    }

    if (key == "pso") {
        std::string psoName;
        propLine >> psoName;
        _currentMaterial->setPSO(psoName);
        return;
    }

    if (key == "texture") {
        std::string textureSlot;
        std::string textureName;
        propLine >> textureSlot >> textureName;
        _currentMaterial->addTexture(textureSlot, textureName);
        return;
    }

    if (key == "uv") {
        float u = 1.0f, v = 1.0f;
        propLine >> u >> v;
        _currentMaterial->setUVScale({ u, v });
        return;
    }

    if (key == "alphatest") {
        float alphatest;
        propLine >> alphatest;
        _currentMaterial->setAlphaTest(alphatest);
        return;
    }

}

void LevelLoader::parseLight(Engine& engine, const std::string& line) {
    if (line.empty() || line.rfind(commentFlag, 0) == 0) return;
    
    if (line.starts_with("skylight")) {
        SkyLight skylight;
        std::string key;
        std::stringstream lightLine(line);

        lightLine >> key 
            >> skylight.color.x >> skylight.color.y >> skylight.color.z 
            >> skylight.intensity;

        engine.scene().setSkyLight(skylight);
    }

    if (line.starts_with("pointlight")) {
        PointLight pointlight;
        std::string key;
        std::stringstream lightLine(line);
        lightLine >> key
            >> pointlight.position.x >> pointlight.position.y >> pointlight.position.z
            >> pointlight.color.x >> pointlight.color.y >> pointlight.color.z
            >> pointlight.range >> pointlight.intensity;

        engine.scene().addLight(pointlight);
    }
}

void LevelLoader::parseSceneLine(Engine& engine, Scene& scene, const std::string& line) {

    if (line.empty() || line.rfind(commentFlag, 0) == 0) return;

    auto& componentFactory = ComponentFactory::shared();

    // ------------------------------------------------------------
    // New GameObject
    // ------------------------------------------------------------
    if (line.starts_with(newObjectFlag)) {
        std::stringstream nameLine(line);
        std::string signal, name;
        nameLine >> signal >> name;
        _currentObject = scene.createObject();
        _currentObject->setName(name);
        return;
    }

    if (!_currentObject) return;

    // ------------------------------------------------------------
    // Transform line
    // ------------------------------------------------------------
    if (line.starts_with(transformFlag)) {
        std::stringstream transformLine(line);

        std::string flag;
        Vec3 position;
        Quaternion rotation;
        Vec3 scale;

        transformLine >> flag >> position.x >> position.y >> position.z
            >> rotation.x >> rotation.y >> rotation.z >> rotation.w
            >> scale.x >> scale.y >> scale.z;

        _currentObject->transform().position = position;
        _currentObject->transform().rotation = rotation;
        _currentObject->transform().scale = scale;
        return;
    }

    // ------------------------------------------------------------
    // Component line
    // ------------------------------------------------------------
    std::stringstream componentLine(line);

    std::string componentName;
    componentLine >> componentName;
    if (componentName.empty()) return;

    std::vector<std::string> args;
    std::string arg;
    while (componentLine >> arg) {
        args.push_back(arg);
    }

    assert(componentFactory.create(
        componentName,
        _currentObject,
        engine,
        args
    ));
}