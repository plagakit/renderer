#pragma once

#include "common.h"
#include <vector>
#include <string>

struct Mesh {
	int vertCount = 0;
	std::vector<Vertex> v;

	static bool LoadFromOBJ(std::string path, Mesh& outMesh, bool isRHS = true, const Mat4& modelMatrix = Mat4(1.0f));
};

struct Texture {

	int width;
	int height;
	int channels;
	unsigned char* data;

	Texture(const char* path);
	~Texture();

	Texture() = delete;
	Texture(const Texture& t) = delete;
	Texture operator=(const Texture& t) = delete;

};