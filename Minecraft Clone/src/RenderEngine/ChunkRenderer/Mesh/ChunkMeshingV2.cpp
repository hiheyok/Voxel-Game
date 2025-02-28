#include "ChunkMeshingV2.h"
#include <immintrin.h>
#include "../../../Level/Chunk/Block/Blocks.h"
#include <bit> 
#include <cstdint>
#include <intrin.h>
using namespace std;
using namespace glm;

constexpr int POSITION_OFFSET = 16;

constexpr int BitShiftAmount = 9; //27 bits
constexpr int NormBitOffset = 27; //2 bits
constexpr int tintBitOffset = 29; // 1 bit

constexpr int textureBitOffset = 10; // 18 bits
constexpr int blockShadingBitOffset = 28; // 4 bits

#define PROFILE_DEBUG

void MeshingV2::ChunkMeshData::reset() {
	if (VerticesBuffer.size() != BUFFER_SIZE_STEP) {
		VerticesBuffer.resize(BUFFER_SIZE_STEP, 0);
	}

	if (TransparentVerticesBuffer.size() != BUFFER_SIZE_STEP) {
		TransparentVerticesBuffer.resize(BUFFER_SIZE_STEP, 0);
	}

	transparentFaceCount = 0;
	solidFaceCount = 0;

	memset(ChunkCache, NULL, 18 * 18 * 18 * sizeof(BlockID));
}

void MeshingV2::ChunkMeshData::GenerateCache() {
	int pos[3]{ 0, 0, 0 };
	int localPos[3]{ 0, 0, 0 };

	const BlockID* container = chunk->BlockStorage.getContainerData();
	for (int x = 0; x < 16; x++) {
		for (int z = 0; z < 16; z++) {
			memcpy(ChunkCache + (x + 1) * 18 * 18 + (z + 1) * 18 + (1), container + (x << 8) + (z << 4), 16 * sizeof(BlockID));
		}
	}

	for (int side = 0; side < 6; side++) {
		int axis = side >> 1;
		int direction = side & 0b1;

		if (chunk->Neighbors[side] == nullptr) {
			continue;
		}

		for (int u = 0; u < 16; u++) {
			for (int v = 0; v < 16; v++) {

				pos[axis] = direction * 15;
				pos[(axis + 1) % 3] = u;
				pos[(axis + 2) % 3] = v;
				
				localPos[axis] = 17 - 17 * direction;
				localPos[(axis + 1) % 3] = u + 1;
				localPos[(axis + 2) % 3] = v + 1;

				ChunkCache[localPos[0] * 18 * 18 + localPos[2] * 18 + localPos[1]] = chunk->Neighbors[side]->GetBlockUnsafe(pos[0], pos[1], pos[2]);
			}
		}
	}
}

void MeshingV2::ChunkMeshData::setChunk(Chunk* pChunk) {
	chunk = pChunk;
}

void MeshingV2::ChunkMeshData::GenerateMesh() {
	//Initialize
	if (chunk == nullptr) {
		return;
	}

	chunk->Use();
	GenerateCache();
	chunk->Unuse();
	
	GenerateFaceCollection();
}

