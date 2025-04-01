#include "BlockTexture.h"
#include "Core/Typenames.h"

void BlockTexture::SetFaceFront(int texId) {
    texture_index_[FRONT] = texId;
}
void BlockTexture::SetFaceBack(int texId) {
    texture_index_[BACK] = texId;
}
void BlockTexture::SetFaceWest(int texId) {
    texture_index_[RIGHT] = texId;
}
void BlockTexture::SetFaceEast(int texId) {
    texture_index_[LEFT] = texId;
}
void BlockTexture::SetFaceTop(int texId) {
    texture_index_[TOP] = texId;
}
void BlockTexture::SetFaceBottom(int texId) {
    texture_index_[BOTTOM] = texId;
}
void BlockTexture::SetFacesCustom(int texId, int TexNum, int side0, int side1, int side2, int side3, int side4, int side5) {

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

void BlockTexture::SetParticle(int tex) {
    particle_ = tex;
}

int BlockTexture::GetParticle() {
    return particle_;
}

int BlockTexture::GetFace(int side) {
    return texture_index_[side];
}

int& BlockTexture::operator[](int i) {
    return texture_index_[i];
}

int BlockTexture::GetNumTextures(int side) {
    return texture_num_index_[side];
}