#pragma once

#include "raylib-cpp.hpp"
#include <vector>

struct Tri {
	RVector3 p[3];
};

struct TriMesh {
	int vertCount = 0;
	std::vector<Tri> tris;
	std::vector<Tri> vertexNormals;

	static bool LoadFromOBJ(const std::string& path, TriMesh& outMesh, const RMatrix& modelMatrix = RMatrix::Identity());
};