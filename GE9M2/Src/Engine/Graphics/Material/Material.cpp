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
    auto* cmd = ctx.renderer().commandList();
    psos.bind(cmd, _psoName);

    // ==================================
    // Update VS constant buffer
    // ==================================
    shaders.updateConstantVS(_shaderName, _cbufferName, "W", &param.W);
    shaders.updateConstantVS(_shaderName, _cbufferName, "VP", &param.VP);
    shaders.updateConstantVS(_shaderName, _cbufferName, "time", &param.time);
    shaders.updateConstantVS(_shaderName, _cbufferName, "cameraRight", &param.cameraRight);
    shaders.updateConstantVS(_shaderName, _cbufferName, "cameraUp", &param.cameraUp);

    if (param.bones) {
        shaders.updateConstantVS(_shaderName, _cbufferName, "bones", param.bones);
    }

    int useVSAnim = _useVSAnim ? 1 : 0;
    shaders.updateConstantVS(_shaderName, _cbufferName, "useVSAnim", &useVSAnim);

    // ==================================
    // Update PS constant buffer
    // ==================================
    shaders.updateConstantPS(_shaderName, _cbufferName, "uvScale", &_uvScale);
    shaders.updateConstantPS(_shaderName, _cbufferName, "uvScale", &_uvScale);
    shaders.updateConstantPS(_shaderName, _cbufferName, "time", &param.time);
    shaders.updateConstantPS(_shaderName, _cbufferName, "fogColor", &param.fogColor);
    shaders.updateConstantPS(_shaderName, _cbufferName, "fogDensity", &param.fogDensity);
    shaders.updateConstantPS(_shaderName, _cbufferName, "fogNoiseScale", &param.fogNoiseScale);
    shaders.updateConstantPS(_shaderName, _cbufferName, "fogOpacity", &param.fogOpacity);

    if (param.isWeapon) {
        shaders.updateConstantPS(_shaderName, _cbufferName, "isWeapon", &param.isWeapon);
    }

    int useAlphaTest = _useAlphaTest ? 1 : 0;
    shaders.updateConstantPS(_shaderName, _cbufferName, "useAlphaTest", &useAlphaTest);

    // Bind shader
    shaders.apply(cmd, _shaderName);

    for (auto& t : _textures) {
        int offset = textures.find(t.name);
        shaders.updateTexturePS(cmd, srvHeap, _shaderName, t.slot, offset);
    }
}