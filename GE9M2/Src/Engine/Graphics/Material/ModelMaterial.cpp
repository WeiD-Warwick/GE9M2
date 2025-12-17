#include "ModelMaterial.h"
#include <string>
#include "../RenderContext.h"
#include "../Texture/TextureManager.h"
#include "../Shader/ShaderManager.h"
#include "../../Foundation/Base/Maths.h"
#include "../../Foundation/DX12/DX12Resources.h"
#include "../../Foundation/DX12/DX12Renderer.h"

void ModelMaterial::setPSO(const std::string& psoName) { _psoName = psoName; }

void ModelMaterial::setShader(const std::string& shaderName, const std::string& cbufferName) {
    _shaderName = shaderName;
    _cbufferName = cbufferName;
}

void ModelMaterial::addTexture(const std::string& slot, const std::string& name) {
    _textures.push_back({ slot, name });
}

bool ModelMaterial::hasTexture(const std::string& slot) const {
    for (auto& t : _textures) {
        if (t.slot == slot)
            return true;
    }
    return false;
}

void ModelMaterial::setUVScale(const Vec2& scale) { _uvScale = scale; }

void ModelMaterial::apply(RenderContext& ctx, MaterialParam& param) {
    auto& shaders = ctx.shaderManager();
    auto& textures = ctx.textureManager();
    auto& srvHeap = ctx.srvHeap();
    auto& psos = ctx.psoManager();
    auto* commandList = ctx.renderer().commandList();

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

    // Normal Map Guard
    //int useNormalMap = hasTexture("normalTex") ? 1 : 0;
    //shaders.updateConstantPS(_shaderName, _cbufferName, "useNormalMap", &useNormalMap);

    for (auto& tex : _textures) {
        int index = textures.find(tex.name);
        shaders.updateTexturePS(commandList, srvHeap, _shaderName, tex.slot, index);
    }
}