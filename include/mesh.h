#pragma once

#include "raylib-cpp.hpp"
#include <vector>

struct Tri {
	RVector3 p[3];
};

struct TriMesh {
	std::vector<Tri> tris;
	std::vector<Tri> vertexNormals;
};

//union TriIdx {
//	std::size_t i[3];
//	struct {
//		std::size_t i0;
//		std::size_t i1;
//		std::size_t i2;
//	};
//};
//
//struct TriMesh {
//	std::vector<TriIdx> indices;
//	std::vector<RVector3> vertices;
//	std::vector<RVector3> normals;
//};