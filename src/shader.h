#pragma once

#include "typedefs.h"

struct VertexAttributes {
	Vec3 normal;
};

struct VertexUniforms {
	Mat4 modelMatrix;
	Mat4 modelViewMatrix;
};

using Vertex = Vec3;
using VertexShader = void(*)(Vertex&, VertexAttributes&, const VertexUniforms&);

void DefaultVertexShader(Vertex& v, VertexAttributes& va, const VertexUniforms& uniform);

