#pragma once

#define IMGUI_DEFINE_MATH_OPERATORS

#include "glad/glad.h"
#include "GLFW/glfw3.h"

#include "imgui.h"
#include "implot.h"
#include "imgui_internal.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "ALB.h";
#include "Octave.h";

#include <string>

#include "Mesh.h"

#if defined(_MSC_VER) && (_MSC_VER >= 1900) && !defined(IMGUI_DISABLE_WIN32_FUNCTIONS)
#pragma comment(lib, "legacy_stdio_definitions")
#endif

class App {
private: 
	GLFWwindow* Window;

	ImVec4 ClearColor;

	AudioLoopBack ALB_Obj;

	OctaveBandAnalyser OBA_Obj;	// set filter order here

	Mesh mesh;

	std::vector<float> bandCenterFreqs;

	const int MESH_SIZE = 256;
	const int TILE_SIZE = 20;

public:
	App(std::string title, int w, int h, int argc, char** argv);

	~App();

	void update();

	void run();

	ImVec2 GetWindowSize() const;

	static void framebuffer_size_callback(GLFWwindow* window, int w, int h);
};