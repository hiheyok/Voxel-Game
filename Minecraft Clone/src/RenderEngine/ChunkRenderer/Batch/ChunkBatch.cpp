#include "ChunkBatch.h"

#include <iterator>
#include "../../../Level/Timer/Timer.h"
#include <unordered_set>
using namespace glm;
using namespace std;

void ChunkDrawBatch::SetupBuffers() {
	MemoryPool.Allocate(MaxBufferSize);

	IBO.GenBuffer();
	SSBO.GenBuffer();
	Array.GenArray();

	IBO.SetType(GL_DRAW_INDIRECT_BUFFER);
	SSBO.SetType(GL_SHADER_STORAGE_BUFFER);

	IBO.SetUsage(GL_STATIC_DRAW);
	SSBO.SetUsage(GL_DYNAMIC_COPY);

	Array.Bind();

	IBO.SetMaxSize((size_t)(MaxBufferSize / 100));
	IBO.InitializeData();

	MemoryPool.buffer.Bind();
	Array.EnableAttriPTR(0, 1, GL_FLOAT, GL_FALSE, 2, 0);
	Array.EnableAttriPTR(1, 1, GL_FLOAT, GL_FALSE, 2, 1);
	MemoryPool.buffer.Unbind();
	Array.Unbind();

	SSBO.SetMaxSize((size_t)(MaxBufferSize / 100));
	SSBO.InitializeData();
}

void ChunkDrawBatch::Reset() {
//	MemoryPool.buffer.ResetBuffer();
	IBO.ResetBuffer();
	SSBO.ResetBuffer();
	Array.ResetArray();
	SetupBuffers();
}

void ChunkDrawBatch::Cleanup() {
	MemoryPool.buffer.Delete();
	IBO.Delete();
	SSBO.Delete();
	Array.~VertexArray();

	DrawCommands.clear();
	RenderList.clear();
	ChunkShaderPos.clear();
	RenderListOffsetLookup.clear();

}

void ChunkDrawBatch::GenDrawCommands(int RenderDistance, int VerticalRenderDistance) {

	Timer time;

	if (RenderList.size() > DrawCommands.size()) {
		UpdateCommandBufferSize();
	}

	if (!UpdateCommands)
		return;

	Frustum.CalculateFrustum(camera);

	ivec3 Position(floor(camera->Position.x / 16.f), floor(camera->Position.y / 16.f), floor(camera->Position.z / 16.f));
	
	bool UseNew = false;

	if (UseNew) {
		std::vector<DrawCommandIndirect> cmds = {};
		std::vector<int> cmdpos = {};

		AmountOfChunkBeingRendered = CommandBuffer.getDrawCommandsSorted(&cmds, &cmdpos);

		SSBO.InsertSubData(0, (AmountOfChunkBeingRendered * 3) * sizeof(int), cmdpos.data());
		IBO.InsertSubData(0, AmountOfChunkBeingRendered * sizeof(DrawCommandIndirect), cmds.data());
	}
	else {
		int Index = 1;

		for (auto& data_ : RenderList) {
			auto& data = data_.second;

			float deltaX = data.x - Position.x;
			float deltaY = data.y - Position.y;
			float deltaZ = data.z - Position.z;

			float dx2 = deltaX * deltaX / (RenderDistance * RenderDistance);
			float dy2 = deltaY * deltaY / (VerticalRenderDistance * VerticalRenderDistance);
			float dz2 = deltaZ * deltaZ / (RenderDistance * RenderDistance);

			if (dx2 + dy2 + dz2 < 1.f) {
				if (Frustum.SphereInFrustum((float)(data.x << 4), (float)(data.y << 4), (float)(data.z << 4), 22.3f)) { // << 4 means multiply by 4
					DrawCommands[Index - 1].set(data.MemSize >> 3, 1, data.MemOffset >> 3, Index);
					ChunkShaderPos[(Index - 1) * 3 + 0] = data.x;
					ChunkShaderPos[(Index - 1) * 3 + 1] = data.y;
					ChunkShaderPos[(Index - 1) * 3 + 2] = data.z;
				
					Index++;
				}
			}
		}


		Index--;

		AmountOfChunkBeingRendered = Index;

		SSBO.InsertSubData(0, (AmountOfChunkBeingRendered * 3) * sizeof(int), ChunkShaderPos.data());
		IBO.InsertSubData(0, AmountOfChunkBeingRendered * sizeof(DrawCommandIndirect), DrawCommands.data());
		
	}

}

void ChunkDrawBatch::UpdateCommandBufferSize() {
	DrawCommands.resize(RenderList.size());
	ChunkShaderPos.resize(RenderList.size() * 3);
}

