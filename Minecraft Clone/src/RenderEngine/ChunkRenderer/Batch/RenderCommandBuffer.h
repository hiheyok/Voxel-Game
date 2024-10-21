#pragma once
#include <unordered_map>
#include <vector>
#include <glm/vec3.hpp>
#include <stack>
#include "../../../Level/Chunk/ChunkID.h"
#include <iostream>
#include "../../../Level/Typenames.h"
struct DrawCommandIndirect {
	unsigned int  count = 0;
	unsigned int  instanceCount = 0;
	unsigned int  first = 0;
	unsigned int  baseInstance = 0;

	DrawCommandIndirect() {

	}

	DrawCommandIndirect(uint32_t count_, uint32_t instanceCount_, uint32_t first_, uint32_t baseInstance_) : count(count_), instanceCount(instanceCount_), first(first_), baseInstance(baseInstance_) {

	}

	void set(uint32_t count_, uint32_t instanceCount_, uint32_t first_, uint32_t baseInstance_) {
		count = count_;
		instanceCount = instanceCount_;
		first = first_;
		baseInstance = baseInstance_;
	}
};

namespace CMDGraph {

	static const int GetOppositeSide(int side) {
		return ((side >> 1) * 2) + ((~side) & 0b1);
	}

	const int NX = 0x00;
	const int PX = 0x01;
	const int NY = 0x02;
	const int PY = 0x03;
	const int NZ = 0x04;
	const int PZ = 0x05;

	struct CommandPtr {
		CommandPtr* neighbor[6]{ nullptr,nullptr,nullptr,nullptr,nullptr,nullptr };
		DrawCommandIndirect cmd;
		glm::ivec3 position;
		int visitID = 0;

		CommandPtr() {
			for (int i = 0; i < 6; i++) {
				neighbor[i] = nullptr;
			}
		}

		CommandPtr(DrawCommandIndirect cmd_) : cmd(cmd_) {
			for (int i = 0; i < 6; i++) {
				neighbor[i] = nullptr;
			}
		}

		CommandPtr(DrawCommandIndirect cmd_, glm::ivec3 pos) : cmd(cmd_), position(pos) {
			for (int i = 0; i < 6; i++) {
				neighbor[i] = nullptr;
			}
		}

		~CommandPtr() {
		//	std::cout << "destroy\n";
		}

		void PrepareDelete() {
			for (int i = 0; i < 3; i++) {
				int index1 = i * 2;
				int index2 = i * 2 + 1;

				if ((neighbor[index1] != nullptr) && (neighbor[index2] != nullptr)) {
					neighbor[index1]->neighbor[index2] = neighbor[index2];
					neighbor[index2]->neighbor[index1] = neighbor[index1];
				}
				else {
					if ((neighbor[index1] != nullptr)) {
						neighbor[index1]->neighbor[index2] = nullptr;
					}
					if ((neighbor[index2] != nullptr)) {
						neighbor[index2]->neighbor[index1] = nullptr;
					}
				}
			}
		}

		float GetSquareDistance(glm::vec3 v) {
			return GetSquareDistance(v.x, v.y, v.z);
		}

		float GetSquareDistance(float px, float py, float pz) {
			return (px - (float)position.x) * (px - (float)position.x) + (py - (float)position.y) * (py - (float)position.y) + (pz - (float)position.z) * (pz - (float)position.z);
		}

