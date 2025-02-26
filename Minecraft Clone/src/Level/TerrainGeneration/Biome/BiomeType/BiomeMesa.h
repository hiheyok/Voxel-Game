#pragma once
#include "../Biome.h"
#include <tgmath.h>

class BiomeMesa : public Biome {
private:
    std::vector<BlockID> clayBands;
    long worldSeed;
    NoiseGeneratorPerlin* pillarNoise;
    NoiseGeneratorPerlin* pillarRoofNoise;
    NoiseGeneratorPerlin* clayBandsOffsetNoise;
    bool brycePillars;

public:
	bool hasForest = false;

	BiomeMesa(bool brycePillarsIn, bool hasForestIn, BiomeProperties properties) : Biome(properties), brycePillars(brycePillarsIn), hasForest(hasForestIn)  {
		topBlock = Blocks.RED_SAND;
		fillerBlock = Blocks.TERRACOTTA; //STAINED_HARDEN_CLAY
	
	}

	const std::type_info& getBiomeClass() const override {
		return typeid(*this);
	}

    void generateBands(long p_150619_1_)
    {
        clayBands.resize(64);
        for (int i = 0; i < 64; i++) clayBands[i] = Blocks.TERRACOTTA;
        JavaRandom random = JavaRandom(p_150619_1_);
        clayBandsOffsetNoise = new NoiseGeneratorPerlin(random, 1);

        for (int l1 = 0; l1 < 64; ++l1)
        {
            l1 += random.nextInt(5) + 1;

            if (l1 < 64)
            {
                clayBands[l1] = Blocks.ORANGE_TERRACOTTA;
            }
        }

        int i2 = random.nextInt(4) + 2;

        for (int i = 0; i < i2; ++i)
        {
            int j = random.nextInt(3) + 1;
            int k = random.nextInt(64);

            for (int l = 0; k + l < 64 && l < j; ++l)
            {
                clayBands[k + l] = Blocks.YELLOW_TERRACOTTA;
            }
        }

        int j2 = random.nextInt(4) + 2;

        for (int k2 = 0; k2 < j2; ++k2)
        {
            int i3 = random.nextInt(3) + 2;
            int l3 = random.nextInt(64);

            for (int i1 = 0; l3 + i1 < 64 && i1 < i3; ++i1)
            {
                clayBands[l3 + i1] = Blocks.BROWN_TERRACOTTA;
            }
        }

        int l2 = random.nextInt(4) + 2;

        for (int j3 = 0; j3 < l2; ++j3)
        {
            int i4 = random.nextInt(3) + 1;
            int k4 = random.nextInt(64);

            for (int j1 = 0; k4 + j1 < 64 && j1 < i4; ++j1)
            {
                clayBands[k4 + j1] = Blocks.RED_TERRACOTTA;
            }
        }

        int k3 = random.nextInt(3) + 3;
        int j4 = 0;

        for (int l4 = 0; l4 < k3; ++l4)
        {
            //int i5 = 1;
            j4 += random.nextInt(16) + 4;

            for (int k1 = 0; j4 + k1 < 64 && k1 < 1; ++k1)
            {
                clayBands[j4 + k1] = Blocks.WHITE_TERRACOTTA;

                if (j4 + k1 > 1 && random.nextBoolean())
                {
                    clayBands[j4 + k1 - 1] = Blocks.GRAY_TERRACOTTA; //silver terracotta
                }

                if (j4 + k1 < 63 && random.nextBoolean())
                {
                    clayBands[j4 + k1 + 1] = Blocks.GRAY_TERRACOTTA;
                }
            }
        }
    }

    BlockID getBand(int p_180629_1_, int p_180629_2_, int p_180629_3_)
    {
        int i = (int)std::round(clayBandsOffsetNoise->getValue((double)p_180629_1_ / 512.0, (double)p_180629_1_ / 512.0) * 2.0);
        return clayBands[(p_180629_2_ + i + 64) % 64];
    }

