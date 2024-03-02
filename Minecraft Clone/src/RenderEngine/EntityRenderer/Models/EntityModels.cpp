#include "EntityModel.h"
#include <iostream>
#include "../../../World/Typenames.h"

Model::ModelData Model::RectangularPrism::GetVertices() {
	ModelData model;

	model.Vertices.insert(model.Vertices.begin(), {
		//Front
		0.f,0.f,0.f,			// 0
		UV_MAP[FRONT].p0.x,UV_MAP[FRONT].p0.y,
		size.x, 0.f,0.f,		// 1
		UV_MAP[FRONT].p1.x,UV_MAP[FRONT].p0.y,
		size.x, size.y, 0.f,	// 2
		UV_MAP[FRONT].p1.x,UV_MAP[FRONT].p1.y,
		0.f, size.y, 0.f,		// 3
		UV_MAP[FRONT].p0.x,UV_MAP[FRONT].p1.y,

		//Back
		0.f,0.f,size.z,			// 4
		UV_MAP[BACK].p0.x,UV_MAP[BACK].p0.y,
		size.x, 0.f,size.z,		// 5
		UV_MAP[BACK].p1.x,UV_MAP[BACK].p0.y,
		size.x, size.y, size.z,	// 6
		UV_MAP[BACK].p1.x,UV_MAP[BACK].p1.y,
		0.f, size.y, size.z,	// 7
		UV_MAP[BACK].p0.x,UV_MAP[BACK].p1.y,

		//Right
		size.x,0.f,0.f,			// 8
		UV_MAP[RIGHT].p0.x,UV_MAP[RIGHT].p0.y,
		size.x,size.y,0.f,		// 9
		UV_MAP[RIGHT].p1.x,UV_MAP[RIGHT].p0.y,
		size.x,size.y,size.z,	// 10
		UV_MAP[RIGHT].p1.x,UV_MAP[RIGHT].p1.y,
		size.x,0.f,size.z,		// 11
		UV_MAP[RIGHT].p0.x,UV_MAP[RIGHT].p1.y,

		//Left
		0.f,0.f,0.f,			// 12
		UV_MAP[LEFT].p0.x,UV_MAP[LEFT].p0.y,
		0.f,size.y,0.f,			// 13
		UV_MAP[LEFT].p1.x,UV_MAP[LEFT].p0.y,
		0.f,size.y,size.z,		// 14
		UV_MAP[LEFT].p1.x,UV_MAP[LEFT].p1.y,
		0.f,0.f,size.z,			// 15
		UV_MAP[LEFT].p0.x,UV_MAP[LEFT].p1.y,

		//Top
		0.f,size.y,0.f,			// 16
		UV_MAP[TOP].p0.x,UV_MAP[TOP].p0.y,
		size.x,size.y,0.f,		// 17
		UV_MAP[TOP].p1.x,UV_MAP[TOP].p0.y,
		size.x,size.y,size.z,	// 18
		UV_MAP[TOP].p1.x,UV_MAP[TOP].p1.y,
		0.f,size.y,size.z,		// 19
		UV_MAP[TOP].p0.x,UV_MAP[TOP].p1.y,

		//Bottom
		0.f,0.f,0.f,			// 20
		UV_MAP[BOTTOM].p0.x,UV_MAP[BOTTOM].p0.y,
		size.x,0.f,0.f,			// 21
		UV_MAP[BOTTOM].p1.x,UV_MAP[BOTTOM].p0.y,
		size.x,0.f,size.z,		// 22
		UV_MAP[BOTTOM].p1.x,UV_MAP[BOTTOM].p1.y,
		0.f,0.f,size.z,			// 23
		UV_MAP[BOTTOM].p0.x,UV_MAP[BOTTOM].p1.y
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
