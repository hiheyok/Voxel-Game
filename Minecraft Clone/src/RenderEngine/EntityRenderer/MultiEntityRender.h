#pragma once
#include "EntityRenderCache.h"
#include <vector>
#include "../OpenGL/Buffers/Buffer.h"
#include "../OpenGL/Shader/Shader.h"
#include "../../Client/Render/PlayerPOV.h"
#include "../../World/Entity/Type/EntityType.h"
#include "../../World/Entity/Entities.h"

class MultiEntityRenderer {
private:

	std::vector<float> EntityVertices;
	std::vector<int> EntityIndices;

	std::unordered_map<EntityTypeID, EntityModel> EntityCachedModels;

	std::unordered_map<EntityTypeID, size_t> EntityElementIndex;
	std::unordered_map<EntityTypeID, size_t> EntityElementSize;

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

	void Initialize();

	void SetTimePastTick(double t);

	void SetupCall();

	void Render();

	void SetRotation(glm::dvec2 rotation);

	void SetPosition(glm::dvec3 position);

	void SetWindow(GLFWwindow* win);

	void Update();

	void Reload();




};