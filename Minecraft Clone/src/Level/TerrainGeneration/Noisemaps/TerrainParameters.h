#pragma once

struct TerrainParamters {
	float HeightIntensity = 96.f;
	float HeightOffset = -16.f;
};
struct ChunkGeneratorSettings {
	double mainNoiseScaleX = 80.000;
	double mainNoiseScaleY = 160.000;
	double mainNoiseScaleZ = 80.000;
	double depthNoiseScaleX = 200.000;
	double depthNoiseScaleZ = 200.000;
	double depthNoiseExponent = 0.500;
	double baseSize = 8.5;
	double coordinateScale = 684.412;
	double stretchY = 684.412;
	double heightStretch = 12.000;
	double upperLimitScale = 512.00;
	double lowerLimitScale = 512.000;
	double biomeDepthWeight = 1.000;
	double biomeDepthOffset = 0.000;
	double biomeScaleWeight = 1.000;
	double biomeScaleOffset = 0.000;
	int seaLevel = 63;
};