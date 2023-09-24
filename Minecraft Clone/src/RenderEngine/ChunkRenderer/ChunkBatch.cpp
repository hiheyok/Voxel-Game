#include "ChunkBatch.h"
#include <iterator>

using namespace glm;
using namespace std;

void ChunkDrawBatch::SetupBuffers() {
	VBO.GenBuffer();
	IBO.GenBuffer();
	SSBO.GenBuffer();
	Array.GenArray();

	VBO.SetType(GL_ARRAY_BUFFER);
	IBO.SetType(GL_DRAW_INDIRECT_BUFFER);
	SSBO.SetType(GL_SHADER_STORAGE_BUFFER);

	VBO.SetUsage(GL_DYNAMIC_DRAW);
	IBO.SetUsage(GL_DYNAMIC_DRAW);
	SSBO.SetUsage(GL_DYNAMIC_COPY);

	Array.Bind();

	VBO.SetMaxSize(MaxBufferSize);
	VBO.InitializeData();
	IBO.SetMaxSize((size_t)(MaxBufferSize / 100));
	IBO.InitializeData();

	VBO.Bind();
	Array.EnableAttriPTR(0, 1, GL_FLOAT, GL_FALSE, 2, 0);
	Array.EnableAttriPTR(1, 1, GL_FLOAT, GL_FALSE, 2, 1);
	VBO.Unbind();
	Array.Unbind();

	SSBO.SetMaxSize((size_t)(MaxBufferSize / 100));
	SSBO.InitializeData();

	InsertSpace.insert(pair<size_t, size_t>((size_t)MaxBufferSize, 0ULL));
}

void ChunkDrawBatch::Reset() {
	VBO.ResetBuffer();
	IBO.ResetBuffer();
	SSBO.ResetBuffer();
	Array.ResetArray();
	SetupBuffers();
}

void ChunkDrawBatch::Cleanup() {
	VBO.Delete();
	VBO.Delete();
	IBO.Delete();
	SSBO.Delete();
	Array.~VertexArray();

	DrawCommands.clear();
	RenderList.clear();
	ChunkShaderPos.clear();
	RenderListOffsetLookup.clear();

}

void ChunkDrawBatch::GenDrawCommands(int RenderDistance) {
	if (!UpdateCommands)
		return;

	DrawCommands.clear();
	ChunkShaderPos.clear();

	DrawCommands.reserve(lastRenderSize);
	ChunkShaderPos.reserve(lastRenderSize * 3);

	Frustum.CalculateFrustum(camera);

	ivec3 Position(floor(camera->Position.x / 16.f), floor(camera->Position.y / 16.f), floor(camera->Position.z / 16.f));

	int Index = 1;

	for (const auto& data_ : RenderList) {
		auto& data = data_.second;
		if (FindDistanceNoSqrt(data.x, data.y, data.z, Position.x, Position.y, Position.z) < RenderDistance * RenderDistance) {
			if (Frustum.SphereInFrustum((float)(data.x << 4), (float)(data.y << 4), (float)(data.z << 4), 32.f)) { // << 4 means multiply by 4

					DrawCommands.emplace_back(data.size >> 3, 1, data.offset >> 3, Index);
					ChunkShaderPos.insert(ChunkShaderPos.end(), { data.x, data.y, data.z });

				
				Index++;
			}
		}
	}

	lastRenderSize = DrawCommands.size();

	SSBO.InsertSubData(0, ChunkShaderPos.size() * sizeof(int), ChunkShaderPos.data());
	IBO.InsertSubData(0, DrawCommands.size() * sizeof(DrawCommandIndirect), DrawCommands.data());
}

