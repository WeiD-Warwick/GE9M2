#pragma once
#include "../../Foundation/Base/Maths.h"

struct MaterialParam {
    Matrix W;
    Matrix V;
    Matrix P;
    Matrix* bones = nullptr;
};

class RenderContext;

class Material
{
public:
    virtual ~Material() = default;

    virtual void apply(RenderContext& ctx, MaterialParam& param) = 0;
     
};