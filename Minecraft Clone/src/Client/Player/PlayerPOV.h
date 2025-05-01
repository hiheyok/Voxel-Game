#pragma once
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <memory>

class Camera;

class PlayerPOV {
 public:
  PlayerPOV();
  ~PlayerPOV();

  void SetPosition(glm::vec3 pos);
  void SetRotation(glm::vec2 rot);

  glm::vec3 GetPosition() const;

  void SetFOV(float fov);

  Camera* GetCamera();

 private:
  std::unique_ptr<Camera> camera_;
};