#pragma once
#include "../OpenGL/Buffers/Buffer.h"
#include "../OpenGL/Shader/Shader.h"
#include "../../Level/Chunk/Block/Block.h"
#include "../../Level/Item/Item.h"
#include "../Camera/camera.h"

class BlockItemRender {
private:
	VertexArray VAO;
	Buffer EBO, VBO;
	Shader shader;

	Camera camera;

	void SetCamera() {
		camera.FOV = 57;
		camera.Position = glm::vec3(1.1f,1.1f,1.1f);
		camera.Pitch = -35;
		camera.Yaw = -135;

		camera.updateCameraVectors();
	}

	void setDrawCalls() {
		glEnable(GL_BLEND);
		glDepthMask(GL_TRUE);
		glDisable(GL_CULL_FACE);
	}

public:
	void Initialize() {
		shader.init("assets/shaders/ItemRender/BlockModelVert.glsl", "assets/shaders/ItemRender/BlockModelFrag.glsl");

		EBO.SetType(GL_ELEMENT_ARRAY_BUFFER);
		EBO.SetUsage(GL_STATIC_DRAW);
		VBO.SetType(GL_ARRAY_BUFFER);
		VBO.SetUsage(GL_STATIC_DRAW);

		VAO.GenArray();
		EBO.GenBuffer();
		VBO.GenBuffer();

		VAO.Bind();
		VBO.Bind();
		VAO.EnableAttriPTR(0, 3, GL_FLOAT, GL_FALSE, 7, 0);
		VAO.EnableAttriPTR(1, 2, GL_FLOAT, GL_FALSE, 7, 3);
		VAO.EnableAttriPTR(2, 1, GL_FLOAT, GL_FALSE, 7, 5);
		VAO.EnableAttriPTR(3, 1, GL_FLOAT, GL_FALSE, 7, 6);
		VAO.Unbind();
		VBO.Unbind();
		SetCamera();

		float dimensions = 0.85f;

		glm::mat4 view = camera.GetViewMatrix();
		glm::mat4 modelMat = glm::mat4(1.f);
		glm::mat4 orthoProj = glm::ortho(-dimensions, dimensions, -dimensions, dimensions, 0.001f, 3.0f);
		shader.use();

		shader.setMat4("view", view);
		shader.setMat4("model", modelMat);
		shader.setMat4("projection", orthoProj);


		shader.bindTexture2D(0, Blocks.BlockTextureAtlas.get(), "BlockTexture");
	}

	void RenderBlock(Item item) {
		if (Blocks.getBlockType(item.GetBlock())->BlockModelData == NULL) {
			return;
		}

		std::vector<float> vertices{};
		std::vector<uint32_t> indices{};

		Blocks.getBlockType(item.GetBlock())->BlockModelData->getVertices(vertices, indices);

		VBO.InsertData(vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
		EBO.InsertData(indices.size() * sizeof(uint32_t), indices.data(), GL_STATIC_DRAW);
		//std::cout << model.Vertices.size() << '\n';
		camera.screenRes = glm::vec2(16.f, 16.f);

		shader.use();
		shader.bindTexture2D(0, Blocks.BlockTextureAtlas.get(), "BlockTexture");
		setDrawCalls();
		VAO.Bind();
		EBO.Bind();
		VBO.Bind();
		glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
		VAO.Unbind();
		EBO.Unbind();
		VBO.Unbind();
	}

};