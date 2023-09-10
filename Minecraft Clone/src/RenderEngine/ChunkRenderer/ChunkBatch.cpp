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

bool ChunkDrawBatch::AddChunkVertices(std::vector<unsigned int> Data, int x, int y, int z) {
	ChunkID id = getChunkID(x, y, z);

	size_t DataSize = Data.size() * sizeof(unsigned int);

	DataBufferAddress RenderingData;
	RenderingData.x = x;
	RenderingData.y = y;
	RenderingData.z = z;
	RenderingData.size = DataSize;

	auto it = InsertSpace.begin();

	for (int i = 1; i < InsertSpace.size(); i++) {
		if (it->first <= DataSize) {
			it++;
		}
		else {
			break;
		}
		
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

					if (_offset  <= DataSize + it->second) {
						std::cout << "ERR\n";
					}
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
		}
		else {
			if (InsertIndex + 1 < RenderList.size()) {
				if (RenderList[InsertIndex + 1].offset == it->second) {
					std::cout << "ERRRRR\n";
				}
			}
			

			RenderingData.offset = it->second;
			MemoryUsage += DataSize;
			VBO.InsertSubData(RenderingData.offset, Data.size() * sizeof(unsigned int), Data.data());
			RenderList.insert(RenderList.begin() + InsertIndex + 1, RenderingData);
			RenderListOffsetLookup[id] = RenderingData.offset;
			UpdateCommands = true;
		}

		if (InsertIndex == 0) {
			std::cout << "PAUSE\n";
		}

		//Update Insert Map

		size_t iteratorOffset = it->second;
		size_t iteratorSize = it->first;

		std::optional<ChunkID> lastID;

		if ((RenderList.size() != 1) && (!InsertBegin)) {  //Case: If the chunk isn't the first chunk in the list
			DataBufferAddress& lastChunk = RenderList[InsertIndex];
			lastID = getChunkID(lastChunk.x, lastChunk.y, lastChunk.z);
		}
		else { //First chunk in the list; doesnt delete InsertSpaceIterators cause there is nothing else yet
			InsertSpace.erase(it);
		}

		if (iteratorSize != DataSize) {
			size_t offset = iteratorOffset + DataSize;
			size_t size = iteratorSize - DataSize;

			if (lastID.has_value()) {
				InsertSpace.erase(InsertSpaceIterators[lastID.value()]);
				InsertSpaceIterators.erase(lastID.value());
			}

			std::multimap<size_t, size_t>::iterator nextIterator = InsertSpace.insert(pair<size_t, size_t>(size, offset));
			InsertSpaceIterators[id] = nextIterator;
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

		if (InsertSpaceIterators.count(id)) { //Checks if there's a gap in front of it
			if (index == 0) { //case; the chunk being deleted is the first one and there is a gap in front of it
				size = InsertSpaceIterators[id]->first + RenderList[index].size;

				InsertSpace.erase(InsertSpaceIterators[id]);
				InsertSpaceIterators.erase(id);

				(void)InsertSpace.insert(pair<size_t, size_t>(size, offset));
			}
			else { //Case; the chunk being deleted isn't in the first index
				auto& lastChunk = RenderList[index - 1];
				ChunkID lastID = getChunkID(lastChunk.x, lastChunk.y, lastChunk.z);

				if (InsertSpaceIterators.count(lastID)) { //Case: There is empty space before and after the chunk being deleted
					offset = InsertSpaceIterators[lastID]->second;
					size = InsertSpaceIterators[id]->first + RenderList[index].size + InsertSpaceIterators[lastID]->first;

					InsertSpace.erase(InsertSpaceIterators[id]);
					InsertSpace.erase(InsertSpaceIterators[lastID]);
					InsertSpaceIterators.erase(id);
					InsertSpaceIterators.erase(lastID);

					std::multimap<size_t, size_t>::iterator nextIterator = InsertSpace.insert(pair<size_t, size_t>(size, offset));

					InsertSpaceIterators[lastID] = nextIterator;
				}
				else { //Case: There is only empty space after
					offset = RenderList[index].offset;
					size = InsertSpaceIterators[id]->first + RenderList[index].size;

					InsertSpace.erase(InsertSpaceIterators[id]);
					InsertSpaceIterators.erase(id);

					std::multimap<size_t, size_t>::iterator nextIterator = InsertSpace.insert(pair<size_t, size_t>(size, offset));

					InsertSpaceIterators[lastID] = nextIterator;
				}
			}
		}
		else { //Case; if the chunk being deleted (might) only have a gap before it
			bool hasGapBefore = false;

			std::optional<ChunkID> lastID;

			if (index != 0) {
				DataBufferAddress& lastChunk = RenderList[index - 1];
				lastID = getChunkID(lastChunk.x, lastChunk.y, lastChunk.z);

				if (InsertSpaceIterators.count(lastID.value())) {
					hasGapBefore = true;
				}
			}

			if (hasGapBefore) { //Case: There is a gap before the chunk being deleted
				std::multimap<size_t, size_t>::iterator lastIterator = InsertSpaceIterators[lastID.value()];

				offset = lastIterator->second;
				size = lastIterator->first + RenderList[index].size;

				InsertSpace.erase(InsertSpaceIterators[lastID.value()]);
				InsertSpaceIterators.erase(lastID.value());

				std::multimap<size_t, size_t>::iterator nextIterator = InsertSpace.insert(pair<size_t, size_t>(size, offset));

				InsertSpaceIterators[lastID.value()] = nextIterator;
			}
			else {
				std::multimap<size_t, size_t>::iterator nextIterator = InsertSpace.insert(pair<size_t, size_t>(RenderList[index].size, RenderList[index].offset));

				if (lastID.has_value()) {//Case: not first chunk in the index
					InsertSpaceIterators[lastID.value()] = nextIterator;
				}

				
			}
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