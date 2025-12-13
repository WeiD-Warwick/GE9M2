#pragma once
#include "../RenderContext.h"

struct MaterialParam {
    Matrix W;
    Matrix V;
    Matrix P;
    Matrix* bones = nullptr;
};


class Material
{
public:
    virtual ~Material() = default;

    virtual void apply(RenderContext& ctx, const std::vector<std::string>& textureNames, MaterialParam param) = 0;
     
};