bool ChunkDrawBatch::AddChunkVertices(std::vector<unsigned int> Data, int x, int y, int z) {
	ChunkID id = getChunkID(x, y, z);

	size_t DataSize = Data.size() * sizeof(unsigned int);

	DataBufferAddress RenderingData;
	RenderingData.x = x;
	RenderingData.y = y;
	RenderingData.z = z;
	RenderingData.size = DataSize;

	std::multimap<size_t, size_t>::iterator it;

	if (InsertSpace.size() > 1) {
		it = --(--InsertSpace.end());

		if (it->first < DataSize) {
			it++;
		}
	}
	else {
		it = InsertSpace.begin();
	}

	size_t iteratorOffset = it->second;
	size_t iteratorSize = it->first;

	if (iteratorSize >= DataSize) {
		MemoryUsage += DataSize;
		RenderingData.offset = iteratorOffset;
		VBO.InsertSubData(RenderingData.offset, Data.size() * sizeof(unsigned int), Data.data());
		RenderList[iteratorOffset] = RenderingData;
		RenderListOffsetLookup[id] = iteratorOffset;
		UpdateCommands = true;

		auto RenderListIterator = RenderList.find(iteratorOffset);
		//Update Insert Map


		ChunkID lastID = 0;
		ChunkID frontID = 0;

		bool lastIDB = false;
		bool frontIDB = false;

		if (RenderListIterator != RenderList.begin()) {
			RenderListIterator--;
			DataBufferAddress& lastChunk = RenderListIterator->second;
			lastID = getChunkID(lastChunk.x, lastChunk.y, lastChunk.z);
			lastIDB = true;
			InsertSpaceIteratorsFront.erase(lastID);
			RenderListIterator++;
		}

		if (RenderListIterator != (--RenderList.end())) {
			RenderListIterator++;
			DataBufferAddress& frontChunk = RenderListIterator->second;
			frontID = getChunkID(frontChunk.x, frontChunk.y, frontChunk.z);
			frontIDB = true;
			InsertSpaceIteratorsBack.erase(frontID);
			RenderListIterator--;
		}

		InsertSpace.erase(it);

		if (iteratorSize != DataSize) {
			size_t offset = iteratorOffset + DataSize;
			size_t size = iteratorSize - DataSize;

			std::multimap<size_t, size_t>::iterator nextIterator = InsertSpace.insert(pair<size_t, size_t>(size, offset));

			if (frontIDB) {
				InsertSpaceIteratorsBack[frontID] = nextIterator;
			}
			InsertSpaceIteratorsFront[id] = nextIterator;
		}

		return true;
	}

	getLogger()->LogDebug("Chunk Renderer", "Draw Batch Out of Memory: " + std::to_string(MemoryUsage) + " | " + std::to_string(it->first));
	return false;
}

