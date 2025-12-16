#pragma once
#include "Material.h"

class ModelMaterial : public Material {

private:
    std::string _psoName;
    std::string _shaderName;
    std::string _textureName;
    std::string _cbufferName;
    std::string _textureSlotName = "tex";


    Vec2        _uvScale         = { 1.0f, 1.0f };

public:
    ModelMaterial(
        const std::string& psoName,
        const std::string& shaderName,
        const std::string& cbufferName,
        const std::string& textureName
    )
        : _psoName(psoName)
        , _shaderName(shaderName)
        , _cbufferName(cbufferName)
        , _textureName(textureName)
    {
    }

    void setUVScale(const Vec2& scale) { _uvScale = scale; }

    void apply(RenderContext& ctx, MaterialParam& param) override {
        auto& shaders       = ctx.shaderManager();
        auto& textures      = ctx.textureManager();
        auto& srvHeap       = ctx.srvHeap();
        auto& psos          = ctx.psoManager();
        auto* commandList   = ctx.renderer().commandList();

        psos.bind(commandList, _psoName);

        // Update constant buffer
        shaders.updateConstantVS(_shaderName, _cbufferName, "W", &param.W);
        shaders.updateConstantVS(_shaderName, _cbufferName, "V", &param.V);
        shaders.updateConstantVS(_shaderName, _cbufferName, "P", &param.P);
        shaders.updateConstantPS(_shaderName, _cbufferName, "uvScale", &_uvScale);

        if (param.bones) {
            shaders.updateConstantVS(_shaderName, _cbufferName, "bones", param.bones);
        }

        // Bind shader
        shaders.apply(commandList, _shaderName);

        int texIndex = textures.find(_textureName);
        shaders.updateTexturePS(commandList, srvHeap, _shaderName, _textureSlotName, texIndex); 
    }
};