bool ChunkDrawBatch::AddChunkVertices(std::vector<uint32_t>& Data, int x, int y, int z) {
	ChunkID id = getChunkID(x, y, z);

	size_t DataSize = Data.size() * sizeof(uint32_t);

	ChunkMemoryPoolOffset MemoryPoolBlockData = MemoryPool.AddChunk(Data, x, y, z, NULL);

	if (MemoryPoolBlockData.MemOffset == ULLONG_MAX) {
		return false;
	}
	
	RenderList[MemoryPoolBlockData.MemOffset] = MemoryPoolBlockData;
	RenderListOffsetLookup[id] = MemoryPoolBlockData.MemOffset;
	AmountOfChunks++;
	UpdateCommands = true;
//	CommandBuffer.AddDrawCommand(DrawCommandIndirect(MemoryPoolBlockData.MemSize >> 3, 1, MemoryPoolBlockData.MemOffset >> 3, 0),x,y,z);
	return true;
}

void ChunkDrawBatch::DeleteChunkVertices(ChunkID id) {
	if (MemoryPool.CheckChunk(id)) {
		ChunkMemoryPoolOffset ChunkMemOffset = MemoryPool.GetChunkMemoryPoolOffset(id);
		if (ChunkMemOffset.MemOffset == ULLONG_MAX) {
			Logger.LogError("Chunk Batch", "Failed to delete chunk: " + std::to_string(id));
			return;
		}
		RenderList.erase(ChunkMemOffset.MemOffset);
		MemoryPool.DeleteChunk(id);
		RenderListOffsetLookup.erase(id);
//		CommandBuffer.DeleteDrawCommand(ChunkIDToPOS(id));
		AmountOfChunks--;
		UpdateCommands = true;
		
	}
}

void ChunkDrawBatch::SetMaxSize(size_t size) {
	MaxBufferSize = size;
}

void ChunkDrawBatch::Bind() {
	Array.Bind();
	IBO.Bind();
	MemoryPool.buffer.Bind();
	SSBO.Bind();
	SSBO.BindBase(2);
}

void ChunkDrawBatch::Unbind() {
	SSBO.UnbindBase(2);
	SSBO.Unbind();
	IBO.Unbind();
	MemoryPool.buffer.Unbind();
	Array.Unbind();
}

void ChunkDrawBatch::Draw() {
	glMultiDrawArraysIndirect(GL_TRIANGLES, (GLvoid*)0, (GLsizei)AmountOfChunkBeingRendered, 0);
}

void ChunkDrawBatch::Defrager(int Cycles) {
	int i = 0;

	int FragmentCount = MemoryPool.MemoryPool.GetFreeSpaceFragmentCount();

	if (FragmentCount == 1) {
		return;
	}

	Cycles = std::min(Cycles, FragmentCount - 1);

	while (i < Cycles) {
		i++;

		if (MemoryPool.MemoryPool.FreeMemoryBlocks.size() == 1) {
			return;
		}

		MemoryManagement::MemoryBlock FreeMemoryBlock = MemoryPool.MemoryPool.FreeMemoryBlocks.begin()->second;

		int FreeSpaceOffset = FreeMemoryBlock.Offset;

		std::map<size_t, MemoryManagement::MemoryBlock>::iterator Reserve = MemoryPool.MemoryPool.ReservedMemoryBlocks.getIterator(FreeMemoryBlock.Size + FreeMemoryBlock.Offset);

		MemoryManagement::MemoryBlock ReservedBlock = Reserve->second;

		ChunkID ID = MemoryPool.MemoryChunkOffset[ReservedBlock.Offset];

		glm::ivec3 pos = ChunkIDToPOS(ID);

		DeleteChunkVertices(ID);

		MemoryPool.buffer.CopyTo(MemoryPool.StaggingBuffer, ReservedBlock.Offset, 0, ReservedBlock.Size);
		
		//Add chunk back

		ChunkMemoryPoolOffset MemoryPoolBlockData = MemoryPool.AddChunkStaggingBuffer(ID, NULL, FreeSpaceOffset, ReservedBlock.Size);

		RenderList[MemoryPoolBlockData.MemOffset] = MemoryPoolBlockData;
		RenderListOffsetLookup[ID] = MemoryPoolBlockData.MemOffset;
	//	CommandBuffer.AddDrawCommand(DrawCommandIndirect(MemoryPoolBlockData.MemSize >> 3, 1, MemoryPoolBlockData.MemOffset >> 3, 0), pos.x, pos.y, pos.z);
		AmountOfChunks++;
		UpdateCommands = true;
	}
}



void ChunkDrawBatch::ErrorCheck() {
	std::unordered_set<ChunkID> UsedChunk;
	std::string Logs = "";

	for (const auto& c : RenderList) {
		if (UsedChunk.count(getChunkID(c.second.x, c.second.y, c.second.z))) {
			Logs += "Chunk: " + std::to_string(c.first) + "\n";
		}
		else {
			UsedChunk.insert(getChunkID(c.second.x, c.second.y, c.second.z));
		}

	}

	Logger.LogDebug("Chunk Batch", "Errors: \n" + Logs);


}