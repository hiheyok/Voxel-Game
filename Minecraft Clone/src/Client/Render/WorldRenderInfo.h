#pragma once

struct WorldRenderInfo {
	double buildTime = 0.0;

	double buildstage0 = 0.0;
	double buildstage1 = 0.0;
	double buildstage2 = 0.0;

	size_t IsFaceVisibleCalls = 0;
	size_t GetFaceCalls = 0;
	size_t SetFaceCalls = 0;
	size_t GetTextureCalls = 0;
	size_t CompareQuadCalls = 0;

	size_t amountOfMeshGenerated = 1;

	int HorizontalRenderDistance = 16;
	int VerticalRenderDistance = 16;
};