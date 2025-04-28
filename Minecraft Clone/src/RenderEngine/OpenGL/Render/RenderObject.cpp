#include "RenderEngine/OpenGL/Render/RenderObject.h"

RenderObject::RenderObject() noexcept = default;
RenderObject::RenderObject(RenderObject&&) noexcept = default;
RenderObject& RenderObject::operator=(RenderObject&&) noexcept = default;
RenderObject::~RenderObject() noexcept = default;

void RenderObject::SetShader(std::unique_ptr<ShaderInterface> shader) {
    shader_ = std::move(shader);
}

ShaderInterface& RenderObject::GetShader() { return *shader_; }