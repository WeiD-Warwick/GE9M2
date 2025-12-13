#pragma once
#include "Material.h"

class SkySphereMaterial : public Material {

private:
    std::string _shaderName;
    std::string _cbufferName = "skySphereBuffer";
    std::string _textureSlotName = "tex";

public:
    SkySphereMaterial(const std::string& shaderName) : _shaderName(shaderName) {}

    void apply(RenderContext& ctx, const std::vector<std::string>& textureNames, MaterialParam parma) override {
        auto& shaders = ctx.shaderManager();
        auto& textures = ctx.textureManager();
        auto& srvHeap = ctx.srvHeap();
        auto* commandList = ctx.renderer().commandList();

        // Update constant buffer
        shaders.updateConstantVS(_shaderName, _cbufferName, "W", &parma.W);
        shaders.updateConstantVS(_shaderName, _cbufferName, "V", &parma.V);
        shaders.updateConstantVS(_shaderName, _cbufferName, "P", &parma.P);

        // Bind shader
        shaders.apply(commandList, _shaderName);

        // Bind texture
        for (auto& textureName : textureNames) {
            int texIndex = textures.find(textureName);
            shaders.updateTexturePS(commandList, srvHeap, _shaderName, _textureSlotName, texIndex);
        }

    }
};
