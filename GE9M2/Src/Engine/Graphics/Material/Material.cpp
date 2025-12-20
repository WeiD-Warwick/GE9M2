#include "Material.h"
#include <string>
#include "../RenderContext.h"
#include "../Texture/TextureManager.h"
#include "../Shader/ShaderManager.h"
#include "../../Foundation/Base/Maths.h"
#include "../../Foundation/DX12/DX12Resources.h"
#include "../../Foundation/DX12/DX12Renderer.h"

void Material::setPSO(const std::string& psoName) { _psoName = psoName; }

void Material::setShader(const std::string& shaderName, const std::string& cbufferName) {
    _shaderName = shaderName;
    _cbufferName = cbufferName;
}

void Material::addTexture(const std::string& slot, const std::string& name) {
    _textures.push_back({ slot, name });
}

bool Material::hasTexture(const std::string& slot) const {
    for (auto& t : _textures) {
        if (t.slot == slot)
            return true;
    }
    return false;
}

void Material::setUVScale(const Vec2& scale) { _uvScale = scale; }

void Material::setAlphaTest(int enable) { _useAlphaTest = enable; }

void Material::setVSAnim(int enable) { _useVSAnim = enable; }

void Material::apply(RenderContext& ctx, MaterialParam& param) {
    auto& shaders = ctx.shaderManager();
    auto& textures = ctx.textureManager();
    auto& srvHeap = ctx.srvHeap();
    auto& psos = ctx.psoManager();
    auto* commandList = ctx.renderer().commandList();

    psos.bind(commandList, _psoName);

    // ==================================
    // Update VS constant buffer
    // ==================================
    shaders.updateConstantVS(_shaderName, _cbufferName, "W", &param.W);
    shaders.updateConstantVS(_shaderName, _cbufferName, "VP", &param.VP);

    if (param.bones) {
        shaders.updateConstantVS(_shaderName, _cbufferName, "bones", param.bones);
    }

    int useVSAnim = _useVSAnim ? 1 : 0;
    shaders.updateConstantVS(_shaderName, _cbufferName, "useVSAnim", &useVSAnim);

    // ==================================
    // Update PS constant buffer
    // ==================================
    shaders.updateConstantPS(_shaderName, _cbufferName, "uvScale", &_uvScale);

    if (param.isWeapon) {
        shaders.updateConstantPS(_shaderName, _cbufferName, "isWeapon", &param.isWeapon);
    }

    // Normal Map Guard
    int useNormalMap = hasTexture("normalTex") ? 1 : 0;
    shaders.updateConstantPS(_shaderName, _cbufferName, "useNormalMap", &useNormalMap);

    int useAlphaTest = _useAlphaTest ? 1 : 0;
    shaders.updateConstantPS(_shaderName, _cbufferName, "useAlphaTest", &useAlphaTest);

    // Bind shader
    shaders.apply(commandList, _shaderName);


    // Upate Texture
    if (!_textures.empty()) {
        int baseOffset = textures.find(_textures[0].name);
        shaders.updateTexturePS(commandList, srvHeap, _shaderName, _textures[0].slot, baseOffset);
    }
}