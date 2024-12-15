#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <glad/glad.h>

//#ifdef __EMSCRIPTEN__
//#include <GLES2/gl2.h>
//#endif
#include <GLFW/glfw3.h>


#include "scene.h"
#include "common.h"
#include <stdio.h>
#include <iostream>

#ifdef __EMSCRIPTEN__
#include <emscripten/emscripten.h>
#endif

GLFWwindow* window;
Scene scene;

double oldTime;
double g_dt;
double g_fps;

#define MAX_SAMPLES 10
int tickIdx = 0;
double deltaTimes[MAX_SAMPLES];

void UpdateDrawFrame(void);

static void glfw_error_callback(int error, const char* description)
{
	fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}

static void glfw_key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		exit(0);

	scene.OnKeyInput(key, action);
}

int main()
{
	// Create window
	glfwSetErrorCallback(glfw_error_callback);
	if (!glfwInit()) 
		return 1;

	// Disabled b/c causes "hint not supported" on web wasm
	//glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	//glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	//glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __EMSCRIPTEN__
	std::cout << "WE ARE IN EMSCRIPTEN!  yar\n";
	EM_ASM(console.log("Debug: Emscripten runtime active."));

	//EmscriptenWebGLContextAttributes atrs;
	//emscripten_webgl_init_context_attributes(&atrs);
	//atrs.majorVersion = 2;
	//atrs.minorVersion = 0;
	//emctx = emscripten_webgl_create_context(id, &atrs);
	//emscripten_webgl_make_context_current(emctx);
	//std::cout << "GL_VERSION=" << glGetString(GL_VERSION) << std::endl;
#endif

	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Renderer", NULL, NULL);
	glfwMakeContextCurrent(window);
	glfwSetKeyCallback(window, glfw_key_callback);
	//glfwSwapInterval(1); // vsync

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
		return 1;

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

	ImGui_ImplGlfw_InitForOpenGL(window, true);
#ifdef __EMSCRIPTEN__
	ImGui_ImplGlfw_InstallEmscriptenCallbacks(window, "#canvas");
	ImGui_ImplOpenGL3_Init("#version 300 es");
#else
	ImGui_ImplOpenGL3_Init("#version 130");
#endif
	

	glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
	glClearColor(0, 0, 0, 1);

	scene.Init();

	oldTime = glfwGetTime();

#ifdef __EMSCRIPTEN__
	// Web main loop
	emscripten_set_main_loop(UpdateDrawFrame, 0, 1);
#else
	// Desktop main loop
	while (!glfwWindowShouldClose(window))
		UpdateDrawFrame();
#endif

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}


void UpdateDrawFrame(void)
{
	g_dt = glfwGetTime() - oldTime;
	oldTime = glfwGetTime();

	deltaTimes[tickIdx] = g_dt;
	tickIdx = (tickIdx + 1) % MAX_SAMPLES;
	double sum = 0.0f;
	for (int i = 0; i < MAX_SAMPLES; i++)
		sum += deltaTimes[i];
	g_fps = 1 / (sum / MAX_SAMPLES);

	// Update
	scene.Update(static_cast<float>(g_dt));

	// Render
	glfwPollEvents();
	scene.Render();

#ifndef __EMSCRIPTEN__
	glfwSwapBuffers(window);
#endif
}