#pragma once
#include "../OpenGL/Buffers/Buffer.h"
#include "../OpenGL/Shader/Shader.h"
#include "../../World/Chunk/Block/Block.h"
#include "../../World/Item/Item.h"
#include "../Camera/camera.h"

class BlockItemRender {
private:
	VertexArray VAO;
	Buffer EBO, VBO;
	Shader shader;

	Camera camera;

	void SetCamera() {
		camera.FOV = 57;
		camera.Position = glm::vec3(1.5f,1.5f,1.5f);
		camera.Pitch = -35;
		camera.Yaw = -135;

		camera.updateCameraVectors();
	}

	void setDrawCalls() {
		glEnable(GL_BLEND);
	//	glDisable(GL_DEPTH_TEST);
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
	}

	void RenderBlock(Item item) {
		BlockModel model = Blocks.getBlockType(item.Properties.block)->GetRenderModel();

		VBO.InsertData(model.Vertices.size() * sizeof(float), model.Vertices.data(), GL_STATIC_DRAW);
		EBO.InsertData(model.Indices.size() * sizeof(float), model.Indices.data(), GL_STATIC_DRAW);

		glm::mat4 modelMat = glm::mat4(1.f);

		camera.screenRes = glm::vec2(16.f, 16.f);

		glm::mat4 view = camera.GetViewMatrix();

	//	glm::mat4 projection = glm::perspective(glm::radians(camera.FOV), 1.f, 0.001f, 10000.0f);
		glm::mat4 orthoProj = glm::ortho(-1.f, 1.f, -1.f, 1.f, 0.001f, 3.0f);
		shader.use();

		shader.setMat4("view", view);
		shader.setMat4("model", modelMat);
		shader.setMat4("projection", orthoProj);

		setDrawCalls();

		shader.bindTextureArray2D(0, Blocks.BlockTextureArray.get(), "BlockTexture");
		VAO.Bind();
		EBO.Bind();
		VBO.Bind();
		glDrawElements(GL_TRIANGLES, model.Indices.size(), GL_UNSIGNED_INT, 0);
		VAO.Unbind();
		EBO.Unbind();
		VBO.Unbind();
	}

};