#include "Level/TerrainGeneration/Generators/Mountains/Mountains.h"
#include "Utils/FastNoiseLite.h"

MountainGenerator::MountainGenerator() : noise_{std::make_unique<FastNoiseLite>()} {
    noise_->SetNoiseType(noise_->NoiseType_OpenSimplex2);
    noise_->SetFrequency(0.005f);
}

void MountainGenerator::Generate(const ChunkPos& pos, std::unique_ptr<Chunk>& chunk) {
    ChunkPos scaledPos = pos * kChunkDim;

    float heightBias = 50;
    float noiseOffset = 0.3;

    for (int x = 0; x < kChunkDim; x++) {
        for (int z = 0; z < kChunkDim; z++) {

            float continental = ContinentialNoise(GetNoise2D(glm::ivec2(pos.x, pos.z), glm::ivec2(x, z), 3, 0.3f));
            float erosion = ErosionNoise(GetNoise2D(glm::ivec2(pos.x, pos.z), glm::ivec2(x + 4345, z + 6443), 3, 1.f)) / 2.f;
            float pv = PeaksAndValley(GetNoise2D(glm::ivec2(pos.x, pos.z), glm::ivec2(x + 65345, z + 12323), 3, 4.f)) / 8;

            for (int y = 0; y < kChunkDim; y++) {

                float gy = (float)(y + scaledPos.y);

                float n = GetNoise3D(glm::ivec3{ pos.x,pos.y ,pos.z }, glm::ivec3(x, y, z), 4, 1.f);

                n = n + noiseOffset;

                n += continental;
                n += erosion;
                n += (pv / (heightBias * (n + 0.5f))) * gy;

                n = n * exp(-gy / heightBias);

                if (n > 0.5f) {
                    if (n < 0.54f) {
                        chunk->SetBlockUnsafe(g_blocks.GRASS, x, y, z);
                        chunk->SetBlock(g_blocks.DIRT, x, y - 1, z);
                    }
                    else {
                        chunk->SetBlockUnsafe(g_blocks.STONE, x, y, z);
                    }
                }
            }
        }
    }

    GenerateEnvironment(scaledPos, chunk.get());
    GenerateDecor(scaledPos, chunk.get());

    int gx = scaledPos.x;
    int gz = scaledPos.z;
    int gy = scaledPos.y;


    if (pos.y == 3) {
        int numBlocks = static_cast<int>(g_blocks.block_type_data_.size());

        for (int x = 0; x < kChunkDim; x++) {
            for (int z = 0; z < kChunkDim; z++) {

                int px = x + gx;
                int pz = z + gz;

                if (((px & 0b1) == 1) || ((pz & 0b1) == 1)) {
                    continue;
                }

                px = px / 2;
                pz = pz / 2;

                if ((px < 0) || (px >= 20)) {
                    continue;
                }

                int b = px + pz * 20;

                if ((b < numBlocks) && (b >= 0)) {
                    chunk->SetBlockUnsafe(b, x, 3, z);
                }
            }
        }
    }
}

void MountainGenerator::GenerateEnvironment(const ChunkPos& pos, Chunk* chunk) {
    for (int x = 0; x < kChunkDim; x++) {
        for (int z = 0; z < kChunkDim; z++) {
            for (int y = 0; y < kChunkDim; y++) {

                if (y + pos.y < 34) {
                    if ((chunk->GetBlockUnsafe(x, y, z) == g_blocks.AIR)) {
                        chunk->SetBlockUnsafe(g_blocks.BLUE_CONCRETE, x, y, z);
                    }

                    if ((chunk->GetBlockUnsafe(x, y, z) == g_blocks.GRASS)) {
                        chunk->SetBlockUnsafe(g_blocks.SAND, x, y, z);
                    }
                }
            }
        }
    }
}

