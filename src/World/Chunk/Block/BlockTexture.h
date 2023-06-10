#pragma once

#include "../../../RenderEngine/OpenGL/Texture/Types/TextureArray.h"
#include <string>

#define FRONT 0x00
#define BACK 0x01
#define RIGHT 0x02
#define LEFT 0x03
#define TOP 0x04
#define BOTTOM 0x05

TextureArray BlockTextureArray;

class BlockTexture {
public:
    void SetFaceFront(std::string file) {
        BlockTex[FRONT] = new RawTextureData;
        BlockTex[FRONT]->Load(file.c_str());
        BlockTextureArray.AddTextureToArray(BlockTex[FRONT]);
        TextureIndex[FRONT] = BlockTextureArray.GetLayers();
    }
    void SetFaceBack(std::string file) {
        BlockTex[BACK] = new RawTextureData;
        BlockTex[BACK]->Load(file.c_str());
        BlockTextureArray.AddTextureToArray(BlockTex[BACK]);
        TextureIndex[BACK] = BlockTextureArray.GetLayers();
    }
    void SetFaceWest(std::string file) {
        BlockTex[RIGHT] = new RawTextureData;
        BlockTex[RIGHT]->Load(file.c_str());
        BlockTextureArray.AddTextureToArray(BlockTex[RIGHT]);
        TextureIndex[RIGHT] = BlockTextureArray.GetLayers();
    }
    void SetFaceEast(std::string file) {
        BlockTex[LEFT] = new RawTextureData;
        BlockTex[LEFT]->Load(file.c_str());
        BlockTextureArray.AddTextureToArray(BlockTex[LEFT]);
        TextureIndex[LEFT] = BlockTextureArray.GetLayers();
    }
    void SetFaceTop(std::string file) {
        BlockTex[TOP] = new RawTextureData;
        BlockTex[TOP]->Load(file.c_str());
        BlockTextureArray.AddTextureToArray(BlockTex[TOP]);
        TextureIndex[TOP] = BlockTextureArray.GetLayers();
    }
    void SetFaceBottom(std::string file) {
        BlockTex[BOTTOM] = new RawTextureData;
        BlockTex[BOTTOM]->Load(file.c_str());
        BlockTextureArray.AddTextureToArray(BlockTex[BOTTOM]);
        TextureIndex[BOTTOM] = BlockTextureArray.GetLayers();
    }
    void SetFacesCustom(std::string file, int side0 = 0xFF, int side1 = 0xFF, int side2 = 0xFF, int side3 = 0xFF, int side4 = 0xFF, int side5 = 0xFF) {
        RawTextureData* tex = new RawTextureData;
        tex->Load(file.c_str());
        BlockTextureArray.AddTextureToArray(tex);
        
        if (side0 != 0xFF) {
            BlockTex[side0] = tex;
            TextureIndex[side0] = BlockTextureArray.GetLayers();
        }
           
        if (side1 != 0xFF) {
            BlockTex[side1] = tex;
            TextureIndex[side1] = BlockTextureArray.GetLayers();
        }
           
        if (side2 != 0xFF) {
            BlockTex[side2] = tex;
            TextureIndex[side2] = BlockTextureArray.GetLayers();
        }
          
        if (side3 != 0xFF) {
            BlockTex[side3] = tex;
            TextureIndex[side3] = BlockTextureArray.GetLayers();
        }
           
        if (side4 != 0xFF) {
            BlockTex[side4] = tex;
            TextureIndex[side4] = BlockTextureArray.GetLayers();
        }
            
        if (side5 != 0xFF) {
            BlockTex[side5] = tex;
            TextureIndex[side5] = BlockTextureArray.GetLayers();
        }
    }

    int GetFace(int side) {
        return TextureIndex[side];
    }

private:
    int TextureIndex[6]{NULL};
    RawTextureData* BlockTex[6]{nullptr};
};

