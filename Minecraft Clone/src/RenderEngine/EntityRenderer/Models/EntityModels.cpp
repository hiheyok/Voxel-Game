#include "EntityModel.h"
#include <iostream>

Model::ModelData Model::RectangularPrism::GetVertices() {
	ModelData model;

	model.Vertices.insert(model.Vertices.begin(), {

		0.f,0.f,0.f, //0
		size.x,0.f,0.f , //1
		0.f,size.y,0.f, //2
		size.x,size.y,0.f, //3
		0.f,0.f,size.z, //4
		size.x,0.f,size.z, //5
		0.f,size.y,size.z, //6
		size.x,size.y,size.z, //7
		});


	model.Indices.insert(model.Indices.begin(), {
		//Front
		0,1,3,
		3,2,0,
		//Back
		4,7,5,
		7,4,6,
		//Right
		1,3,7,
		7,5,1,
		//Left
		0,6,2,
		6,0,4,
		//Top
		2,3,7,
		2,7,6,
		//Bottom
		0,5,1,
		5,0,4
		});

	model.VerticesCount = 8;

	return model;
}
