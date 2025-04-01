#pragma once
#include <vector>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include "Core/Typenames.h"

class Buffer;
class VertexArray;
class EntityModel;
class Shader;
class PerformanceProfiler;
class Camera;
class PlayerPOV;

struct EntityRenderCache;
struct EntityProperty;
struct GLFWwindow;

class MultiEntityRenderer {
private:

    std::vector<float> entity_vertices_;
    std::vector<unsigned int> entity_indices_;

    FastHashMap<EntityTypeID, EntityModel> entity_cached_models_;
    FastHashMap<EntityTypeID, size_t> entity_element_index_;
    FastHashMap<EntityTypeID, size_t> entity_element_size_;

    std::unique_ptr<EntityRenderCache> renderable_entities_;
    std::unique_ptr<Shader> shader_;

    std::unique_ptr<PlayerPOV> player_;
    Camera* camera_;
    GLFWwindow* window_;

    std::unique_ptr<Buffer> vbo_;
    std::unique_ptr<Buffer> ebo_;
    std::unique_ptr<Buffer> ssbo_pos_, ssbo_vel_, ssbo_acc_;
    std::unique_ptr<VertexArray> vao_;

    std::vector<float> position_arr_;
    std::vector<float> velocity_arr_;
    std::vector<float> acceleration_arr_;

    PerformanceProfiler* profiler_;

    size_t num_entity_rendered_ = 0;

    double time_past_tick_ = 0.0;
public:

    int vertical_render_distance_ = 16;
    int horizontal_render_distance_ = 16;

    MultiEntityRenderer();
    ~MultiEntityRenderer();

    void Clean();

    size_t GetNumEntityRendered();

    void AddEntity(EntityProperty& entity);

    void RemoveEntity(EntityUUID EntityUUID);

    void Initialize(PerformanceProfiler* pProfilerIn);

    void SetTimePastTick(double t);

    void SetupCall();

    void Render();

    void SetRotation(glm::dvec2 rotation_);

    void SetPosition(glm::dvec3 position);

    void SetWindow(GLFWwindow* win);

    void Update();

    void Reload();
};