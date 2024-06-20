//#pragma once
//#include "MapGenBase.h"
//#include "../../Chunk/Block/Blocks.h"
//
//class MapGenCaves : public MapGenBase
//{
//protected:
//    static BlockID BLK_LAVA;
//    static BlockID BLK_AIR;
//    static BlockID BLK_SANDSTONE;
//    static BlockID BLK_RED_SANDSTONE;
//
//    void addRoom(long p_180703_1_, int p_180703_3_, int p_180703_4_, Chunk* p_180703_5_, double p_180703_6_, double p_180703_8_, double p_180703_10_)
//    {
//        addTunnel(p_180703_1_, p_180703_3_, p_180703_4_, p_180703_5_, p_180703_6_, p_180703_8_, p_180703_10_, 1.0F + rand.nextFloat() * 6.0F, 0.0F, 0.0F, -1, -1, 0.5);
//    }
//
//    void addTunnel(long p_180702_1_, int p_180702_3_, int p_180702_4_, Chunk* p_180702_5_, double p_180702_6_, double p_180702_8_, double p_180702_10_, float p_180702_12_, float p_180702_13_, float p_180702_14_, int p_180702_15_, int p_180702_16_, double p_180702_17_)
//    {
//        double d0 = (double)(p_180702_3_ * 16 + 8);
//        double d1 = (double)(p_180702_4_ * 16 + 8);
//        float f = 0.0F;
//        float f1 = 0.0F;
//        JavaRandom random = JavaRandom(p_180702_1_);
//
//        if (p_180702_16_ <= 0)
//        {
//            int i = range * 16 - 16;
//            p_180702_16_ = i - random.nextInt(i / 4);
//        }
//
//        bool flag2 = false;
//
//        if (p_180702_15_ == -1)
//        {
//            p_180702_15_ = p_180702_16_ / 2;
//            flag2 = true;
//        }
//
//        int j = random.nextInt(p_180702_16_ / 2) + p_180702_16_ / 4;
//
//        for (bool flag = random.nextInt(6) == 0; p_180702_15_ < p_180702_16_; ++p_180702_15_)
//        {
//            double d2 = 1.5 + (double)(sin((float)p_180702_15_ * (float)3.14159265358979323 / (float)p_180702_16_) * p_180702_12_);
//            double d3 = d2 * p_180702_17_;
//            float f2 = cos(p_180702_14_);
//            float f3 = sin(p_180702_14_);
//            p_180702_6_ += (double)(cos(p_180702_13_) * f2);
//            p_180702_8_ += (double)f3;
//            p_180702_10_ += (double)(sin(p_180702_13_) * f2);
//
//            if (flag)
//            {
//                p_180702_14_ = p_180702_14_ * 0.92F;
//            }
//            else
//            {
//                p_180702_14_ = p_180702_14_ * 0.7F;
//            }
//
//            p_180702_14_ = p_180702_14_ + f1 * 0.1F;
//            p_180702_13_ += f * 0.1F;
//            f1 = f1 * 0.9F;
//            f = f * 0.75F;
//            f1 = f1 + (random.nextFloat() - random.nextFloat()) * random.nextFloat() * 2.0F;
//            f = f + (random.nextFloat() - random.nextFloat()) * random.nextFloat() * 4.0F;
//
//            if (!flag2 && p_180702_15_ == j && p_180702_12_ > 1.0F && p_180702_16_ > 0)
//            {
//                addTunnel(random.nextLong(), p_180702_3_, p_180702_4_, p_180702_5_, p_180702_6_, p_180702_8_, p_180702_10_, random.nextFloat() * 0.5F + 0.5F, p_180702_13_ - ((float)3.14159265358979323 / 2F), p_180702_14_ / 3.0F, p_180702_15_, p_180702_16_, 1.0);
//                addTunnel(random.nextLong(), p_180702_3_, p_180702_4_, p_180702_5_, p_180702_6_, p_180702_8_, p_180702_10_, random.nextFloat() * 0.5F + 0.5F, p_180702_13_ + ((float)3.14159265358979323 / 2F), p_180702_14_ / 3.0F, p_180702_15_, p_180702_16_, 1.0);
//                return;
//            }
//
//            if (flag2 || random.nextInt(4) != 0)
//            {
//                double d4 = p_180702_6_ - d0;
//                double d5 = p_180702_10_ - d1;
//                double d6 = (double)(p_180702_16_ - p_180702_15_);
//                double d7 = (double)(p_180702_12_ + 2.0F + 16.0F);
//
//                if (d4 * d4 + d5 * d5 - d6 * d6 > d7 * d7)
//                {
//                    return;
//                }
//
//                if (p_180702_6_ >= d0 - 16.0 - d2 * 2.0 && p_180702_10_ >= d1 - 16.0 - d2 * 2.0 && p_180702_6_ <= d0 + 16.0 + d2 * 2.0 && p_180702_10_ <= d1 + 16.0 + d2 * 2.0)
//                {
//                    int k2 = floor(p_180702_6_ - d2) - p_180702_3_ * 16 - 1;
//                    int k = floor(p_180702_6_ + d2) - p_180702_3_ * 16 + 1;
//                    int l2 = floor(p_180702_8_ - d3) - 1;
//                    int l = floor(p_180702_8_ + d3) + 1;
//                    int i3 = floor(p_180702_10_ - d2) - p_180702_4_ * 16 - 1;
//                    int i1 = floor(p_180702_10_ + d2) - p_180702_4_ * 16 + 1;
//
//                    if (k2 < 0)
//                    {
//                        k2 = 0;
//                    }
//
//                    if (k > 16)
//                    {
//                        k = 16;
//                    }
//
//                    if (l2 < 1)
//                    {
//                        l2 = 1;
//                    }
//
//                    if (l > 248)
//                    {
//                        l = 248;
//                    }
//
//                    if (i3 < 0)
//                    {
//                        i3 = 0;
//                    }
//
//                    if (i1 > 16)
//                    {
//                        i1 = 16;
//                    }
//
//                    bool flag3 = false;
//
//                    for (int j1 = k2; !flag3 && j1 < k; ++j1)
//                    {
//                        for (int k1 = i3; !flag3 && k1 < i1; ++k1)
//                        {
//                            for (int l1 = l + 1; !flag3 && l1 >= l2 - 1; --l1)
//                            {
//                                if (l1 >= 0 && l1 < 256)
//                                {
//                                    BlockID iblockstate = p_180702_5_.getBlockState(j1, l1, k1);
//
//                                    if (iblockstate == Blocks.FLOWING_WATER || iblockstate == Blocks.WATER)
//                                    {
//                                        flag3 = true;
//                                    }
//
//                                    if (l1 != l2 - 1 && j1 != k2 && j1 != k - 1 && k1 != i3 && k1 != i1 - 1)
//                                    {
//                                        l1 = l2;
//                                    }
//                                }
//                            }
//                        }
//                    }
//
//                    if (!flag3)
//                    {
//                        BlockPos.MutableBlockPos blockpos$mutableblockpos = new BlockPos.MutableBlockPos();
//
//                        for (int j3 = k2; j3 < k; ++j3)
//                        {
//                            double d10 = ((double)(j3 + p_180702_3_ * 16) + 0.5 - p_180702_6_) / d2;
//
//                            for (int i2 = i3; i2 < i1; ++i2)
//                            {
//                                double d8 = ((double)(i2 + p_180702_4_ * 16) + 0.5 - p_180702_10_) / d2;
//                                bool flag1 = false;
//
//                                if (d10* d10 + d8 * d8 < 1.0)
//                                {
//                                    for (int j2 = l; j2 > l2; --j2)
//                                    {
//                                        double d9 = ((double)(j2 - 1) + 0.5 - p_180702_8_) / d3;
//
//                                        if (d9 > -0.7 && d10 * d10 + d9 * d9 + d8 * d8 < 1.0)
//                                        {
//                                            BlockID iblockstate1 = p_180702_5_->GetBlockUnsafe(j3, j2, i2);
//                                            BlockID iblockstate2 = (BlockID)MoreObjects.firstNonNull(p_180702_5_, BLK_AIR);
//
//                                            if (iblockstate1 == Blocks.GRASS || iblockstate1 == Blocks.MYCELIUM)
//                                            {
//                                                flag1 = true;
//                                            }
//
//                                            if (canReplaceBlock(iblockstate1, iblockstate2))
//                                            {
//                                                if (j2 - 1 < 10)
//                                                {
//                                                    p_180702_5_->SetBlockUnsafe(j3, j2, i2, BLK_LAVA);
//                                                }
//                                                else
//                                                {
//                                                    p_180702_5_->SetBlockUnsafe(j3, j2, i2, BLK_AIR);
//
//                                                    if (flag1 && p_180702_5_.getBlockState(j3, j2 - 1, i2) == Blocks.DIRT)
//                                                    {
//                                                        blockpos$mutableblockpos.setPos(j3 + p_180702_3_ * 16, 0, i2 + p_180702_4_ * 16);
//                                                        p_180702_5_.setBlockState(j3, j2 - 1, i2, world.getBiome(blockpos$mutableblockpos).topBlock.getDefaultState());
//                                                    }
//                                                }
//                                            }
//                                        }
//                                    }
//                                }
//                            }
//                        }
//
//                        if (flag2)
//                        {
//                            break;
//                        }
//                    }
//                }
//            }
//        }
//    }
//
//    bool canReplaceBlock(BlockID p_175793_1_, BlockID p_175793_2_)
//    {
//        if (p_175793_1_ == Blocks.STONE)
//        {
//            return true;
//        }
//        else if (p_175793_1_ == Blocks.DIRT)
//        {
//            return true;
//        }
//        else if (p_175793_1_ == Blocks.GRASS)
//        {
//            return true;
//        }
//        else if (p_175793_1_ == Blocks.TERRACOTTA)
//        {
//            return true;
//        }
//        else if (p_175793_1_ == Blocks.WHITE_TERRACOTTA)
//        {
//            return true;
//        }
//        else if (p_175793_1_ == Blocks.SANDSTONE)
//        {
//            return true;
//        }
//        else if (p_175793_1_ == Blocks.RED_SANDSTONE)
//        {
//            return true;
//        }
//        else if (p_175793_1_ == Blocks.MYCELIUM)
//        {
//            return true;
//        }
//        else if (p_175793_1_ == Blocks.SNOW_BLOCK)
//        {
//            return true;
//        }
//        else
//        {
//            return (p_175793_1_ == Blocks.SAND || p_175793_1_ == Blocks.GRAVEL) && p_175793_2_ != Blocks.WATER;
//        }
//    }
//
//    /**
//     * Recursively called by generate()
//     */
//    void recursiveGenerate(int chunkX, int chunkZ, int originalX, int originalZ, Chunk* chunk) override {
//        int i = rand.nextInt(rand.nextInt(rand.nextInt(15) + 1) + 1);
//
//        if (rand.nextInt(7) != 0)
//        {
//            i = 0;
//        }
//
//        for (int j = 0; j < i; ++j)
//        {
//            double d0 = (double)(chunkX * 16 + rand.nextInt(16));
//            double d1 = (double)rand.nextInt(rand.nextInt(120) + 8);
//            double d2 = (double)(chunkZ * 16 + rand.nextInt(16));
//            int k = 1;
//
//            if (rand.nextInt(4) == 0)
//            {
//                addRoom(rand.nextLong(), originalX, originalZ, chunk, d0, d1, d2);
//                k += rand.nextInt(4);
//            }
//
//            for (int l = 0; l < k; ++l)
//            {
//                float f = rand.nextFloat() * ((float)3.14159265358979323 * 2.0F);
//                float f1 = (rand.nextFloat() - 0.5F) * 2.0F / 8.0F;
//                float f2 = rand.nextFloat() * 2.0F + rand.nextFloat();
//
//                if (rand.nextInt(10) == 0)
//                {
//                    f2 *= rand.nextFloat() * rand.nextFloat() * 3.0F + 1.0F;
//                }
//
//                addTunnel(rand.nextLong(), originalX, originalZ, chunk, d0, d1, d2, f2, f, f1, 0, 0, 1.0);
//            }
//        }
//    }
//};
//
//BlockID MapGenCaves::BLK_LAVA = Blocks.LAVA;
//BlockID MapGenCaves::BLK_AIR = Blocks.AIR;
//BlockID MapGenCaves::BLK_SANDSTONE = Blocks;
//BlockID MapGenCaves::BLK_RED_SANDSTONE = Blocks;