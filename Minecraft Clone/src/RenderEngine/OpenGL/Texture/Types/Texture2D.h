#pragma once
#include "RenderEngine/OpenGL/Texture/Texture.h"

class Texture2D : public Texture {
public:
    Texture2D(RawTextureData data);

    Texture2D();

    bool Load(RawTextureData data) override;
};