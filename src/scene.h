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
	WarbleVertexShader vsWarble;

	int currentPreset = 0;

	template<typename T>
	struct ComboSelector {
		const char* title;
		std::vector<const char*> names;
		std::vector<T*> items;

		int currentIndex = 0;

		bool DisplayCombo() { return ImGui::Combo(title, &currentIndex, names.data(), static_cast<int>(items.size())); }
		void Increment() { currentIndex = (currentIndex + 1) % static_cast<int>(items.size()); }
		void Decrement() { currentIndex = (currentIndex - 1 + static_cast<int>(items.size())) % static_cast<int>(items.size()); }
		T* Current() { return items[currentIndex]; }
		void SetCurrent(const char* targetName) 
		{
			for (size_t i = 0; i < names.size(); i++)
				if (std::strcmp(names[i], targetName) == 0)
					currentIndex = static_cast<int>(i);
		}
	};

	ComboSelector<Mesh> meshes = {
		"Mesh",
		{ "Triangle", "Cube", "Suzanne", "Teapot", "Neeko", "Megaman" },
		{ &triangle, &cube, &suzanne, &teapot, &neeko, &megaman }
	};

	ComboSelector<Texture> textures = {
		"Texture",
		{ "UV Map Test", "Neeko", "Megaman" },
		{ &testTexture, &neekoTexture, &megamanTexture }
	};

	ComboSelector<VertexShader> vertexShaders = {
		"Vertex Shader",
		{ "Default", "PS1", "Warble"},
		{ &vsDefault, &vsPS1, &vsWarble}
	};

	// Window management	

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