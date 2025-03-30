#pragma once

#include "common.h"
#include "mesh.h"

class VertexShader
{
public:
	virtual void Process(Vertex& v) = 0;
};

class DefaultVertexShader : public VertexShader
{
public:
	void Process(Vertex& v) override;

	void SetModel(const Mat4& model);
	void SetView(const Mat4& view);
	void SetProjection(const Mat4& projection);

private:
	Mat4 model, view, projection, MVP;
};

class PS1VertexShader : public VertexShader
{
public:
	void Process(Vertex& v) override;

	float GetRounding();
	void SetMVP(const Mat4& MVP);
	void SetRounding(float rounding);

private:
	Mat4 MVP;
	float rounding = 50.0f;
};

class WarbleVertexShader : public VertexShader
{
public:
	void Process(Vertex& v) override
	{
		float t = (sinf(time * 0.01f + v.p.y * 10.0f) + 1.0f) * 0.5f * 0.5f + 0.5f;
		v.p.x *= t;
		v.p.z *= t;
		v.p = MVP * v.p;
		//v.p.x *= (((float)rand() / RAND_MAX) * 2.0f + 1.0f) * 0.1f + 1.0f;
	}

	void SetMVP(const Mat4& mat) { MVP = mat; }
	void SetTime(float t) { time = t;  }

private:
	Mat4 MVP;
	float time;

};