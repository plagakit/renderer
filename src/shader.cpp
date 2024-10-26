#include "shader.h"

void DefaultVertexShader(
	Vertex& vertex, 
	VertexAttributes& attributes, 
	const VertexUniforms& uniforms)
{
	vertex = vertex.Transform(uniforms.modelViewMatrix);
}
