#pragma once

#include "raylib-cpp.hpp"
#include <vector>


struct Tri {
	RVector3 p[3];
};

struct TriMesh {
	std::vector<Tri> tris;
	std::vector<Tri> vertexNormals;
};

class Renderer {

public:
	void Init();
	void Update(float dt);
	void Render();
	void Shutdown();

private:
	const int SCREEN_WIDTH = 1280;
	const int SCREEN_HEIGHT = 720;

	std::vector<float> depthBuffer;
	float minDepth; // for depth map

	enum LightingType {
		FLAT,
		GOURAUD,
		DEPTH_MAP
	};
	LightingType lightingType = LightingType::GOURAUDk;
	bool useDepthBuffer = true;

	RVector3 cameraPos;
	float near = 0.1f;
	float far = 1000.0f;
	float fov = 90.0f;
	float aspectRatio = (float)SCREEN_HEIGHT / (float)SCREEN_WIDTH;
	float fovRad = 1.0f / tanf(fov * 0.5f / 180.0f * PI);
	RMatrix projectionMatrix;

	RVector3 lightDirection = RVector3(1, 0, -1).Normalize();

	TriMesh object;
	RMatrix objRotX = RMatrix::RotateX(0.0f);
	RMatrix objRotZ = RMatrix::RotateZ(0.0f);
	float rotation = 0.0f;

	void ConstructProjectionMatrix();
	void ResetDepthBuffer();

	bool LoadFromObjFile(std::string path, TriMesh& outMesh) const;
	Color DarkenColor(Color col, float factor) const;
	RVector3 TriangleFaceNormal(const Tri& tri) const;
	float Triangle2DArea(const RVector3& a, const RVector3& b, const RVector3& c) const;
	bool IsPointInsideTriangle2D(const Tri& tri, const RVector3& point) const;

	void ModelSpaceToWorldSpace(Tri& tri) const;
	void WorldSpaceToScreenSpace(Tri& tri) const;

	void DrawTriangle(const Tri& tri, Color color);
	void RasterizeTriangle(const Tri& tri, const Tri& vertexNormals, const RVector3& normal, Color color);
};