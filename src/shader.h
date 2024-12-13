#pragma once

#include "common.h"

struct VertexUniforms {
	Mat4 model;
	Mat4 view;
	Mat4 projection;
	Mat4 MVP;
};

using VertexShader = void(*)(Vertex&, const VertexUniforms&, void* m);

void DefaultVertexShader(Vertex& v, const VertexUniforms& u, void* m);