    void genTerrainBlocks(JavaRandom& rand, TallChunk* chunk, int x, int z, double noiseVal, ChunkGeneratorSettings* settings) override { //doiuble check this again later
        if (clayBands.size() == 0 || worldSeed != -1587754402)
        {
            generateBands(-1587754402); //world seed here
        }

        if (pillarNoise == nullptr || pillarRoofNoise == nullptr || worldSeed != -1587754402)
        {
            JavaRandom random = JavaRandom(worldSeed);
            pillarNoise = new NoiseGeneratorPerlin(random, 4);
            pillarRoofNoise = new NoiseGeneratorPerlin(random, 1);
        }

        worldSeed = -1587754402;
        double d4 = 0.0;

        if (brycePillars)
        {
            int i = (x & -16) + (z & 15);
            int j = (z & -16) + (x & 15);
            double d0 = std::min(abs(noiseVal), pillarNoise->getValue((double)i * 0.25, (double)j * 0.25));

            if (d0 > 0.0)
            {
                // double d1 = 0.001953125;
                double d2 = std::abs(pillarRoofNoise->getValue((double)i * 0.001953125, (double)j * 0.001953125));
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
        int i2 = settings->seaLevel;
        BlockID iblockstate = Blocks.WHITE_TERRACOTTA;
        BlockID iblockstate3 = fillerBlock;
        int k = (int)(noiseVal / 3.0 + 3.0 + rand.nextDouble() * 0.25);
        bool flag = cos(noiseVal / 3.0 * 3.14159) > 0.0;
        int l = -1;
        bool flag1 = false;
        int i1 = 0;

        for (int j1 = 255; j1 >= 0; --j1)
        {
            if (chunk->GetBlockUnsafe(l1, j1, k1) == Blocks.AIR && j1 < (int)d4)
            {
                chunk->SetBlockUnsafe(l1, j1, k1, Blocks.STONE);
            }

            if (j1 <= rand.nextInt(5))
            {
                chunk->SetBlockUnsafe(l1, j1, k1, Blocks.BEDROCK);
            }
            else if (i1 < 15 || brycePillars)
            {
                BlockID iblockstate1 = chunk->GetBlockUnsafe(l1, j1, k1);

                if (iblockstate1 == Blocks.AIR)
                {
                    l = -1;
                }
                else if (iblockstate1 == Blocks.STONE)
                {
                    if (l == -1)
                    {
                        flag1 = false;

                        if (k <= 0)
                        {
                            iblockstate = Blocks.AIR;
                            iblockstate3 = Blocks.STONE;
                        }
                        else if (j1 >= i2 - 4 && j1 <= i2 + 1)
                        {
                            iblockstate = Blocks.WHITE_TERRACOTTA;
                            iblockstate3 = fillerBlock;
                        }

                        if (j1 < i2 && (iblockstate == Blocks.AIR))
                        {
                            iblockstate = Blocks.WATER;
                        }

                        l = k + std::max(0, j1 - i2);

                        if (j1 >= i2 - 1)
                        {
                            if (hasForest && j1 > 86 + k * 2)
                            {
                                if (flag)
                                {
                                    chunk->SetBlockUnsafe(l1, j1, k1, Blocks.COARSE_DIRT);
                                }
                                else
                                {
                                    chunk->SetBlockUnsafe(l1, j1, k1, Blocks.GRASS);
                                }
                            }
                            else if (j1 > i2 + 3 + k)
                            {
                                BlockID iblockstate2;

                                if (j1 >= 64 && j1 <= 127)
                                {
                                    if (flag)
                                    {
                                        iblockstate2 = Blocks.TERRACOTTA;
                                    }
                                    else
                                    {
                                        iblockstate2 = getBand(x, j1, z);
                                    }
                                }
                                else
                                {
                                    iblockstate2 = Blocks.ORANGE_TERRACOTTA;
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

                            if (iblockstate3 == Blocks.WHITE_TERRACOTTA)
                            {
                                chunk->SetBlockUnsafe(l1, j1, k1, Blocks.ORANGE_TERRACOTTA);
                            }
                        }
                    }
                    else if (l > 0)
                    {
                        --l;

                        if (flag1)
                        {
                            chunk->SetBlockUnsafe(l1, j1, k1, Blocks.ORANGE_TERRACOTTA);
                        }
                        else
                        {
                            chunk->SetBlockUnsafe(l1, j1, k1, getBand(x, j1, z));
                        }
                    }

                    ++i1;
                }
            }
        }
    }
};