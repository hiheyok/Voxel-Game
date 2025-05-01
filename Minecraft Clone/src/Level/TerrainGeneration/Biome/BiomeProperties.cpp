#include "BiomeProperties.h"

#include "Utils/LogUtils.h"

BiomeProperties& BiomeProperties::setTemperature(float temperatureIn) {
  if (temperatureIn > 0.1F && temperatureIn < 0.2F) {
    g_logger.LogError(
        "BiomeProperties::setTemperature",
        "Please avoid temperatures in the range 0.1 - 0.2 because of snow");

  } else {
    temperature = temperatureIn;
    return *this;
  }
}

BiomeProperties& BiomeProperties::setRainfall(float rainfallIn) {
  rainfall = rainfallIn;
  return *this;
}

BiomeProperties& BiomeProperties::setBaseHeight(float baseHeightIn) {
  baseHeight = baseHeightIn;
  return *this;
}

BiomeProperties& BiomeProperties::setHeightVariation(float heightVariationIn) {
  heightVariation = heightVariationIn;
  return *this;
}

BiomeProperties& BiomeProperties::setRainDisabled() {
  enableRain = false;
  return *this;
}

BiomeProperties& BiomeProperties::setSnowEnabled() {
  enableSnow = true;
  return *this;
}

BiomeProperties& BiomeProperties::setWaterColor(int waterColorIn) {
  waterColor = waterColorIn;
  return *this;
}

BiomeProperties& BiomeProperties::setBaseBiome(std::string baseBiomeIn) {
  baseBiome = baseBiomeIn;
  return *this;
}
