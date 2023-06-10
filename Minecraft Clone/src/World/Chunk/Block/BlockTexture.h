#pragma once
#ifndef BLOCK_TEX
#define BLOCK_TEX

#include "../../../RenderEngine/OpenGL/Texture/Types/TextureArray.h"
#include <string>

#define FRONT 0x00
#define BACK 0x01
#define TOP 0x02
#define BOTTOM 0x03
#define RIGHT 0x04
#define LEFT 0x05


__declspec(selectany) TextureArray BlockTextureArray;

class BlockTexture {
public:
    int SetFaceFront(std::string file) {
        BlockTextureArray.AddTextureToArray(file);
        TextureIndex[FRONT] = BlockTextureArray.GetLayers();
        return BlockTextureArray.GetLayers();
    }
    int SetFaceBack(std::string file) {
        BlockTextureArray.AddTextureToArray(file);
        TextureIndex[BACK] = BlockTextureArray.GetLayers();
        return BlockTextureArray.GetLayers();
    }
    int SetFaceWest(std::string file) {
        BlockTextureArray.AddTextureToArray(file);
        TextureIndex[RIGHT] = BlockTextureArray.GetLayers();
        return BlockTextureArray.GetLayers();
    }
    int SetFaceEast(std::string file) {
        BlockTextureArray.AddTextureToArray(file);
        TextureIndex[LEFT] = BlockTextureArray.GetLayers();
        return BlockTextureArray.GetLayers();
    }
    int SetFaceTop(std::string file) {
        BlockTextureArray.AddTextureToArray(file);
        TextureIndex[TOP] = BlockTextureArray.GetLayers();
        return BlockTextureArray.GetLayers();
    }
    int SetFaceBottom(std::string file) {
        BlockTextureArray.AddTextureToArray(file);
        TextureIndex[BOTTOM] = BlockTextureArray.GetLayers();
        return BlockTextureArray.GetLayers();
    }
    int SetFacesCustom(std::string file, int side0 = 0xFF, int side1 = 0xFF, int side2 = 0xFF, int side3 = 0xFF, int side4 = 0xFF, int side5 = 0xFF) {
        BlockTextureArray.AddTextureToArray(file);

        if (side0 != 0xFF) {
            TextureIndex[side0] = BlockTextureArray.GetLayers();
        }

        if (side1 != 0xFF) {
            TextureIndex[side1] = BlockTextureArray.GetLayers();
        }

        if (side2 != 0xFF) {
            TextureIndex[side2] = BlockTextureArray.GetLayers();
        }

        if (side3 != 0xFF) {
            TextureIndex[side3] = BlockTextureArray.GetLayers();
        }

        if (side4 != 0xFF) {
            TextureIndex[side4] = BlockTextureArray.GetLayers();
        }

        if (side5 != 0xFF) {
            TextureIndex[side5] = BlockTextureArray.GetLayers();
        }
        return BlockTextureArray.GetLayers();
    }

    int GetFace(int side) {
        return TextureIndex[side];
    }

private:
    int TextureIndex[6]{ NULL };
    RawTextureData* BlockTex[6]{ nullptr };
};



#endif // !BLOCK_TEX

