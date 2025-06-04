#pragma once
#include <memory>

class Dimension;
class GameContext;

class Level {
 public:
  std::unique_ptr<Dimension> main_world_;

  explicit Level(GameContext&);
  ~Level();

  void Start();
  void Stop();
  void updateDimensions();

  GameContext& game_context_;
};
