#pragma once
#include "Biome.h"
#include "BiomeType/BiomeOcean.h"
#include "BiomeType/BiomeDesert.h"
#include "BiomeType/BiomeForest.h"
#include "BiomeType/BiomeHills.h"
#include "BiomeType/BiomePlains.h"
#include "BiomeType/BiomeRiver.h"
#include "BiomeType/BiomeSwamp.h"
#include "BiomeType/BiomeTaiga.h"
#include "BiomeType/BiomeHell.h"
#include "BiomeType/BiomeEnd.h"
#include "BiomeType/BiomeSnow.h"
#include "BiomeType/BiomeMushroomIsland.h"
#include "BiomeType/BiomeBeach.h"
#include "BiomeType/BiomeJungle.h"
#include "BiomeType/BiomeStoneBeach.h"
#include "BiomeType/BiomeSavanna.h"
#include "BiomeType/BiomeMesa.h"
#include "BiomeType/BiomeVoid.h"
#include "BiomeType/BiomeForestMutated.h"
#include "BiomeType/BiomeSavannaMutated.h"

class Biomes {
public:
    static Biome* OCEAN;
    static Biome* DEFAULT;
    static Biome* PLAINS;
    static Biome* DESERT;
    static Biome* EXTREME_HILLS;
    static Biome* FOREST;
    static Biome* TAIGA;
    static Biome* SWAMPLAND;
    static Biome* RIVER;
    static Biome* HELL;
    static Biome* SKY;
    static Biome* FROZEN_OCEAN;
    static Biome* FROZEN_RIVER;
    static Biome* ICE_PLAINS;
    static Biome* ICE_MOUNTAINS;
    static Biome* MUSHROOM_ISLAND;
    static Biome* MUSHROOM_ISLAND_SHORE;
    static Biome* BEACH;
    static Biome* DESERT_HILLS;
    static Biome* FOREST_HILLS;
    static Biome* TAIGA_HILLS;
    static Biome* EXTREME_HILLS_EDGE;
    static Biome* JUNGLE;
    static Biome* JUNGLE_HILLS;
    static Biome* JUNGLE_EDGE;
    static Biome* DEEP_OCEAN;
    static Biome* STONE_BEACH;
    static Biome* COLD_BEACH;
    static Biome* BIRCH_FOREST;
    static Biome* BIRCH_FOREST_HILLS;
    static Biome* ROOFED_FOREST;
    static Biome* COLD_TAIGA;
    static Biome* COLD_TAIGA_HILLS;
    static Biome* REDWOOD_TAIGA;
    static Biome* REDWOOD_TAIGA_HILLS;
    static Biome* EXTREME_HILLS_WITH_TREES;
    static Biome* SAVANNA;
    static Biome* SAVANNA_PLATEAU;
    static Biome* MESA;
    static Biome* MESA_ROCK;
    static Biome* MESA_CLEAR_ROCK;
    static Biome* VOID;
    static Biome* MUTATED_PLAINS;
    static Biome* MUTATED_DESERT;
    static Biome* MUTATED_EXTREME_HILLS;
    static Biome* MUTATED_FOREST;
    static Biome* MUTATED_TAIGA;
    static Biome* MUTATED_SWAMPLAND;
    static Biome* MUTATED_ICE_FLATS;
    static Biome* MUTATED_JUNGLE;
    static Biome* MUTATED_JUNGLE_EDGE;
    static Biome* MUTATED_BIRCH_FOREST;
    static Biome* MUTATED_BIRCH_FOREST_HILLS;
    static Biome* MUTATED_ROOFED_FOREST;
    static Biome* MUTATED_TAIGA_COLD;
    static Biome* MUTATED_REDWOOD_TAIGA;
    static Biome* MUTATED_REDWOOD_TAIGA_HILLS;
    static Biome* MUTATED_EXTREME_HILLS_WITH_TREES;
    static Biome* MUTATED_SAVANNA;
    static Biome* MUTATED_SAVANNA_ROCK;
    static Biome* MUTATED_MESA;
    static Biome* MUTATED_MESA_ROCK;
    static Biome* MUTATED_MESA_CLEAR_ROCK;

