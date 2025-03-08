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

	triangle.vertCount = 3;
	triangle.v = {
		Vertex{Vec4(-0.87f, -0.5f, 0, 1), Vec3(0, 0, 1), Vec2(0, 0),	Vec3(1, 0, 0)},
		Vertex{Vec4(0.87f, -0.5f, 0, 1), Vec3(0, 0, 1), Vec2(1, 0),		Vec3(0, 1, 0)},
		Vertex{Vec4(0, 1, 0, 1), Vec3(0, 0, 1), Vec2(0, 1),				Vec3(0, 0, 1)},
	};

	Mat4 cubeModel = glm::scale(Mat4(1.0f), Vec3(0.5f, 0.5f, 0.5f));
	Mesh::LoadFromOBJ("res/cube.obj", cube, true, cubeModel);
	for (auto& v : cube.v) v.c = Vec3(RandomFloat(), RandomFloat(), RandomFloat());

	Mat4 suzanneModel = glm::scale(Mat4(1.0f), Vec3(0.5f, 0.5f, 0.5f));
	Mesh::LoadFromOBJ("res/suzanne.obj", suzanne, true, suzanneModel);
	for (auto& v : suzanne.v) v.c = Vec3(RandomFloat(), RandomFloat(), RandomFloat());

	Mat4 teapotModel = glm::translate(glm::scale(Mat4(1.0f), Vec3(0.2f, 0.2f, 0.2f)), Vec3(0, -1, 0));
	Mesh::LoadFromOBJ("res/teapot.obj", teapot, true, teapotModel);
	for (auto& v : teapot.v) v.c = Vec3(RandomFloat(), RandomFloat(), RandomFloat());

	Mat4 neekoModel = glm::scale(Mat4(1.0f), Vec3(1.5f, 1.5f, 1.5f));
	Mesh::LoadFromOBJ("res/neeko.obj", neeko, true, neekoModel);
	for (auto& v : neeko.v) v.c = Vec3(RandomFloat(), RandomFloat(), RandomFloat());

	Mat4 megamanModel = glm::translate(glm::scale(Mat4(1.0f), Vec3(.04f, .04f, .04f)), Vec3(0, 10, 0));
	Mesh::LoadFromOBJ("res/megaman/megaman.obj", megaman, true, megamanModel);

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

	if (isKeyDown[GLFW_KEY_H])	camera.transform.rotation = glm::quat(glm::vec3(0, -dt, 0)) * camera.transform.rotation;
	if (isKeyDown[GLFW_KEY_K])	camera.transform.rotation = glm::quat(glm::vec3(0, dt, 0)) * camera.transform.rotation;

	// Weeeeeeeird bug on web where K is I and I doesn't work. Just use ImGui instead
#ifdef __EMSCRIPTEN__
	if (ImGui::IsKeyDown(ImGuiKey_U))	camera.transform.rotation *= glm::quat(glm::vec3(-dt, 0, 0));
	if (ImGui::IsKeyDown(ImGuiKey_J))	camera.transform.rotation *= glm::quat(glm::vec3(dt, 0, 0));
#else
	if (isKeyDown[GLFW_KEY_U])	camera.transform.rotation *= glm::quat(glm::vec3(-dt, 0, 0));
	if (isKeyDown[GLFW_KEY_J])	camera.transform.rotation *= glm::quat(glm::vec3(dt, 0, 0));
#endif

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
	vsDefault.SetModel(object.GetTransform());
	vsDefault.SetView(camera.GetViewMatrix());
	vsDefault.SetProjection(camera.GetProjectionMatrix());

	vsPS1.SetMVP(camera.GetProjectionMatrix() * camera.GetViewMatrix() * object.GetTransform());

	Renderer::RenderCommand c = {
		meshes[currentMeshIndex],
		textures[currentTextureIndex],
		vertexShaders[currentVertexShaderIdx]
	};
	renderer.SendCommand(c);

	renderer.FlushCommands();
	renderer.BlitToScreen();

	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

// Drawing the FBO to the screen works on desktop but not with WebGL in Emscripten
// for some reason, so we're doing a little hack where we draw our screen texture
// into an ImGui window
#ifdef __EMSCRIPTEN__
	ImGuiWindowFlags SCREEN_FLAGS = ImGuiWindowFlags_NoTitleBar |
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoCollapse |
		ImGuiWindowFlags_NoScrollbar |
		ImGuiWindowFlags_NoScrollWithMouse |
		ImGuiWindowFlags_NoBringToFrontOnFocus |
		ImGuiWindowFlags_NoNavFocus |
		ImGuiWindowFlags_NoDecoration |
		ImGuiWindowFlags_NoInputs |
		ImGuiWindowFlags_NoNavInputs |
		ImGuiWindowFlags_NoMove;

	ImGui::SetNextWindowPos(ImVec2(0, 0));
	ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);

	ImGui::Begin("Framebuffer", nullptr, SCREEN_FLAGS);
	ImGui::Image((ImTextureID)renderer.GetGLScreenTexture(), ImVec2(SCREEN_WIDTH, SCREEN_HEIGHT), {0, 1}, { 1, 0 });
	ImGui::End();
