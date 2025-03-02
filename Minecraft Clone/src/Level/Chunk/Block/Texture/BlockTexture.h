#pragma once

#include "../../../../RenderEngine/OpenGL/Texture/Types/TextureArray.h"
#include "../../../../RenderEngine/OpenGL/Texture/Types/TextureAtlas.h"
#include <string>
#include "../../../Typenames.h"

class BlockTexture {
public:
    void SetFaceFront(int texId) {
        texture_index_[FRONT] = texId;
    }
    void SetFaceBack(int texId) {
        texture_index_[BACK] = texId;
    }
    void SetFaceWest(int texId) {
        texture_index_[RIGHT] = texId;
    }
    void SetFaceEast(int texId) {
        texture_index_[LEFT] = texId;
    }
    void SetFaceTop(int texId) {
        texture_index_[TOP] = texId;
    }
    void SetFaceBottom(int texId) {
        texture_index_[BOTTOM] = texId;
    }
    void SetFacesCustom(int texId, int TexNum, int side0 = 0xFF, int side1 = 0xFF, int side2 = 0xFF, int side3 = 0xFF, int side4 = 0xFF, int side5 = 0xFF) {

        if (side0 != 0xFF) {
            texture_index_[side0] = texId;
            texture_num_index_[side0] = TexNum;
        }

        if (side1 != 0xFF) {
            texture_index_[side1] = texId;
            texture_num_index_[side1] = TexNum;
        }

        if (side2 != 0xFF) {
            texture_index_[side2] = texId;
            texture_num_index_[side2] = TexNum;
        }

        if (side3 != 0xFF) {
            texture_index_[side3] = texId;
            texture_num_index_[side3] = TexNum;
        }

        if (side4 != 0xFF) {
            texture_index_[side4] = texId;
            texture_num_index_[side4] = TexNum;
        }

        if (side5 != 0xFF) {
            texture_index_[side5] = texId;
            texture_num_index_[side5] = TexNum;
        }
    }

    void SetParticle(int tex) {
        particle_ = tex;
    }

    int GetParticle() {
        return particle_;
    }

    int GetFace(int side) {
        return texture_index_[side];
    }

    int& operator[](int i){
        return texture_index_[i];
    }

    int GetNumTextures(int side) {
        return texture_num_index_[side];
    }

private:
    int texture_index_[6]{ NULL };
    int texture_num_index_[6]{ NULL };
    int particle_ = NULL;
};
