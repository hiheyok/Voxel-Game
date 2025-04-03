#pragma once
#include <string>
class BiomeProperties {
private:
    
public:
    std::string biomeName = "";
    std::string baseBiome = "";
    float baseHeight = 0.1F;
    float heightVariation = 0.2F;
    float temperature = 0.5F;
    float rainfall = 0.5F;
    int waterColor = 16777215;
    bool enableSnow = false;
    bool enableRain = true;

    BiomeProperties(std::string biomeNameIn) : biomeName(biomeNameIn) {}

    BiomeProperties() {}

    BiomeProperties& setTemperature(float temperatureIn);

    BiomeProperties& setRainfall(float rainfallIn);

    BiomeProperties& setBaseHeight(float baseHeightIn);

    BiomeProperties& setHeightVariation(float heightVariationIn);

    BiomeProperties& setRainDisabled();

    BiomeProperties& setSnowEnabled();

    BiomeProperties& setWaterColor(int waterColorIn);

    BiomeProperties& setBaseBiome(std::string baseBiomeIn);
};