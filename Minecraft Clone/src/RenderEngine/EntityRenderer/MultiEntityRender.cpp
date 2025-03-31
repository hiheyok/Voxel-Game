#include "MultiEntityRender.h"
#include "EntityRenderCache.h"
#include "../Camera/camera.h"
#include "../OpenGL/Buffers/Buffer.h"
#include "../OpenGL/Buffers/VertexArray.h"
#include "../OpenGL/Shader/Shader.h"
#include "../../Client/Render/PlayerPOV.h"
#include "../../Level/Entity/Type/EntityType.h"
#include "../../Level/Entity/Entities.h"
#include "../../Client/Profiler/PerformanceProfiler.h"

MultiEntityRenderer::MultiEntityRenderer() : 
    vbo_{ std::make_unique<Buffer>() },
    ebo_{ std::make_unique<Buffer>() },
    ssbo_pos_{ std::make_unique<Buffer>() },
    ssbo_vel_{ std::make_unique<Buffer>() },
    ssbo_acc_{ std::make_unique<Buffer>() },
    vao_{ std::make_unique<VertexArray>() },
    player_{ std::make_unique<PlayerPOV>() },
    shader_{ std::make_unique<Shader>() },
    renderable_entities_{ std::make_unique<EntityRenderCache>() } {}

MultiEntityRenderer::~MultiEntityRenderer() = default;

size_t MultiEntityRenderer::GetNumEntityRendered() {
    return num_entity_rendered_;
}

void MultiEntityRenderer::AddEntity(EntityProperty& entity) {
    renderable_entities_->AddEntity(entity);
}

void MultiEntityRenderer::RemoveEntity(EntityUUID entityUUID) {
    renderable_entities_->RemoveEntity(entityUUID);
}

void MultiEntityRenderer::Clean() {
    vbo_->Delete();
    ebo_->Delete();
    ssbo_pos_->Delete();
    ssbo_vel_->Delete();
    ssbo_acc_->Delete();
    vao_->Delete();
}

void MultiEntityRenderer::Initialize(PerformanceProfiler* pProfilerIn) {
    for (int i = 0; i < g_entity_list.entity_type_list_.size(); i++) {
        EntityModel model = g_entity_list.entity_type_list_[i]->render_model_;
        entity_cached_models_[i] = model;
        profiler_ = pProfilerIn;

    }

    shader_->Init("assets/shaders/Entity/MultiEntityVert.glsl", "assets/shaders/Entity/MultiEntityFrag.glsl");
    camera_ = player_->GetCamera();

    //Initialize Buffers

    for (auto& EntityModels : entity_cached_models_) {
        Model::ModelData model = EntityModels.second.GetVertices();

        unsigned int ModelIndex = static_cast<unsigned int>(entity_vertices_.size() / 5);

        entity_element_index_[EntityModels.first] = entity_indices_.size(); //temp solution
        entity_element_size_[EntityModels.first] = model.indices_.size();

        for (int i = 0; i < model.indices_.size(); i++) {
            entity_indices_.emplace_back(ModelIndex + model.indices_[i]);
        }

        entity_vertices_.insert(entity_vertices_.end(), model.vertices_.begin(), model.vertices_.end());
    }


    vbo_->GenBuffer();
    ebo_->GenBuffer();
    ssbo_pos_->GenBuffer();
    ssbo_vel_->GenBuffer();
    ssbo_acc_->GenBuffer();
    vao_->GenArray();

    vao_->Bind();

    vbo_->SetType(GL_ARRAY_BUFFER);
    vbo_->SetUsage(GL_STATIC_DRAW);
    vbo_->InsertData(entity_vertices_.size() * sizeof(float), entity_vertices_.data(), GL_STATIC_DRAW);

    ebo_->SetUsage(GL_STATIC_DRAW);
    ebo_->SetType(GL_ELEMENT_ARRAY_BUFFER);
    ebo_->InsertData(entity_indices_.size() * sizeof(unsigned int), entity_indices_.data(), GL_STATIC_DRAW);

    ssbo_pos_->SetType(GL_SHADER_STORAGE_BUFFER);
    ssbo_pos_->SetUsage(GL_DYNAMIC_COPY);
    ssbo_pos_->SetMaxSize(3000000);
    ssbo_pos_->InitializeData();

    ssbo_vel_->SetType(GL_SHADER_STORAGE_BUFFER);
    ssbo_vel_->SetUsage(GL_DYNAMIC_COPY);
    ssbo_vel_->SetMaxSize(3000000);
    ssbo_vel_->InitializeData();

    ssbo_acc_->SetType(GL_SHADER_STORAGE_BUFFER);
    ssbo_acc_->SetUsage(GL_DYNAMIC_COPY);
    ssbo_acc_->SetMaxSize(3000000);
    ssbo_acc_->InitializeData();

    vbo_->Bind();
    vao_->EnableAttriPTR(0, 3, GL_FLOAT, GL_FALSE, 5, 0);
    vao_->EnableAttriPTR(1, 2, GL_FLOAT, GL_FALSE, 5, 3);
    vbo_->Unbind();
    vao_->Unbind();

    position_arr_.resize(3000000);
    velocity_arr_.resize(3000000);
    acceleration_arr_.resize(3000000);

}

