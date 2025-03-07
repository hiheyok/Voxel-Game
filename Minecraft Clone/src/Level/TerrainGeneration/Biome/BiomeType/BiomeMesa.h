#pragma once
#include "../Biome.h"
#include <cmath>

class BiomeMesa : public Biome {
private:
    std::vector<BlockID> clay_bands_;
    long world_seed_;
    std::unique_ptr<NoiseGeneratorPerlin> pillar_noise_;
    std::unique_ptr<NoiseGeneratorPerlin> pillar_roof_noise_;
    std::unique_ptr<NoiseGeneratorPerlin> clay_bands_offset_noise_;
    bool bryce_pillars_;

public:
    bool has_forest_ = false;

    BiomeMesa(bool brycePillarsIn, bool hasForestIn, BiomeProperties properties) : 
        Biome{ properties },
        bryce_pillars_{ brycePillarsIn },
        has_forest_{ hasForestIn } {

        topBlock = g_blocks.RED_SAND;
        fillerBlock = g_blocks.TERRACOTTA; //STAINED_HARDEN_CLAY
    
    }

    const std::type_info& getBiomeClass() const override {
        return typeid(*this);
    }

    void GenerateBands(long p_150619_1_)
    {
        clay_bands_.resize(64);
        for (int i = 0; i < 64; i++) clay_bands_[i] = g_blocks.TERRACOTTA;
        JavaRandom random = JavaRandom(p_150619_1_);
        clay_bands_offset_noise_ = std::make_unique<NoiseGeneratorPerlin>(random, 1);

        for (int l1 = 0; l1 < 64; ++l1)
        {
            l1 += random.NextInt(5) + 1;

            if (l1 < 64)
            {
                clay_bands_[l1] = g_blocks.ORANGE_TERRACOTTA;
            }
        }

        int i2 = random.NextInt(4) + 2;

        for (int i = 0; i < i2; ++i)
        {
            int j = random.NextInt(3) + 1;
            int k = random.NextInt(64);

            for (int l = 0; k + l < 64 && l < j; ++l)
            {
                clay_bands_[k + l] = g_blocks.YELLOW_TERRACOTTA;
            }
        }

        int j2 = random.NextInt(4) + 2;

        for (int k2 = 0; k2 < j2; ++k2)
        {
            int i3 = random.NextInt(3) + 2;
            int l3 = random.NextInt(64);

            for (int i1 = 0; l3 + i1 < 64 && i1 < i3; ++i1)
            {
                clay_bands_[l3 + i1] = g_blocks.BROWN_TERRACOTTA;
            }
        }

        int l2 = random.NextInt(4) + 2;

        for (int j3 = 0; j3 < l2; ++j3)
        {
            int i4 = random.NextInt(3) + 1;
            int k4 = random.NextInt(64);

            for (int j1 = 0; k4 + j1 < 64 && j1 < i4; ++j1)
            {
                clay_bands_[k4 + j1] = g_blocks.RED_TERRACOTTA;
            }
        }

        int k3 = random.NextInt(3) + 3;
        int j4 = 0;

        for (int l4 = 0; l4 < k3; ++l4)
        {
            //int i5 = 1;
            j4 += random.NextInt(16) + 4;

            for (int k1 = 0; j4 + k1 < 64 && k1 < 1; ++k1)
            {
                clay_bands_[j4 + k1] = g_blocks.WHITE_TERRACOTTA;

                if (j4 + k1 > 1 && random.NextBoolean())
                {
                    clay_bands_[j4 + k1 - 1] = g_blocks.GRAY_TERRACOTTA; //silver terracotta
                }

                if (j4 + k1 < 63 && random.NextBoolean())
                {
                    clay_bands_[j4 + k1 + 1] = g_blocks.GRAY_TERRACOTTA;
                }
            }
        }
    }

    BlockID GetBand(int p_180629_1_, int p_180629_2_, int p_180629_3_)
    {
        int i = (int)std::round(clay_bands_offset_noise_->getValue((double)p_180629_1_ / 512.0, (double)p_180629_1_ / 512.0) * 2.0);
        return clay_bands_[(p_180629_2_ + i + 64) % 64];
    }