void MountainGenerator::GenerateDecor(const ChunkPos& pos, Chunk* chunk) {
    const int tree_height = 3;

    for (int x = 0; x < kChunkDim; x++) {
        for (int z = 0; z < kChunkDim; z++) {

            //Global Pos
            int gx = pos.x + x;
            int gz = pos.z + z;

            float TREE_MAP = (float)((double)(noise_->GetNoise((float)gx * 100.f, (float)gz * 100.f, 3453454.f) + 1.f) / 2.f);
            for (int y = 0; y < kChunkDim; y++) {
                if (chunk->GetBlock(x, y - 1, z) == g_blocks.GRASS) {

                    if (TREE_MAP <= 0.04) {
                        
                        for (int tx = -2; tx <= 2; tx++) {
                            for (int tz = -2; tz <= 2; tz++) {
                                if ((abs(tx) == 2) && (abs(tz) == 2))
                                    continue;
                                
                                for (int ty = tree_height; ty <= tree_height + 1; ty++)
                                    chunk->SetBlock(g_blocks.OAK_LEAF, x + tx, y + ty, z + tz);

                            }
                        }

                        for (int tx = -1; tx <= 1; tx++) {
                            for (int tz = -1; tz <= 1; tz++) {
                                for (int ty = tree_height + 2; ty <= tree_height + 3; ty++) {

                                    if ((abs(tx) == 1) && (abs(tz) == 1) && (ty == tree_height + 3)) {
                                        continue;
                                    }
                                    chunk->SetBlock(g_blocks.OAK_LEAF, x + tx, y + ty, z + tz);
                                }
                            }
                        }
                        for (int ty = 0; ty < tree_height + 2; ty++) {
                            chunk->SetBlock(g_blocks.OAK_LOG, x, y + ty, z);
                        }
                    }
                }
            }
        }
    }


    int radius = 40;

    for (int x = 0 + pos.x; x < kChunkDim + pos.x; x++) {
        for (int z = 0 + pos.z; z < kChunkDim + pos.z; z++) {
            for (int y = 0 + pos.y; y < kChunkDim + pos.y; y++) {
                if ((x * x) + (y - 140) * (y - 140) + z * z <= radius * radius) {
                    chunk->SetBlock(g_blocks.SAND, x - pos.x, y - pos.y, z - pos.z);
                }

                //if (y == 90) {
                //    SetBlock(Blocks.SAND, x - cx, y - cy, z - cz);
                //}
            }
        }
    }

    // Create a roof to test lighting 
    if (pos.y != kChunkDim * 5) return;
    for (int x = 0 + pos.x; x < kChunkDim + pos.x; x++) {
        for (int z = 0 + pos.z; z < kChunkDim + pos.z; z++) {
            if ((x - 100) * (x - 100) + (z - 100) * (z - 100) <= 100 * 100) {
                chunk->SetBlock(g_blocks.WHITE_CONCRETE, x - pos.x, 10, z - pos.z);
            }

            //if (y == 90) {
            //    SetBlock(Blocks.SAND, x - cx, y - cy, z - cz);
            //}             
        }
    }
}


float MountainGenerator::GetNoise3D(glm::ivec3 ChunkCoordinate, glm::ivec3 RelativeBlockCoords, int samples, float frequency) {
    glm::vec3 GlobalBlockPosition = glm::vec3(ChunkCoordinate * kChunkDim + RelativeBlockCoords);

    GlobalBlockPosition *= frequency;

    float out = 0.0f;

    for (int i = 0; i < samples; i++) {
        float n = noise_->GetNoise(
            GlobalBlockPosition.x * powf(2.0, static_cast<float>(i)),
            GlobalBlockPosition.y * powf(2.0, static_cast<float>(i)),
            GlobalBlockPosition.z * powf(2.0, static_cast<float>(i))) + 1;

        n *= 0.5f;
        out += n * powf(0.5f, static_cast<float>(i));
    }

    out = out * ((-0.5f) / (powf(0.5, static_cast<float>(samples)) - 1));

    return out;

}
float MountainGenerator::GetNoise2D(glm::ivec2 ChunkCoordinate, glm::ivec2 RelativeBlockCoords, int samples, float frequency) {
    glm::vec2 GlobalBlockPosition = glm::vec2(ChunkCoordinate * kChunkDim + RelativeBlockCoords);

    GlobalBlockPosition *= frequency;

    float out = 0.0f;

    for (int i = 0; i < samples; i++) {
        float n = noise_->GetNoise(GlobalBlockPosition.x * powf(2.0, static_cast<float>(i)),
            GlobalBlockPosition.y * powf(2.0, static_cast<float>(i))) + 1;
        n *= 0.5f;
        out += n * powf(0.5f, static_cast<float>(i));
    }

    out = out * ((-0.5f) / (powf(0.5f, static_cast<float>(samples)) - 1.0f));

    return out;
}


float MountainGenerator::ContinentialNoise(float n) {
    size_t index = GetIndex(continentalness_interpolation_, n);

    float x1 = continentalness_interpolation_[index].x;
    float y1 = continentalness_interpolation_[index].y;

    index++;

    float x2 = continentalness_interpolation_[index].x;
    float y2 = continentalness_interpolation_[index].y;

    float m = (y1 - y2) / (x1 - x2);

    float out = m * (n - x1) + y1;

    return out;
}

float MountainGenerator::ErosionNoise(float n) {
    size_t index = GetIndex(erosionness_interpolation_, n);

    float x1 = erosionness_interpolation_[index].x;
    float y1 = erosionness_interpolation_[index].y;

    index++;

    float x2 = erosionness_interpolation_[index].x;
    float y2 = erosionness_interpolation_[index].y;

    float m = (y1 - y2) / (x1 - x2);

    float out = m * (n - x1) + y1;

    return out;
}

float MountainGenerator::PeaksAndValley(float n) {
    size_t index = GetIndex(peaks_valley_interpolation_, n);

    float x1 = peaks_valley_interpolation_[index].x;
    float y1 = peaks_valley_interpolation_[index].y;

    index++;

    float x2 = peaks_valley_interpolation_[index].x;
    float y2 = peaks_valley_interpolation_[index].y;

    float m = (y1 - y2) / (x1 - x2);

    float out = m * (n - x1) + y1;

    return out;
}

size_t MountainGenerator::GetIndex(std::vector<glm::vec2>& vec, float bottomBound) {
    for (int i = 0; i < vec.size(); i++) {
        if (vec[i].x >= bottomBound) {
            return i - 1;
        }
    }

    return vec.size() - 1;
}