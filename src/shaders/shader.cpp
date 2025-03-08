#include "shader.h"

//#include "renderer.h"

void DefaultVertexShader::Process(Vertex& v)
{
	v.p = MVP * v.p;
}

void DefaultVertexShader::SetModel(const Mat4& m)
{
	model = m;
	MVP = projection * view * model;
}

void DefaultVertexShader::SetView(const Mat4& v)
{
	view = v;
	MVP = projection * view * model;
}

void DefaultVertexShader::SetProjection(const Mat4& p)
{
	projection = p;
	MVP = projection * view * model;
}

//#include <random>
//#define randf() (float)rand() / RAND_MAX
//
//void DefaultVertexShader(
//	Vertex& v, 
//	const VertexUniforms& uniforms,
//	void* m)
//{
//	//v.p += Vec4(randf() * 0.05f, randf() * 0.05f, randf() * 0.05f, 1.0f);
//
//	v.p = uniforms.MVP * v.p;
//	v.n = Vec3(uniforms.model * Vec4(v.n, 0));
//
//	//float f = 50.0f;
//	//v.p = Vec4(
//	//	floorf(v.p.x * f) / f,
//	//	floorf(v.p.y * f) / f,
//	//	floorf(v.p.z * f) / f,
//	//	v.p.w
//	//);
//		
//}

void PS1VertexShader::Process(Vertex& v)
{
	v.p = MVP * v.p;
	v.p.x = floorf(v.p.x * rounding) / rounding;
	v.p.y = floorf(v.p.y * rounding) / rounding;
	v.p.z = floorf(v.p.z * rounding) / rounding;
}

float PS1VertexShader::GetRounding()
{
	return rounding;
}

void PS1VertexShader::SetMVP(const Mat4& mat)
{
	MVP = mat;
}

void PS1VertexShader::SetRounding(float r)
{
	rounding = r;
}