		void SetNeighbor(CommandPtr* node, int side, int depth = 0) {

			// Early exit if trying to set a neighbor to itself
			if (node == this) return;

			if ((neighbor[side] == nullptr) && (node->neighbor[GetOppositeSide(side)] == nullptr)) {
				neighbor[side] = node;
				node->neighbor[GetOppositeSide(side)] = this;
				return;
			}

			CommandPtr* p1 = this;
			CommandPtr* p2 = node;
			CommandPtr* p3 = neighbor[side];
			CommandPtr* p4 = node->neighbor[GetOppositeSide(side)];

			//Temp set to null
			neighbor[side] = nullptr;
			if (p3 != nullptr) {
				p3->neighbor[GetOppositeSide(side)] == nullptr;
			}
			node->neighbor[GetOppositeSide(side)] = nullptr;
			if (p4 != nullptr) {
				p4->neighbor[side] = nullptr;
			}

			if ((p4 != nullptr) && (p3 == nullptr)) {
				int side_p4 = p1->GetSideLocation(p4->position);

				if (side_p4 == side) { //OK
					p1->SetNeighbor(p4, side, depth + 1);

					p4->neighbor[side] = p2;
					p2->neighbor[GetOppositeSide(side)] = p4;
				}
				else {
					p1->SetNeighbor(p4, side_p4, depth + 1);

					p1->neighbor[side] = p2;
					p2->neighbor[GetOppositeSide(side)] = p1;
				}
				return;
			}

			

			if ((p4 == nullptr) && (p3 != nullptr)) {
				//first check if p4 or p3 is closer to p1

				float p1_distance_p3 = p1->GetSquareDistance(p3->position);
				float p1_distance_p2 = p1->GetSquareDistance(p2->position);

				if (p1_distance_p3 <= p1_distance_p2) { //this implies that p3 is the closest is is kept the same
					
					//reconnects p3 and p1
					p1->neighbor[side] = p3;
					p3->neighbor[GetOppositeSide(side)] = p1;

					int side_p2 = p3->GetSideLocation(p2->position);
					p3->SetNeighbor(p2, side_p2, depth + 1);

				}
				else {
					p1->neighbor[side] = p2; //no data lost occur here since if p4 is null, it implies p2 neighbor is null
					p2->neighbor[GetOppositeSide(side)] = p1;

					int side_p3 = p2->GetSideLocation(p3->position);
					p2->SetNeighbor(p3, side_p3, depth + 1);
				}
				return;
			}

			//return;
			//last case (p4 != nullptr) && (p3 != nullptr)
				
			float p1_distance_p3 = p1->GetSquareDistance(p3->position);
			float p1_distance_p2 = p1->GetSquareDistance(p2->position);
			float p1_distance_p4 = p1->GetSquareDistance(p4->position);

			if ((p1_distance_p3 <= p1_distance_p2) && (p1_distance_p3 <= p1_distance_p4)) {  // p3 is closest to p1 //OK
				//reconnects p3 and p1
				p1->neighbor[side] = p3;
				p3->neighbor[GetOppositeSide(side)] = p1;

				//connect p4 to p3 at side p3_side_p4
				int p3_side_p4 = p3->GetSideLocation(p4->position);
				p3->SetNeighbor(p4, p3_side_p4, depth + 1);
				
				p4->neighbor[side] = p2;
				p2->neighbor[GetOppositeSide(side)] = p4;
				

				return;
			}
			

			if ((p1_distance_p4 <= p1_distance_p3) && (p1_distance_p4 <= p1_distance_p2)) {  //p4 is closest

				//connect p4 and p1
				p1->SetNeighbor(p4, side, depth + 1);

				if (p1_distance_p3 >= p1_distance_p2) {
					int p4_side_p2 = p4->GetSideLocation(p2->position);

					if (p4_side_p2 == side) {
						p4->SetNeighbor(p2, p4_side_p2, depth + 1);

						int p2_side_p3 = p2->GetSideLocation(p3->position);
						p2->SetNeighbor(p3, p2_side_p3, depth + 1);

					}
					else {
						p4->SetNeighbor(p2, p4_side_p2, depth + 1);
						int p4_side_p3 = p4->GetSideLocation(p3->position);
						p4->SetNeighbor(p3, p4_side_p3, depth + 1);
					}
				}
				else {
					std::cout << "test\n";
				}

				
				return;
				
			}

			if ((p1_distance_p2 <= p1_distance_p3) && (p1_distance_p2 <= p1_distance_p4)) { //if p2 is closer, it implies p4 is behind p1

				//connect p4 and p1
				int p1_side_p4 = p1->GetSideLocation(p4->position);
				p1->SetNeighbor(p4, p1_side_p4, depth + 1);

				//connect p2 to p1
				p1->SetNeighbor(p2, side, depth + 1);

				//connect p3 to p2
				int p2_side_p3 = p2->GetSideLocation(p3->position);
				p2->SetNeighbor(p3, p2_side_p3, depth + 1);
			}
		}

		const int GetSideLocation(glm::ivec3 p2) {
			glm::ivec3 relativePosition = p2 - position;

			int x = relativePosition.x;
			int y = relativePosition.y;
			int z = relativePosition.z;

			if ((abs(x) >= abs(z)) && (abs(x) >= abs(y))) {
				if (x >= 0) {
					return PX;
				}
				else {
					return NX;
				}
			}
			if ((abs(z) >= abs(x)) && (abs(z) >= abs(y))) {
				if (z >= 0) {
					return PZ;
				}
				else {
					return NZ;
				}
			}
			if ((abs(y) >= abs( x)) && (abs(y) >= abs(z))) {
				if (y >= 0) {
					return PY;
				}
				else {
					return NY;
				}
			}
			
		}
	};
}

class ChunkDrawCommandBuffer {
public:
	void Initialize() {

	}

	void AddDrawCommand(DrawCommandIndirect cmd, int x, int y, int z) {
		CMDGraph::CommandPtr* ptr = new CMDGraph::CommandPtr(cmd, glm::ivec3(x, y, z));
		CommandMap[getChunkID(x, y, z)] = ptr;
		AddToGraph(ptr);
	}

	void UpdateCurrentPosition(glm::vec3 NewPosition) {
		CurrentPosition = NewPosition;
	}

