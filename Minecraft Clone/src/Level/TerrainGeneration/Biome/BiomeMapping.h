#pragma once

struct BiomeMapping {
	enum MAJOR_BIOME_TYPE {
		TUNDRA, TAIGA, PLAINS, SHRUBLAND, FOREST, SWAMP, DESERT, SAVANNA, SEASONAL_FOREST, RAIN_FOREST
	};

	enum TEMPERATURE {
		WARM, TEMPERATE, COLD, FREEZING
	};

	enum RAINFALL {
		DRY, LIGHT, HUMID, HEAVY
	};

	RAINFALL getRainfallEnum(float Humidity) {
		if (Humidity < 0.4f) {
			return DRY;
		}
		else if (Humidity < 0.6f) {
			return LIGHT;
		}
		else if (Humidity < 0.7f) {
			return HUMID;
		}
		else {
			return HEAVY;
		}
	}

	TEMPERATURE getTemperatureEnum(float Temperature) {
		if (Temperature < 0.2f) {
			return FREEZING;
		}
		else if (Temperature < 0.3f) {
			return COLD;
		}
		else if (Temperature < 0.7f) {
			return TEMPERATE;
		}
		else {
			return WARM;
		}
	}

	MAJOR_BIOME_TYPE getBiome(float Temperature, float Humidity) {
		TEMPERATURE temp = getTemperatureEnum(Temperature);
		RAINFALL rain = getRainfallEnum(Humidity);

		switch (temp) {
		case FREEZING:
			switch (rain) {
			case DRY:
				return TUNDRA;
			case LIGHT:
				return TUNDRA;
			case HUMID:
				return TUNDRA;
			case HEAVY:
				return TUNDRA;
			}

		case COLD:
			switch (rain) {
			case DRY:
				return PLAINS;
			case LIGHT:
				return TAIGA;
			case HUMID:
				return TAIGA;
			case HEAVY:
				return TAIGA;
			}
		case TEMPERATE:
			switch (rain) {
			case DRY:
				return PLAINS;
			case LIGHT:
				return SHRUBLAND;
			case HUMID:
				return FOREST;
			case HEAVY:
				return SWAMP;
			}
		case WARM:
			switch (rain) {
			case DRY:
				return DESERT;
			case LIGHT:
				return SAVANNA;
			case HUMID:
				return SEASONAL_FOREST;
			case HEAVY:
				return RAIN_FOREST;
			}
		}
	}
};