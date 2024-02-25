#pragma once
#include "EntityRenderCache.h"
#include <vector>
#include "../OpenGL/Buffers/Buffer.h"
#include "../OpenGL/Shader/Shader.h"
#include "../../Client/Render/PlayerPOV.h"
#include "../../World/Entity/Type/EntityType.h"
#include "../../World/Entity/Entities.h"
class EntityRenderer {
private:
	EntityRenderCache cachedEntities;

	std::vector<float> vertices;
	std::vector<int> indices;

	std::unordered_map<EntityTypeID, EntityModel> EntityCachedModels;
	Shader shader;

	PlayerPOV player;
	Camera* camera;
	GLFWwindow* window;

	double TimePastTick = 0.0;
public:
	void AddEntity(Entity entity) {
		cachedEntities.AddEntity(entity);
	}

	void RemoveEntity(EntityUUID EntityUUID) {
		cachedEntities.RemoveEntity(EntityUUID);
	}

	void Initialize() {
		for (const auto& types : EntityList.EntityTypeList) {
			EntityModel model = types.second->RenderModel;
			EntityCachedModels[types.first] = model;
		}

		shader.init("assets/shaders/Entity/EntityVert.glsl", "assets/shaders/Entity/EntityFrag.glsl");
		camera = player.getCamera();
	}

	void PrepareRenderer() {
		for (const auto& entity : cachedEntities.EntityContainer) {
			Entity e = entity.second;
			Model::ModelData model = EntityCachedModels[e.Type].GetVertices();

			int ModelIndex = vertices.size();

			for (int i = 0; i < model.Indices.size(); i++) {
				indices.emplace_back(ModelIndex + model.Indices[i]);
			}

			glm::vec3 EntityPosition = entity.second.Properties.Position + (float)TimePastTick * entity.second.Properties.Velocity;

			for (int i = 0; i < model.Vertices.size(); i++) {
				vertices.emplace_back(EntityPosition[i % 3] + model.Vertices[i]);
			}

		//	Logger.LogInfo("Entity Render Debug", "Added Entity");

		}
	}

	void SetTimePastTick(double t) {
		TimePastTick = t;
	}

	void Render() {
		VertexArray VAO;
		Buffer VBO;
		Buffer EBO;

		VAO.GenArray();
		VBO.GenBuffer();
		EBO.GenBuffer();

		VAO.Bind();
		VBO.SetType(GL_ARRAY_BUFFER);
		VBO.SetMaxSize(vertices.size() * sizeof(float));
		VBO.InsertData(vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

		
		EBO.SetType(GL_ELEMENT_ARRAY_BUFFER);
		EBO.SetMaxSize(indices.size() * sizeof(float));
		EBO.InsertData(indices.size() * sizeof(float), indices.data(), GL_STATIC_DRAW);

		VBO.Bind();
		VAO.EnableAttriPTR(0, 3, GL_FLOAT, GL_FALSE, 3, 0);
		VBO.Unbind();
		VAO.Unbind();

		shader.use();
		VAO.Bind();
		EBO.Bind();
		glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
		//Logger.LogInfo("Entity Render Debug", std::to_string(indices.size()));

		//Clean

		vertices.clear();

		indices.clear();

	}

	void SetRotation(glm::dvec2 rotation) {
		player.SetRotation(rotation);
	}

	void SetPosition(glm::dvec3 position) {
		player.SetPosition(position);
	}

	void SetWindow(GLFWwindow* win) {
		window = win;
	}


	void Update() {
		int width, height;

		glfwGetWindowSize(window, &width, &height);
		glm::mat4 model = glm::mat4(1.f);

		camera->screenRes = glm::vec2(width, height);

		glm::mat4 view = camera->GetViewMatrix();

		int x = width;
		int y = height;
		glm::mat4 projection = glm::perspective(glm::radians(camera->FOV), (float)x / (float)y, 0.1f, 1000000.0f);
		shader.use();

		shader.setMat4("view", view);
		shader.setMat4("model", model);
		shader.setMat4("projection", projection);

		PrepareRenderer();
	}




};