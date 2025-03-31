#include "ChunkMeshingV2.h"
#include "../../BlockModel/BlockModels.h"
#include "../../../Level/Chunk/Chunk.h"
#include "../../../Level/Chunk/Block/Blocks.h"
#include <cstdint>

constexpr int POSITION_OFFSET = 16;

constexpr int BitShiftAmount = 9; //27 bits
constexpr int NormBitOffset = 27; //2 bits
constexpr int tintBitOffset = 29; // 1 bit

constexpr int textureBitOffset = 10; // 18 bits
constexpr int blockShadingBitOffset = 28; // 4 bits
#define PROFILE_DEBUG

void Mesh::ChunkMeshData::Reset() {
    if (vertices_buffer_.size() != BUFFER_SIZE_STEP) {
        vertices_buffer_.resize(BUFFER_SIZE_STEP, 0);
    }

    if (transparent_vertices_buffer_.size() != BUFFER_SIZE_STEP) {
        transparent_vertices_buffer_.resize(BUFFER_SIZE_STEP, 0);
    }

    transparent_face_count_ = 0;
    solid_face_count_ = 0;

    memset(chunk_cache_, NULL, 18 * 18 * 18 * sizeof(BlockID));
}

void Mesh::ChunkMeshData::GenerateCache() {
    int pos[3]{ 0, 0, 0 };
    int localPos[3]{ 0, 0, 0 };

    for (int x = 0; x < 16; x++) {
        for (int z = 0; z < 16; z++) {
            for (int y = 0; y < 16; ++y) {
                SetCachedBlockID(chunk_->GetBlockUnsafe(x, y, z), x, y, z);
            }
        }
    }

    for (int side = 0; side < 6; side++) {
        int axis = side >> 1;
        int direction = side & 0b1;

        if (chunk_->neighbors_[side] == nullptr) {
            continue;
        }

        for (int u = 0; u < 16; u++) {
            for (int v = 0; v < 16; v++) {

                pos[axis] = direction * 15;
                pos[(axis + 1) % 3] = u;
                pos[(axis + 2) % 3] = v;
                
                localPos[axis] = 17 - 17 * direction;
                localPos[(axis + 1) % 3] = u + 1;
                localPos[(axis + 2) % 3] = v + 1;

                chunk_cache_[localPos[0] * 18 * 18 + localPos[2] * 18 + localPos[1]] = chunk_->neighbors_[side]->GetBlockUnsafe(pos[0], pos[1], pos[2]);
            }
        }
    }
}

void Mesh::ChunkMeshData::SetChunk(Chunk* pChunk) {
    chunk_ = pChunk;
}

void Mesh::ChunkMeshData::GenerateMesh() {
    //Initialize
    if (chunk_ == nullptr) {
        return;
    }

    chunk_->Use();
    GenerateCache();
    chunk_->Unuse();
    
    GenerateFaceCollection();
}