//Loops through all the blocks in the chunk and check if each block side is visible. If a block side is visible, it generates the quad and puts it in the cache
void MeshingV2::ChunkMeshData::GenerateFaceCollection() {
	std::vector<uint8_t> FaceVisibilityBack(4096, 0b00);
	std::vector<uint8_t> FaceVisibility(4096, 0b00);
	std::vector<uint8_t> usedBlock(16 * 16, 0b00);

	for (int Axis = 0; Axis < 3; Axis++) {
		int AxisU = (Axis + 2) % 3;
		int AxisV = (Axis + 1) % 3;
		int pos[3]{ 0,0,0 };

		for (pos[Axis] = 0; pos[Axis] < 17; ++pos[Axis]) {//Slice
			memset(usedBlock.data(), 0x00, 16 * 16);
			for (pos[AxisU] = 0; pos[AxisU] < 16; ++pos[AxisU]) {
				for (pos[AxisV] = 0; pos[AxisV] < 16; ++pos[AxisV]) {

					if (usedBlock[(pos[AxisU] << 4) + pos[AxisV]] != 0x00) {
						pos[AxisV] += usedBlock[(pos[AxisU] << 4) + pos[AxisV]] - 1; 
						continue;
					}

					usedBlock[(pos[AxisU] << 4) + pos[AxisV]] = 0xFF;

					const BlockID& currBlock = getCachedBlockID(pos[0], pos[1], pos[2]);
					--pos[Axis];
					const BlockID& backBlock = getCachedBlockID(pos[0], pos[1], pos[2]);
					++pos[Axis];

					const ModelV2::BlockModelV2& currModel = Blocks.getBlockModelDereferenced(currBlock);
					const ModelV2::BlockModelV2& backModel = Blocks.getBlockModelDereferenced(backBlock);

					bool blankCurrModel = !currModel.isInitialized || pos[Axis] == 16;
					bool blankBackModel = !backModel.isInitialized || pos[Axis] == 0;

					//Check if it is visible from the back and front
					if (!blankCurrModel) {
						for (int i = 0; i < currModel.Elements.size(); ++i) {
							FaceVisibility[i] = 0;
							const Cuboid& element = currModel.Elements[i];

							if (element.Faces[Axis * 2 + 1].ReferenceTexture.length() == 0)
								continue;

							if (element.Faces[Axis * 2 + 1].CullFace != -1) {
								if (!IsFaceVisible(element, pos[0], pos[1], pos[2], element.Faces[Axis * 2 + 1].CullFace)) continue;
							}

							FaceVisibility[i] |= 0b1;
						}

					}
					//Check if it is visible from the back and front
					--pos[Axis];
					if (!blankBackModel) {
						for (int i = 0; i < backModel.Elements.size(); ++i) {
							FaceVisibilityBack[i] = 0;
							const Cuboid& element = backModel.Elements[i];

							if (element.Faces[Axis * 2].ReferenceTexture.length() == 0)
								continue;

							if (element.Faces[Axis * 2].CullFace != -1) {
								if (!IsFaceVisible(element, pos[0], pos[1], pos[2], element.Faces[Axis * 2].CullFace)) continue;
							}

							FaceVisibilityBack[i] |= 0b1;
						}
					}
					++pos[Axis];

					//Spread

					int uLength = 1;
					int vLength = 1;

					int qPos[3]{ pos[0], pos[1], pos[2] };

					for (qPos[AxisV] = pos[AxisV] + 1; qPos[AxisV] < 16; ++qPos[AxisV]) {
						//Check if they are the same
						const BlockID& currBlock2 = getCachedBlockID(qPos[0], qPos[1], qPos[2]);
						--qPos[Axis];
						const BlockID& backBlock2 = getCachedBlockID(qPos[0], qPos[1], qPos[2]);
						++qPos[Axis];

						if (currBlock2 != currBlock || backBlock2 != backBlock)
							break;

						usedBlock[(qPos[AxisU] << 4) + qPos[AxisV]] = 0xFFU;
						vLength++;
					}

					//memset(usedBlock.data() + (pos[AxisU] << 4) + pos[AxisV], vLength, vLength);

					qPos[0] = pos[0];
					qPos[1] = pos[1];
					qPos[2] = pos[2];

					for (qPos[AxisU] = pos[AxisU] + 1; qPos[AxisU] < 16; ++qPos[AxisU]) {
						bool isValid = true;

						for (qPos[AxisV] = pos[AxisV]; qPos[AxisV] < pos[AxisV] + vLength; ++qPos[AxisV]) {
							const BlockID& currBlock2 = getCachedBlockID(qPos[0], qPos[1], qPos[2]);
							--qPos[Axis];
							const BlockID& backBlock2 = getCachedBlockID(qPos[0], qPos[1], qPos[2]);
							++qPos[Axis];

							if (currBlock2 != currBlock || backBlock2 != backBlock) {
								isValid = false;
								break;
							}
						}

						if (!isValid) {
							break;
						}

						memset(usedBlock.data() + (qPos[AxisU] << 4) + pos[AxisV], vLength, vLength);

						++uLength;
					}

					qPos[0] = pos[0];
					qPos[1] = pos[1];
					qPos[2] = pos[2];

					//Memorize & Add Faces
					if (!blankCurrModel) {
						for (int i = 0; i < currModel.Elements.size(); i++) {
							if (FaceVisibility[i] != 1) continue;
							const Cuboid& element = currModel.Elements[i];
							for (qPos[AxisU] = pos[AxisU]; qPos[AxisU] < pos[AxisU] + uLength; ++qPos[AxisU]) {
								for (qPos[AxisV] = pos[AxisV]; qPos[AxisV] < pos[AxisV] + vLength; ++qPos[AxisV]) {
									AddFacetoMesh(element.Faces[Axis * 2 + 1], Axis * 2 + 1, element.From, element.To, currModel.AmbientOcclusion, qPos[0], qPos[1], qPos[2]);
								}
							}
						}
					}
					--qPos[Axis];
					if (!blankBackModel) {
						for (int i = 0; i < backModel.Elements.size(); i++) {
							if (FaceVisibilityBack[i] != 1) continue;
							const Cuboid& element = backModel.Elements[i];
							for (qPos[AxisU] = pos[AxisU]; qPos[AxisU] < pos[AxisU] + uLength; ++qPos[AxisU]) {
								for (qPos[AxisV] = pos[AxisV]; qPos[AxisV] < pos[AxisV] + vLength; ++qPos[AxisV]) {
									AddFacetoMesh(element.Faces[Axis * 2], Axis * 2, element.From, element.To, backModel.AmbientOcclusion, qPos[0], qPos[1], qPos[2]);
								}
							}
						}
					}

					qPos[0] = pos[0];
					qPos[1] = pos[1];
					qPos[2] = pos[2];

					if (vLength == 16 && uLength == 16) { //Skip entire layer
						pos[AxisV] = 15;
						pos[AxisU] = 15;
					}
					else {
						pos[AxisV] += vLength - 1;
					}
				}
			}
		}
	}
}

