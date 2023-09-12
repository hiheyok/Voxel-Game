#include "ChunkBatch.h"
#include <iterator>
#include <optional>

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

	VBO.SetUsage(GL_STATIC_DRAW);
	IBO.SetUsage(GL_DYNAMIC_DRAW);
	SSBO.SetUsage(GL_DYNAMIC_COPY);

	Array.Bind();

	VBO.SetMaxSize(MaxBufferSize);
	VBO.InitializeData();
	IBO.SetMaxSize((size_t)(MaxBufferSize / 1000));
	IBO.InitializeData();

	VBO.Bind();
	Array.EnableAttriPTR(0, 1, GL_FLOAT, GL_FALSE, 2, 0);
	Array.EnableAttriPTR(1, 1, GL_FLOAT, GL_FALSE, 2, 1);
	VBO.Unbind();
	Array.Unbind();

	SSBO.SetMaxSize((size_t)(MaxBufferSize / 1000));
	SSBO.InitializeData();

	
	GapIteratorsSortedOffset[0] = InsertSpace.insert(pair<size_t, size_t>((size_t)MaxBufferSize, 0ULL));
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

	Frustum.CalculateFrustum(camera);

	ivec3 Position(floor(camera->Position.x / 16.f), floor(camera->Position.y / 16.f), floor(camera->Position.z / 16.f));

	int Index = 1;

	for (int i = 0; i < RenderList.size(); i++) {
		DataBufferAddress data = RenderList[(RenderList.size() - 1) - i];
		if (FindDistanceNoSqrt(data.x, data.y, data.z, Position.x, Position.y, Position.z) < pow(RenderDistance, 2)) {
			if (Frustum.SphereInFrustum((float)data.x * 16, (float)data.y * 16, (float)data.z * 16, (float)32)) {
				DrawCommandIndirect cmd;
				cmd.count = (unsigned int)data.size / (sizeof(unsigned int) * 2);
				cmd.instanceCount = 1;
				cmd.first = (unsigned int)data.offset / (sizeof(unsigned int) * 2);
				cmd.baseInstance = Index;
				DrawCommands.push_back(cmd);
				ChunkShaderPos.emplace_back(data.x);
				ChunkShaderPos.emplace_back(data.y);
				ChunkShaderPos.emplace_back(data.z);
				Index++;
			}
		}
	}

	SSBO.InsertSubData(0, ChunkShaderPos.size() * sizeof(int), ChunkShaderPos.data());
	IBO.InsertSubData(0, DrawCommands.size() * sizeof(DrawCommandIndirect), DrawCommands.data());
}

