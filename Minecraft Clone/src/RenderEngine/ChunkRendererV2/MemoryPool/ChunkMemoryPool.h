#pragma once
#include "../../OpenGL/Buffers/Buffer.h"
#include "../../../World/Chunk/ChunkID.h"
#include "../../../World/Typenames.h"
#include <vector>
#include <map>
#include <unordered_map>

struct ChunkMemoryPoolOffset {
	size_t MemOffset = NULL;
	size_t MemSize = NULL;
	int x = 0, y = 0, z = 0;

	ChunkMemoryPoolOffset() {};
	ChunkMemoryPoolOffset(size_t Offset, size_t Size) : MemOffset(Offset), MemSize(Size) {};
	ChunkMemoryPoolOffset(size_t Offset, size_t Size, int X, int Y, int Z) : MemOffset(Offset), MemSize(Size), x(X), y(Y), z(Z) {};
};


namespace MemoryManagement {

	struct MemoryBlock {
		MemoryBlock() {};
		MemoryBlock(size_t MemOffset, size_t MemSize) : Offset(MemOffset), Size(MemSize){}

		size_t Offset = NULL;
		size_t Size = NULL;
	};

	class BlockManagement {
	public:
		void Add(MemoryBlock block) {
			std::map<size_t, MemoryBlock>::iterator it = MemBlocks.insert(std::pair<size_t, MemoryBlock>(block.Offset, block)).first;
			MemBlocksIterators.insert(std::pair<size_t, std::map<size_t, MemoryBlock>::iterator>(block.Offset, it));
		}

		void Delete(size_t Offset) {
			if (!MemBlocksIterators.count(Offset)) {
				return;
			}
			std::map<size_t, MemoryBlock>::iterator it = MemBlocksIterators[Offset];
			Delete(it);
		}

		void Delete(std::map<size_t, MemoryBlock>::iterator it) {
			size_t offset = it->first;

			MemBlocks.erase(it);
			MemBlocksIterators.erase(offset);
		}

		std::map<size_t, MemoryBlock>::iterator getIterator(size_t offset) {
			return MemBlocksIterators.count(offset) ? MemBlocksIterators[offset] : end();
		}

		std::map<size_t, MemoryBlock>::iterator begin() {
			return MemBlocks.begin();
		}

		std::map<size_t, MemoryBlock>::iterator end() {
			return MemBlocks.end();
		}

		std::map<size_t, MemoryBlock>::reverse_iterator rbegin() {
			return MemBlocks.rbegin();
		}

		std::map<size_t, MemoryBlock>::reverse_iterator rend() {
			return MemBlocks.rend();
		}

		std::map<size_t, MemoryBlock>::iterator lower_bound(size_t k) { //Get latest element less than k
			auto it = MemBlocks.lower_bound(k);
			it--;
			return it;
		}

		std::map<size_t, MemoryBlock>::iterator upper_bound(size_t k) { //Get latest element greater than k
			return MemBlocks.upper_bound(k);
		}

		size_t size() {
			return MemBlocks.size();
		}

		int GetFragmentCount() {
			return MemBlocks.size();
		}

	private:
		std::map<size_t, MemoryBlock> MemBlocks;
		std::unordered_map<size_t, std::map<size_t, MemoryBlock>::iterator> MemBlocksIterators;
	};

	class MemoryPoolManager {
	public:
		void Initialize(size_t SpaceAvailable) {
			PoolSize = SpaceAvailable;
			FreeMemoryBlocks.Add(MemoryBlock(0, PoolSize));
			std::multimap<size_t, size_t>::iterator it = SortedMemSizes.insert(std::pair<size_t, size_t>(PoolSize, 0)); //use for deletions
			SortedMemSizesIterators[0] = it;
		}

