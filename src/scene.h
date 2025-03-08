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
	Mesh neeko;
	Mesh megaman;

	Texture testTexture{ "res/uv_map_2.png" };
	Texture neekoTexture{ "res/neeko.png" };
	Texture megamanTexture{ "res/megaman/megaman.png" };

	DefaultVertexShader vsDefault;
	PS1VertexShader vsPS1;

	int currentPreset = 0;

	int currentMeshIndex = 0;
	static constexpr char* meshNames[] = { "Triangle", "Cube", "Suzanne", "Teapot", "Neeko", "Megaman" };
	const std::vector<Mesh*> meshes = { &triangle, &cube, &suzanne, &teapot, &neeko, &megaman };

	int currentTextureIndex = 0;
	static constexpr char* textureNames[] = { "UV Map Test", "Neeko", "Megaman" };
	const std::vector<Texture*> textures = { &testTexture, &neekoTexture, &megamanTexture };

	int currentVertexShaderIdx = 0;
	static constexpr char* vsNames[] = { "Default", "PS1" };
	const std::vector<VertexShader*> vertexShaders = { &vsDefault, &vsPS1 };

	ImFont* font;
	void DrawConfigGUI();

	bool isKeyDown[GLFW_KEY_LAST];
	bool isKeyJustDown[GLFW_KEY_LAST];

	float frameTimes[FRAME_TIME_SAMPLES];
	int frameTimeIdx;
	float avgFrameTime;
	float sampleTime;

	float RandomFloat() const;

};