#include "MultiEntityRender.h"

int MultiEntityRenderer::getNumEntityRendered() {
	return NumEntityRendered;
}

void MultiEntityRenderer::AddEntity(EntityProperty& entity) {
	RenderableEntities.AddEntity(entity);
	isDirty = true;
}

void MultiEntityRenderer::RemoveEntity(EntityUUID EntityUUID) {
	RenderableEntities.RemoveEntity(EntityUUID);
	isDirty = true;
}

void MultiEntityRenderer::clean() {
	VBO.Delete();
	EBO.Delete();
	SSBO_Pos.Delete();
	SSBO_Vel.Delete();
	SSBO_Acc.Delete();
	VAO.Delete();
}

void MultiEntityRenderer::Initialize() {
	for (int i = 0; i < EntityList.EntityTypeList.size(); i++) {
		EntityModel model = EntityList.EntityTypeList[i]->RenderModel;
		EntityCachedModels[i] = model;
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
	SSBO_Pos.GenBuffer();
	SSBO_Vel.GenBuffer();
	SSBO_Acc.GenBuffer();
	VAO.GenArray();

	VAO.Bind();

	VBO.SetType(GL_ARRAY_BUFFER);
	VBO.SetUsage(GL_STATIC_DRAW);
	VBO.InsertData(EntityVertices.size() * sizeof(float), EntityVertices.data(), GL_STATIC_DRAW);

	EBO.SetUsage(GL_STATIC_DRAW);
	EBO.SetType(GL_ELEMENT_ARRAY_BUFFER);
	EBO.InsertData(EntityIndices.size() * sizeof(unsigned int), EntityIndices.data(), GL_STATIC_DRAW);

	SSBO_Pos.SetType(GL_SHADER_STORAGE_BUFFER);
	SSBO_Pos.SetUsage(GL_DYNAMIC_COPY);
	SSBO_Pos.SetMaxSize(3000000);
	SSBO_Pos.InitializeData();

	SSBO_Vel.SetType(GL_SHADER_STORAGE_BUFFER);
	SSBO_Vel.SetUsage(GL_DYNAMIC_COPY);
	SSBO_Vel.SetMaxSize(3000000);
	SSBO_Vel.InitializeData();

	SSBO_Acc.SetType(GL_SHADER_STORAGE_BUFFER);
	SSBO_Acc.SetUsage(GL_DYNAMIC_COPY);
	SSBO_Acc.SetMaxSize(3000000);
	SSBO_Acc.InitializeData();

	VBO.Bind();
	VAO.EnableAttriPTR(0, 3, GL_FLOAT, GL_FALSE, 5, 0);
	VAO.EnableAttriPTR(1, 2, GL_FLOAT, GL_FALSE, 5, 3);
	VBO.Unbind();
	VAO.Unbind();

	PositionArr.resize(3000000);
	VelocityArr.resize(3000000);
	AccelerationArr.resize(3000000);

}

void MultiEntityRenderer::SetTimePastTick(double t) {
	TimePastTick = t;
}

void MultiEntityRenderer::SetupCall() {
	glDisable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
	glDisable(GL_CULL_FACE);
}

void MultiEntityRenderer::Render() {

	SetupCall();

	int n = 0;

	for (auto& entityarr : RenderableEntities.EntitySeparated) {

		//Fill SSBO POS data

		int i = 0;
		//if (isDirty) {
			for (auto& e : entityarr.second) {
				EntityProperty properties = e.second;

				PositionArr[(i * 3 + 0)] = properties.Position.x;
				PositionArr[(i * 3 + 1)] = properties.Position.y;
				PositionArr[(i * 3 + 2)] = properties.Position.z;

				VelocityArr[(i * 3 + 0)] = properties.Velocity.x;
				VelocityArr[(i * 3 + 1)] = properties.Velocity.y;
				VelocityArr[(i * 3 + 2)] = properties.Velocity.z;

				AccelerationArr[(i * 3 + 0)] = properties.Acceleration.x;
				AccelerationArr[(i * 3 + 1)] = properties.Acceleration.y;
				AccelerationArr[(i * 3 + 2)] = properties.Acceleration.z;

				i++;
			}
			isDirty = false;
	//	}
		

		n += entityarr.second.size();

		SSBO_Pos.InsertSubData(0, (i * 3) * sizeof(float), PositionArr.data());
		SSBO_Vel.InsertSubData(0, (i * 3) * sizeof(float), VelocityArr.data());
		SSBO_Acc.InsertSubData(0, (i * 3) * sizeof(float), AccelerationArr.data());

		shader.bindTexture2D(0, EntityList.EntityTypeList[entityarr.first]->texture.get(), "EntityTexture");
		shader.use();

		VAO.Bind();
		EBO.Bind();
		VBO.Bind();
		SSBO_Pos.Bind();
		SSBO_Pos.BindBase(1);
		SSBO_Vel.Bind();
		SSBO_Vel.BindBase(2);
		SSBO_Acc.Bind();
		SSBO_Acc.BindBase(3);

		glDrawElementsInstanced(GL_TRIANGLES, EntityElementSize[entityarr.first], GL_UNSIGNED_INT, (void*)(EntityElementIndex[entityarr.first] * sizeof(unsigned int)), i);

		SSBO_Acc.UnbindBase(3);
		SSBO_Acc.Unbind();
		SSBO_Vel.UnbindBase(2);
		SSBO_Vel.Unbind();
		SSBO_Pos.UnbindBase(1);
		SSBO_Pos.Unbind();
		VBO.Unbind();
		EBO.Unbind();
		VAO.Unbind();
	}
	NumEntityRendered = n;


}

void MultiEntityRenderer::SetRotation(glm::dvec2 rotation) {
	player.SetRotation(rotation);
}

void MultiEntityRenderer::SetPosition(glm::dvec3 position) {
	player.SetPosition(position);
}

void MultiEntityRenderer::SetWindow(GLFWwindow* win) {
	window = win;
}


void MultiEntityRenderer::Update() {
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
	shader.setFloat("TimeDelta", TimePastTick);
}

void MultiEntityRenderer::Reload() {

	//reset gpu data
	VBO.Delete();
	EBO.Delete();
	VAO.ResetArray();

	//reset entity models
	for (auto e : EntityList.EntityTypeList) {
		e->RenderModel.Clear();
	}

	EntityList.Initialize();

	Initialize();
}