inline void MeshingV2::ChunkMeshData::AddFacetoMesh(const BlockFace& face, uint8_t axis, glm::ivec3 From, glm::ivec3 To, bool allowAO, int x, int y, int z) {
	uint8_t NN = 15, PN = 15, PP = 15, NP = 15;
	uint8_t direction = axis & 0b1;
	uint8_t facing = axis >> 1;

	x *= 16;
	y *= 16;
	z *= 16;

	glm::ivec3 P0{ x + From[0] + POSITION_OFFSET, y + From[1] + POSITION_OFFSET, z + From[2] + POSITION_OFFSET };
	glm::ivec3 P1{ x + To[0] + POSITION_OFFSET, y + To[1] + POSITION_OFFSET, z + To[2] + POSITION_OFFSET };

	glm::ivec2 TexNN = face.UVCoordNN;
	glm::ivec2 TexPN = face.UVCoordNP;
	glm::ivec2 TexPP = face.UVCoordPP;
	glm::ivec2 TexNP = face.UVCoordPN;

	TexNN.y <<= 5;
	TexNP.y <<= 5;
	TexPP.y <<= 5;
	TexPN.y <<= 5;

	P0.x <<= 0;
	P1.x <<= 0;
	P0.y <<= BitShiftAmount;
	P1.y <<= BitShiftAmount;
	P0.z <<= BitShiftAmount * 2;
	P1.z <<= BitShiftAmount * 2;

	glm::ivec3 tP0 = P0;
	glm::ivec3 tP1 = P1;

	P0.x = tP0[facing % 3];
	P1.x = tP1[facing % 3];
	P0.y = tP0[(facing + 1) % 3];
	P1.y = tP1[(facing + 1) % 3];
	P0.z = tP0[(facing + 2) % 3];
	P1.z = tP1[(facing + 2) % 3];

	if (direction == 0)
		swap(P0.x, P1.x);
	if (facing == 2)
		swap(TexPN, TexNP);

	uint32_t tex = face.TextureID << textureBitOffset;
	uint32_t Norm = facing << NormBitOffset;
	uint32_t tint = (static_cast<uint32_t>(!!face.TintIndex)) << tintBitOffset;

	vector<uint32_t>& out = face.hasTransparency ? TransparentVerticesBuffer : VerticesBuffer;

	uint64_t currIndex = face.hasTransparency ? transparentFaceCount++ : solidFaceCount++;

	if (out.size() <= (currIndex + 1) * 12)
		out.resize(out.size() + BUFFER_SIZE_STEP);

	//Get AO
	glm::ivec4 AO = allowAO ? getAO(axis, x / 16, y / 16, z / 16) : glm::ivec4{ 15, 15, 15, 15 };

	PP = AO[0], PN = AO[1], NP = AO[2], NN = AO[3];

	out[currIndex * 12 + 0]  = 0u | P0[0] | P0[1] | P0[2] | Norm | tint;
	out[currIndex * 12 + 1]  = 0u | TexNN.x | TexNN.y | tex | (NN << blockShadingBitOffset);
	out[currIndex * 12 + 2]  = 0u | P0[0] | P1[1] | P0[2] | Norm | tint;
	out[currIndex * 12 + 3]  = 0u | TexPN.x | TexPN.y | tex | (PN << blockShadingBitOffset);
	out[currIndex * 12 + 4]  = 0u | P0[0] | P0[1] | P1[2] | Norm | tint;
	out[currIndex * 12 + 5]  = 0u | TexNP.x | TexNP.y | tex | (NP << blockShadingBitOffset);
	out[currIndex * 12 + 6]  = 0u | P0[0] | P0[1] | P1[2] | Norm | tint;
	out[currIndex * 12 + 7]  = 0u | TexNP.x | TexNP.y | tex | (NP << blockShadingBitOffset);
	out[currIndex * 12 + 8]  = 0u | P0[0] | P1[1] | P0[2] | Norm | tint;
	out[currIndex * 12 + 9]  = 0u | TexPN.x | TexPN.y | tex | (PN << blockShadingBitOffset);
	out[currIndex * 12 + 10] = 0u | P0[0] | P1[1] | P1[2] | Norm | tint;
	out[currIndex * 12 + 11] = 0u | TexPP.x | TexPP.y | tex | (PP << blockShadingBitOffset);

	if (direction == 0) {
		swap(out[currIndex * 12 + 2], out[currIndex * 12 + 4]);
		swap(out[currIndex * 12 + 3], out[currIndex * 12 + 5]);

		swap(out[currIndex * 12 + 8], out[currIndex * 12 + 10]);
		swap(out[currIndex * 12 + 9], out[currIndex * 12 + 11]);
	}
}