//Loops through all the blocks in the chunk and check if each block side is visible. If a block side is visible, it generates the quad and puts it in the cache
void Mesh::ChunkMeshData::GenerateFaceCollection() {
    std::vector<uint8_t> faceVisibilityBack(4096, 0b00);
    std::vector<uint8_t> faceVisibility(4096, 0b00);
    std::vector<uint8_t> usedBlock(16 * 16, 0b00);

    for (int axis = 0; axis < 3; axis++) {
        int axisU = (axis + 2) % 3;
        int axisV = (axis + 1) % 3;
        int pos[3]{ 0,0,0 };

        for (pos[axis] = 0; pos[axis] < 17; ++pos[axis]) {//Slice
            memset(usedBlock.data(), 0x00, 16 * 16);
            for (pos[axisU] = 0; pos[axisU] < 16; ++pos[axisU]) {
                for (pos[axisV] = 0; pos[axisV] < 16; ++pos[axisV]) {

                    if (usedBlock[(pos[axisU] << 4) + pos[axisV]] != 0x00) {
                        pos[axisV] += usedBlock[(pos[axisU] << 4) + pos[axisV]] - 1; 
                        continue;
                    }

                    usedBlock[(pos[axisU] << 4) + pos[axisV]] = 0xFF;

                    const BlockID& currBlock = GetCachedBlockID(pos[0], pos[1], pos[2]);
                    --pos[axis];
                    const BlockID& backBlock = GetCachedBlockID(pos[0], pos[1], pos[2]);
                    ++pos[axis];

                    const Model::BlockModel& currModel = g_blocks.GetBlockModelDereferenced(currBlock);
                    const Model::BlockModel& backModel = g_blocks.GetBlockModelDereferenced(backBlock);

                    bool blankCurrModel = !currModel.is_initialized_ || pos[axis] == 16;
                    bool blankBackModel = !backModel.is_initialized_ || pos[axis] == 0;

                    //Check if it is visible from the back and front
                    if (!blankCurrModel) {
                        for (int i = 0; i < currModel.elements_.size(); ++i) {
                            faceVisibility[i] = 0;
                            const Cuboid& element = currModel.elements_[i];

                            if (element.faces_[axis * 2 + 1].reference_texture_.length() == 0)
                                continue;

                            if (element.faces_[axis * 2 + 1].cull_face_ != -1) {
                                if (!IsFaceVisible(element, pos[0], pos[1], pos[2], element.faces_[axis * 2 + 1].cull_face_)) continue;
                            }

                            faceVisibility[i] |= 0b1;
                        }

                    }
                    //Check if it is visible from the back and front
                    --pos[axis];
                    if (!blankBackModel) {
                        for (int i = 0; i < backModel.elements_.size(); ++i) {
                            faceVisibilityBack[i] = 0;
                            const Cuboid& element = backModel.elements_[i];

                            if (element.faces_[axis * 2].reference_texture_.length() == 0)
                                continue;

                            if (element.faces_[axis * 2].cull_face_ != -1) {
                                if (!IsFaceVisible(element, pos[0], pos[1], pos[2], element.faces_[axis * 2].cull_face_)) continue;
                            }

                            faceVisibilityBack[i] |= 0b1;
                        }
                    }
                    ++pos[axis];

                    //Spread

                    int uLength = 1;
                    int vLength = 1;

                    int qPos[3]{ pos[0], pos[1], pos[2] };

                    for (qPos[axisV] = pos[axisV] + 1; qPos[axisV] < 16; ++qPos[axisV]) {
                        //Check if they are the same
                        const BlockID& currBlock2 = GetCachedBlockID(qPos[0], qPos[1], qPos[2]);
                        --qPos[axis];
                        const BlockID& backBlock2 = GetCachedBlockID(qPos[0], qPos[1], qPos[2]);
                        ++qPos[axis];

                        if (currBlock2 != currBlock || backBlock2 != backBlock)
                            break;

                        usedBlock[(qPos[axisU] << 4) + qPos[axisV]] = 0xFFU;
                        vLength++;
                    }

                    //memset(usedBlock.data() + (pos[axisU] << 4) + pos[axisV], vLength, vLength);

                    qPos[0] = pos[0];
                    qPos[1] = pos[1];
                    qPos[2] = pos[2];

                    for (qPos[axisU] = pos[axisU] + 1; qPos[axisU] < 16; ++qPos[axisU]) {
                        bool isValid = true;

                        for (qPos[axisV] = pos[axisV]; qPos[axisV] < pos[axisV] + vLength; ++qPos[axisV]) {
                            const BlockID& currBlock2 = GetCachedBlockID(qPos[0], qPos[1], qPos[2]);
                            --qPos[axis];
                            const BlockID& backBlock2 = GetCachedBlockID(qPos[0], qPos[1], qPos[2]);
                            ++qPos[axis];

                            if (currBlock2 != currBlock || backBlock2 != backBlock) {
                                isValid = false;
                                break;
                            }
                        }

                        if (!isValid) {
                            break;
                        }

                        memset(usedBlock.data() + (static_cast<long long>(qPos[axisU]) << 4) + pos[axisV], vLength, vLength);

                        ++uLength;
                    }

                    qPos[0] = pos[0];
                    qPos[1] = pos[1];
                    qPos[2] = pos[2];

                    //Memorize & Add Faces

                    // TODO: Add some AO Checks and Greedy Meshing here

                    /*
                    * Center wont have AO check as it is no sides
                    * Do lighting checks when combining faces
                    */
                    if (!blankCurrModel) {
                        for (int i = 0; i < currModel.elements_.size(); i++) {
                            if (faceVisibility[i] != 1) continue;
                            const Cuboid& element = currModel.elements_[i];
                            for (qPos[axisU] = pos[axisU]; qPos[axisU] < pos[axisU] + uLength; ++qPos[axisU]) {
                                for (qPos[axisV] = pos[axisV]; qPos[axisV] < pos[axisV] + vLength; ++qPos[axisV]) {
                                    AddFacetoMesh(element.faces_[axis * 2 + 1], axis * 2 + 1, element.from_, element.to_, currModel.ambient_occlusion_, qPos[0], qPos[1], qPos[2]);
                                }
                            }
                        }
                    }
                    --qPos[axis];
                    if (!blankBackModel) {
                        for (int i = 0; i < backModel.elements_.size(); i++) {
                            if (faceVisibilityBack[i] != 1) continue;
                            const Cuboid& element = backModel.elements_[i];
                            for (qPos[axisU] = pos[axisU]; qPos[axisU] < pos[axisU] + uLength; ++qPos[axisU]) {
                                for (qPos[axisV] = pos[axisV]; qPos[axisV] < pos[axisV] + vLength; ++qPos[axisV]) {
                                    AddFacetoMesh(element.faces_[axis * 2], axis * 2, element.from_, element.to_, backModel.ambient_occlusion_, qPos[0], qPos[1], qPos[2]);
                                }
                            }
                        }
                    }

                    qPos[0] = pos[0];
                    qPos[1] = pos[1];
                    qPos[2] = pos[2];

                    if (vLength == 16 && uLength == 16) { //Skip entire layer
                        pos[axisV] = 15;
                        pos[axisU] = 15;
                    }
                    else {
                        pos[axisV] += vLength - 1;
                    }
                }
            }
        }
    }
}