    static Biome* getRegisteredBiome(std::string name) {
        Biome* biome;

        biome = Biome::REGISTRY.GetValue(ResourceLocation(name).PATH);

        return biome;
    }

	static void RegisterBiome() {
        Biome::Register(0, "ocean", new BiomeOcean(BiomeProperties("Ocean").setBaseHeight(-1.0F).setHeightVariation(0.1F)));
        Biome::Register(1, "plains", new BiomePlains(false, BiomeProperties("Plains").setBaseHeight(0.125F).setHeightVariation(0.05F).setTemperature(0.8F).setRainfall(0.4F)));
        Biome::Register(2, "desert", new BiomeDesert(BiomeProperties("Desert").setBaseHeight(0.125F).setHeightVariation(0.05F).setTemperature(2.0F).setRainfall(0.0F).setRainDisabled()));
        Biome::Register(3, "extreme_hills", new BiomeHills(BiomeHills::Type::NORMAL, BiomeProperties("Extreme Hills").setBaseHeight(1.0F).setHeightVariation(0.5F).setTemperature(0.2F).setRainfall(0.3F)));
        Biome::Register(4, "forest", new BiomeForest(BiomeForest::Type::NORMAL, BiomeProperties("Forest").setTemperature(0.7F).setRainfall(0.8F)));
        Biome::Register(5, "taiga", new BiomeTaiga(BiomeTaiga::Type::NORMAL, BiomeProperties("Taiga").setBaseHeight(0.2F).setHeightVariation(0.2F).setTemperature(0.25F).setRainfall(0.8F)));
        Biome::Register(6, "swampland", new BiomeSwamp(BiomeProperties("Swampland").setBaseHeight(-0.2F).setHeightVariation(0.1F).setTemperature(0.8F).setRainfall(0.9F).setWaterColor(14745518)));
        Biome::Register(7, "river", new BiomeRiver(BiomeProperties("River").setBaseHeight(-0.5F).setHeightVariation(0.0F)));
        Biome::Register(8, "hell", new BiomeHell(BiomeProperties("Hell").setTemperature(2.0F).setRainfall(0.0F).setRainDisabled()));
        Biome::Register(9, "sky", new BiomeEnd(BiomeProperties("The End").setRainDisabled()));
        Biome::Register(10, "frozen_ocean", new BiomeOcean(BiomeProperties("FrozenOcean").setBaseHeight(-1.0F).setHeightVariation(0.1F).setTemperature(0.0F).setRainfall(0.5F).setSnowEnabled()));
        Biome::Register(11, "frozen_river", new BiomeRiver(BiomeProperties("FrozenRiver").setBaseHeight(-0.5F).setHeightVariation(0.0F).setTemperature(0.0F).setRainfall(0.5F).setSnowEnabled()));
        Biome::Register(12, "ice_flats", new BiomeSnow(false, BiomeProperties("Ice Plains").setBaseHeight(0.125F).setHeightVariation(0.05F).setTemperature(0.0F).setRainfall(0.5F).setSnowEnabled()));
        Biome::Register(13, "ice_mountains", new BiomeSnow(false, BiomeProperties("Ice Mountains").setBaseHeight(0.45F).setHeightVariation(0.3F).setTemperature(0.0F).setRainfall(0.5F).setSnowEnabled()));
        Biome::Register(14, "mushroom_island", new BiomeMushroomIsland(BiomeProperties("MushroomIsland").setBaseHeight(0.2F).setHeightVariation(0.3F).setTemperature(0.9F).setRainfall(1.0F)));
        Biome::Register(15, "mushroom_island_shore", new BiomeMushroomIsland(BiomeProperties("MushroomIslandShore").setBaseHeight(0.0F).setHeightVariation(0.025F).setTemperature(0.9F).setRainfall(1.0F)));
        Biome::Register(16, "beaches", new BiomeBeach(BiomeProperties("Beach").setBaseHeight(0.0F).setHeightVariation(0.025F).setTemperature(0.8F).setRainfall(0.4F)));
        Biome::Register(17, "desert_hills", new BiomeDesert(BiomeProperties("DesertHills").setBaseHeight(0.45F).setHeightVariation(0.3F).setTemperature(2.0F).setRainfall(0.0F).setRainDisabled()));
        Biome::Register(18, "forest_hills", new BiomeForest(BiomeForest::Type::NORMAL, BiomeProperties("ForestHills").setBaseHeight(0.45F).setHeightVariation(0.3F).setTemperature(0.7F).setRainfall(0.8F)));
        Biome::Register(19, "taiga_hills", new BiomeTaiga(BiomeTaiga::Type::NORMAL, BiomeProperties("TaigaHills").setTemperature(0.25F).setRainfall(0.8F).setBaseHeight(0.45F).setHeightVariation(0.3F)));
        Biome::Register(20, "smaller_extreme_hills", new BiomeHills(BiomeHills::Type::EXTRA_TREES, BiomeProperties("Extreme Hills Edge").setBaseHeight(0.8F).setHeightVariation(0.3F).setTemperature(0.2F).setRainfall(0.3F)));
        Biome::Register(21, "jungle", new BiomeJungle(false, BiomeProperties("Jungle").setTemperature(0.95F).setRainfall(0.9F)));
        Biome::Register(22, "jungle_hills", new BiomeJungle(false, BiomeProperties("JungleHills").setBaseHeight(0.45F).setHeightVariation(0.3F).setTemperature(0.95F).setRainfall(0.9F)));
        Biome::Register(23, "jungle_edge", new BiomeJungle(true, BiomeProperties("JungleEdge").setTemperature(0.95F).setRainfall(0.8F)));
        Biome::Register(24, "deep_ocean", new BiomeOcean(BiomeProperties("Deep Ocean").setBaseHeight(-1.8F).setHeightVariation(0.1F)));
        Biome::Register(25, "stone_beach", new BiomeStoneBeach(BiomeProperties("Stone Beach").setBaseHeight(0.1F).setHeightVariation(0.8F).setTemperature(0.2F).setRainfall(0.3F)));
        Biome::Register(26, "cold_beach", new BiomeBeach(BiomeProperties("Cold Beach").setBaseHeight(0.0F).setHeightVariation(0.025F).setTemperature(0.05F).setRainfall(0.3F).setSnowEnabled()));
        Biome::Register(27, "birch_forest", new BiomeForest(BiomeForest::Type::BIRCH, BiomeProperties("Birch Forest").setTemperature(0.6F).setRainfall(0.6F)));
        Biome::Register(28, "birch_forest_hills", new BiomeForest(BiomeForest::Type::BIRCH, BiomeProperties("Birch Forest Hills").setBaseHeight(0.45F).setHeightVariation(0.3F).setTemperature(0.6F).setRainfall(0.6F)));
        Biome::Register(29, "roofed_forest", new BiomeForest(BiomeForest::Type::ROOFED, BiomeProperties("Roofed Forest").setTemperature(0.7F).setRainfall(0.8F)));
        Biome::Register(30, "taiga_cold", new BiomeTaiga(BiomeTaiga::Type::NORMAL, BiomeProperties("Cold Taiga").setBaseHeight(0.2F).setHeightVariation(0.2F).setTemperature(-0.5F).setRainfall(0.4F).setSnowEnabled()));
        Biome::Register(31, "taiga_cold_hills", new BiomeTaiga(BiomeTaiga::Type::NORMAL, BiomeProperties("Cold Taiga Hills").setBaseHeight(0.45F).setHeightVariation(0.3F).setTemperature(-0.5F).setRainfall(0.4F).setSnowEnabled()));
        Biome::Register(32, "redwood_taiga", new BiomeTaiga(BiomeTaiga::Type::MEGA, BiomeProperties("Mega Taiga").setTemperature(0.3F).setRainfall(0.8F).setBaseHeight(0.2F).setHeightVariation(0.2F)));
        Biome::Register(33, "redwood_taiga_hills", new BiomeTaiga(BiomeTaiga::Type::MEGA, BiomeProperties("Mega Taiga Hills").setBaseHeight(0.45F).setHeightVariation(0.3F).setTemperature(0.3F).setRainfall(0.8F)));
        Biome::Register(34, "extreme_hills_with_trees", new BiomeHills(BiomeHills::Type::EXTRA_TREES, BiomeProperties("Extreme Hills+").setBaseHeight(1.0F).setHeightVariation(0.5F).setTemperature(0.2F).setRainfall(0.3F)));
        Biome::Register(35, "savanna", new BiomeSavanna(BiomeProperties("Savanna").setBaseHeight(0.125F).setHeightVariation(0.05F).setTemperature(1.2F).setRainfall(0.0F).setRainDisabled()));
        Biome::Register(36, "savanna_rock", new BiomeSavanna(BiomeProperties("Savanna Plateau").setBaseHeight(1.5F).setHeightVariation(0.025F).setTemperature(1.0F).setRainfall(0.0F).setRainDisabled()));
        Biome::Register(37, "mesa", new BiomeMesa(false, false, BiomeProperties("Mesa").setTemperature(2.0F).setRainfall(0.0F).setRainDisabled()));
        Biome::Register(38, "mesa_rock", new BiomeMesa(false, true, BiomeProperties("Mesa Plateau F").setBaseHeight(1.5F).setHeightVariation(0.025F).setTemperature(2.0F).setRainfall(0.0F).setRainDisabled()));
        Biome::Register(39, "mesa_clear_rock", new BiomeMesa(false, false, BiomeProperties("Mesa Plateau").setBaseHeight(1.5F).setHeightVariation(0.025F).setTemperature(2.0F).setRainfall(0.0F).setRainDisabled()));
        Biome::Register(127, "void", new BiomeVoid(BiomeProperties("The Void").setRainDisabled()));
        Biome::Register(129, "mutated_plains", new BiomePlains(true, BiomeProperties("Sunflower Plains").setBaseBiome("plains").setBaseHeight(0.125F).setHeightVariation(0.05F).setTemperature(0.8F).setRainfall(0.4F)));
        Biome::Register(130, "mutated_desert", new BiomeDesert(BiomeProperties("Desert M").setBaseBiome("desert").setBaseHeight(0.225F).setHeightVariation(0.25F).setTemperature(2.0F).setRainfall(0.0F).setRainDisabled()));
        Biome::Register(131, "mutated_extreme_hills", new BiomeHills(BiomeHills::Type::MUTATED, BiomeProperties("Extreme Hills M").setBaseBiome("extreme_hills").setBaseHeight(1.0F).setHeightVariation(0.5F).setTemperature(0.2F).setRainfall(0.3F)));
        Biome::Register(132, "mutated_forest", new BiomeForest(BiomeForest::Type::FLOWER, BiomeProperties("Flower Forest").setBaseBiome("forest").setHeightVariation(0.4F).setTemperature(0.7F).setRainfall(0.8F)));
        Biome::Register(133, "mutated_taiga", new BiomeTaiga(BiomeTaiga::Type::NORMAL, BiomeProperties("Taiga M").setBaseBiome("taiga").setBaseHeight(0.3F).setHeightVariation(0.4F).setTemperature(0.25F).setRainfall(0.8F)));
        Biome::Register(134, "mutated_swampland", new BiomeSwamp(BiomeProperties("Swampland M").setBaseBiome("swampland").setBaseHeight(-0.1F).setHeightVariation(0.3F).setTemperature(0.8F).setRainfall(0.9F).setWaterColor(14745518)));
        Biome::Register(140, "mutated_ice_flats", new BiomeSnow(true, BiomeProperties("Ice Plains Spikes").setBaseBiome("ice_flats").setBaseHeight(0.425F).setHeightVariation(0.45000002F).setTemperature(0.0F).setRainfall(0.5F).setSnowEnabled()));
        Biome::Register(149, "mutated_jungle", new BiomeJungle(false, BiomeProperties("Jungle M").setBaseBiome("jungle").setBaseHeight(0.2F).setHeightVariation(0.4F).setTemperature(0.95F).setRainfall(0.9F)));
        Biome::Register(151, "mutated_jungle_edge", new BiomeJungle(true, BiomeProperties("JungleEdge M").setBaseBiome("jungle_edge").setBaseHeight(0.2F).setHeightVariation(0.4F).setTemperature(0.95F).setRainfall(0.8F)));
        Biome::Register(155, "mutated_birch_forest", new BiomeForestMutated(BiomeProperties("Birch Forest M").setBaseBiome("birch_forest").setBaseHeight(0.2F).setHeightVariation(0.4F).setTemperature(0.6F).setRainfall(0.6F)));
        Biome::Register(156, "mutated_birch_forest_hills", new BiomeForestMutated(BiomeProperties("Birch Forest Hills M").setBaseBiome("birch_forest_hills").setBaseHeight(0.55F).setHeightVariation(0.5F).setTemperature(0.6F).setRainfall(0.6F)));
        Biome::Register(157, "mutated_roofed_forest", new BiomeForest(BiomeForest::Type::ROOFED, BiomeProperties("Roofed Forest M").setBaseBiome("roofed_forest").setBaseHeight(0.2F).setHeightVariation(0.4F).setTemperature(0.7F).setRainfall(0.8F)));
        Biome::Register(158, "mutated_taiga_cold", new BiomeTaiga(BiomeTaiga::Type::NORMAL, BiomeProperties("Cold Taiga M").setBaseBiome("taiga_cold").setBaseHeight(0.3F).setHeightVariation(0.4F).setTemperature(-0.5F).setRainfall(0.4F).setSnowEnabled()));
        Biome::Register(160, "mutated_redwood_taiga", new BiomeTaiga(BiomeTaiga::Type::MEGA_SPRUCE, BiomeProperties("Mega Spruce Taiga").setBaseBiome("redwood_taiga").setBaseHeight(0.2F).setHeightVariation(0.2F).setTemperature(0.25F).setRainfall(0.8F)));
        Biome::Register(161, "mutated_redwood_taiga_hills", new BiomeTaiga(BiomeTaiga::Type::MEGA_SPRUCE, BiomeProperties("Redwood Taiga Hills M").setBaseBiome("redwood_taiga_hills").setBaseHeight(0.2F).setHeightVariation(0.2F).setTemperature(0.25F).setRainfall(0.8F)));
        Biome::Register(162, "mutated_extreme_hills_with_trees", new BiomeHills(BiomeHills::Type::MUTATED, BiomeProperties("Extreme Hills+ M").setBaseBiome("extreme_hills_with_trees").setBaseHeight(1.0F).setHeightVariation(0.5F).setTemperature(0.2F).setRainfall(0.3F)));
        Biome::Register(163, "mutated_savanna", new BiomeSavannaMutated(BiomeProperties("Savanna M").setBaseBiome("savanna").setBaseHeight(0.3625F).setHeightVariation(1.225F).setTemperature(1.1F).setRainfall(0.0F).setRainDisabled()));
        Biome::Register(164, "mutated_savanna_rock", new BiomeSavannaMutated(BiomeProperties("Savanna Plateau M").setBaseBiome("savanna_rock").setBaseHeight(1.05F).setHeightVariation(1.2125001F).setTemperature(1.0F).setRainfall(0.0F).setRainDisabled()));
        Biome::Register(165, "mutated_mesa", new BiomeMesa(true, false, BiomeProperties("Mesa (Bryce)").setBaseBiome("mesa").setTemperature(2.0F).setRainfall(0.0F).setRainDisabled()));
        Biome::Register(166, "mutated_mesa_rock", new BiomeMesa(false, true, BiomeProperties("Mesa Plateau F M").setBaseBiome("mesa_rock").setBaseHeight(0.45F).setHeightVariation(0.3F).setTemperature(2.0F).setRainfall(0.0F).setRainDisabled()));
        Biome::Register(167, "mutated_mesa_clear_rock", new BiomeMesa(false, false, BiomeProperties("Mesa Plateau M").setBaseBiome("mesa_clear_rock").setBaseHeight(0.45F).setHeightVariation(0.3F).setTemperature(2.0F).setRainfall(0.0F).setRainDisabled()));
	
        Biomes::OCEAN = getRegisteredBiome("ocean");
        Biomes::DEFAULT = OCEAN;
        Biomes::PLAINS = getRegisteredBiome("plains");
        Biomes::DESERT = getRegisteredBiome("desert");
        Biomes::EXTREME_HILLS = getRegisteredBiome("extreme_hills");
        Biomes::FOREST = getRegisteredBiome("forest");
        Biomes::TAIGA = getRegisteredBiome("taiga");
        Biomes::SWAMPLAND = getRegisteredBiome("swampland");
        Biomes::RIVER = getRegisteredBiome("river");
        Biomes::HELL = getRegisteredBiome("hell");
        Biomes::SKY = getRegisteredBiome("sky");
        Biomes::FROZEN_OCEAN = getRegisteredBiome("frozen_ocean");
        Biomes::FROZEN_RIVER = getRegisteredBiome("frozen_river");
        Biomes::ICE_PLAINS = getRegisteredBiome("ice_flats");
        Biomes::ICE_MOUNTAINS = getRegisteredBiome("ice_mountains");
        Biomes::MUSHROOM_ISLAND = getRegisteredBiome("mushroom_island");
        Biomes::MUSHROOM_ISLAND_SHORE = getRegisteredBiome("mushroom_island_shore");
        Biomes::BEACH = getRegisteredBiome("beaches");
        Biomes::DESERT_HILLS = getRegisteredBiome("desert_hills");
        Biomes::FOREST_HILLS = getRegisteredBiome("forest_hills");
        Biomes::TAIGA_HILLS = getRegisteredBiome("taiga_hills");
        Biomes::EXTREME_HILLS_EDGE = getRegisteredBiome("smaller_extreme_hills");
        Biomes::JUNGLE = getRegisteredBiome("jungle");
        Biomes::JUNGLE_HILLS = getRegisteredBiome("jungle_hills");
        Biomes::JUNGLE_EDGE = getRegisteredBiome("jungle_edge");
        Biomes::DEEP_OCEAN = getRegisteredBiome("deep_ocean");
        Biomes::STONE_BEACH = getRegisteredBiome("stone_beach");
        Biomes::COLD_BEACH = getRegisteredBiome("cold_beach");
        Biomes::BIRCH_FOREST = getRegisteredBiome("birch_forest");
        Biomes::BIRCH_FOREST_HILLS = getRegisteredBiome("birch_forest_hills");
        Biomes::ROOFED_FOREST = getRegisteredBiome("roofed_forest");
        Biomes::COLD_TAIGA = getRegisteredBiome("taiga_cold");
        Biomes::COLD_TAIGA_HILLS = getRegisteredBiome("taiga_cold_hills");
        Biomes::REDWOOD_TAIGA = getRegisteredBiome("redwood_taiga");
        Biomes::REDWOOD_TAIGA_HILLS = getRegisteredBiome("redwood_taiga_hills");
        Biomes::EXTREME_HILLS_WITH_TREES = getRegisteredBiome("extreme_hills_with_trees");
        Biomes::SAVANNA = getRegisteredBiome("savanna");
        Biomes::SAVANNA_PLATEAU = getRegisteredBiome("savanna_rock");
        Biomes::MESA = getRegisteredBiome("mesa");
        Biomes::MESA_ROCK = getRegisteredBiome("mesa_rock");
        Biomes::MESA_CLEAR_ROCK = getRegisteredBiome("mesa_clear_rock");
        Biomes::VOID = getRegisteredBiome("void");
        Biomes::MUTATED_PLAINS = getRegisteredBiome("mutated_plains");
        Biomes::MUTATED_DESERT = getRegisteredBiome("mutated_desert");
        Biomes::MUTATED_EXTREME_HILLS = getRegisteredBiome("mutated_extreme_hills");
        Biomes::MUTATED_FOREST = getRegisteredBiome("mutated_forest");
        Biomes::MUTATED_TAIGA = getRegisteredBiome("mutated_taiga");
        Biomes::MUTATED_SWAMPLAND = getRegisteredBiome("mutated_swampland");
        Biomes::MUTATED_ICE_FLATS = getRegisteredBiome("mutated_ice_flats");
        Biomes::MUTATED_JUNGLE = getRegisteredBiome("mutated_jungle");
        Biomes::MUTATED_JUNGLE_EDGE = getRegisteredBiome("mutated_jungle_edge");
        Biomes::MUTATED_BIRCH_FOREST = getRegisteredBiome("mutated_birch_forest");
        Biomes::MUTATED_BIRCH_FOREST_HILLS = getRegisteredBiome("mutated_birch_forest_hills");
        Biomes::MUTATED_ROOFED_FOREST = getRegisteredBiome("mutated_roofed_forest");
        Biomes::MUTATED_TAIGA_COLD = getRegisteredBiome("mutated_taiga_cold");
        Biomes::MUTATED_REDWOOD_TAIGA = getRegisteredBiome("mutated_redwood_taiga");
        Biomes::MUTATED_REDWOOD_TAIGA_HILLS = getRegisteredBiome("mutated_redwood_taiga_hills");
        Biomes::MUTATED_EXTREME_HILLS_WITH_TREES = getRegisteredBiome("mutated_extreme_hills_with_trees");
        Biomes::MUTATED_SAVANNA = getRegisteredBiome("mutated_savanna");
        Biomes::MUTATED_SAVANNA_ROCK = getRegisteredBiome("mutated_savanna_rock");
        Biomes::MUTATED_MESA = getRegisteredBiome("mutated_mesa");
        Biomes::MUTATED_MESA_ROCK = getRegisteredBiome("mutated_mesa_rock");
        Biomes::MUTATED_MESA_CLEAR_ROCK = getRegisteredBiome("mutated_mesa_clear_rock");
    }
};

