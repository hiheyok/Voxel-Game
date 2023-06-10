#include <tgmath.h>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#define _CRTDBG_MAP_ALLOC
using namespace glm;

double toRadians(double x)
{
    return x* 0.01745329251;
}

double FindDistanceNoSqrt(double x, double y, double z, double x1, double y1, double z1) {
	return ((double)x - (double)x1) * ((double)x - (double)x1) + ((double)y - (double)y1) * ((double)y - (double)y1) + ((double)z - (double)z1) * ((double)z - (double)z1);
}

double FindDistanceNoSqrt(int x, int y, int x1, int y1) {
	return pow(x - x1, 2) + pow(y - y1, 2);
}

double FindDistance(int x, int y, int z, int x1, int y1, int z1) {
	return sqrt(pow(x - x1, 2) + pow(y - y1, 2) + pow(z - z1, 2));
}

double FindDistance(int x, int y, int x1, int y1) {
	return sqrt(pow(x - x1, 2) + pow(y - y1, 2));
}

template <typename T> double FindDistance(T x, T x1) {
	return sqrt(pow((double)x - x1, 2));
}

double FindDistance(ivec3 pos0, ivec3 pos1) {
	return sqrt(pow((double)pos0.x - pos1.x, 2) + pow((double)pos0.y - pos1.y, 2) + pow((double)pos0.z - pos1.z, 2));
}

double FindDistance(ivec2 pos0, ivec2 pos1) {
	return sqrt(pow((double)pos0.x - pos1.x, 2) + pow((double)pos0.y - pos1.y, 2));
}

double FindDistance(vec3 pos0, vec3 pos1) {
	return sqrt(pow((double)pos0.x - pos1.x, 2) + pow((double)pos0.y - pos1.y, 2) + pow((double)pos0.z - pos1.z, 2));
}

double FindDistance(vec2 pos0, vec2 pos1) {
	return sqrt(pow((double)pos0.x - pos1.x, 2) + pow((double)pos0.y - pos1.y, 2));
}

