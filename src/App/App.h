#pragma once

//#define DESKTOP_BG_MODE
//#define ENABLE_CAM
//#define CAPTURE_MOUSE

#define IMGUI_DEFINE_MATH_OPERATORS

#ifdef _WIN32
#define GLFW_EXPOSE_NATIVE_WIN32

#include <Windows.h>
#endif

#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include <GLFW/glfw3native.h>

#include "imgui.h"
#include "implot.h"
#include "imgui_internal.h"
#include "../OpenGL/Imgui/imgui_impl_glfw.h"
#include "../OpenGL/Imgui/imgui_impl_opengl3.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "../AudioLoopBack/ALB.h";
#include "../BandAnalyzers/Octave/Octave.h";

#include <string>

#include "../OpenGL/Meshes/PointCloud/Mesh.h"
#include "../OpenGL/Camera/Camera.h"

#include <functional>
#include <cmath>

#if defined(_MSC_VER) && (_MSC_VER >= 1900) && !defined(IMGUI_DISABLE_WIN32_FUNCTIONS)
#pragma comment(lib, "legacy_stdio_definitions")
#endif

class App {
private: 
	GLFWwindow* Window;

	ImVec4 ClearColor;

	AudioLoopBack ALB_Obj;

	OctaveBandAnalyser OBA_Obj;	// set filter order here

	Mesh* mesh;

	std::vector<float> bandCenterFreqs;

	const int MESH_SIZE = 32; //32 * 9;
	const int MESH_LENGTH = 32; //128
	const int MESH_WIDTH = 32; // must be an odd number
	const int TILE_SIZE = 20; // originally 20

	static const int maxHeight = 500;

public:
	static Camera cam;	// there may only be one camera

	static bool firstMouseDetection;
	static float lastXpos;
	static float lastYpos;

	static float deltaTime;
	static float lastFrame;

	App(std::string title, int w, int h, int argc, char** argv);

	~App();

	void update();

	void run();

	ImVec2 GetWindowSize() const;

	static void framebuffer_size_callback(GLFWwindow* window, int w, int h);

	static void mouse_movement_callback(GLFWwindow* window, double xPosInput, double yPosInput);

	static void mouse_scroll_callback(GLFWwindow* window, double xOffset, double yOffset);

	static void process_keyboard_input(GLFWwindow* window);

	static void genNewHeightRow(const std::vector<float>& inputVec, std::vector<float>& outputVec, int meshWidth, int bandCount);

	static bool checkNewRowForNan(const std::vector<float>& newRow);

	static float mapValToRange(float inputVal, float curMin, float curMax, float newMin, float newMax);

	static void applyDecay(std::vector<float>& heightMap, int meshWidth, float decayVal = 0.99);
};