	int getDrawCommandsSorted(std::vector<DrawCommandIndirect>* cmds, std::vector<int>* cmdPos) {
		
		for (auto& arr : Commands) {
			arr.clear();
		}

		for (auto& arr : CommandsPosition) {
			arr.clear();
		}
		count = 0;
		int size = 0;
		TraverseTree(root, 0);

		CurrentNodeVisitID++;

		if (CurrentNodeVisitID == 16) {
			CurrentNodeVisitID = 0;
		}

		for (int i = Commands.size() - 1; i >= 0; i--) {
			size += Commands[i].size();
			cmds->insert(cmds->end(), Commands[i].begin(), Commands[i].end());
			cmdPos->insert(cmdPos->end(), CommandsPosition[i].begin(), CommandsPosition[i].end());
		}

		for (int i = 0; i < size; i++) {
			cmds->at(i).baseInstance = i + 1;
		}

		//std::cout << count << ", " << CommandMap.size() << "\n";

		return size;
	}

	void DeleteDrawCommand(glm::ivec3 pos) {
		uint64_t ID = getChunkID(pos);
		CMDGraph::CommandPtr* node = CommandMap[ID];
		CommandMap.erase(ID);

		if (node != root) {
			node->PrepareDelete();
			delete node;
			return;
		}

		float Distances[6]{0.f,0.f,0.f,0.f,0.f,0.f};

		CMDGraph::CommandPtr* RootNeighbors[6]{nullptr};

		for (int i = 0; i < 6; i++) {
			RootNeighbors[i] = node->neighbor[i];
		}
		node->PrepareDelete();
		delete node;

		for (int i = 0; i < 6; i++) {
			if (RootNeighbors[i] == nullptr) {
				continue;
			}

			Distances[i] = RootNeighbors[i]->GetSquareDistance(CurrentPosition);
		}

		float min = FLT_MAX;

		for (int i = 0; i < 6; i++) {
			if (Distances[i] == 0.f) {
				continue;
			}

			min = std::min(min, Distances[i]);
		}

		if (min == FLT_MAX) {
			root = nullptr;
			return;
		}

		for (int i = 0; i < 6; i++) {
			if (Distances[i] == min) {
				root = RootNeighbors[i];
				break;
			}
		}

	}

	int count = 0;

	void TraverseTree(CMDGraph::CommandPtr* node, int depth) {
		if (node == nullptr) {
			return;
		}

		if (node->visitID == CurrentNodeVisitID) {
			return;
		}

		node->visitID = CurrentNodeVisitID;

		if (Commands.size() <= depth) {
			Commands.emplace_back();
			CommandsPosition.emplace_back();
			CommandsLayer.emplace_back();
		}

		Commands[depth].push_back(node->cmd);
		CommandsPosition[depth].push_back(node->position.x);
		CommandsPosition[depth].push_back(node->position.y);
		CommandsPosition[depth].push_back(node->position.z);
	//	CommandsLayer[depth].push_back(depth);
		count++;
		for (int side = 0; side < 6; side++) {
 			TraverseTree(node->neighbor[side], depth + 1);
		}
	}

	
private:
	bool verifyTree(CMDGraph::CommandPtr* node) { //debug
		bool isCorrect = true;

		if (node->visitID == CurrentNodeVisitID) {
			return isCorrect;
		}

		for (int i = 0; i < 6; i++) {
			if (node->neighbor[i] == nullptr) {
				continue;
			}

			if (node->neighbor[i]->neighbor[CMDGraph::GetOppositeSide(i)] != node) {
				return false;
			}
			else {
				if (node->neighbor[i]->visitID != CurrentNodeVisitID) {
					isCorrect = verifyTree(node->neighbor[i]);
				}
				
			}
		}

		node->visitID = CurrentNodeVisitID;

		return isCorrect;
	}

	std::vector<std::vector<DrawCommandIndirect>> Commands; //[Tree Depth][Draw Commands]
	std::vector<std::vector<int>> CommandsPosition; //[Tree Depth][Draw Commands Position]
	std::vector<std::vector<int>> CommandsLayer; //[Tree Depth][Layer]

	int CurrentNodeVisitID = 1;

	void AddToGraph(CMDGraph::CommandPtr* node) {
		//If root doesn't exist add to root
		if (root == nullptr) {
			root = node;
			return;
		}

		//If root exist, check if the chunk is closer to the actual root if so change the root

		if (root->GetSquareDistance(CurrentPosition) > node->GetSquareDistance(CurrentPosition)) {
			int loc = node->GetSideLocation(root->position);

			CMDGraph::CommandPtr* tmp = root;
			root = node;
			node->SetNeighbor(tmp, loc);
			return;
		}

		CMDGraph::CommandPtr* curr = root;

		while (true) {
			int side = curr->GetSideLocation(node->position);

			if (curr->neighbor[side] == nullptr) {
				curr->SetNeighbor(node, side);
				return;
			}

			if (curr->neighbor[side]->GetSquareDistance(node->position) >= curr->GetSquareDistance(node->position)) {
				curr->SetNeighbor(node, side);
				return;
			}

			curr = curr->neighbor[side];
		}
	}

	glm::vec3 CurrentPosition = glm::vec3(0.f,0.f,0.f);

	CMDGraph::CommandPtr* root = nullptr;
	FastHashMap<uint64_t, CMDGraph::CommandPtr*> CommandMap;
};


