#pragma once

#include "mesh.h"
#include <raylib-cpp.hpp>

class Renderer {

public:
	const int SCREEN_WIDTH = 1280;
	const int SCREEN_HEIGHT = 720;

	enum LightingType {
		WIREFRAME,
		FLAT,
		GOURAUD,
		DEPTH_MAP
	};

	struct RenderCommand {
		TriMesh* mesh;
		RMatrix transform;
	};

	struct Config {
		LightingType lightingType = LightingType::WIREFRAME;
		bool useDepthBuffer = true;
		bool doBackfaceCulling = true;
	};
	Config config;

	void Init();
	void Shutdown();

	void SetClearColor(Color color);
	void DrawMesh(TriMesh* mesh, RMatrix transform);

	void FlushCommands();
	void BlitToScreen();

private:
	// For drawing to the window w/ raylib
	Image framebufferImage;
	Texture2D framebufferTexture;

	std::vector<RenderCommand> commandQueue;

	std::vector<Color> framebuffer;
	std::vector<float> depthBuffer;

	Color clearColor = BLACK;
	float minDepth; // for depth map
	RVector3 lightDirection = RVector3(1, 1, -1).Normalize();

	RVector3 cameraPos;
	float near = 0.1f;
	float far = 1000.0f;
	float fov = 90.0f;
	float aspectRatio = (float)SCREEN_HEIGHT / (float)SCREEN_WIDTH;
	float fovRad = 1.0f / tanf(fov * 0.5f / 180.0f * PI);
	RMatrix projectionMatrix;

	
	void ConstructProjectionMatrix();
	void ClearScreen();
	void ResetDepthBuffer();

	Color DarkenColor(Color col, float factor) const;
	RVector3 TriangleFaceNormal(const Tri& tri) const;
	float Triangle2DArea(const RVector3& a, const RVector3& b, const RVector3& c) const;
	bool IsPointInsideTriangle2D(const Tri& tri, const RVector3& point) const;

	void BresenhamMajorAxis(Color* curPixel, int dx, int dy, int stepMajor, int stepMinor, Color color);
	void BresenhamStraight(Color* curPixel, int dx, int step, Color color);
	void DrawLineBresenham(int x0, int y0, int x1, int y1, Color color);

	void DrawTriangle(const Tri& tri, Color color);
	void RasterizeTriangle(const Tri& tri, const Tri& vertexNormals, const RVector3& normal, Color color);
};