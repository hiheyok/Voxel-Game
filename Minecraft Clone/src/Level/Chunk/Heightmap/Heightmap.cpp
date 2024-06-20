#include "Heightmap.h"
#include <new> 
#include <iostream>

void Heightmap::edit(int x, int z, int height) {
	data[x * 16 + z] = height;
}

uint16_t Heightmap::get(int x, int z) {
	return data[x * 16 + z];
}

std::vector<uint16_t> Heightmap::getData() {
	return data;
}

void Heightmap::init() {
	data.resize(256);
	isInit = true;
}

void Heightmap::Clear() {
	data.clear();
}