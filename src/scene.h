#pragma once

#include "common.h"
#include "camera.h"
#include "renderer.h"
#include "imgui.h"
#include <string>

#define FRAME_TIME_SAMPLES 10
#define SAMPLE_TIME_MAX 1

class Scene {

public:
	void Init();
	void Update(float dt);
	void Render();
	void Shutdown();

	void OnKeyInput(int key, int action);

private:
	Renderer renderer;

	Transform object;
	Vec3 angularVelocity = Vec3(0.0f, -1.0f, -1.0f);

	Camera camera;

	Mesh triangle;
	Mesh cube;
	Mesh suzanne;
	Mesh teapot;

	int currentMeshIndex = 0;
	static const char* meshNames[];
	const std::vector<Mesh*> meshes = { &triangle, &cube, &suzanne, &teapot };

	ImFont* font;
	void DrawConfigGUI();

	bool isKeyDown[GLFW_KEY_LAST];
	bool isKeyJustDown[GLFW_KEY_LAST];

	float frameTimes[FRAME_TIME_SAMPLES];
	int frameTimeIdx;
	float avgFrameTime;
	float sampleTime;

};