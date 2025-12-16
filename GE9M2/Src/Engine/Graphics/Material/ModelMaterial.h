#pragma once
#include <vector>
#include <string>
#include "Material.h"

struct MaterialTexture {
    std::string slot;
    std::string name;
};

class RenderContext;

class ModelMaterial : public Material {

private:
    std::string                  _psoName;
    std::string                  _shaderName;
    std::string                  _cbufferName;
    std::vector<MaterialTexture> _textures;

    Vec2                         _uvScale         = { 1.0f, 1.0f };

public:

    void setPSO(const std::string& psoName);

    void setShader(const std::string& shaderName, const std::string& cbufferName);

    void addTexture(const std::string& slot, const std::string& name);

    void setUVScale(const Vec2& scale);

    void apply(RenderContext& ctx, MaterialParam& param) override;
};
