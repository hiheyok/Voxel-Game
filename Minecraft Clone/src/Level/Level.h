#pragma once
#include <memory>

class Dimension;
class GameContext;

class Level {
 public:
  explicit Level(GameContext&);
  ~Level();

  void Start();
  void Stop();
  void UpdateDimensions();
  void ResetState();

  GameContext& context_;
  std::unique_ptr<Dimension> main_world_;
};
