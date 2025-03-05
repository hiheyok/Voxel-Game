#pragma once
#include "EntityRenderCache.h"
#include <vector>
#include "../OpenGL/Buffers/Buffer.h"
#include "../OpenGL/Shader/Shader.h"
#include "../../Client/Render/PlayerPOV.h"
#include "../../Level/Entity/Type/EntityType.h"
#include "../../Level/Entity/Entities.h"
#include "../../Client/Profiler/PerformanceProfiler.h"

class MultiEntityRenderer {
private:

	std::vector<float> entity_vertices_;
	std::vector<int> entity_indices_;

	FastHashMap<EntityTypeID, EntityModel> entity_cached_models_;

	FastHashMap<EntityTypeID, size_t> entity_element_index_;
	FastHashMap<EntityTypeID, size_t> entity_element_size_;

	EntityRenderCache renderable_entities_;

	Shader shader_;

	PlayerPOV player_;
	Camera* camera_;
	GLFWwindow* window_;

	Buffer vbo_;
	Buffer ebo_;
	Buffer ssbo_pos_, ssbo_vel_, ssbo_acc_;
	VertexArray vao_;

	std::vector<float> position_arr_;
	std::vector<float> velocity_arr_;
	std::vector<float> acceleration_arr_;

	PerformanceProfiler* profiler_;

	size_t num_entity_rendered_ = 0;

	double time_past_tick_ = 0.0;
public:

	int vertical_render_distance_ = 16;
	int horizontal_render_distance_ = 16;

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