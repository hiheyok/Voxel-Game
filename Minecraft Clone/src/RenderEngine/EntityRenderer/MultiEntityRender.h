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

	std::vector<float> EntityVertices;
	std::vector<int> EntityIndices;

	FastHashMap<EntityTypeID, EntityModel> EntityCachedModels;

	FastHashMap<EntityTypeID, size_t> EntityElementIndex;
	FastHashMap<EntityTypeID, size_t> EntityElementSize;

	EntityRenderCache RenderableEntities;

	Shader shader;

	PlayerPOV player;
	Camera* camera;
	GLFWwindow* window;

	Buffer VBO;
	Buffer EBO;
	Buffer SSBO_Pos, SSBO_Vel, SSBO_Acc;
	VertexArray VAO;

	std::vector<float> PositionArr;
	std::vector<float> VelocityArr;
	std::vector<float> AccelerationArr;

	PerformanceProfiler* mProfiler;

	bool isDirty = true;

	int NumEntityRendered = 0;

	double TimePastTick = 0.0;
public:

	int VerticalRenderDistance = 16;
	int HorizontalRenderDistance = 16;

	void clean();

	int getNumEntityRendered();

	void AddEntity(EntityProperty& entity);

	void RemoveEntity(EntityUUID EntityUUID);

	void Initialize(PerformanceProfiler* pProfilerIn);

	void SetTimePastTick(double t);

	void SetupCall();

	void Render();

	void SetRotation(glm::dvec2 rotation);

	void SetPosition(glm::dvec3 position);

	void SetWindow(GLFWwindow* win);

	void Update();

	void Reload();




};