bool ChunkDrawBatch::AddChunkVertices(std::vector<unsigned int> Data, bool insertBack, int x, int y, int z) {
	ChunkID id = getChunkID(x, y, z);

	size_t DataSize = Data.size() * sizeof(unsigned int);

	DataBufferAddress RenderingData;
	RenderingData.x = x;
	RenderingData.y = y;
	RenderingData.z = z;
	RenderingData.size = DataSize;

	std::multimap<size_t, size_t>::iterator it;
	if (insertBack) {
		it = --InsertSpace.end();
	}
	else {
		auto gapIt = GapIteratorsSortedOffset.begin();

		for (int i = 1; i < InsertSpace.size(); i++) {
			if (gapIt->second->first <= DataSize) {
				gapIt++;
			}
			else {
				break;
			}
		}
		it = gapIt->second;
	}
	



	if (it->first >= DataSize) {

		size_t InsertIndex = 0;
		bool InsertBegin = false;
			
		if (RenderList.size() != 0) {
			if ((RenderList[0].offset >= it->second + DataSize)) {
				InsertIndex = 0;
				InsertBegin = true;
			}
			else {
				InsertIndex = FindClosestRenderObjIndex(it->second);

				if (InsertIndex == 0 && RenderList.size() != 1) {
					int _offset = RenderList[1].offset;
				}
			}
		}

		if ((RenderList.size() == 0) || InsertBegin) {
			RenderingData.offset = it->second;
			MemoryUsage += DataSize;
			VBO.InsertSubData(RenderingData.offset, Data.size() * sizeof(unsigned int), Data.data());
			RenderList.insert(RenderList.begin() + InsertIndex, RenderingData);
			RenderListOffsetLookup[id] = RenderingData.offset;
			UpdateCommands = true;
			//std::cout << DataSize << ", " << RenderList[0].size << "\n";
		}
		else {
			RenderingData.offset = it->second;
			MemoryUsage += DataSize;
			VBO.InsertSubData(RenderingData.offset, Data.size() * sizeof(unsigned int), Data.data());
			RenderList.insert(RenderList.begin() + InsertIndex + 1, RenderingData);
			RenderListOffsetLookup[id] = RenderingData.offset;
			UpdateCommands = true;
		}

		//Update Insert Map

		size_t iteratorOffset = it->second;
		size_t iteratorSize = it->first;

		std::optional<ChunkID> lastID;
		std::optional<ChunkID> frontID;

		if ((RenderList.size() > 1) && (!InsertBegin)) {  //Case: If the chunk isn't the first chunk in the list
			DataBufferAddress& lastChunk = RenderList[InsertIndex];
			lastID = getChunkID(lastChunk.x, lastChunk.y, lastChunk.z);
			GapIteratorsSortedOffset.erase(iteratorOffset);
			
			InsertSpaceIteratorsFront.erase(lastID.value());

			if (InsertIndex + 2 != RenderList.size()) {
				DataBufferAddress& nextChunk = RenderList[InsertIndex + 2];
				frontID = getChunkID(nextChunk.x, nextChunk.y, nextChunk.z);

				InsertSpaceIteratorsBack.erase(frontID.value());
			}

			InsertSpace.erase(it);
		}
		else { //First chunk in the list; doesnt delete InsertSpaceIteratorsFront cause there is nothing else yet, but attempts to clear InsertSpaceIteratorsBack


			if (RenderList.size() > 1) {
				auto& nextChunk = RenderList[1];

				frontID = getChunkID(nextChunk.x, nextChunk.y, nextChunk.z);

				InsertSpaceIteratorsBack.erase(frontID.value());
				GapIteratorsSortedOffset.erase(iteratorOffset);
				InsertSpace.erase(it);
				std::cout << iteratorSize << "\n";
			}
			else {
				GapIteratorsSortedOffset.erase(iteratorOffset);
				InsertSpace.erase(it);
			}

			
		}

		if (iteratorSize > DataSize) {
			size_t offset = iteratorOffset + DataSize;
			size_t size = iteratorSize - DataSize;

			std::multimap<size_t, size_t>::iterator nextIterator = InsertSpace.insert(pair<size_t, size_t>(size, offset));
			InsertSpaceIteratorsFront[id] = nextIterator;

			if (frontID.has_value()) {
				InsertSpaceIteratorsBack[frontID.value()] = nextIterator;
			}

			GapIteratorsSortedOffset[offset] = nextIterator;
		}
		return true;
	}

	getLogger()->LogDebug("Chunk Renderer", "Draw Batch Out of Memory: " + std::to_string(MemoryUsage));
	return false;
}

