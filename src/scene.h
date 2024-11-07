#pragma once

#include "typedefs.h"
#include "camera.h"
#include "renderer.h"
#include "imgui.h"

#define FRAME_TIME_SAMPLES 10
#define SAMPLE_TIME_MAX 1

class Scene {

public:
	void Init();
	void Update(float dt);
	void Render();
	void Shutdown();


private:
	Renderer renderer;

	Transform object;
	Vec3 angularVelocity = Vec3(0.0f, -1.0f, -1.0f);

	Camera camera;

	int currentMeshIndex = 0;
	TriMesh* currentMesh;
	Mat4 currentTransform = Mat4(1.0f);

	TriMesh cube;
	TriMesh suzanne;
	TriMesh teapot;

	ImFont* font;
	void DrawConfigGUI();

	float frameTimes[FRAME_TIME_SAMPLES];
	int frameTimeIdx;
	float avgFrameTime;
	float sampleTime;

};