#pragma once
#include <vector>
#include <string>
#include "../../Foundation/Base/Maths.h"

class RenderContext;

struct MaterialParam {
    Matrix W;
    Matrix V;
    Matrix P;
    Matrix* bones = nullptr;
};

struct MaterialTexture {
    std::string slot;
    std::string name;
};

class Material {

private:
    std::string                  _psoName;
    std::string                  _shaderName;
    std::string                  _cbufferName;
    std::vector<MaterialTexture> _textures;

    Vec2                         _uvScale = { 1.0f, 1.0f };
    bool                         _useAlphaTest = false;

public:

    void setPSO(const std::string& psoName);

    void setShader(const std::string& shaderName, const std::string& cbufferName);

    void addTexture(const std::string& slot, const std::string& name);

    bool hasTexture(const std::string& slot) const;

    void setUVScale(const Vec2& scale);

    void setAlphaTest(bool enable);

    void apply(RenderContext& ctx, MaterialParam& param);
};
