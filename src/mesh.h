#pragma once

#include "typedefs.h"
#include <vector>
#include <string>

struct Tri {
	Vec3 p[3];
};

struct TriMesh {
	int vertCount = 0;
	std::vector<Tri> tris;
	std::vector<Tri> vertexNormals;

	static bool LoadFromOBJ(const std::string& path, TriMesh& outMesh, const Mat4& modelMatrix = Mat4(1.0f));
};