#pragma once
#include <memory>

class VertexArray;
class Buffer;
class Shader;
class Camera;
class Item;

class BlockItemRender {
 public:
  BlockItemRender();
  ~BlockItemRender();

  void Initialize();
  void RenderBlock(Item item);

 private:
  std::unique_ptr<VertexArray> vao_;
  std::unique_ptr<Buffer> ebo_, vbo_;
  std::unique_ptr<Shader> shader_;
  std::unique_ptr<Camera> camera_;

  void SetCamera();
  void setDrawCalls();
};
