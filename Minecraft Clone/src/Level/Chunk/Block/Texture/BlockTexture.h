#pragma once

class BlockTexture {
public:
    void SetFaceFront(int texId);
    void SetFaceBack(int texId);
    void SetFaceWest(int texId);
    void SetFaceEast(int texId);
    void SetFaceTop(int texId);
    void SetFaceBottom(int texId);
    void SetFacesCustom(int texId, int TexNum, int side0 = 0xFF, int side1 = 0xFF, int side2 = 0xFF, int side3 = 0xFF, int side4 = 0xFF, int side5 = 0xFF);

    void SetParticle(int tex);

    int GetParticle();

    int GetFace(int side);

    int& operator[](int i);

    int GetNumTextures(int side);

private:
    int texture_index_[6]{ 0 };
    int texture_num_index_[6]{ 0 };
    int particle_ = 0;
};
