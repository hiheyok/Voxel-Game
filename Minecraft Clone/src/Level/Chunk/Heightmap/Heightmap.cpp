#include "Heightmap.h"
#include <new> 
#include <iostream>

void Heightmap::Edit(int x, int z, int height) {
	data_[x * 16 + z] = height;
}

uint16_t Heightmap::Get(int x, int z) const {
	return data_[x * 16 + z];
}

std::vector<uint16_t> Heightmap::GetData() const {
	return data_;
}

void Heightmap::Init() {
	data_.resize(256);
	is_init_ = true;
}

void Heightmap::Clear() {
	data_.clear();
}