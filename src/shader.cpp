#include "shader.h"

void DefaultVertexShader(
	Vertex& vertex, 
	VertexAttributes& attributes, 
	const VertexUniforms& uniforms)
{

	Vec4 v = uniforms.modelViewMatrix * Vec4(vertex, 1.0f);
	vertex = Vec3(v.x, v.y, v.z);
}
