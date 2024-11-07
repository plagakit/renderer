#pragma once

#define GLM_ENABLE_EXPERIMENTAL // for quaternions
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>

//double g_FPS = 0.0f;
extern double g_fps;
extern double g_dt;

const int SCREEN_WIDTH = 1280;
const int SCREEN_HEIGHT = 720;

const float PI = 3.14159265358979323846;

using Vec3 = glm::vec3;
using Vec4 = glm::vec4;
using Quat = glm::quat;
using Mat4 = glm::mat4;

struct Color {
	unsigned char r;
	unsigned char g;
	unsigned char b;
	unsigned char a;

	Color(unsigned char _r, unsigned char _g, unsigned char _b, unsigned char _a) :
		r(_r), g(_g), b(_b), a(_a)
	{}
};

const Color BLACK = { 0, 0, 0, 255};
const Color WHITE = { 255, 255, 255, 255 };

struct Transform 
{
	Vec3 position = Vec3(0.0f, 0.0f, 0.0f);
	Quat rotation = Quat(0.0f, 0.0f, 0.0f, 1.0f);
	Vec3 scale = Vec3(1.0f, 1.0f, 1.0f);

	Mat4 GetTransform() const
	{
		Mat4 m = glm::scale(Mat4(1.0f), scale);
		m = glm::toMat4(rotation) * m;
		m = glm::translate(m, position);
		return m;
	}
};