    void GenTerrainBlocks(JavaRandom& rand, TallChunk* chunk, int x, int z, double noiseVal, ChunkGeneratorSettings* settings_) override { //doiuble check this again later
        if (clay_bands_.size() == 0 || world_seed_ != -1587754402)
        {
            GenerateBands(-1587754402); //world seed here
        }

        if (pillar_noise_ == nullptr || pillar_roof_noise_ == nullptr || world_seed_ != -1587754402)
        {
            JavaRandom random = JavaRandom(world_seed_);
            pillar_noise_ = std::make_unique<NoiseGeneratorPerlin>(random, 4);
            pillar_roof_noise_ = std::make_unique<NoiseGeneratorPerlin>(random, 1);
        }

        world_seed_ = -1587754402;
        double d4 = 0.0;

        if (bryce_pillars_)
        {
            int i = (x & -16) + (z & 15);
            int j = (z & -16) + (x & 15);
            double d0 = std::min(abs(noiseVal), pillar_noise_->getValue((double)i * 0.25, (double)j * 0.25));

            if (d0 > 0.0)
            {
                // double d1 = 0.001953125;
                double d2 = std::abs(pillar_roof_noise_->getValue((double)i * 0.001953125, (double)j * 0.001953125));
                d4 = d0 * d0 * 2.5;
                double d3 = ceil(d2 * 50.0) + 14.0;

                if (d4 > d3)
                {
                    d4 = d3;
                }

                d4 = d4 + 64.0;
            }
        }

        int k1 = x & 15;
        int l1 = z & 15;
        int i2 = settings_->seaLevel;
        BlockID iblockstate = g_blocks.WHITE_TERRACOTTA;
        BlockID iblockstate3 = fillerBlock;
        int k = (int)(noiseVal / 3.0 + 3.0 + rand.NextDouble() * 0.25);
        bool flag = cos(noiseVal / 3.0 * 3.14159) > 0.0;
        int l = -1;
        bool flag1 = false;
        int i1 = 0;

        for (int j1 = 255; j1 >= 0; --j1)
        {
            if (chunk->GetBlockUnsafe(l1, j1, k1) == g_blocks.AIR && j1 < (int)d4)
            {
                chunk->SetBlockUnsafe(l1, j1, k1, g_blocks.STONE);
            }

            if (j1 <= rand.NextInt(5))
            {
                chunk->SetBlockUnsafe(l1, j1, k1, g_blocks.BEDROCK);
            }
            else if (i1 < 15 || bryce_pillars_)
            {
                BlockID iblockstate1 = chunk->GetBlockUnsafe(l1, j1, k1);

                if (iblockstate1 == g_blocks.AIR)
                {
                    l = -1;
                }
                else if (iblockstate1 == g_blocks.STONE)
                {
                    if (l == -1)
                    {
                        flag1 = false;

                        if (k <= 0)
                        {
                            iblockstate = g_blocks.AIR;
                            iblockstate3 = g_blocks.STONE;
                        }
                        else if (j1 >= i2 - 4 && j1 <= i2 + 1)
                        {
                            iblockstate = g_blocks.WHITE_TERRACOTTA;
                            iblockstate3 = fillerBlock;
                        }

                        if (j1 < i2 && (iblockstate == g_blocks.AIR))
                        {
                            iblockstate = g_blocks.WATER;
                        }

                        l = k + std::max(0, j1 - i2);

                        if (j1 >= i2 - 1)
                        {
                            if (has_forest_ && j1 > 86 + k * 2)
                            {
                                if (flag)
                                {
                                    chunk->SetBlockUnsafe(l1, j1, k1, g_blocks.COARSE_DIRT);
                                }
                                else
                                {
                                    chunk->SetBlockUnsafe(l1, j1, k1, g_blocks.GRASS);
                                }
                            }
                            else if (j1 > i2 + 3 + k)
                            {
                                BlockID iblockstate2;

                                if (j1 >= 64 && j1 <= 127)
                                {
                                    if (flag)
                                    {
                                        iblockstate2 = g_blocks.TERRACOTTA;
                                    }
                                    else
                                    {
                                        iblockstate2 = GetBand(x, j1, z);
                                    }
                                }
                                else
                                {
                                    iblockstate2 = g_blocks.ORANGE_TERRACOTTA;
                                }

                                chunk->SetBlockUnsafe(l1, j1, k1, iblockstate2);
                            }
                            else
                            {
                                chunk->SetBlockUnsafe(l1, j1, k1, topBlock);
                                flag1 = true;
                            }
                        }
                        else
                        {
                            chunk->SetBlockUnsafe(l1, j1, k1, iblockstate3);

                            if (iblockstate3 == g_blocks.WHITE_TERRACOTTA)
                            {
                                chunk->SetBlockUnsafe(l1, j1, k1, g_blocks.ORANGE_TERRACOTTA);
                            }
                        }
                    }
                    else if (l > 0)
                    {
                        --l;

                        if (flag1)
                        {
                            chunk->SetBlockUnsafe(l1, j1, k1, g_blocks.ORANGE_TERRACOTTA);
                        }
                        else
                        {
                            chunk->SetBlockUnsafe(l1, j1, k1, GetBand(x, j1, z));
                        }
                    }

                    ++i1;
                }
            }
        }
    }
};