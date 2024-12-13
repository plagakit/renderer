 #pragma once

#define GLM_ENABLE_EXPERIMENTAL // for quaternions
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <array>
#include <cstring>

//double g_FPS = 0.0f;
extern double g_fps;
extern double g_dt;

const int SCREEN_WIDTH = 1280;
const int SCREEN_HEIGHT = 720;

const float PI = 3.14159265358979323846f;
//const Vec3 ZERO = Vec3(0, 0, 0);
//const Vec3 LEFT = Vec3(-1, 0, 0);
//const Vec3 RIGHT = Vec3(1, 0, 0);
//const Vec3 DOWN = Vec3(0, -1, 0);
//const Vec3 UP = Vec3(0, 1, 0);
//const Vec3 BACK = Vec3(0, 0, -1);
//const Vec3 FORWARD = Vec3(0, 0, 1);

using Vec2 = glm::vec2;
using Vec3 = glm::vec3;
using Vec4 = glm::vec4;
using Quat = glm::quat;
using Mat4 = glm::mat4;

constexpr std::array<uint32_t, 104> TO_SRGB8_TABLE = {
	0x0073000d, 0x007a000d, 0x0080000d, 0x0087000d, 0x008d000d, 0x0094000d, 0x009a000d, 0x00a1000d,
	0x00a7001a, 0x00b4001a, 0x00c1001a, 0x00ce001a, 0x00da001a, 0x00e7001a, 0x00f4001a, 0x0101001a,
	0x010e0033, 0x01280033, 0x01410033, 0x015b0033, 0x01750033, 0x018f0033, 0x01a80033, 0x01c20033,
	0x01dc0067, 0x020f0067, 0x02430067, 0x02760067, 0x02aa0067, 0x02dd0067, 0x03110067, 0x03440067,
	0x037800ce, 0x03df00ce, 0x044600ce, 0x04ad00ce, 0x051400ce, 0x057b00c5, 0x05dd00bc, 0x063b00b5,
	0x06970158, 0x07420142, 0x07e30130, 0x087b0120, 0x090b0112, 0x09940106, 0x0a1700fc, 0x0a9500f2,
	0x0b0f01cb, 0x0bf401ae, 0x0ccb0195, 0x0d950180, 0x0e56016e, 0x0f0d015e, 0x0fbc0150, 0x10630143,
	0x11070264, 0x1238023e, 0x1357021d, 0x14660201, 0x156601e9, 0x165a01d3, 0x174401c0, 0x182401af,
	0x18fe0331, 0x1a9602fe, 0x1c1502d2, 0x1d7e02ad, 0x1ed4028d, 0x201a0270, 0x21520256, 0x227d0240,
	0x239f0443, 0x25c003fe, 0x27bf03c4, 0x29a10392, 0x2b6a0367, 0x2d1d0341, 0x2ebe031f, 0x304d0300,
	0x31d105b0, 0x34a80555, 0x37520507, 0x39d504c5, 0x3c37048b, 0x3e7c0458, 0x40a8042a, 0x42bd0401,
	0x44c20798, 0x488e071e, 0x4c1c06b6, 0x4f76065d, 0x52a50610, 0x55ac05cc, 0x5892058f, 0x5b590559,
	0x5e0c0a23, 0x631c0980, 0x67db08f6, 0x6c55087f, 0x70940818, 0x74a007bd, 0x787d076c, 0x7c330723
};

struct Color {
	unsigned char r;
	unsigned char g;
	unsigned char b;
	unsigned char a;

	//static unsigned char LinearToSRGB(float val)
	//{
	//	uint32_t MAXV_BITS = 0x3F7FFFFF; // 1.0 - f32::EPSILON
	//	uint32_t MINV_BITS = 0x39000000; // 2^(-13)
	//	float minv = *reinterpret_cast<float*>(&MINV_BITS);
	//	float maxv = *reinterpret_cast<float*>(&MAXV_BITS);

	//	float input = val;
	//	if (input <= minv)	input = minv;
	//	if (input > maxv)	input = maxv;

	//	uint32_t fu = *reinterpret_cast<uint32_t*>(&input);
	//	uint32_t entry = TO_SRGB8_TABLE[((fu - MINV_BITS) >> 20)];

	//	uint32_t bias = (entry >> 16) << 9;
	//	uint32_t scale = entry & 0xFFFF;
	//	uint32_t t = (fu >> 15) & 0xFF;

	//	uint32_t res = (bias + scale + t) >> 16;
	//	return static_cast<unsigned char>(res);
	//}

	//static Color FromSRGB(Vec3 v)
	//{
	//	Color col;
	//	col.r = LinearToSRGB(v.r);
	//	col.g = LinearToSRGB(v.g);
	//	col.b = LinearToSRGB(v.b);
	//	col.a = 255;
	//	return col;
	//}

};

const Color BLACK = { 0, 0, 0, 255 };
const Color WHITE = { 255, 255, 255, 255 };

struct Vertex {
	Vec4 p;
	Vec3 n;
	Vec2 t;
	Vec3 c;
};

struct Tri {
	union {
		struct {
			Vertex v0;
			Vertex v1;
			Vertex v2;
		};
		Vertex v[3];
	};
	Vec3 faceNormal;
};

struct Tri3 {
	union {
		struct {
			Vec3 v0;
			Vec3 v1;
			Vec3 v2;
		};
		Vec3 p[3];
	};
};

struct Tri4 {
	union {
		struct {
			Vec4 v0;
			Vec4 v1;
			Vec4 v2;
		};
		Vec4 p[3];
	};
};

struct Line {
	Vec4 start;
	Vec4 end;
};

struct Plane {
	Vec3 pos;
	Vec3 normal;
};

struct Transform {
	Vec3 position = Vec3(0.0f, 0.0f, 0.0f);
	Quat rotation = Quat(1.0f, 0.0f, 0.0f, 0.0f);
	Vec3 scale = Vec3(1.0f, 1.0f, 1.0f);

	Mat4 GetTransform() const
	{
		Mat4 m = glm::scale(Mat4(1.0f), scale);
		m = glm::toMat4(rotation) * m;
		m = glm::translate(Mat4(1.0f), position) * m;
		return m;
	}
};