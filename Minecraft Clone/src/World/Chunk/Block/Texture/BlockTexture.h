#pragma once
#ifndef BLOCK_TEX
#define BLOCK_TEX

#include "../../../../RenderEngine/OpenGL/Texture/Types/TextureArray.h"
#include <string>

#define FRONT 0x00
#define BACK 0x01
#define TOP 0x02
#define BOTTOM 0x03
#define RIGHT 0x04
#define LEFT 0x05

__declspec(selectany) 

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
    void SetFacesCustom(int TexID, int side0 = 0xFF, int side1 = 0xFF, int side2 = 0xFF, int side3 = 0xFF, int side4 = 0xFF, int side5 = 0xFF) {

        if (side0 != 0xFF) {
            TextureIndex[side0] = TexID;
        }

        if (side1 != 0xFF) {
            TextureIndex[side1] = TexID;
        }

        if (side2 != 0xFF) {
            TextureIndex[side2] = TexID;
        }

        if (side3 != 0xFF) {
            TextureIndex[side3] = TexID;
        }

        if (side4 != 0xFF) {
            TextureIndex[side4] = TexID;
        }

        if (side5 != 0xFF) {
            TextureIndex[side5] = TexID;
        }
    }

    int GetFace(int side) {
        return TextureIndex[side];
    }

    int& operator[](int i){
        return TextureIndex[i];
    }

private:
    int TextureIndex[6]{ NULL };
    RawTextureData* BlockTex[6]{ nullptr };
};



#endif // !BLOCK_TEX

