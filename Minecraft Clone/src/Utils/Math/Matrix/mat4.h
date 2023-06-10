#pragma once

struct mat4 {
	mat4() {}
	float& operator[](int index) {
		return data[index];
	}
	float data[16]{1.0f};
};