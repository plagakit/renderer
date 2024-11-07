#pragma once

#include "typedefs.h"
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

	struct RenderCommand {
		TriMesh* mesh;
		Mat4 transform;
	};

	struct Config {
		// settings
		PolygonMode polygonMode = PolygonMode::WIREFRAME;
		bool useDepthBuffer = true;
		bool doBackfaceCulling = true;
		
		// info
		int rasterizedTriangles = 0;
	};
	Config config;

	void Init();
	void Shutdown();

	void SetClearColor(Color color);
	void SetViewMatrix(const Mat4& view);
	void DrawMesh(TriMesh* mesh, Mat4 transform);

	void FlushCommands();
	void BlitToScreen();

private:
	// For drawing to the window
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

	Color clearColor = BLACK;
	float minDepth; // for depth map
	Vec3 lightDirection = glm::normalize(Vec3(1, 1, -1));

	//RMatrix projectionMatrix;
	Camera camera;
	Mat4 viewMatrix;

	VertexShader vertexShader = DefaultVertexShader;
	
	void ConstructProjectionMatrix();

	Color DarkenColor(Color col, float factor) const;
	Vec3 TriangleFaceNormal(const Tri& tri) const;
	float Triangle2DArea(const Vec3& a, const Vec3& b, const Vec3& c) const;
	bool IsPointInsideTriangle2D(const Tri& tri, const Vec3& point) const;

	void DrawLineBresenham(int x0, int y0, int x1, int y1, Color color);

	void DrawTriangle(const Tri& tri, Color color);
	void RasterizeTriangle(const Tri& tri, const Tri& vertexNormals, const Vec3& normal, Color color);
};