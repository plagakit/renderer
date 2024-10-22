#include "scene.h"

#include "imgui.h"
#include "rlImGui.h"
#include <iostream>

void Scene::Init()
{
	std::cout << "Initting ImGui..." << std::endl;
	ImGuiIO& io = ImGui::GetIO();
	io.IniFilename = NULL;

	const char* fontPath = "res/Segoe-UI-Variable.ttf";
	font = io.Fonts->AddFontFromFileTTF(fontPath, 22.0f);

	rlImGuiSetup(true);

	std::cout << "Initting objs..." << std::endl;
	RMatrix suzanneModel = RMatrix::Identity().Scale(0.25f, 0.25f, 0.25f);
	TriMesh::LoadFromOBJ("res/suzanne.obj", suzanne, suzanneModel);
	TriMesh::LoadFromOBJ("res/teapot.obj", teapot);

	std::cout << "Initting renderer..." << std::endl;
	renderer.Init();
}

void Scene::Update(float dt)
{
	float rotSpeed = objectAngVel.Length();
	RQuaternion rotDelta = RQuaternion::FromEuler(objectAngVel.Normalize());
	objectRot = QuaternionSlerp(objectRot, objectRot.Multiply(rotDelta), dt * rotSpeed);
	objectRot = objectRot.Normalize();

	currentTransform = objectRot.ToMatrix();

	//RVector3 x = RVector3(1, 0, 0).RotateByQuaternion(objectRot).Scale(objectScale.x);
	//RVector3 y = RVector3(0, 1, 0).RotateByQuaternion(objectRot).Scale(objectScale.y);
	//RVector3 z = RVector3(0, 0, 1).RotateByQuaternion(objectRot).Scale(objectScale.z);

	//currentTransform = RMatrix(
	//	x.x, y.x, z.x, objectPos.x,
	//	x.y, y.y, z.y, objectPos.y,
	//	x.z, y.z, z.z, objectPos.z
	//);
}

void Scene::Render()
{
	renderer.DrawMesh(&suzanne, currentTransform);
	renderer.FlushCommands();
	renderer.BlitToScreen();

	rlImGuiBegin();
	ImGui::PushFont(font);
	DrawConfigGUI();
	ImGui::PopFont();
	rlImGuiEnd();
}

void Scene::Shutdown()
{
	renderer.Shutdown();
}

void Scene::DrawConfigGUI()
{
	const ImGuiViewport* viewport = ImGui::GetMainViewport();
	static const ImGuiWindowFlags WINDOW_FLAGS = ImGuiWindowFlags_None
		| ImGuiWindowFlags_NoResize;

	DrawFPS(viewport->Size.x - 90, 10);

	ImVec2 settingsSize = ImVec2(viewport->Size.x / 5, viewport->Size.y / 2);
	ImGui::SetNextWindowPos(ImVec2(10, 10));
	ImGui::SetNextWindowSize(ImVec2(0, 0)); // size 0 will make the window autofit conte
	ImGui::Begin("Config", NULL, WINDOW_FLAGS);

	//ImGui::ShowDemoWindow();

	const char* types[] = { "Wireframe", "Flat", "Gouraud",  "Depth Map" };
	ImGui::Combo("Lighting Type", (int*)&renderer.config.lightingType, types, std::size(types));

	ImGui::Checkbox("Use Depth Buffer", &renderer.config.useDepthBuffer);
	ImGui::Checkbox("Do Backface Culling", &renderer.config.doBackfaceCulling);

	float pos[] = { objectPos.x, objectPos.y, objectPos.z };
	ImGui::InputFloat3("Object Pos", pos, "%.02f");
	objectPos = RVector3(pos[0], pos[1], pos[2]);

	float angVel[] = { objectAngVel.x, objectAngVel.y, objectAngVel.z };
	ImGui::InputFloat3("Object Angular Velocity", angVel, "%.02f");
	objectAngVel = RVector3(angVel[0], angVel[1], angVel[2]);

	ImGui::End();
}
