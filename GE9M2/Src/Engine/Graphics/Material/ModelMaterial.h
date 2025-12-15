#pragma once
#include "Material.h"

class ModelMaterial : public Material {

private:
    std::string                 _shaderName;
    std::string                 _cbufferName;
    std::string                 _textureSlotName = "tex";

public:
    ModelMaterial(const std::string& shaderName, const std::string& cbufferName)
        : _shaderName(shaderName), _cbufferName(cbufferName) {}

    void apply(
        RenderContext& ctx,
        const std::vector<std::string>& textureNames,
        MaterialParam& param
    ) override {
        auto& shaders = ctx.shaderManager();
        auto& textures = ctx.textureManager();
        auto& srvHeap = ctx.srvHeap();
        auto* commandList = ctx.renderer().commandList();

        // Update constant buffer
        shaders.updateConstantVS(_shaderName, _cbufferName, "W", &param.W);
        shaders.updateConstantVS(_shaderName, _cbufferName, "V", &param.V);
        shaders.updateConstantVS(_shaderName, _cbufferName, "P", &param.P);

        if (param.bones) {
            shaders.updateConstantVS(_shaderName, _cbufferName, "bones", param.bones);
        }

        // Bind shader
        shaders.apply(commandList, _shaderName);

        // Bind texture
        for (auto& textureName : textureNames) {
            int texIndex = textures.find(textureName);
            shaders.updateTexturePS(commandList, srvHeap, _shaderName, _textureSlotName, texIndex);
        }
    }
};