inline void Mesh::ChunkMeshData::AddFacetoMesh(const BlockFace& face, uint8_t axis_, glm::ivec3 from_, glm::ivec3 to_, bool allowAO, int x, int y, int z) {
    uint8_t NN = 15, PN = 15, PP = 15, NP = 15;
    uint8_t direction = axis_ & 0b1;
    uint8_t facing = axis_ >> 1;

    x *= 16;
    y *= 16;
    z *= 16;

    glm::ivec3 P0{ x + from_[0] + POSITION_OFFSET, y + from_[1] + POSITION_OFFSET, z + from_[2] + POSITION_OFFSET };
    glm::ivec3 P1{ x + to_[0] + POSITION_OFFSET, y + to_[1] + POSITION_OFFSET, z + to_[2] + POSITION_OFFSET };

    glm::ivec2 texNN = face.uv_coord_nn;
    glm::ivec2 texPN = face.uv_coord_np;
    glm::ivec2 texPP = face.uv_coord_pp;
    glm::ivec2 texNP = face.uv_coord_pn;

    texNN.y <<= 5;
    texNP.y <<= 5;
    texPP.y <<= 5;
    texPN.y <<= 5;

    P0.x <<= 0;
    P1.x <<= 0;
    P0.y <<= BitShiftAmount;
    P1.y <<= BitShiftAmount;
    P0.z <<= BitShiftAmount * 2;
    P1.z <<= BitShiftAmount * 2;

    glm::ivec3 tP0 = P0;
    glm::ivec3 tP1 = P1;

    P0.x = tP0[facing % 3];
    P1.x = tP1[facing % 3];
    P0.y = tP0[(facing + 1) % 3];
    P1.y = tP1[(facing + 1) % 3];
    P0.z = tP0[(facing + 2) % 3];
    P1.z = tP1[(facing + 2) % 3];

    if (direction == 0)
        std::swap(P0.x, P1.x);
    if (facing == 2)
        std::swap(texPN, texNP);

    uint32_t tex = face.texture_id_ << textureBitOffset;
    uint32_t norm = facing << NormBitOffset;
    uint32_t tint = (static_cast<uint32_t>(!!face.tint_index_)) << tintBitOffset;

    std::vector<uint32_t>& out = face.has_transparency_ ? transparent_vertices_buffer_ : vertices_buffer_;

    uint64_t currIndex = face.has_transparency_ ? transparent_face_count_++ : solid_face_count_++;

    if (out.size() <= (currIndex + 1) * 12)
        out.resize(out.size() + BUFFER_SIZE_STEP);

    //Get AO
    glm::u8vec4 AO = allowAO ? getAO(axis_, x / 16, y / 16, z / 16) : glm::u8vec4{ 15, 15, 15, 15 };
   
    PP = AO[0], PN = AO[1], NP = AO[2], NN = AO[3];

    out[currIndex * 12 + 0]  = 0u | P0[0] | P0[1] | P0[2] | norm | tint;
    out[currIndex * 12 + 1]  = 0u | texNN.x | texNN.y | tex | (NN << blockShadingBitOffset);
    out[currIndex * 12 + 2]  = 0u | P0[0] | P1[1] | P0[2] | norm | tint;
    out[currIndex * 12 + 3]  = 0u | texPN.x | texPN.y | tex | (PN << blockShadingBitOffset);
    out[currIndex * 12 + 4]  = 0u | P0[0] | P0[1] | P1[2] | norm | tint;
    out[currIndex * 12 + 5]  = 0u | texNP.x | texNP.y | tex | (NP << blockShadingBitOffset);
    out[currIndex * 12 + 6]  = 0u | P0[0] | P0[1] | P1[2] | norm | tint;
    out[currIndex * 12 + 7]  = 0u | texNP.x | texNP.y | tex | (NP << blockShadingBitOffset);
    out[currIndex * 12 + 8]  = 0u | P0[0] | P1[1] | P0[2] | norm | tint;
    out[currIndex * 12 + 9]  = 0u | texPN.x | texPN.y | tex | (PN << blockShadingBitOffset);
    out[currIndex * 12 + 10] = 0u | P0[0] | P1[1] | P1[2] | norm | tint;
    out[currIndex * 12 + 11] = 0u | texPP.x | texPP.y | tex | (PP << blockShadingBitOffset);

    if (direction == 0) {
        std::swap(out[currIndex * 12 + 2], out[currIndex * 12 + 4]);
        std::swap(out[currIndex * 12 + 3], out[currIndex * 12 + 5]);

        std::swap(out[currIndex * 12 + 8], out[currIndex * 12 + 10]);
        std::swap(out[currIndex * 12 + 9], out[currIndex * 12 + 11]);
    }
}

