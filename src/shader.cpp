#include "shader.h"

#include "renderer.h"

void DefaultVertexShader(
	Vertex& v, 
	const VertexUniforms& uniforms,
	void* m)
{
	if (m != nullptr)
	{
		((Renderer::Metrics*)m)->modelSpace.v0 = v.p;
		((Renderer::Metrics*)m)->worldSpace.v0 = uniforms.model * v.p;
		((Renderer::Metrics*)m)->viewSpace.v0 = uniforms.view * uniforms.model * v.p;
	}

	v.p = uniforms.MVP * v.p;
	v.n = Vec3(uniforms.model * Vec4(v.n, 0));
		
}