		void AllocateSpace(size_t MemOffset, size_t MemSize) { //Assumes input are valid

			ReservedMemoryBlocks.Add(MemoryBlock(MemOffset, MemSize));

			std::map<size_t, MemoryBlock>::iterator left = FreeMemoryBlocks.getIterator(MemOffset);

			MemoryBlock current = left->second;

			FreeMemoryBlocks.Delete(left);
			SortedMemSizes.erase(SortedMemSizesIterators[left->first]);
			SortedMemSizesIterators.erase(left->first);

			if (current.Size == MemSize)
				return;

			size_t newFreeSpaceOffset = MemOffset + MemSize;
			size_t newFreeSpaceSize = current.Size - MemSize;

			MemoryBlock NewFreeBlock(newFreeSpaceOffset, newFreeSpaceSize);
			
			std::multimap<size_t, size_t>::iterator it = SortedMemSizes.insert(std::pair<size_t, size_t>(newFreeSpaceSize, newFreeSpaceOffset)); //use for deletions
			SortedMemSizesIterators[newFreeSpaceOffset] = it;

			FreeMemoryBlocks.Add(NewFreeBlock);
		}

		void DeallocateSpace(size_t MemOffset, size_t MemSize) {
			std::map<size_t, MemoryBlock>::iterator RightBlockFree = FreeMemoryBlocks.upper_bound(MemOffset);
			std::map<size_t, MemoryBlock>::iterator LeftBlockFree = FreeMemoryBlocks.lower_bound(MemOffset);

			bool isFreeBlockRight = true;
			bool isFreeBlockLeft = true;

			if (RightBlockFree == FreeMemoryBlocks.end()) {
				isFreeBlockRight = false;
			}
			else {
				if (RightBlockFree->second.Offset != MemOffset + MemSize) {
					isFreeBlockRight = false;
				}
			}

			if (LeftBlockFree == FreeMemoryBlocks.end()) {
				isFreeBlockLeft = false;
			}
			else {
				if (LeftBlockFree->second.Offset + LeftBlockFree->second.Size != MemOffset) {
					isFreeBlockLeft = false;
				}
			}

			size_t MemoryRight = 0, MemoryLeft = 0;

			if (isFreeBlockRight) {
				MemoryRight = RightBlockFree->second.Offset + RightBlockFree->second.Size;
			}
			else {
				MemoryRight = MemOffset + MemSize;
			}

			if (isFreeBlockLeft) {
				MemoryLeft = LeftBlockFree->second.Offset;
			}
			else {
				MemoryLeft = MemOffset;
			}

			size_t FreeMemoryOffset = MemoryLeft;
			size_t FreeMemorySize = MemoryRight - MemoryLeft;

			//Clear Free Spaces

			if (isFreeBlockLeft) {
				FreeMemoryBlocks.Delete(LeftBlockFree);
				SortedMemSizes.erase(SortedMemSizesIterators[LeftBlockFree->first]);
				SortedMemSizesIterators.erase(LeftBlockFree->first);
			}
				
			if (isFreeBlockRight) {
				FreeMemoryBlocks.Delete(RightBlockFree);
				SortedMemSizes.erase(SortedMemSizesIterators[RightBlockFree->first]);
				SortedMemSizesIterators.erase(RightBlockFree->first);
			}
				

			//Add Free Space
			FreeMemoryBlocks.Add(MemoryBlock(FreeMemoryOffset, FreeMemorySize));

			std::multimap<size_t, size_t>::iterator it = SortedMemSizes.insert(std::pair<size_t, size_t>(FreeMemorySize, FreeMemoryOffset)); //use for deletions
			SortedMemSizesIterators[FreeMemoryOffset] = it;

			//Clear Reserve Space
			ReservedMemoryBlocks.Delete(MemOffset);
		}

		size_t FindFreeSpace(size_t MemSize) {
			auto it1 = SortedMemSizes.lower_bound(MemSize);

			if (it1 != SortedMemSizes.end())
				return it1->second;
			return ULLONG_MAX;
		}

		int GetFreeSpaceFragmentCount() {
			return FreeMemoryBlocks.GetFragmentCount();
		}

		int GetReserveSpaceFragmentCount() {
			return ReservedMemoryBlocks.GetFragmentCount();
		}

		BlockManagement FreeMemoryBlocks;
		BlockManagement ReservedMemoryBlocks;

	private:
		size_t PoolSize = 0;
		