void ChunkDrawBatch::DeleteChunkVertices(ChunkID id) {
	if (RenderListOffsetLookup.count(id)) {
		size_t index = GetRenderObjIndex(RenderListOffsetLookup[id]);

		MemoryUsage -= RenderList[index].size;

		size_t offset = 0;
		size_t size = 0;

		bool FrontGap = InsertSpaceIteratorsFront.count(id);
		bool BackGap = InsertSpaceIteratorsBack.count(id);
		bool FirstChunkInIndex = (index == 0);

		std::multimap<size_t, size_t>::iterator FrontIt;
		std::multimap<size_t, size_t>::iterator BackIt;

		if (FrontGap) {
			FrontIt = InsertSpaceIteratorsFront[id];
		}

		if (BackGap) {
			BackIt = InsertSpaceIteratorsBack[id];
		}

		auto& RemovalChunk = RenderList[index];

		//Manages gap space
		if (FrontGap) {
			offset = RemovalChunk.offset;
			size = RemovalChunk.size + FrontIt->first;
		}

		if (BackGap) {
			if (FrontGap) {
				offset = BackIt->second;
				size = BackIt->first + RemovalChunk.size + FrontIt->first;
			}
			else {
				offset = BackIt->second;
				size = BackIt->first + RemovalChunk.size;
			}
		}

		if (FrontGap && BackGap) {
			offset = RemovalChunk.offset;
			size = RemovalChunk.size;
		}
		//Erase Iterators

		std::cout << "-\n";

		if (FrontGap) {
			GapIteratorsSortedOffset.erase(FrontIt->second);
			InsertSpace.erase(FrontIt);
			InsertSpaceIteratorsFront.erase(id);
		}

		if (BackGap) {
			GapIteratorsSortedOffset.erase(BackIt->second);
			InsertSpace.erase(BackIt);
			InsertSpaceIteratorsBack.erase(id);
		}
		std::cout << "-\n";

		//Add new iterators

		auto NewIterator = InsertSpace.insert(std::pair<size_t, size_t>(size, offset));

		GapIteratorsSortedOffset[offset] = NewIterator;

		if (index + 1 != RenderList.size()) { //Checks if there is chunk after the new gap
			auto& FrontChunk = RenderList[index + 1];
			ChunkID frontID = getChunkID(FrontChunk.x, FrontChunk.y, FrontChunk.z);
			InsertSpaceIteratorsBack[frontID] = NewIterator;
		}

		if (index - 1 != -1) {
			auto& BackChunk = RenderList[index - 1]; //Checks if there is chunk before the new gap
			ChunkID backID = getChunkID(BackChunk.x, BackChunk.y, BackChunk.z);
			InsertSpaceIteratorsFront[backID] = NewIterator;
		}

		RenderList.erase(RenderList.begin() + index);
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

size_t ChunkDrawBatch::GetRenderObjIndex(size_t Offset) {

	if (RenderList.size() == 0)
		return 0;

	size_t low = 0;
	size_t high = RenderList.size() - 1;

	while (true) {
		size_t mid = (size_t)((float)(low + high) * 0.5f);
		if (Offset == RenderList[mid].offset) {
			return mid;
		}
		if (low == high) {
			break;
		}
		if (Offset > RenderList[mid].offset) {
			low = mid + 1;
		}
		else {
			high = mid - 1;
		}
	}

	if (Offset == RenderList[high].offset) {
		return high;
	}
	return 0ULL;

}
size_t ChunkDrawBatch::FindClosestRenderObjIndex(size_t Offset) {

	if (RenderList.size() == 0)
		return 0;

	size_t low = 0;
	size_t high = RenderList.size() - 1;

	while (true) {
		size_t mid = (size_t)((float)(low + high) * 0.5f);
		if (Offset == RenderList[mid].offset + RenderList[mid].size) {
			return mid;
		}
		if (low == high) {
			break;
		}
		if (Offset > RenderList[mid].offset + RenderList[mid].size) {
			low = mid + 1;
		}
		else {
			high = mid - 1;
		}
		
	}

	if (Offset == RenderList[high].offset) {
		return high;
	}
	return 0ULL;
}

void ChunkDrawBatch::Defrager(int iterations) {
	int i = 0;

	if (GapIteratorsSortedOffset.size() == 1) { //space already fully defragged
		return;
	}

	while (i < iterations) {
		auto it = GapIteratorsSortedOffset.begin();

		if (GapIteratorsSortedOffset.size() == 1) {
			return;
		}

		size_t offset = it->first;
		size_t size = it->second->first;

		int index = GetRenderObjIndex(offset + size);

		auto& data = RenderList[index];

		size_t bufferOffset = data.offset;
		size_t bufferSize = data.size;

		ChunkID id = getChunkID(data.x, data.y, data.z);

		std::vector<uint32_t> VertexData;
		VertexData.resize(bufferSize / sizeof(unsigned int));
		VBO.getData(VertexData.data(), bufferOffset, bufferSize);

		DeleteChunkVertices(id);

		glm::ivec3 pos = ChunkIDToPOS(id);
		AddChunkVertices(VertexData, false, data.x, data.y, data.z);

		//

		i++;

		if (i > iterations) {
			return;
		}
	}



}