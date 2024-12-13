#include "scene.h"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <iostream>

const char* Scene::meshNames[] = {"Triangle", "Cube", "Suzanne", "Teapot"};

void Scene::Init()
{
	std::cout << "Initting ImGui..." << std::endl;
	ImGuiIO& io = ImGui::GetIO();
	io.IniFilename = NULL;

	const char* fontPath = "res/Segoe-UI-Variable.ttf";
	font = io.Fonts->AddFontFromFileTTF(fontPath, 22.0f);

	std::cout << "Initting objs..." << std::endl;

	triangle.vertCount = 3;
	triangle.v = {
		Vertex{Vec4(-0.87f, -0.5f, 0, 1), Vec3(0, 0, 1), Vec2(0, 0),	Vec3(1, 0, 0)},
		Vertex{Vec4(0.87f, -0.5f, 0, 1), Vec3(0, 0, 1), Vec2(1, 0),		Vec3(0, 1, 0)},
		Vertex{Vec4(0, 1, 0, 1), Vec3(0, 0, 1), Vec2(0, 1),				Vec3(0, 0, 1)},
	};

	Mat4 cubeModel = glm::scale(Mat4(1.0f), Vec3(0.5f, 0.5f, 0.5f));
	Mesh::LoadFromOBJ("res/cube.obj", cube, true, cubeModel);
	for (auto& v : cube.v)
		v.c = Vec3(1, 1, 1);

	Mat4 suzanneModel = glm::scale(Mat4(1.0f), Vec3(0.5f, 0.5f, 0.5f));
	Mesh::LoadFromOBJ("res/suzanne.obj", suzanne, true, suzanneModel);
	for (auto& v : suzanne.v)
		v.c = Vec3(1, 1, 1);

	Mat4 teapotModel = glm::translate(glm::scale(Mat4(1.0f), Vec3(0.2f, 0.2f, 0.2f)), Vec3(0, -1, 0));
	Mesh::LoadFromOBJ("res/teapot.obj", teapot, true, teapotModel);

	currentMeshIndex = 1;
	object.position.z = 3;

	std::cout << "Initting renderer..." << std::endl;
	renderer.Init();

	std::cout << "Done!" << std::endl;
}

void Scene::Update(float dt)
{
	// Game stuff
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

	// Scene stuff
	float rotSpeed = glm::length(angularVelocity);
	if (rotSpeed >= FLT_EPSILON)
	{
		Quat rotDelta = Quat(glm::normalize(angularVelocity)) * object.rotation;
		object.rotation = glm::normalize(glm::slerp(object.rotation, rotDelta, rotSpeed * dt));
	}

	Vec3 cameraMove = Vec3(0, 0, 0);
	if (isKeyDown[GLFW_KEY_W])	cameraMove.z += dt;
	if (isKeyDown[GLFW_KEY_S])	cameraMove.z -= dt;
	if (isKeyDown[GLFW_KEY_A])	cameraMove.x -= dt;
	if (isKeyDown[GLFW_KEY_D])	cameraMove.x += dt;
	if (isKeyDown[GLFW_KEY_Q])	cameraMove.y -= dt;
	if (isKeyDown[GLFW_KEY_E])	cameraMove.y += dt;
	cameraMove = camera.transform.rotation * cameraMove;
	camera.transform.position += cameraMove;

	if (isKeyDown[GLFW_KEY_J])	camera.transform.rotation = glm::quat(glm::vec3(0, -dt, 0)) * camera.transform.rotation;
	if (isKeyDown[GLFW_KEY_L])	camera.transform.rotation = glm::quat(glm::vec3(0, dt, 0)) * camera.transform.rotation;
	if (isKeyDown[GLFW_KEY_I])	camera.transform.rotation *= glm::quat(glm::vec3(-dt, 0, 0));
	if (isKeyDown[GLFW_KEY_K])	camera.transform.rotation *= glm::quat(glm::vec3(dt, 0, 0));

	if (isKeyJustDown[GLFW_KEY_R])
	{
		currentMeshIndex++;
		if (currentMeshIndex >= meshes.size())
			currentMeshIndex = 0;
	}


	for (int i = 0; i < GLFW_KEY_LAST; i++)
		if (isKeyJustDown[i])
			isKeyJustDown[i] = false;
}

