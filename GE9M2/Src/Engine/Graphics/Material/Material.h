#pragma once
#include "../RenderContext.h"

class Material
{
public:
    virtual ~Material() = default;

    virtual void apply(RenderContext& ctx, Matrix& W, Matrix& V, Matrix& P) = 0;
     
};

class MeshMaterial : public Material {


};

class SkySphereMaterial : public Material {

private:
    std::string _shaderName;
    std::string _textureName;
    std::string _cbufferName = "skySphereBuffer";
    std::string _textureSlotName = "tex";

public:
    SkySphereMaterial(const std::string& shaderName, const std::string& textureName)
        : _shaderName(shaderName)
        , _textureName(textureName)
    {
    }

    void apply(RenderContext& ctx, Matrix& W, Matrix& V, Matrix& P) override {
        auto& shaders = ctx.shaderManager();
        auto& textures = ctx.textureManager();
        auto& srvHeap = ctx.srvHeap();
        auto* commandList = ctx.renderer().commandList();

        // Update constant buffer
        shaders.updateConstantVS(_shaderName, _cbufferName, "W", &W);
        shaders.updateConstantVS(_shaderName, _cbufferName, "V", &V);
        shaders.updateConstantVS(_shaderName, _cbufferName, "P", &P);

        // Bind shader
        shaders.apply(commandList, _shaderName);

        // Bind texture
        int texIndex = textures.find(_textureName);
        shaders.updateTexturePS(commandList, srvHeap, _shaderName, _textureSlotName, texIndex);
    }
};
