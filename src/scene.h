#pragma once

#include "my_transform.h"
#include "renderer.h"
#include "camera.h"
#include "imgui.h"

class Scene {

public:
	void Init();
	void Update(float dt);
	void Render();
	void Shutdown();


private:
	const static int SCREEN_WIDTH = 1280;
	const static int SCREEN_HEIGHT = 720;

	Renderer renderer;

	MyTransform object;
	RVector3 angularVelocity = RVector3(0.0f, -1.0f, -1.0f);

	MyCamera camera;

	int currentMeshIndex = 0;
	TriMesh* currentMesh;
	RMatrix currentTransform = RMatrix::Identity();

	TriMesh cube;
	TriMesh suzanne;
	TriMesh teapot;

	ImFont* font;
	void DrawConfigGUI();

};