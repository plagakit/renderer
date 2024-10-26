#pragma once

#include "mesh.h"
#include "camera.h"
#include "shader.h"
#include <raylib-cpp.hpp>

class Renderer {

public:
	const int SCREEN_WIDTH = 1280;
	const int SCREEN_HEIGHT = 720;
	const int SCREEN_MAX = SCREEN_WIDTH * SCREEN_HEIGHT;

	enum PolygonMode {
		POINT,
		WIREFRAME,
		FILLED,
		DEPTH_MAP
	};

	struct RenderCommand {
		TriMesh* mesh;
		RMatrix transform;
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
	void SetViewMatrix(const RMatrix& view);
	void DrawMesh(TriMesh* mesh, RMatrix transform);

	void FlushCommands();
	void BlitToScreen();

private:
	// For drawing to the window w/ raylib
	Texture2D screenTexture;

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
	RVector3 lightDirection = RVector3(1, 1, -1).Normalize();

	

	//RMatrix projectionMatrix;
	MyCamera camera;
	RMatrix viewMatrix;

	VertexShader vertexShader = DefaultVertexShader;
	
	void ConstructProjectionMatrix();

	Color DarkenColor(Color col, float factor) const;
	RVector3 TriangleFaceNormal(const Tri& tri) const;
	float Triangle2DArea(const RVector3& a, const RVector3& b, const RVector3& c) const;
	bool IsPointInsideTriangle2D(const Tri& tri, const RVector3& point) const;

	void DrawLineBresenham(int x0, int y0, int x1, int y1, Color color);

	void DrawTriangle(const Tri& tri, Color color);
	void RasterizeTriangle(const Tri& tri, const Tri& vertexNormals, const RVector3& normal, Color color);
};