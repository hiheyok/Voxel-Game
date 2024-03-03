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
	Buffer SSBO;
	VertexArray VAO;

	std::vector<float> PositionArr;

	int NumEntityRendered = 0;

	double TimePastTick = 0.0;
public:

	int VerticalRenderDistance = 16;
	int HorizontalRenderDistance = 16;

	int getNumEntityRendered() {
		return NumEntityRendered;
	}

	void AddEntity(Entity entity) {
		RenderableEntities.AddEntity(entity);
	}

	void RemoveEntity(EntityUUID EntityUUID) {
		RenderableEntities.RemoveEntity(EntityUUID);
	}

	void Initialize() {
		for (const auto& types : EntityList.EntityTypeList) {
			EntityModel model = types.second->RenderModel;
			EntityCachedModels[types.first] = model;
		}

		shader.init("assets/shaders/Entity/MultiEntityVert.glsl", "assets/shaders/Entity/MultiEntityFrag.glsl");
		camera = player.getCamera();

		//Initialize Buffers

		for (auto& EntityModels : EntityCachedModels) {
			Model::ModelData model = EntityModels.second.GetVertices();

			int ModelIndex = EntityVertices.size() / 5;

			EntityElementIndex[EntityModels.first] = EntityIndices.size(); //temp solution
			EntityElementSize[EntityModels.first] = model.Indices.size();

			for (int i = 0; i < model.Indices.size(); i++) {
				EntityIndices.emplace_back(ModelIndex + model.Indices[i]);
			}

			EntityVertices.insert(EntityVertices.end(), model.Vertices.begin(), model.Vertices.end());
		}


		VBO.GenBuffer();
		EBO.GenBuffer();
		SSBO.GenBuffer();
		VAO.GenArray();

		VAO.Bind();

		VBO.SetType(GL_ARRAY_BUFFER);
		VBO.SetUsage(GL_STATIC_DRAW);
		VBO.InsertData(EntityVertices.size() * sizeof(float), EntityVertices.data(), GL_STATIC_DRAW);

		EBO.SetUsage(GL_STATIC_DRAW);
		EBO.SetType(GL_ELEMENT_ARRAY_BUFFER);
		EBO.InsertData(EntityIndices.size() * sizeof(unsigned int), EntityIndices.data(), GL_STATIC_DRAW);

		SSBO.SetType(GL_SHADER_STORAGE_BUFFER);
		SSBO.SetUsage(GL_DYNAMIC_COPY);
		SSBO.SetMaxSize(10000000);
		SSBO.InitializeData();

		VBO.Bind();
		VAO.EnableAttriPTR(0, 3, GL_FLOAT, GL_FALSE, 5, 0);
		VAO.EnableAttriPTR(1, 2, GL_FLOAT, GL_FALSE, 5, 3);
		VBO.Unbind();
		VAO.Unbind();

		PositionArr.resize(10000000);
	}

	void SetTimePastTick(double t) {
		TimePastTick = t;
	}

	void SetupCall() {
		glDisable(GL_BLEND);
		glEnable(GL_DEPTH_TEST);
		glDepthMask(GL_TRUE);
		glDisable(GL_CULL_FACE);
	}

	void Render() {

		SetupCall();

		int n = 0;

		for (auto& entityarr : RenderableEntities.EntitySeparated) {

			//Fill SSBO POS data

			int i = 0;

			for (auto& e : entityarr.second) {
				EntityProperty properties = e.second.Properties;

				PositionArr[(i * 3 + 0)] = (properties.Position.x + properties.Velocity.x * TimePastTick + properties.Acceleration.x * TimePastTick * TimePastTick * 0.5f);
				PositionArr[(i * 3 + 1)] = (properties.Position.y + properties.Velocity.y * TimePastTick + properties.Acceleration.y * TimePastTick * TimePastTick * 0.5f);
				PositionArr[(i * 3 + 2)] = (properties.Position.z + properties.Velocity.z * TimePastTick + properties.Acceleration.z * TimePastTick * TimePastTick * 0.5f);

				i++;
			}

			n += i;

			SSBO.InsertSubData(0, (i * 3) * sizeof(float), PositionArr.data());
			shader.bindTexture2D(0, EntityList.EntityTypeList[entityarr.first]->texture.get(), "EntityTexture");
			shader.use();

			VAO.Bind();
			EBO.Bind();
			VBO.Bind();
			SSBO.Bind();
			SSBO.BindBase(2);

			glDrawElementsInstanced(GL_TRIANGLES, EntityElementSize[entityarr.first], GL_UNSIGNED_INT, (void*)(EntityElementIndex[entityarr.first] * sizeof(unsigned int)), i);

			SSBO.UnbindBase(2);
			SSBO.Unbind();
			VBO.Unbind();
			EBO.Unbind();
			VAO.Unbind();
		}
		NumEntityRendered = n;


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
		shader.setFloat("RenderDistance", (float)(HorizontalRenderDistance * 16));
		shader.setFloat("VerticalRenderDistance", (float)(VerticalRenderDistance * 16));
		shader.setVec3("camPos", camera->Position);
	}

	void Reload() {

		//reset gpu data
		VBO.Delete();
		EBO.Delete();
		VAO.ResetArray();

		//reset entity models
		for (auto e : EntityList.EntityTypeList) {
			e.second->RenderModel.Clear();
		}

		EntityList.Initialize();

		Initialize();
	}




};