void ChunkDrawBatch::DeleteChunkVertices(ChunkID id) {
	if (RenderListOffsetLookup.count(id)) {
		auto ChunkDataIterator = RenderList.find(RenderListOffsetLookup[id]);

		auto& RemovalChunk = ChunkDataIterator->second;

		MemoryUsage -= RemovalChunk.size;

		size_t offset = 0;
		size_t size = 0;

		bool FrontGap = false;
		bool BackGap = false;

		std::multimap<size_t, size_t>::iterator FrontIt;
		std::multimap<size_t, size_t>::iterator BackIt;

		if (InsertSpaceIteratorsFront.count(id)) {
			FrontIt = InsertSpaceIteratorsFront[id];
			FrontGap = true;
		}

		if (InsertSpaceIteratorsBack.count(id)) {
			BackIt = InsertSpaceIteratorsBack[id];
			BackGap = true;
		}

		//Manages gap space
		if ((FrontGap) && (!BackGap)) {
			offset = RemovalChunk.offset;
			size = RemovalChunk.size + FrontIt->first;
		}

		if ((FrontGap) && (BackGap)) {
			offset = BackIt->second;
			size = BackIt->first + RemovalChunk.size + FrontIt->first;
		}

		if ((!FrontGap) && (BackGap)) {
			offset = BackIt->second;
			size = BackIt->first + RemovalChunk.size;
		}

		if ((!FrontGap) && (!BackGap)) {
			offset = RemovalChunk.offset;
			size = RemovalChunk.size;
		}
		//Get Front and Back chunks

		bool FrontChunkB = false;
		bool BackChunkB = false;

		ChunkID FrontChunk;
		ChunkID BackChunk;

		if (ChunkDataIterator != (--RenderList.end())) {
			ChunkDataIterator++;
			auto& FrontGapChunk = ChunkDataIterator->second;
			FrontChunk = getChunkID(FrontGapChunk.x, FrontGapChunk.y, FrontGapChunk.z);
			FrontChunkB = true;
			ChunkDataIterator--;
		}

		if (ChunkDataIterator != RenderList.begin()) {
			ChunkDataIterator--;
			auto& BackGapChunk = ChunkDataIterator->second;
			BackChunk = getChunkID(BackGapChunk.x, BackGapChunk.y, BackGapChunk.z);
			BackChunkB = true;
			ChunkDataIterator++;
		}

		//Erase Iterators

		if (FrontGap) {

			if (FrontChunkB) {
				InsertSpaceIteratorsBack.erase(FrontChunk);
			}

			InsertSpaceIteratorsFront.erase(id);
			InsertSpace.erase(FrontIt);
		}

		if (BackGap) {

			if (BackChunkB) {
				InsertSpaceIteratorsFront.erase(BackChunk);
			}

			InsertSpaceIteratorsBack.erase(id);
			InsertSpace.erase(BackIt);
		}

		//Add new iterators

		auto NewIterator = InsertSpace.insert(std::pair<size_t, size_t>(size, offset));

		if (FrontChunkB) {
			InsertSpaceIteratorsBack[FrontChunk] = NewIterator;
		}

		if (BackChunkB) {
			InsertSpaceIteratorsFront[BackChunk] = NewIterator;
		}

		RenderList.erase(ChunkDataIterator);
		RenderListOffsetLookup.erase(id);
	}
}

void ChunkDrawBatch::SetMaxSize(size_t size) {
	MaxBufferSize = size;
}

void ChunkDrawBatch::Bind() {
	Array.Bind();
	IBO.Bind();
	VBO.Bind();
	SSBO.Bind();
	SSBO.BindBase(2);
}

void ChunkDrawBatch::Unbind() {
	SSBO.UnbindBase(2);
	SSBO.Unbind();
	IBO.Unbind();
	VBO.Unbind();
	Array.Unbind();
}

void ChunkDrawBatch::Draw() {
	glMultiDrawArraysIndirect(GL_TRIANGLES, (GLvoid*)0, (GLsizei)DrawCommands.size(), 0);
}

void ChunkDrawBatch::Defrager(int iterations) {
	int i = 0;

	if (InsertSpace.size() == 1) { //space already fully defragged
		return;
	}

	std::multimap<size_t, size_t>::iterator it;

	while (i < iterations) {

		if (InsertSpace.size() == 1) {
			return;
		}



		std::multimap<size_t, size_t>::iterator it0 = --(--InsertSpace.end());
		std::multimap<size_t, size_t>::iterator it1 = (--InsertSpace.end());

		if (it0->second > it1->second) {
			it = it1;
		}
		else {
			it = it0;
		}

		size_t offset = it->second;
		size_t size = it->first;

		auto& data = RenderList[offset + size];

		size_t bufferOffset = data.offset;
		size_t bufferSize = data.size;

		ChunkID id = getChunkID(data.x, data.y, data.z);

		std::vector<uint32_t> VertexData(bufferSize / sizeof(unsigned int));

		VBO.getData(VertexData.data(), bufferOffset, bufferSize);

		int x = data.x;
		int y = data.y;
		int z = data.z;

		DeleteChunkVertices(id);

		AddChunkVertices(VertexData, x, y, z);

		i++;
		if (i > iterations) {
			return;
		}
	}



}