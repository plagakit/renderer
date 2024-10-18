#include "raylib-cpp.hpp"
#include "imgui.h"
#include "rlImGui.h"

#ifdef PLATFORM_WEB
#include <emscripten/emscripten.h>
#endif

#include "renderer.h"

int screenWidth = 1280;
int screenHeight = 720;
Renderer renderer;

void UpdateDrawFrame(void);

int main()
{
	// Disable raylib warnings (GetWindowScaleDPI is warned every frame on web)
#ifdef PLATFORM_WEB
	SetTraceLogLevel(LOG_ERROR);
#endif

	// Create window
	SetConfigFlags(FLAG_VSYNC_HINT);
	InitWindow(screenWidth, screenHeight, "Engine");
	SetTargetFPS(60);
	rlImGuiSetup(true);

	renderer.Init();

	// Web main loop
#ifdef PLATFORM_WEB

	emscripten_set_main_loop(UpdateDrawFrame, 0, 1);
#else
	// Desktop main loop
	while (!WindowShouldClose())
		UpdateDrawFrame();
#endif
	
	renderer.Shutdown();
	rlImGuiShutdown();
	CloseWindow();
	return 0;
}


void UpdateDrawFrame(void)
{
	renderer.Update(GetFrameTime());

	BeginDrawing();
	ClearBackground(BLACK);
	DrawFPS(0, 0);
	renderer.Render();
	EndDrawing();
}