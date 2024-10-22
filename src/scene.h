#pragma once

#include "renderer.h"
#include "imgui.h"

class Scene {

public:
	void Init();
	void Update(float dt);
	void Render();
	void Shutdown();


private:
	Renderer renderer;

	RVector3 objectPos = RVector3::Zero();
	RQuaternion objectRot = RQuaternion::Identity();
	RVector3 objectScale = RVector3(1.0f, 1.0f, 1.0f);
	RVector3 objectAngVel = RVector3(-1, -0.5, 0.2);//RVector3::Zero();//RQuaternion::FromAxisAngle(RVector3(1, 0, 1), 0.1f);

	TriMesh* currentMesh;
	RMatrix currentTransform = RMatrix::Identity();

	TriMesh suzanne;
	TriMesh teapot;

	ImFont* font;
	void DrawConfigGUI();

};