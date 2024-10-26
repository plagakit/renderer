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

	RMatrix cubeModel = RMatrix::Scale(0.25f, 0.25f, 0.25f);
	TriMesh::LoadFromOBJ("res/cube.obj", cube, cubeModel);

	RMatrix suzanneModel = RMatrix::Identity().Scale(0.25f, 0.25f, 0.25f);
	TriMesh::LoadFromOBJ("res/suzanne.obj", suzanne, suzanneModel);

	RMatrix teapotModel = RMatrix::Identity().Scale(0.2f, 0.2f, 0.2f);
	TriMesh::LoadFromOBJ("res/teapot.obj", teapot, teapotModel);

	currentMesh = &cube;

	std::cout << "Initting renderer..." << std::endl;
	renderer.Init();
}

void Scene::Update(float dt)
{
	float rotSpeed = angularVelocity.Length();
	RQuaternion rotDelta = RQuaternion::FromEuler(angularVelocity.Normalize());
	object.SetRot(QuaternionSlerp(object.GetRot(), object.GetRot().Multiply(rotDelta), dt * rotSpeed));

	//RMatrix scaling = RMatrix::Scale(objectScale.x, objectScale.y, objectScale.z);

	//float rotSpeed = objectAngVel.Length();
	//RQuaternion rotDelta = RQuaternion::FromEuler(objectAngVel.Normalize());
	//objectRot = QuaternionSlerp(objectRot, objectRot.Multiply(rotDelta), dt * rotSpeed);
	//objectRot = objectRot.Normalize();
	//RMatrix rotation = objectRot.ToMatrix();
	//
	//RMatrix translation = RMatrix::Translate(objectPos.x, objectPos.y, objectPos.z);

	//currentTransform = scaling * rotation * translation;
		
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
	renderer.DrawMesh(currentMesh, object.GetMatrix());
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

	const char* meshes[] = { "Cube", "Suzanne", "Teapot" };
	ImGui::Combo("Mesh", &currentMeshIndex, meshes, std::size(meshes));
	if (currentMeshIndex == 0)		currentMesh = &cube;
	else if (currentMeshIndex == 1)	currentMesh = &suzanne;
	else if (currentMeshIndex == 2)	currentMesh = &teapot;

	ImGui::Text("Vertices: %d, Triangles: %d, Rasterized Triangles: %d", currentMesh->vertCount, currentMesh->tris.size(), renderer.config.rasterizedTriangles);

	const char* pmTypes[] = { "Point", "Wireframe", "Filled", "Depth Map" };
	ImGui::Combo("Polygon Mode", (int*)&renderer.config.polygonMode, pmTypes, std::size(pmTypes));

	ImGui::Checkbox("Use Depth Buffer", &renderer.config.useDepthBuffer);
	ImGui::Checkbox("Do Backface Culling", &renderer.config.doBackfaceCulling);

	const auto& v = object.GetPos();
	float pos[] = { v.x, v.y, v.z };
	ImGui::InputFloat3("Object Pos", pos, "%.02f");
	object.SetPos(RVector3(pos[0], pos[1], pos[2]));

	float angVel[] = { angularVelocity.x, angularVelocity.y, angularVelocity.z };
	ImGui::InputFloat3("Object Angular Velocity", angVel, "%.02f");
	angularVelocity = RVector3(angVel[0], angVel[1], angVel[2]);

	ImGui::End();
}