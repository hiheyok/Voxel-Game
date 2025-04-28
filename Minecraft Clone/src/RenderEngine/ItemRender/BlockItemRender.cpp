#include "BlockitemRender.h"

#include "Level/Block/Block.h"
#include "Level/Block/Blocks.h"
#include "Level/Item/Item.h"
#include "RenderEngine/BlockModel/BlockModels.h"
#include "RenderEngine/Camera/camera.h"
#include "RenderEngine/ChunkRender/BlockTextureAtlas.h"
#include "RenderEngine/OpenGL/Buffers/Buffer.h"
#include "RenderEngine/OpenGL/Buffers/VertexArray.h"
#include "RenderEngine/OpenGL/Shader/Shader.h"
#include "RenderEngine/OpenGL/Texture/Types/TextureAtlas.h"

BlockItemRender::BlockItemRender() : camera_{std::make_unique<Camera>()} {}
BlockItemRender::~BlockItemRender() = default;

void BlockItemRender::Initialize() {
    shader_ = std::make_unique<Shader>(
        "assets/shaders/ItemRender/BlockModelVert.glsl",
        "assets/shaders/ItemRender/BlockModelFrag.glsl");

    vao_ = std::make_unique<VertexArray>();
    ebo_ = std::make_unique<Buffer>();
    vbo_ = std::make_unique<Buffer>();

    ebo_->SetType(GL_ELEMENT_ARRAY_BUFFER);
    ebo_->SetUsage(GL_STATIC_DRAW);
    vbo_->SetType(GL_ARRAY_BUFFER);
    vbo_->SetUsage(GL_STATIC_DRAW);

    vao_->Bind();
    vbo_->Bind();
    vao_->EnableAttriPTR(0, 3, GL_FLOAT, GL_FALSE, 7, 0);
    vao_->EnableAttriPTR(1, 2, GL_FLOAT, GL_FALSE, 7, 3);
    vao_->EnableAttriPTR(2, 1, GL_FLOAT, GL_FALSE, 7, 5);
    vao_->EnableAttriPTR(3, 1, GL_FLOAT, GL_FALSE, 7, 6);
    vao_->Unbind();
    vbo_->Unbind();
    SetCamera();

    float dimensions = 0.85f;

    glm::mat4 view = camera_->GetViewMatrix();
    glm::mat4 modelMat = glm::mat4(1.f);
    glm::mat4 orthoProj = glm::ortho(-dimensions, dimensions, -dimensions,
                                     dimensions, 0.001f, 3.0f);
    shader_->Use();

    shader_->SetMat4("view", view);
    shader_->SetMat4("model", modelMat);
    shader_->SetMat4("projection", orthoProj);

    shader_->BindTexture2D(0, g_blocks.block_texture_atlas_->Get(),
                           "BlockTexture");
}

void BlockItemRender::RenderBlock(Item item) {
    if (g_blocks.GetBlockType(item.GetBlock())->block_model_data_ == 0) {
        return;
    }

    std::vector<float> vertices{};
    std::vector<uint32_t> indices{};

    g_blocks.GetBlockType(item.GetBlock())
        ->block_model_data_->GetVertices(vertices, indices);

    vbo_->InsertData(vertices.size() * sizeof(float), vertices.data(),
                     GL_STATIC_DRAW);
    ebo_->InsertData(indices.size() * sizeof(uint32_t), indices.data(),
                     GL_STATIC_DRAW);
    // std::cout << model.Vertices.size() << '\n';
    camera_->screen_res_ = glm::vec2(16.f, 16.f);

    shader_->Use();
    shader_->BindTexture2D(0, g_blocks.block_texture_atlas_->Get(),
                           "BlockTexture");
    setDrawCalls();
    vao_->Bind();
    ebo_->Bind();
    vbo_->Bind();
    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indices.size()),
                   GL_UNSIGNED_INT, 0);
    vao_->Unbind();
    ebo_->Unbind();
    vbo_->Unbind();
}

void BlockItemRender::SetCamera() {
    camera_->fov_ = 57;
    camera_->position_ = glm::vec3(1.1f, 1.1f, 1.1f);
    camera_->pitch_ = -35;
    camera_->yaw_ = -135;

    camera_->UpdateCameraVectors();
}

void BlockItemRender::setDrawCalls() {
    glEnable(GL_BLEND);
    glDepthMask(GL_TRUE);
    glDisable(GL_CULL_FACE);
}