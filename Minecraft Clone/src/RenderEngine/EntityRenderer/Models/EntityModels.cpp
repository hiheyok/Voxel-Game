#include "EntityModel.h"
#include <iostream>

Model::ModelData Model::RectangularPrism::GetVertices() {
	ModelData model;

	model.Vertices.insert(model.Vertices.begin(), {
		//Front
		0.f,0.f,0.f,			// 0
		size.x, 0.f,0.f,		// 1
		size.x, size.y, 0.f,	// 2
		0.f, size.y, 0.f,		// 3

		//Back
		0.f,0.f,size.z,			// 4
		size.x, 0.f,size.z,		// 5
		size.x, size.y, size.z,	// 6
		0.f, size.y, size.z,	// 7

		//Right
		size.x,0.f,0.f,			// 8
		size.x,size.y,0.f,		// 9
		size.x,size.y,size.z,	// 10
		size.x,0.f,size.z,		// 11

		//Left
		0.f,0.f,0.f,			// 12
		0.f,size.y,0.f,			// 13
		0.f,size.y,size.z,		// 14
		0.f,0.f,size.z,			// 15

		//Top
		0.f,size.y,0.f,			// 16
		size.x,size.y,0.f,		// 17
		size.x,size.y,size.z,	// 18
		0.f,size.y,size.z,		// 19

		//Bottom
		0.f,0.f,0.f,			// 20
		size.x,0.f,0.f,			// 21
		size.x,0.f,size.z,		// 22
		0.f,0.f,size.z			// 23
		});


	model.Indices.insert(model.Indices.begin(), {
		//Front
		0,1,2,
		2,3,0,

		//Back
		4,6,5,
		6,4,7,

		//Right
		8,9,10,
		10,11,8,

		//Left
		12,14,13,
		14,12,15,

		//Top
		16,17,18,
		18,19,16,

		//Bottom
		20,22,21,
		22,20,23,
		});

	model.VerticesCount = 24;

	return model;
}
