// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#include "Client/Render/ClientRenderer.h"

#include <gl/glew.h>

#include <memory>

#include "Client/Render/WorldRender.h"
#include "Core/GameContext/GameContext.h"
#include "Core/Options/Option.h"
#include "RenderEngine/EntityRender/ECSEntityRender.h"
#include "RenderEngine/EntityRender/MultiEntityRender.h"
#include "RenderEngine/OpenGL/Framebuffer/Framebuffer.h"
#include "RenderEngine/Window.h"

using std::make_unique;

ClientRenderer::ClientRenderer(GameContext& context, PlayerPOV* player,
                               IECSManager& ecs_manager)
    : context_{context},
      framebuffer_{make_unique<TexturedFrameBuffer>(context)},
      terrain_render_{make_unique<WorldRender>(context, player)},
      entity_render_{make_unique<MultiEntityRender>(context, player)},
      ecs_entity_render_{
          make_unique<ECSEntityRender>(context, ecs_manager, player)} {}

ClientRenderer::~ClientRenderer() = default;

void ClientRenderer::Initialize(Window* window, ClientCache* cache,
                                float graphics_scale) {
  entity_render_->Initialize();
  entity_render_->SetWindow(window->GetWindow());
  ecs_entity_render_->Initialize();
  ecs_entity_render_->SetWindow(window->GetWindow());
  terrain_render_->Start(window->GetWindow(), cache);

  // Use window properties for framebuffer size
  framebuffer_->GenBuffer(window->GetProperties().res_x_,
                          window->GetProperties().res_y_, graphics_scale,
                          GL_RGB);
}

void ClientRenderer::Render(Window* window) {
  framebuffer_->BindFBO();

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  if (!window->GetProperties().draw_solid_) window->RenderLines();

  terrain_render_->Render();
  entity_render_->Render();
  ecs_entity_render_->Render();

  if (!window->GetProperties().draw_solid_) window->RenderSolid();

  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

  framebuffer_->UnbindFBO();
  framebuffer_->Render();
}

void ClientRenderer::Update(Window* window, float graphics_scale) {
  if (window->GetProperties().size_dirty_) {
    window->GetProperties().size_dirty_ = false;

    framebuffer_->Clear();
    framebuffer_->GenBuffer(window->GetProperties().res_x_,
                            window->GetProperties().res_y_, graphics_scale);
  }

  entity_render_->Update();
  terrain_render_->Update();
}

void ClientRenderer::HandleReload() { entity_render_->Reload(); }

WorldRender& ClientRenderer::GetTerrainRender() { return *terrain_render_; }

MultiEntityRender& ClientRenderer::GetEntityRender() { return *entity_render_; }

ECSEntityRender& ClientRenderer::GetECSEntityRender() {
  return *ecs_entity_render_;
}

void ClientRenderer::SetTimePastTick(double t) {
  entity_render_->SetTimePastTick(t);
  ecs_entity_render_->SetTimePastTick(t);
}