		std::multimap<size_t, size_t> SortedMemSizes; // BlockSize, BlockOffset
		std::unordered_map<size_t, std::multimap<size_t, size_t>::iterator> SortedMemSizesIterators; // Offset, iterator
	};

	struct MemoryPoolStatistics {
		double FragmentationRate = 0.0f;
		size_t MemoryUsage = 0;
		size_t FullMemoryUsage = 0;
	};
}

class ChunkGPUMemoryPool {
public:

	void Allocate(int MemoryPoolSize) {
		MEMORY_POOL_SIZE = MemoryPoolSize;

		buffer.Create(GL_ARRAY_BUFFER, MemoryPoolSize);

		StaggingBuffer.GenBuffer();
		StaggingBuffer.SetType(GL_COPY_WRITE_BUFFER);
		StaggingBuffer.SetUsage(GL_DYNAMIC_COPY);
		StaggingBuffer.SetMaxSize(10000000);
		StaggingBuffer.InitializeData();

		MemoryPool.Initialize(MEMORY_POOL_SIZE);
	}

	void DeleteChunk(int x, int y, int z) {
		DeleteChunk(getChunkID(x, y, z));
	}

	void DeleteChunk(ChunkID id) {
		if (!ChunkMemoryOffsets.count(id)) {
			Logger.LogDebug("GPU Memory Pool","Attempted to delete non-existant chunk with ID " + std::to_string(id));
			return;
		}

		ChunkMemoryPoolOffset ChunkOffsetData = ChunkMemoryOffsets[id];
		MemoryPool.DeallocateSpace(ChunkOffsetData.MemOffset, ChunkOffsetData.MemSize);
		ChunkMemoryOffsets.erase(id);
		if (UpdatedChunkMemoryOffsets.count(id)) {
			UpdatedChunkMemoryOffsets.erase(id);
		}

		MemoryChunkOffset.erase(ChunkOffsetData.MemOffset);

		Statistics.MemoryUsage -= ChunkOffsetData.MemSize;
		Update();
	}

	ChunkMemoryPoolOffset AddChunk(std::vector<uint32_t>& vertices, int x, int y, int z, int side) { //assumes vertices.size() != 0
		size_t BlockSize = vertices.size() * sizeof(uint32_t);
		size_t BlockOffset = MemoryPool.FindFreeSpace(BlockSize);

		ChunkID ID = getChunkID(x, y, z);

		if (BlockOffset == ULLONG_MAX) { //Check if it is out of space
			Logger.LogError("GPU Memory Pool", "Out of space!");
			return ChunkMemoryPoolOffset(ULLONG_MAX, ULLONG_MAX);
		}

		ChunkMemoryPoolOffset ChunkMemoryBlock;
		ChunkMemoryBlock.MemOffset = BlockOffset;
		ChunkMemoryBlock.MemSize = BlockSize;
		ChunkMemoryBlock.x = x;
		ChunkMemoryBlock.y = y;
		ChunkMemoryBlock.z = z;

		MemoryPool.AllocateSpace(BlockOffset, BlockSize);

		//Send Vertices Data into GPU
		buffer.InsertData(BlockOffset, BlockSize, vertices.data());

		//Store Memory Offset
		ChunkMemoryOffsets[ID] = ChunkMemoryBlock;
		MemoryChunkOffset[BlockOffset] = ID;

		Statistics.MemoryUsage += BlockSize;
		Update();
		return ChunkMemoryBlock;
	}

