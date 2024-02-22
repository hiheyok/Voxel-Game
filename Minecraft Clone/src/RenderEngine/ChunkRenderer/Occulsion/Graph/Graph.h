#pragma once

#include <list>

template <class T>
struct Node {
	T* Vertex;
	Node* Edge;
};

template <class T> 
class Graph {

private:
	std::list<Node> Nodes;
public:

	void InsertVertex(T* vertex) {

	}

	void InsertEdge(T* vertex0, T* vertex1) {

	}

	void DeleteEdge() {

	}

	void DeleteVertex(T* vertex) {

	}


};