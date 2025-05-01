#pragma once

#include <gl/glew.h>
#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/vec2.hpp>

// Defines several possible options for camera movement. Used as abstraction to
// stay away from window-system specific input methods
enum Camera_Movement { CAM_FORWARD, CAM_BACKWARD, CAM_LEFT, CAM_RIGHT };

static constexpr float YAW = -90.0f;
static constexpr float PITCH = 0.0f;
static constexpr float SPEED = 10.0f;
static constexpr float SENSITIVITY = 0.1f;
static constexpr float ZOOM = 45.0f;

// An abstract camera class that processes input and calculates the
// corresponding Euler Angles, Vectors and Matrices for use in OpenGL
class Camera {
 public:
  // constructor with vectors
  Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f),
         glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW,
         float pitch = PITCH);
  // constructor with scalar values
  Camera(float posX, float posY, float posZ, float upX, float upY, float upZ,
         float yaw, float pitch);

  // returns the view matrix calculated using Euler Angles and the LookAt
  // Matrix
  glm::mat4 GetViewMatrix() const;

  glm::mat4 GetModelMatrix();

  // processes input received from any keyboard-like input system. Accepts
  // input parameter in the form of camera defined ENUM (to abstract it from
  // windowing systems)
  void ProcessKeyboard(Camera_Movement direction, float deltaTime);

  // processes input received from a mouse input system. Expects the offset
  // value in both the x and y direction.
  void ProcessMouseMovement(float xoffset, float yoffset,
                            GLboolean constrainPitch = true);

  // processes input received from a mouse scroll-wheel event. Only requires
  // input on the vertical wheel-axis
  void ProcessMouseScroll(float yoffset);

  // calculates the front vector from the Camera's (updated) Euler Angles
  void UpdateCameraVectors();

  // camera Attributes
  glm::vec3 position_;
  glm::vec3 front_;
  glm::vec3 up_;
  glm::vec3 right_;
  glm::vec3 world_up_;

  glm::ivec2 screen_res_;
  // euler Angles
  float yaw_;
  float pitch_;
  // camera options
  float movement_speed_;
  float mouse_sensitivity_;
  float zoom_;
  float fov_ = 90;

 private:
};
