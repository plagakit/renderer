#pragma once

#include "common.h"
#include "mesh.h"
#include "camera.h"
#include "shader.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>


class Renderer {

public:
	const int SCREEN_MAX = SCREEN_WIDTH * SCREEN_HEIGHT;

	enum PolygonMode {
		POINT,
		WIREFRAME,
		FILLED,
		DEPTH_MAP
	};

	enum ObjectColor {
		FLAT,
		COLORED_VERTS,
		CHECKERBOARD,
		TEXTURED
	};

	//enum ColorMode {
	//	FLAT_WHITE,
	//};

	struct RenderCommand {
		Mesh* mesh;
		VertexUniforms uniforms;
	};

	struct Config {
		PolygonMode polygonMode = PolygonMode::FILLED;
		ObjectColor objectColor = ObjectColor::TEXTURED;
		bool useDepthBuffer = true;
		bool doBackfaceCulling = true;
		bool doPerspCorrectInterp = true;

		bool doNearCulling = true;
		Vec3 nearPlanePos = Vec3(0, 0, -0.9f);
		Vec3 nearPlaneNormal = Vec3(0, 0, 1);
	};
	Config config;

	struct Metrics {
		int rasterizedTriangles = 0;
		int clippedTriangles = 0;
		Tri3 modelSpace;
		Tri4 worldSpace;
		Tri4 viewSpace;
		Tri clipSpace;
		Tri ndcSpace;
		Tri screenSpace;
	};
	Metrics metrics;

	void Init();
	void Shutdown();

	void SetClearColor(Color color);
	void DrawMesh(Mesh* mesh, Mat4 transform, Mat4 view, Mat4 proj);

	void FlushCommands();
	void BlitToScreen();

private:
	// For drawing to the window
	GLuint FBO;
	GLuint screenTexture;

	Texture testTexture{ "res/uv_map_2.png" };

	std::vector<RenderCommand> commandQueue;

	struct Framebuffer {
		std::vector<Color> colorBuffer;
		std::vector<float> depthBuffer;
	};
	Framebuffer fb;

	void InitFramebuffer();
	void ClearFramebuffer();

	Color clearColor = BLACK;
	float minDepth; // for depth map
	float maxDepth;

	Vec3 lightDirection = glm::normalize(Vec3(1, 1, -1));

	VertexShader vertexShader = DefaultVertexShader;

	Color DarkenColor(Color col, float factor) const;
	Color LerpColor(Color begin, Color end, float t) const;

	bool IsEdgeTopOrLeft(Vec3 a, Vec3 b) const;
	bool IsPointInsideTriangle2D(Vec3 a, Vec3 b, Vec3 c, Vec3 point) const;
	float Triangle2DArea(Vec3 a, Vec3 b, Vec3 c) const;
	float TriangleHomogenousArea(Vec4 a, Vec4 b, Vec4 c) const;
	Vec3 TriangleFaceNormal(Vec3 a, Vec3 b, Vec3 c) const;

	float LinePlaneIntersection(Plane plane, Line line, Vec4& intersection) const;
	int ClipTriAgainstPlane(Plane plane, Tri tri, Tri& outTri1, Tri& outTri2);
	bool IsCounterClockwise(Tri& t) const;

	void DrawLineBresenham(int x0, int y0, int x1, int y1, Color color);
	void RasterizeTriangle(Tri tri);
};