inline const BlockID& Mesh::ChunkMeshData::GetCachedBlockID(int x, int y, int z) const {
    return chunk_cache_[(x + 1) * 18 * 18 + (z + 1) * 18 + (y + 1)];
}

inline const BlockID& Mesh::ChunkMeshData::GetCachedBlockID(int* pos) const {
    return GetCachedBlockID(pos[0], pos[1], pos[2]);
}

inline void Mesh::ChunkMeshData::SetCachedBlockID(BlockID b, int x, int y, int z) {
    chunk_cache_[(x + 1) * 18 * 18 + (z + 1) * 18 + (y + 1)] = b;
}

inline glm::u8vec4 Mesh::ChunkMeshData::getAO(uint8_t direction, int x, int y, int z) {
    const uint8_t AMBIENT_OCCLUSION_STRENGTH = 2;
    glm::ivec3 pos{x, y, z};

    char InitialLighting = chunk_->lighting_->GetLighting(x, y, z);

    uint8_t PP{ 15 }, PN{ 15 }, NP{ 15 }, NN{ 15 };

    uint8_t axis_ = direction >> 1;
    uint8_t facing = direction & 0b1;

    pos[axis_] += 1 - 2 * facing;

    //Check up down left right

    int axis1 = (axis_ + 1) % 3;
    int axis2 = (axis_ + 2) % 3;

    //Check up
    pos[axis1] += 1;

    if (GetCachedBlockID(pos.x, pos.y, pos.z) != g_blocks.AIR) {
        PP -= AMBIENT_OCCLUSION_STRENGTH;
        PN -= AMBIENT_OCCLUSION_STRENGTH;
    }

    pos[axis1] -= 2;

    if (GetCachedBlockID(pos.x, pos.y, pos.z) != g_blocks.AIR) {
        NP -= AMBIENT_OCCLUSION_STRENGTH;
        NN -= AMBIENT_OCCLUSION_STRENGTH;
    }

    pos[axis1] += 1;
    pos[axis2] += 1;

    if (GetCachedBlockID(pos.x, pos.y, pos.z) != g_blocks.AIR) {
        NP -= AMBIENT_OCCLUSION_STRENGTH;
        PP -= AMBIENT_OCCLUSION_STRENGTH;
    }

    pos[axis2] -= 2;

    if (GetCachedBlockID(pos.x, pos.y, pos.z) != g_blocks.AIR) {
        PN -= AMBIENT_OCCLUSION_STRENGTH;
        NN -= AMBIENT_OCCLUSION_STRENGTH;
    }

    pos[axis2] += 1;


    //Check corners now
    pos[axis1] += 1;
    pos[axis2] += 1;

    if (GetCachedBlockID(pos.x, pos.y, pos.z) != g_blocks.AIR)
        PP -= AMBIENT_OCCLUSION_STRENGTH;

    pos[axis1] -= 2;
    if (GetCachedBlockID(pos.x, pos.y, pos.z) != g_blocks.AIR)
        NP -= AMBIENT_OCCLUSION_STRENGTH;

    pos[axis2] -= 2;

    if (GetCachedBlockID(pos.x, pos.y, pos.z) != g_blocks.AIR)
        NN -= AMBIENT_OCCLUSION_STRENGTH;

    pos[axis1] += 2;

    if (GetCachedBlockID(pos.x, pos.y, pos.z) != g_blocks.AIR)
        PN -= AMBIENT_OCCLUSION_STRENGTH;


    if (PP >= (15 - InitialLighting))
        PP = PP - (15 - InitialLighting);
    else
        PP = 0;

    if (PN >= (15 - InitialLighting))
        PN = PN - (15 - InitialLighting);
    else
        PN = 0;

    if (NN >= (15 - InitialLighting))
        NN = NN - (15 - InitialLighting);
    else
        NN = 0;

    if (NP >= (15 - InitialLighting))
        NP = NP - (15 - InitialLighting);
    else
        NP = 0;

    return glm::u8vec4(PP, PN, NP, NN);

}

