#pragma once

#include "../../../../RenderEngine/OpenGL/Texture/Types/TextureArray.h"
#include "../../../../RenderEngine/OpenGL/Texture/Types/TextureAtlas.h"
#include <string>
#include "../../../Typenames.h"

class BlockTexture {
public:
    void SetFaceFront(int TexID) {
        TextureIndex[FRONT] = TexID;
    }
    void SetFaceBack(int TexID) {
        TextureIndex[BACK] = TexID;
    }
    void SetFaceWest(int TexID) {
        TextureIndex[RIGHT] = TexID;
    }
    void SetFaceEast(int TexID) {
        TextureIndex[LEFT] = TexID;
    }
    void SetFaceTop(int TexID) {
        TextureIndex[TOP] = TexID;
    }
    void SetFaceBottom(int TexID) {
        TextureIndex[BOTTOM] = TexID;
    }
    void SetFacesCustom(int TexID, int TexNum, int side0 = 0xFF, int side1 = 0xFF, int side2 = 0xFF, int side3 = 0xFF, int side4 = 0xFF, int side5 = 0xFF) {

        if (side0 != 0xFF) {
            TextureIndex[side0] = TexID;
            TextureNumIndex[side0] = TexNum;
        }

        if (side1 != 0xFF) {
            TextureIndex[side1] = TexID;
            TextureNumIndex[side1] = TexNum;
        }

        if (side2 != 0xFF) {
            TextureIndex[side2] = TexID;
            TextureNumIndex[side2] = TexNum;
        }

        if (side3 != 0xFF) {
            TextureIndex[side3] = TexID;
            TextureNumIndex[side3] = TexNum;
        }

        if (side4 != 0xFF) {
            TextureIndex[side4] = TexID;
            TextureNumIndex[side4] = TexNum;
        }

        if (side5 != 0xFF) {
            TextureIndex[side5] = TexID;
            TextureNumIndex[side5] = TexNum;
        }
    }

    void SetParticle(int tex) {
        Particle = tex;
    }

    int GetParticle() {
        return Particle;
    }

    int GetFace(int side) {
        return TextureIndex[side];
    }

    int& operator[](int i){
        return TextureIndex[i];
    }

    int GetNumTextures(int side) {
        return TextureNumIndex[side];
    }

private:
    int TextureIndex[6]{ NULL };
    int TextureNumIndex[6]{ NULL };
    int Particle = NULL;
};