#endif

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

	const char* presets[] = { "Spinning Cube", "Neeko", "PS1 Megaman", "Triangle" };
	if (ImGui::Combo("Preset", &currentPreset, presets, std::size(presets)))
	{
		object.position = Vec3(0.0f, 0.0f, 3.0f);
		object.rotation = Quat();
		object.scale = Vec3(1.0f);
		camera.transform.position = Vec3(0.0f, 0.0f, 0.0f);
		camera.transform.rotation = Quat();

		// Spinning Cube preset
		if (currentPreset == 0)
		{
			angularVelocity = Vec3(0.2f, -1.0f, -1.5f);
			currentMeshIndex = 1;
			currentTextureIndex = 0;
			currentVertexShaderIdx = 0;

			renderer.config.polygonMode = Renderer::PolygonMode::FILLED;
			renderer.config.objectColor = Renderer::ObjectColor::TEXTURED;
			renderer.config.doBackfaceCulling = true;
			renderer.config.doPerspCorrectInterp = true;
			renderer.config.useDepthBuffer = true;


		}
		// Neeko preset
		else if (currentPreset == 1)
		{
			angularVelocity = Vec3(0.0f, -1.0f, 0.0f);
			currentMeshIndex = 4;
			currentTextureIndex = 1;
			currentVertexShaderIdx = 0;

			renderer.config.polygonMode = Renderer::PolygonMode::FILLED;
			renderer.config.objectColor = Renderer::ObjectColor::TEXTURED;
			renderer.config.useDepthBuffer = true;
			renderer.config.doBackfaceCulling = true;
			renderer.config.doPerspCorrectInterp = true;
		}
		// Megaman PS1 preset
		else if (currentPreset == 2)
		{
			angularVelocity = Vec3(0.0f, -1.0f, 0.0f);
			currentMeshIndex = 5;
			currentTextureIndex = 2;
			currentVertexShaderIdx = 1;
			vsPS1.SetRounding(50.0f);

			renderer.config.polygonMode = Renderer::PolygonMode::FILLED;
			renderer.config.objectColor = Renderer::ObjectColor::TEXTURED;
			renderer.config.useDepthBuffer = true;
			renderer.config.doBackfaceCulling = true;
			renderer.config.doPerspCorrectInterp = false;
		}
		// Triangle
		else if (currentPreset == 3)
		{
			angularVelocity = Vec3(0.0f, 0.0f, 0.0f);
			currentMeshIndex = 0;
			currentVertexShaderIdx = 0;

			renderer.config.polygonMode = Renderer::PolygonMode::FILLED;
			renderer.config.objectColor = Renderer::ObjectColor::COLORED_VERTS;
			renderer.config.doPerspCorrectInterp = true;
		}
	}

	ImGui::Text("W/S - Forward/Back, A/D - Left/Right, Q/E - Up/Down");
	ImGui::Text("U/J - Look Up/Down, H/K - Look Left/Right");
	ImGui::Text("R - Cycle next mesh");
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

	if (renderer.config.polygonMode == Renderer::PolygonMode::FILLED)
	{
		const char* ocTypes[] = { "Flat", "Coloured Vertices", "Checkerboard", "Textured" };
		ImGui::Combo("Object Color", (int*)&renderer.config.objectColor, ocTypes, std::size(ocTypes));
		if (renderer.config.objectColor == Renderer::ObjectColor::TEXTURED)
			ImGui::Combo("Texture", &currentTextureIndex, &textureNames[0], static_cast<int>(textures.size()));
	}

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

	if (ImGui::CollapsingHeader("Vertex Shader"))
	{
		ImGui::Combo("Current VS", &currentVertexShaderIdx, &vsNames[0], static_cast<int>(vertexShaders.size()));
		
		if (currentVertexShaderIdx == 1)
		{
			float r = vsPS1.GetRounding();
			ImGui::InputFloat("Rounding", &r, 0.0f, 0.0f, "%.1f");
			vsPS1.SetRounding(r);
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

	if (ImGui::CollapsingHeader("Mesh Coordinates"))
	{
		uint32_t count = meshes[currentMeshIndex]->v.size() / 3;

		if (ImGui::ArrowButton("##left", ImGuiDir_Left)) 
			renderer.metrics.curTriIdx = (renderer.metrics.curTriIdx == 0) ? count - 1 : renderer.metrics.curTriIdx - 1;;
		ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);
		if (ImGui::ArrowButton("##right", ImGuiDir_Right)) 
			renderer.metrics.curTriIdx = (renderer.metrics.curTriIdx + 1) % count;

		ImGui::SameLine();
		ImGui::Text("Current Triangle: %d/%d", renderer.metrics.curTriIdx + 1, count);

		int i = 0;
		ImGui::Text("Model Space");
		for (const auto& p : renderer.metrics.modelSpace)
			ImGui::Text("\t v%d: (%0.2f, %0.2f, %0.2f)", i++, p.x, p.y, p.z);

		i = 0;
		ImGui::Text("World Space");
		for (const auto& p : renderer.metrics.worldSpace)
			ImGui::Text("\t v%d: (%0.2f, %0.2f, %0.2f, %0.2f)", i++, p.x, p.y, p.z, p.w);

		i = 0;
		ImGui::Text("View Space");
		for (const auto& p : renderer.metrics.viewSpace)
			ImGui::Text("\t v%d: (%0.2f, %0.2f, %0.2f, %0.2f)", i++, p.x, p.y, p.z, p.w);

		i = 0;
		ImGui::Text("Clip Space");
		for (const auto& v : renderer.metrics.clipSpace)
			ImGui::Text("\t v%d: (%0.2f, %0.2f, %0.2f, %0.2f)", i++, v.x, v.y, v.z, v.w);

		i = 0;
		ImGui::Text("Normalized Device Coordinates");
		for (const auto& v : renderer.metrics.ndcSpace)
			ImGui::Text("\t v%d: (%0.2f, %0.2f, %0.2f, %0.2f)", i++, v.x, v.y, v.z, v.w);

		i = 0;
		ImGui::Text("Screen Space");
		for (const auto& v : renderer.metrics.screenSpace)
			ImGui::Text("\t v%d: (%.0f, %.0f, %0.2f, %0.2f)", i++, v.x, v.y, v.z, v.w);
	}
	
	ImGui::End();
}

float Scene::RandomFloat() const
{
	return static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
}
