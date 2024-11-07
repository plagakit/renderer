#include "scene.h"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <iostream>

void Scene::Init()
{
	std::cout << "Initting ImGui..." << std::endl;
	ImGuiIO& io = ImGui::GetIO();
	io.IniFilename = NULL;

	const char* fontPath = "res/Segoe-UI-Variable.ttf";
	font = io.Fonts->AddFontFromFileTTF(fontPath, 22.0f);

	std::cout << "Initting objs..." << std::endl;

	Mat4 cubeModel = glm::scale(Mat4(1.0f), Vec3(0.25f, 0.25f, 0.25f));
	TriMesh::LoadFromOBJ("res/cube.obj", cube, cubeModel);

	Mat4 suzanneModel = glm::scale(Mat4(1.0f), Vec3(0.25f, 0.25f, 0.25f));
	TriMesh::LoadFromOBJ("res/suzanne.obj", suzanne, suzanneModel);

	Mat4 teapotModel = glm::translate(glm::scale(Mat4(1.0f), Vec3(0.2f, 0.2f, 0.2f)), Vec3(0, -1, 0));
	TriMesh::LoadFromOBJ("res/teapot.obj", teapot, teapotModel);

	currentMesh = &cube;

	std::cout << "Initting renderer..." << std::endl;
	renderer.Init();

	std::cout << "Done!" << std::endl;
}

void Scene::Update(float dt)
{
	frameTimes[frameTimeIdx] = dt;
	frameTimeIdx = (frameTimeIdx + 1) % FRAME_TIME_SAMPLES;
	sampleTime += dt;
	if (sampleTime > SAMPLE_TIME_MAX)
	{
		sampleTime = 0.0f;
		float sum = 0.0f;
		for (int i = 0; i < FRAME_TIME_SAMPLES; i++)
			sum += frameTimes[i];
		avgFrameTime = sum / FRAME_TIME_SAMPLES;
	}

	float rotSpeed = glm::length(angularVelocity);
	if (rotSpeed >= FLT_EPSILON)
	{
		Quat rotDelta = Quat(glm::normalize(angularVelocity)) * object.rotation;
		object.rotation = glm::normalize(glm::slerp(object.rotation, rotDelta, rotSpeed * dt));
	}
}

void Scene::Render()
{
	renderer.DrawMesh(currentMesh, object.GetTransform());
	renderer.FlushCommands();
	renderer.BlitToScreen();

	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	ImGui::PushFont(font);
	DrawConfigGUI();
	ImGui::PopFont();
	
	ImGui::EndFrame();
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
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

	//DrawFPS(viewport->Size.x - 90, 10);

	ImVec2 settingsSize = ImVec2(viewport->Size.x / 5, viewport->Size.y / 2);
	ImGui::SetNextWindowPos(ImVec2(10, 10));
	ImGui::SetNextWindowSize(ImVec2(0, 0)); // size 0 will make the window autofit conte
	ImGui::Begin("Config", NULL, WINDOW_FLAGS);

	//ImGui::ShowDemoWindow();

	ImGui::Text("FPS: %.01f", g_fps);
	ImGui::SameLine(); ImGui::SetCursorPosX(80);
	ImGui::Text("Frame Time: %.01f", (float)(g_dt * 1000));
	ImGui::SameLine(); ImGui::SetCursorPosX(205);
	ImGui::Text("ms (avg %.01f)", (float)(avgFrameTime * 1000));


	const char* meshes[] = { "Cube", "Suzanne", "Teapot" };
	ImGui::Combo("Mesh", &currentMeshIndex, meshes, std::size(meshes));
	if (currentMeshIndex == 0)		currentMesh = &cube;
	else if (currentMeshIndex == 1)	currentMesh = &suzanne;
	else if (currentMeshIndex == 2)	currentMesh = &teapot;

	ImGui::Text("Vertices: %d, Triangles: %d", currentMesh->vertCount, currentMesh->tris.size());
	ImGui::Text("Rasterized Triangles: %d", renderer.config.rasterizedTriangles);

	const char* pmTypes[] = { "Point", "Wireframe", "Filled", "Depth Map" };
	ImGui::Combo("Polygon Mode", (int*)&renderer.config.polygonMode, pmTypes, std::size(pmTypes));

	const char* pjTypes[] = { "Perspective", "Orthographic" };
	Camera::Mode pj = camera.GetMode();
	ImGui::Combo("Camera Mode", (int*) & pj, pjTypes, std::size(pjTypes));
	camera.SetMode(pj);
	if (pj == Camera::Mode::PERSPECTIVE)
	{
		auto fov = camera.GetFov();
		ImGui::DragFloat("FOV", &fov, 0.33f, 5.0f, 90.0f, "%.0f");
		camera.SetFov(fov);
	}

	ImGui::Checkbox("Use Depth Buffer", &renderer.config.useDepthBuffer);
	ImGui::Checkbox("Do Backface Culling", &renderer.config.doBackfaceCulling);

	if (ImGui::CollapsingHeader("Object"))
	{
		const auto& v = object.position;
		float pos[] = { v.x, v.y, v.z };
		ImGui::SetNextItemWidth(200);
		ImGui::InputFloat3("Object Pos", pos, "%.02f");
		object.position = Vec3(pos[0], pos[1], pos[2]);

		Vec3 rotEul = glm::eulerAngles(object.rotation);
		float rot[] = { glm::degrees(rotEul.x), glm::degrees(rotEul.y), glm::degrees(rotEul.z) };
		ImGui::SetNextItemWidth(200);
		ImGui::InputFloat3("Object Rot", rot, "%.02f");
		object.rotation = glm::quat(glm::vec3(glm::radians(rot[0]), glm::radians(rot[1]), glm::radians(rot[2])));

		float angVel[] = { angularVelocity.x, angularVelocity.y, angularVelocity.z };
		ImGui::SetNextItemWidth(200);
		ImGui::InputFloat3("Object Angular Velocity", angVel, "%.02f");
		angularVelocity = Vec3(angVel[0], angVel[1], angVel[2]);
	}

	ImGui::End();
}