inline const BlockID& MeshingV2::ChunkMeshData::getCachedBlockID(int x, int y, int z) const {
	return ChunkCache[(x + 1) * 18 * 18 + (z + 1) * 18 + (y + 1)];
}

inline const BlockID& MeshingV2::ChunkMeshData::getCachedBlockID(int* pos) const {
	return getCachedBlockID(pos[0], pos[1], pos[2]);
}

inline void MeshingV2::ChunkMeshData::setCachedBlockID(BlockID b, int x, int y, int z) {
	ChunkCache[(x + 1) * 18 * 18 + (z + 1) * 18 + (y + 1)] = b;
}

inline glm::ivec4 MeshingV2::ChunkMeshData::getAO(uint8_t direction, int x, int y, int z) {
	const uint8_t AMBIENT_OCCLUSION_STRENGTH = 2;
	glm::ivec3 pos{x, y, z};

	char InitialLighting = chunk->Lighting.GetLighting(x, y, z);

	uint8_t PP{ 15 }, PN{ 15 }, NP{ 15 }, NN{ 15 };

	uint8_t axis = direction >> 1;
	uint8_t facing = direction & 0b1;

	pos[axis] += 1 - 2 * facing;

	//Check up down left right

	int axis1 = (axis + 1) % 3;
	int axis2 = (axis + 2) % 3;

	//Check up
	pos[axis1] += 1;

	if (getCachedBlockID(pos.x, pos.y, pos.z) != Blocks.AIR) {
		PP -= AMBIENT_OCCLUSION_STRENGTH;
		PN -= AMBIENT_OCCLUSION_STRENGTH;
	}

	pos[axis1] -= 2;

	if (getCachedBlockID(pos.x, pos.y, pos.z) != Blocks.AIR) {
		NP -= AMBIENT_OCCLUSION_STRENGTH;
		NN -= AMBIENT_OCCLUSION_STRENGTH;
	}

	pos[axis1] += 1;
	pos[axis2] += 1;

	if (getCachedBlockID(pos.x, pos.y, pos.z) != Blocks.AIR) {
		NP -= AMBIENT_OCCLUSION_STRENGTH;
		PP -= AMBIENT_OCCLUSION_STRENGTH;
	}

	pos[axis2] -= 2;

	if (getCachedBlockID(pos.x, pos.y, pos.z) != Blocks.AIR) {
		PN -= AMBIENT_OCCLUSION_STRENGTH;
		NN -= AMBIENT_OCCLUSION_STRENGTH;
	}

	pos[axis2] += 1;


	//Check corners now
	pos[axis1] += 1;
	pos[axis2] += 1;

	if (getCachedBlockID(pos.x, pos.y, pos.z) != Blocks.AIR)
		PP -= AMBIENT_OCCLUSION_STRENGTH;

	pos[axis1] -= 2;
	if (getCachedBlockID(pos.x, pos.y, pos.z) != Blocks.AIR)
		NP -= AMBIENT_OCCLUSION_STRENGTH;

	pos[axis2] -= 2;

	if (getCachedBlockID(pos.x, pos.y, pos.z) != Blocks.AIR)
		NN -= AMBIENT_OCCLUSION_STRENGTH;

	pos[axis1] += 2;

	if (getCachedBlockID(pos.x, pos.y, pos.z) != Blocks.AIR)
		PN -= AMBIENT_OCCLUSION_STRENGTH;


	if (PP >= (15 - InitialLighting))
		PP = PP - (15 - InitialLighting);
	else
		PP = 0;

	if (PN >= (15 - InitialLighting))
		PN = PN - (15 - InitialLighting);
	else
		PN = 0;

	if (NN >= (15 - InitialLighting))
		NN = NN - (15 - InitialLighting);
	else
		NN = 0;

	if (NP >= (15 - InitialLighting))
		NP = NP - (15 - InitialLighting);
	else
		NP = 0;

	return glm::ivec4(PP, PN, NP, NN);

}

