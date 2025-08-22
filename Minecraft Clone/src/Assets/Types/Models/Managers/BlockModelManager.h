#include "Assets/Types/Models/Managers/ModelManager.h"

class GameContext;

class BlockModelManager : public ModelManager {
 public:
  using ModelList = std::vector<AssetHandle<RenderableModel>>;
  BlockModelManager(GameContext&);
  ~BlockModelManager();

  const ModelList& GetModels() const noexcept;
  void Load();

 private:
  std::vector<AssetHandle<RenderableModel>> models_;
};