//Checks if a block side is visible to the player
inline bool Mesh::ChunkMeshData::IsFaceVisible(const Cuboid& cube, int x, int y, int z, uint8_t side) {
    const uint8_t axis_ = (side >> 1); //Get side
    const uint8_t axis1 = (axis_ + 1) % 3;
    const uint8_t axis2 = (axis_ + 2) % 3;
    const uint8_t oppositeSide = axis_ * 2 + static_cast<uint8_t>(!(side & 0b1));

    int p[3]{ x, y, z };

    p[axis_] += 1 - 2 * (side & 0b1);
//    return getCachedBlockID(p[0], p[1], p[2]) == Blocks.AIR;

    const Model::BlockModel& model = g_blocks.GetBlockModelDereferenced(GetCachedBlockID(p[0], p[1], p[2]));

    if (!model.is_initialized_) return true;

    for (int i = 0; i < model.elements_.size(); ++i) {
        const Cuboid& element = model.elements_[i];
        if (element.faces_[oppositeSide].cull_face_ != oppositeSide ||
            element.faces_[oppositeSide].is_see_through_ ||
            element.faces_[oppositeSide].has_transparency_ ||
            element.faces_[oppositeSide].reference_texture_.empty())
            continue; 

        if (side & 1) //if the  block arent touching
            if (element.to_[axis_] < 16) continue;
        else
            if (element.from_[axis_] > 0) continue;
        
        //Check if the faces aren't overlapping

        if (element.from_[axis1] <= cube.from_[axis1] && element.to_[axis1] >= cube.to_[axis1] &&
            element.from_[axis2] <= cube.from_[axis2] && element.to_[axis2] >= cube.to_[axis2])
            return false;

    }
    return true;
}

inline bool Mesh::ChunkMeshData::IsFaceVisibleUnsafe(const Cuboid& cube, int x, int y, int z, uint8_t side) {
    uint8_t axis_ = (side >> 1); //Get side

    int p[3]{ x,y,z };

    p[axis_] += 1 - 2 * (side & 0b1);

    return g_blocks.GetBlockType(GetCachedBlockID(p[0], p[1], p[2]))->properties_->transparency_;
}

inline bool Mesh::ChunkMeshData::CompareBlockSide(int x, int y, int z, uint8_t side, BlockID b) {
    //IsFaceVisibleCalls++;

    uint8_t axis_ = (side >> 1); //Get side

    int p[3]{ x,y,z };

    p[axis_] += 1 - 2 * (side & 0b1);

    return GetCachedBlockID(p[0], p[1], p[2]) == b;
}

inline bool Mesh::ChunkMeshData::CompareBlockSideUnsafe(int x, int y, int z, uint8_t side, BlockID b) {
    uint8_t axis_ = (side >> 1); //Get side

    int p[3]{ x,y,z };

    p[axis_] += 1 - 2 * (side & 0b1);

    return GetCachedBlockID(p[0], p[1], p[2]) == b;
}