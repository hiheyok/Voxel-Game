#pragma once
#include <string>
#include <exception>
class BiomeProperties {
private:
    std::string biomeName;
    float baseHeight = 0.1F;
    float heightVariation = 0.2F;
    float temperature = 0.5F;
    float rainfall = 0.5F;
    int waterColor = 16777215;
    bool enableSnow;
    bool enableRain = true;
public:
    BiomeProperties& setTemperature(float temperatureIn);

    BiomeProperties& setRainfall(float rainfallIn);

    BiomeProperties& setBaseHeight(float baseHeightIn);

    BiomeProperties& setHeightVariation(float heightVariationIn);

    BiomeProperties& setRainDisabled();

    BiomeProperties& setSnowEnabled();

    BiomeProperties& setWaterColor(int waterColorIn);
};