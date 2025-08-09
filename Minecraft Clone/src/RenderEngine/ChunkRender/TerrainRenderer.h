// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#pragma once
#include <memory>
#include <vector>

#include "Core/Typenames.h"
#include "RenderEngine/RenderResources/RenderHandle.h"

class Timer;
class Camera;
class Shader;
class ChunkDrawBatch;
class GameContext;

struct GLFWwindow;
struct BlockVertexFormat;

namespace Mesh {
struct ChunkVertexData;
}

class TerrainRenderer {
 public:
  explicit TerrainRenderer(GameContext&);
  ~TerrainRenderer();

  void Initialize(GLFWwindow* window_, Camera* camera_);
  void PrepareRenderer();
  void SetupCallSolid();
  void SetupCallTransparent();
  void RenderSky();
  void Render();
  void Defrag(int iterations);
  void Update();
  void SetSettings(uint32_t RenderDistance, uint32_t verticalRenderDistance,
                   float fov_);
  void LoadAssets();
  void AddChunk(std::unique_ptr<Mesh::ChunkVertexData> MeshData);
  double GetDebugTime();
  double GetFragmentationRate();
  size_t GetVRAMUsageFull();

  size_t amountOfMeshGenerated = 1;

 private:
  void AddChunk(ChunkPos pos, const std::vector<BlockVertexFormat>& data,
                std::vector<ChunkDrawBatch>& BatchType,
                FastHashMap<ChunkPos, int>& LookUpMap);
  void CreateNewSolidBatch();
  void CreateNewTransparentBatch();

  GameContext& context_;
  int horizontal_render_distance_ = 16;
  int vertical_render_distance_ = 16;
  float fov_ = 80.f;
  int TextureAminationIndex = 0;

  std::vector<ChunkDrawBatch> chunk_solid_batches_;
  FastHashMap<ChunkPos, int>
      chunk_batch_solid_lookup_;  // f: ChunkPos -> SolidBatchIndex

  std::vector<ChunkDrawBatch> chunk_transparent_batches_;
  FastHashMap<ChunkPos, int>
      chunk_batch_transparent_lookup_;  // f: ChunkPos ->
                                        // TransparentBatchIndex

  Camera* camera_;
  GLFWwindow* window_ = nullptr;
  std::unique_ptr<Timer> time_;
  RenderHandle<Shader> cubic_shader_;
};