//Checks if a block side is visible to the player
inline bool MeshingV2::ChunkMeshData::IsFaceVisible(const Cuboid& cube, int x, int y, int z, uint8_t side) {
	const uint8_t axis = (side >> 1); //Get side
	const uint8_t axis1 = (axis + 1) % 3;
	const uint8_t axis2 = (axis + 2) % 3;
	const uint8_t oppositeSide = axis * 2 + static_cast<uint8_t>(!(side & 0b1));

	int p[3]{ x, y, z };

	p[axis] += 1 - 2 * (side & 0b1);
//	return getCachedBlockID(p[0], p[1], p[2]) == Blocks.AIR;

	const ModelV2::BlockModelV2& model = Blocks.getBlockModelDereferenced(getCachedBlockID(p[0], p[1], p[2]));

	if (!model.isInitialized) return true;

	for (int i = 0; i < model.Elements.size(); ++i) {
		const Cuboid& element = model.Elements[i];
		if (element.Faces[oppositeSide].CullFace != oppositeSide ||
			element.Faces[oppositeSide].isSeeThrough ||
			element.Faces[oppositeSide].hasTransparency ||
			element.Faces[oppositeSide].ReferenceTexture.empty())
			continue; 

		if (side & 1) //if the  block arent touching
			if (element.To[axis] < 16) continue;
		else
			if (element.From[axis] > 0) continue;
		
		//Check if the faces aren't overlapping

		if (element.From[axis1] <= cube.From[axis1] && element.To[axis1] >= cube.To[axis1] &&
			element.From[axis2] <= cube.From[axis2] && element.To[axis2] >= cube.To[axis2])
			return false;

	}
	return true;
}

inline bool MeshingV2::ChunkMeshData::IsFaceVisibleUnsafe(const Cuboid& cube, int x, int y, int z, uint8_t side) {
	uint8_t axis = (side >> 1); //Get side

	int p[3]{ x,y,z };

	p[axis] += 1 - 2 * (side & 0b1);

	return Blocks.getBlockType(getCachedBlockID(p[0], p[1], p[2]))->Properties->transparency;
}

inline bool MeshingV2::ChunkMeshData::CompareBlockSide(int x, int y, int z, uint8_t side, BlockID b) {
	//IsFaceVisibleCalls++;

	uint8_t axis = (side >> 1); //Get side

	int p[3]{ x,y,z };

	p[axis] += 1 - 2 * (side & 0b1);

	return getCachedBlockID(p[0], p[1], p[2]) == b;
}

inline bool MeshingV2::ChunkMeshData::CompareBlockSideUnsafe(int x, int y, int z, uint8_t side, BlockID b) {
	uint8_t axis = (side >> 1); //Get side

	int p[3]{ x,y,z };

	p[axis] += 1 - 2 * (side & 0b1);

	return getCachedBlockID(p[0], p[1], p[2]) == b;
}