void MultiEntityRenderer::SetTimePastTick(double t) {
    time_past_tick_ = t;
}

void MultiEntityRenderer::SetupCall() {
    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);
    glDisable(GL_CULL_FACE);
}

void MultiEntityRenderer::Render() {
    profiler_->ProfileStart("root/render/entity");
    SetupCall();

    size_t n = 0;

    for (auto& entityarr : renderable_entities_->entity_separated_) {
        //Fill SSBO POS data

        size_t i = 0;
        for (auto& e : entityarr.second) {
            EntityProperty properties = e.second;

            position_arr_[(i * 3 + 0)] = properties.position_.x;
            position_arr_[(i * 3 + 1)] = properties.position_.y;
            position_arr_[(i * 3 + 2)] = properties.position_.z;

            velocity_arr_[(i * 3 + 0)] = properties.velocity_.x;
            velocity_arr_[(i * 3 + 1)] = properties.velocity_.y;
            velocity_arr_[(i * 3 + 2)] = properties.velocity_.z;

            acceleration_arr_[(i * 3 + 0)] = properties.acceleration_.x;
            acceleration_arr_[(i * 3 + 1)] = properties.acceleration_.y;
            acceleration_arr_[(i * 3 + 2)] = properties.acceleration_.z;

            i++;
        }
        

        n += entityarr.second.size();

        ssbo_pos_->InsertSubData(0, (i * 3) * sizeof(float), position_arr_.data());
        ssbo_vel_->InsertSubData(0, (i * 3) * sizeof(float), velocity_arr_.data());
        ssbo_acc_->InsertSubData(0, (i * 3) * sizeof(float), acceleration_arr_.data());

        shader_->BindTexture2D(0, g_entity_list.entity_type_list_[entityarr.first]->texture_.get(), "EntityTexture");
        shader_->Use();

        vao_->Bind();
        ebo_->Bind();
        vbo_->Bind();
        ssbo_pos_->Bind();
        ssbo_pos_->BindBase(1);
        ssbo_vel_->Bind();
        ssbo_vel_->BindBase(2);
        ssbo_acc_->Bind();
        ssbo_acc_->BindBase(3);

        glDrawElementsInstanced(GL_TRIANGLES, static_cast<GLsizei>(entity_element_size_[entityarr.first]),
            GL_UNSIGNED_INT, (void*)(entity_element_index_[entityarr.first] * sizeof(unsigned int)), static_cast<GLsizei>(i));

        ssbo_acc_->UnbindBase(3);
        ssbo_acc_->Unbind();
        ssbo_vel_->UnbindBase(2);
        ssbo_vel_->Unbind();
        ssbo_pos_->UnbindBase(1);
        ssbo_pos_->Unbind();
        vbo_->Unbind();
        ebo_->Unbind();
        vao_->Unbind();
    }
    num_entity_rendered_ = n;
    profiler_->ProfileStop("root/render/entity");

}

void MultiEntityRenderer::SetRotation(glm::dvec2 rotation_) {
    player_->SetRotation(rotation_);
}

void MultiEntityRenderer::SetPosition(glm::dvec3 position) {
    player_->SetPosition(position);
}

void MultiEntityRenderer::SetWindow(GLFWwindow* win) {
    window_ = win;
}


void MultiEntityRenderer::Update() {
    profiler_->ProfileStart("root/update/entity");
    int width, height;

    glfwGetWindowSize(window_, &width, &height);
    glm::mat4 model = glm::mat4(1.f);

    camera_->screen_res_ = glm::vec2(width, height);

    glm::mat4 view = camera_->GetViewMatrix();

    int x = width;
    int y = height;
    glm::mat4 projection = glm::perspective(glm::radians(camera_->fov_), (float)x / (float)y, 0.1f, 1000000.0f);

    shader_->Use();
    shader_->SetMat4("view", view);
    shader_->SetMat4("model", model);
    shader_->SetMat4("projection", projection);
    shader_->SetFloat("RenderDistance", (float)(horizontal_render_distance_ * 16));
    shader_->SetFloat("VerticalRenderDistance", (float)(vertical_render_distance_ * 16));
    shader_->SetVec3("camPos", camera_->position_);
    shader_->SetFloat("TimeDelta", static_cast<float>(time_past_tick_));
    profiler_->ProfileStop("root/update/entity");
}

void MultiEntityRenderer::Reload() {

    //reset gpu data
    vbo_->Delete();
    ebo_->Delete();
    vao_->ResetArray();

    //reset entity models
    for (auto e : g_entity_list.entity_type_list_) {
        e->render_model_.Clear();
    }

    g_entity_list.Initialize();

    Initialize(profiler_);
}