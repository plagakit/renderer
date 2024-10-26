﻿#include "raylib-cpp.hpp"
#include "imgui.h"
#include "rlImGui.h"

#ifdef PLATFORM_WEB
#include <emscripten/emscripten.h>
#endif

#include "scene.h"

const int SCREEN_WIDTH = 1280;
const int SCREEN_HEIGHT = 720;
Scene scene;

void UpdateDrawFrame(void);

int main()
{
	// Disable raylib warnings (GetWindowScaleDPI is warned every frame on web)
#ifdef PLATFORM_WEB
	SetTraceLogLevel(LOG_ERROR);
#endif

	// Create window
	SetConfigFlags(FLAG_VSYNC_HINT);
	InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Renderer");
	SetTargetFPS(GetMonitorRefreshRate(GetCurrentMonitor()));
	rlImGuiSetup(true);

	scene.Init();

	// Web main loop
#ifdef PLATFORM_WEB

	emscripten_set_main_loop(UpdateDrawFrame, 0, 1);
#else
	// Desktop main loop
	while (!WindowShouldClose())
		UpdateDrawFrame();
#endif

	rlImGuiShutdown();
	CloseWindow();
	return 0;
}


void UpdateDrawFrame(void)
{
	scene.Update(GetFrameTime());

	BeginDrawing();
	scene.Render();
	EndDrawing();
}