void Scene::Render()
{
	renderer.DrawMesh(
		meshes[currentMeshIndex],
		object.GetTransform(), 
		camera.GetViewMatrix(), 
		camera.GetProjectionMatrix()
	);

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

void Scene::OnKeyInput(int key, int action)
{
	if (action == GLFW_PRESS)
	{
		isKeyJustDown[key] = true;
		isKeyDown[key] = true;
	}
	else if (action == GLFW_RELEASE)
	{
		isKeyJustDown[key] = false;
		isKeyDown[key] = false;
	}
}

void Scene::DrawConfigGUI()
{
	auto InputVec3 = [](const char* label, Vec3& v)
		{
			float vf[] = { v.x, v.y, v.z };
			ImGui::SetNextItemWidth(200);
			ImGui::InputFloat3(label, vf, "%0.2f");
			v = Vec3(vf[0], vf[1], vf[2]);
		};

	const ImGuiViewport* viewport = ImGui::GetMainViewport();
	static const ImGuiWindowFlags WINDOW_FLAGS = ImGuiWindowFlags_None
		| ImGuiWindowFlags_NoResize;

	//DrawFPS(viewport->Size.x - 90, 10);

	ImVec2 settingsSize = ImVec2(viewport->Size.x / 5, viewport->Size.y / 2);
	ImGui::SetNextWindowPos(ImVec2(10, 10));
	ImGui::SetNextWindowSize(ImVec2(0, 0)); // size 0 will make the window autofit conte
	ImGui::Begin("Configuration & Metrics", NULL, WINDOW_FLAGS);

	//ImGui::ShowDemoWindow();
	ImGui::Text("W/S - Forward/Back, A/D - Left/Right, Q/E - Up/Down");
	ImGui::Text("I/K - Look Up/Down, J/L - Look Left/Right");
	ImGui::Text("R - Switch to next mesh");
	ImGui::Separator();
	ImGui::Text("FPS: %.01f", g_fps);
	ImGui::SameLine(); ImGui::SetCursorPosX(80);
	ImGui::Text("Frame Time: %.01f", (float)(g_dt * 1000));
	ImGui::SameLine(); ImGui::SetCursorPosX(205);
	ImGui::Text("ms (avg %.01f)", (float)(avgFrameTime * 1000));

	ImGui::Combo("Mesh", &currentMeshIndex, &meshNames[0], static_cast<int>(meshes.size()));

	ImGui::Text("Vertices: %d, Triangles: %d", meshes[currentMeshIndex]->vertCount, 0);
	ImGui::Text("Rasterized Triangles: %d", renderer.metrics.rasterizedTriangles);
	ImGui::Text("Clipped Triangles: %d", renderer.metrics.clippedTriangles);

	const char* pmTypes[] = { "Point", "Wireframe", "Filled", "Depth Map" };
	ImGui::Combo("Polygon Mode", (int*)&renderer.config.polygonMode, pmTypes, std::size(pmTypes));

	const char* ocTypes[] = { "Flat", "Coloured Vertices", "Checkerboard", "Textured" };
	ImGui::Combo("Object Color", (int*)&renderer.config.objectColor, ocTypes, std::size(ocTypes));

	ImGui::Checkbox("Use Depth Buffer", &renderer.config.useDepthBuffer);
	ImGui::Checkbox("Do Backface Culling", &renderer.config.doBackfaceCulling);
	ImGui::Checkbox("Do Perspective-Correct Interpolation", &renderer.config.doPerspCorrectInterp);
	
	ImGui::Checkbox("Do Near Culling", &renderer.config.doNearCulling);
	if (renderer.config.doNearCulling)
	{
		if (ImGui::CollapsingHeader("Near Plane"))
		{
			InputVec3("Pos", renderer.config.nearPlanePos);
			InputVec3("Normal", renderer.config.nearPlaneNormal);
		}
	}

	if (ImGui::CollapsingHeader("Camera"))
	{
		const char* pjTypes[] = { "Perspective", "Orthographic" };
		Camera::Mode pj = camera.GetMode();
		ImGui::Combo("Camera Mode", (int*)&pj, pjTypes, std::size(pjTypes));
		camera.SetMode(pj);
		if (pj == Camera::Mode::PERSPECTIVE)
		{
			float fov = camera.GetFov();
			ImGui::DragFloat("FOV", &fov, 0.33f, 5.0f, 90.0f, "%.0f");
			camera.SetFov(fov);
		}
		else if (pj == Camera::Mode::ORTHOGRAPHIC)
		{
			float osz = camera.GetOrthographicScale();
			ImGui::InputFloat("Size", &osz, 0.0f, 0.0f, "%.1f");
			camera.SetOrthographicScale(osz);
		}

		InputVec3("Camera Pos", camera.transform.position);

		Vec3 rotEul = glm::degrees(glm::eulerAngles(camera.transform.rotation));
		ImGui::Text("Rot Quat: (%0.2f, %0.2f, %0.2f, %0.2f)\n", camera.transform.rotation.x, camera.transform.rotation.y, camera.transform.rotation.z, camera.transform.rotation.w);
		InputVec3("Camera Rotation", rotEul);
		camera.transform.rotation = glm::quat(glm::radians(rotEul));
		
	}

	if (ImGui::CollapsingHeader("Object"))
	{
		InputVec3("Object Pos", object.position);

		Vec3 rotEul = glm::degrees(glm::eulerAngles(object.rotation));
		ImGui::Text("Rot Quat: (%0.2f, %0.2f, %0.2f, %0.2f)\n", object.rotation.x, object.rotation.y, object.rotation.z, object.rotation.w);
		InputVec3("Object Rotation", rotEul);
		object.rotation = glm::quat(glm::radians(rotEul));

		InputVec3("Object Angular Vel", angularVelocity);
		InputVec3("Object Scale", object.scale);
	}

	if (ImGui::CollapsingHeader("Triangle Coordinates"))
	{
		int i = 0;
		ImGui::Text("Model Space");
		ImGui::Text("TODO (currently broken)\n");
		//for (const auto& p : renderer.metrics.modelSpace.p)
		//	ImGui::Text("\t v%d: (%0.2f, %0.2f, %0.2f)", i++, p.x, p.y, p.z);

		i = 0;
		ImGui::Text("World Space");
		ImGui::Text("TODO (currently broken)\n");
		/*for (const auto& p : renderer.metrics.worldSpace.p)
			ImGui::Text("\t v%d: (%0.2f, %0.2f, %0.2f, %0.2f)", i++, p.x, p.y, p.z, p.w);*/

		i = 0;
		ImGui::Text("View Space");
		ImGui::Text("TODO (currently broken)\n");
		//for (const auto& p : renderer.metrics.viewSpace.p)
		//	ImGui::Text("\t v%d: (%0.2f, %0.2f, %0.2f, %0.2f)", i++, p.x, p.y, p.z, p.w);

		i = 0;
		ImGui::Text("Clip Space");
		for (const auto& v : renderer.metrics.clipSpace.v)
			ImGui::Text("\t v%d: (%0.2f, %0.2f, %0.2f, %0.2f)", i++, v.p.x, v.p.y, v.p.z, v.p.w);

		i = 0;
		ImGui::Text("Normalized Device Coordinates");
		for (const auto& v : renderer.metrics.ndcSpace.v)
			ImGui::Text("\t v%d: (%0.2f, %0.2f, %0.2f, %0.2f)", i++, v.p.x, v.p.y, v.p.z, v.p.w);

		i = 0;
		ImGui::Text("Screen Space");
		for (const auto& v : renderer.metrics.screenSpace.v)
			ImGui::Text("\t v%d: (%0.2f, %0.2f, %0.2f, %0.2f)", i++, v.p.x, v.p.y, v.p.z, v.p.w);
	}

	ImGui::End();
}