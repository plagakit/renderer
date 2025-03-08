#pragma once

#include "common.h"

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

class FragmentShader
{
public:

};

class DefaultFragmentShader : public FragmentShader
{

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