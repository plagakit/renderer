#pragma once

#include <raylib-cpp.hpp>

struct VertexAttributes {
	RVector3 normal;
};

struct VertexUniforms {
	RMatrix modelMatrix;
	RMatrix modelViewMatrix;
};

using Vertex = RVector3;
using VertexShader = void(*)(Vertex&, VertexAttributes&, const VertexUniforms&);

void DefaultVertexShader(Vertex& v, VertexAttributes& va, const VertexUniforms& uniform);