	bool DefragMemoryPool(int Cycles) {
		int i = 0;

		int FragmentCount = MemoryPool.GetFreeSpaceFragmentCount();

		if (FragmentCount == 1) {
			return false;
		}
		
		Cycles = std::min(Cycles, FragmentCount - 1);

		while (i < Cycles) {
			i++;

			if (MemoryPool.FreeMemoryBlocks.size() == 1) {
				return true;
			}

			MemoryManagement::MemoryBlock FreeMemoryBlock = MemoryPool.FreeMemoryBlocks.begin()->second;

			int FreeSpaceOffset = FreeMemoryBlock.Offset;

			std::map<size_t, MemoryManagement::MemoryBlock>::iterator Reserve = MemoryPool.ReservedMemoryBlocks.getIterator(FreeMemoryBlock.Size + FreeMemoryBlock.Offset);
			
			MemoryManagement::MemoryBlock ReservedBlock = Reserve->second;

			ChunkID ID = MemoryChunkOffset[ReservedBlock.Offset];

			DeleteChunk(ID);

			buffer.CopyTo(StaggingBuffer, ReservedBlock.Offset, 0, ReservedBlock.Size);

			UpdatedChunkMemoryOffsets[ID] = AddChunkStaggingBuffer(ID, NULL, FreeSpaceOffset, ReservedBlock.Size);
		}

		return true;
	}

	ChunkMemoryPoolOffset GetChunkMemoryPoolOffset(int x, int y, int z) {
		return GetChunkMemoryPoolOffset(getChunkID(x, y, z));
	}

	ChunkMemoryPoolOffset GetChunkMemoryPoolOffset(ChunkID ID) {
		if (ChunkMemoryOffsets.count(ID)) {
			return ChunkMemoryOffsets[ID];
		}	
		return ChunkMemoryPoolOffset(ULLONG_MAX, ULLONG_MAX, 666, 666, 666);
	}

	std::vector<ChunkMemoryPoolOffset> GetUpdatedChunks() { //Returns chunks that has been defragged with their new offsets
		std::vector<ChunkMemoryPoolOffset> Chunks(UpdatedChunkMemoryOffsets.size());

		int i = 0;

		for (auto c : UpdatedChunkMemoryOffsets) {
			Chunks[i] = c.second;
			i++;
		}

		UpdatedChunkMemoryOffsets.clear();

		return Chunks;
	}

	bool CheckChunk(ChunkID ID) {
		return ChunkMemoryOffsets.count(ID);
	}

	MemoryManagement::MemoryPoolStatistics Statistics;
	Buffer StaggingBuffer;
	BufferStorage buffer;

	MemoryManagement::MemoryPoolManager MemoryPool;
private:

	ChunkMemoryPoolOffset AddChunkStaggingBuffer(ChunkID ID, int side, uint64_t BlockOffset, uint64_t BlockSize) { //assumes vertices.size() != 0
		glm::ivec3 pos = ChunkIDToPOS(ID);
		
		ChunkMemoryPoolOffset ChunkMemoryBlock;
		ChunkMemoryBlock.MemOffset = BlockOffset;
		ChunkMemoryBlock.MemSize = BlockSize;
		ChunkMemoryBlock.x = pos.x;
		ChunkMemoryBlock.y = pos.y;
		ChunkMemoryBlock.z = pos.z;

		MemoryPool.AllocateSpace(BlockOffset, BlockSize);

		buffer.CopyFrom(StaggingBuffer, 0, BlockOffset, BlockSize);

		ChunkMemoryOffsets[ID] = ChunkMemoryBlock;
		UpdatedChunkMemoryOffsets[ID] = ChunkMemoryBlock;
		MemoryChunkOffset[BlockOffset] = ID;

		Statistics.MemoryUsage += BlockSize;
		Update();
		return ChunkMemoryBlock;
	}

	void Update() {
		if (MemoryPool.ReservedMemoryBlocks.size() == 0) {
			return;
		}

		Statistics.FullMemoryUsage = MemoryPool.ReservedMemoryBlocks.rbegin()->first + MemoryPool.ReservedMemoryBlocks.rbegin()->second.Size;
		if (Statistics.FullMemoryUsage != 0) {
			Statistics.FragmentationRate = (double)Statistics.MemoryUsage / (double)Statistics.FullMemoryUsage;
		}
	}

	std::unordered_map<ChunkID, ChunkMemoryPoolOffset> ChunkMemoryOffsets;
	std::unordered_map<size_t, ChunkID> MemoryChunkOffset;

	std::unordered_map<ChunkID, ChunkMemoryPoolOffset> UpdatedChunkMemoryOffsets;

	int MEMORY_POOL_SIZE = 0;

};