_declspec(selectany) Biome* Biomes::OCEAN = nullptr;
_declspec(selectany) Biome* Biomes::DEFAULT = nullptr;
_declspec(selectany) Biome* Biomes::PLAINS = nullptr;
_declspec(selectany) Biome* Biomes::DESERT = nullptr;
_declspec(selectany) Biome* Biomes::EXTREME_HILLS = nullptr;
_declspec(selectany) Biome* Biomes::FOREST = nullptr;
_declspec(selectany) Biome* Biomes::TAIGA = nullptr;
_declspec(selectany) Biome* Biomes::SWAMPLAND = nullptr;
_declspec(selectany) Biome* Biomes::RIVER = nullptr;
_declspec(selectany) Biome* Biomes::HELL = nullptr;
_declspec(selectany) Biome* Biomes::SKY = nullptr;
_declspec(selectany) Biome* Biomes::FROZEN_OCEAN = nullptr;
_declspec(selectany) Biome* Biomes::FROZEN_RIVER = nullptr;
_declspec(selectany) Biome* Biomes::ICE_PLAINS = nullptr;
_declspec(selectany) Biome* Biomes::ICE_MOUNTAINS = nullptr;
_declspec(selectany) Biome* Biomes::MUSHROOM_ISLAND = nullptr;
_declspec(selectany) Biome* Biomes::MUSHROOM_ISLAND_SHORE = nullptr;
_declspec(selectany) Biome* Biomes::BEACH = nullptr;
_declspec(selectany) Biome* Biomes::DESERT_HILLS = nullptr;
_declspec(selectany) Biome* Biomes::FOREST_HILLS = nullptr;
_declspec(selectany) Biome* Biomes::TAIGA_HILLS = nullptr;
_declspec(selectany) Biome* Biomes::EXTREME_HILLS_EDGE = nullptr;
_declspec(selectany) Biome* Biomes::JUNGLE = nullptr;
_declspec(selectany) Biome* Biomes::JUNGLE_HILLS = nullptr;
_declspec(selectany) Biome* Biomes::JUNGLE_EDGE = nullptr;
_declspec(selectany) Biome* Biomes::DEEP_OCEAN = nullptr;
_declspec(selectany) Biome* Biomes::STONE_BEACH = nullptr;
_declspec(selectany) Biome* Biomes::COLD_BEACH = nullptr;
_declspec(selectany) Biome* Biomes::BIRCH_FOREST = nullptr;
_declspec(selectany) Biome* Biomes::BIRCH_FOREST_HILLS = nullptr;
_declspec(selectany) Biome* Biomes::ROOFED_FOREST = nullptr;
_declspec(selectany) Biome* Biomes::COLD_TAIGA = nullptr;
_declspec(selectany) Biome* Biomes::COLD_TAIGA_HILLS = nullptr;
_declspec(selectany) Biome* Biomes::REDWOOD_TAIGA = nullptr;
_declspec(selectany) Biome* Biomes::REDWOOD_TAIGA_HILLS = nullptr;
_declspec(selectany) Biome* Biomes::EXTREME_HILLS_WITH_TREES = nullptr;
_declspec(selectany) Biome* Biomes::SAVANNA = nullptr;
_declspec(selectany) Biome* Biomes::SAVANNA_PLATEAU = nullptr;
_declspec(selectany) Biome* Biomes::MESA = nullptr;
_declspec(selectany) Biome* Biomes::MESA_ROCK = nullptr;
_declspec(selectany) Biome* Biomes::MESA_CLEAR_ROCK = nullptr;
_declspec(selectany) Biome* Biomes::VOID = nullptr;
_declspec(selectany) Biome* Biomes::MUTATED_PLAINS = nullptr;
_declspec(selectany) Biome* Biomes::MUTATED_DESERT = nullptr;
_declspec(selectany) Biome* Biomes::MUTATED_EXTREME_HILLS = nullptr;
_declspec(selectany) Biome* Biomes::MUTATED_FOREST = nullptr;
_declspec(selectany) Biome* Biomes::MUTATED_TAIGA = nullptr;
_declspec(selectany) Biome* Biomes::MUTATED_SWAMPLAND = nullptr;
_declspec(selectany) Biome* Biomes::MUTATED_ICE_FLATS = nullptr;
_declspec(selectany) Biome* Biomes::MUTATED_JUNGLE = nullptr;
_declspec(selectany) Biome* Biomes::MUTATED_JUNGLE_EDGE = nullptr;
_declspec(selectany) Biome* Biomes::MUTATED_BIRCH_FOREST = nullptr;
_declspec(selectany) Biome* Biomes::MUTATED_BIRCH_FOREST_HILLS = nullptr;
_declspec(selectany) Biome* Biomes::MUTATED_ROOFED_FOREST = nullptr;
_declspec(selectany) Biome* Biomes::MUTATED_TAIGA_COLD = nullptr;
_declspec(selectany) Biome* Biomes::MUTATED_REDWOOD_TAIGA = nullptr;
_declspec(selectany) Biome* Biomes::MUTATED_REDWOOD_TAIGA_HILLS = nullptr;
_declspec(selectany) Biome* Biomes::MUTATED_EXTREME_HILLS_WITH_TREES = nullptr;
_declspec(selectany) Biome* Biomes::MUTATED_SAVANNA = nullptr;
_declspec(selectany) Biome* Biomes::MUTATED_SAVANNA_ROCK = nullptr;
_declspec(selectany) Biome* Biomes::MUTATED_MESA = nullptr;
_declspec(selectany) Biome* Biomes::MUTATED_MESA_ROCK = nullptr;
_declspec(selectany) Biome* Biomes::MUTATED_MESA_CLEAR_ROCK = nullptr;