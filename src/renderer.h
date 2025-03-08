#pragma once

#include "common.h"
#include "mesh.h"
#include "camera.h"
#include "shaders/shader.h"
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

	struct RenderCommand 
	{
		Mesh* mesh;
		Texture* texture;
		VertexShader* vertexShader;
	};

	struct Config 
	{
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

	struct Metrics 
	{
		uint32_t curTriIdx = 0;
		uint32_t rasterizedTriangles = 0;
		uint32_t clippedTriangles = 0;
		Vec3 modelSpace[3];
		Vec4 worldSpace[3];
		Vec4 viewSpace[3];
		Vec4 clipSpace[3];
		Vec4 ndcSpace[3];
		Vec4 screenSpace[3];
	};
	Metrics metrics;

	void Init();
	void Shutdown();

	void SetClearColor(Color color);
	void SetModel(const Mat4& model);
	void SetView(const Mat4& view);
	void SetProjection(const Mat4& projection);
	void SendCommand(RenderCommand command);

	void FlushCommands();
	void BlitToScreen();

	GLuint GetGLScreenTexture() const;


private:

	// For drawing pixels to the window
	GLuint FBO;
	GLuint screenTexture;

	std::vector<RenderCommand> commandQueue;

	struct Framebuffer {
		std::vector<Color> colorBuffer;
		std::vector<float> depthBuffer;
	};
	Framebuffer fb;

	void InitFramebuffer();
	void ClearFramebuffer();

	Texture* curTexture;
	Color clearColor = BLACK;
	float minDepth; // for depth map
	float maxDepth;

	Vec3 lightDirection = glm::normalize(Vec3(1, 1, -1));

	/*VertexShader vertexShader = DefaultVertexShader;*/

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