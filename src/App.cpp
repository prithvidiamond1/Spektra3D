#include "App.h"

static void glfw_error_callback(int error, const char* description)
{
    fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}

App::App(std::string title, int w, int h, int argc, char** argv)
{
    // Setup window
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit()) {
        abort();
    }

    // GL 3.3 + GLSL 330
    const char* glsl_version = "#version 330";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // 3.2+ only

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);           // Required on Mac
#endif

    // Create window with graphics context
    Window = glfwCreateWindow(w, h, title.c_str(), NULL, NULL);
    if (Window == NULL)
    {
        fprintf(stderr, "Failed to initialize GLFW window!\n");
        abort();
    }
    glfwMakeContextCurrent(Window);
    //glfwSwapInterval(1);    // Enables vsync but maybe not needed

    glfwSetWindowTitle(Window, title.c_str());

    // Glad requires you to pass a function pointer that returns the context address
        // Note: reinterpret_cast is equivalent to a c-style cast, dont use lightly, always prefer static_cast
    if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
        unsigned int error_code = glad_glGetError();
        std::cout << "Error initializing Glad: " << error_code << std::endl; 
    }

    // framebuffer resize callback
    glfwSetFramebufferSizeCallback(Window, framebuffer_size_callback);

    //Setting OpenGL Global State
    glEnable(GL_DEPTH_TEST);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    //ImPlot::CreateContext();
    ImGui_ImplGlfw_InitForOpenGL(Window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    //// App Color scheme/theme
    //ImGui::StyleColorsDark();
    //ImPlot::StyleColorsDark();

    ClearColor = { 0.2f, 0.3f, 0.3f, 1.0f };

    OBA_Obj.setParams(2, ALB_Obj.ALB_captureSampleRate, 3, 20, 20000);

    this->bandCenterFreqs = OBA_Obj.getCenterFreqsOfBands();
    ALB_Obj.ALB_displayVector(this->bandCenterFreqs);

    this->mesh = Mesh(MESH_SIZE, TILE_SIZE);

    glm::mat4 projection = glm::perspective(glm::radians(45.0f), w / (float)h, 0.1f, 1000.f);
    int projectionLocation = glGetUniformLocation(this->mesh.shaderProgram.getID(), "projection");
    glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, glm::value_ptr(projection));

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, 0.0f, -5.0f));
    int modelLocation = glGetUniformLocation(this->mesh.shaderProgram.getID(), "model");
    glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));
}

App::~App()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    //ImPlot::DestroyContext();
    ImGui::DestroyContext();
    glfwDestroyWindow(Window);
    glfwTerminate();
}

void App::update()
{
    std::vector<float> leftChData, rightChData;

    ALB_Obj.ALB_getAudioData(leftChData, rightChData);

    // Process the Audio Data
    std::vector<float> leftChOut, rightChOut;

    OBA_Obj.analyseFrames(leftChData, leftChOut);
    OBA_Obj.analyseFrames(rightChData, rightChOut);

    //// Draw graph
    //// Set up ImGUI
    //ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always);
    //ImGui::SetNextWindowSize(GetWindowSize(), ImGuiCond_Always);
    //ImGui::Begin("AudioLoopBackSpectrums", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar);

    //// Left Channel graph
    //ImGui::BeginChild("LeftChannelSpectrumSection", ImVec2(ImGui::GetWindowContentRegionWidth() * 0.5f, -1));
    //ImGui::Text("Left Channel Spectrum");
    //ImGui::Separator();

    //// plot Left Channel data
    //if (ImPlot::BeginPlot("LeftChannelSpectrumPlot", ImVec2(-1,-1))) {
    //    ImPlot::SetupAxesLimits(16, ALB_Obj.ALB_captureSampleRate / 2, -100, 0);
    //    ImPlot::SetupAxes("Frequency (Hz)", "Amplitude (dB)", 0, ImPlotAxisFlags_Invert);
    //    ImPlot::SetupAxisScale(ImAxis_X1, ImPlotScale_SymLog);
    //    //ImPlot::SetupAxisScale(ImAxis_Y1, ImPlotScale_Log10);

    //    ImPlot::PlotBars("LeftAmplitudeCurve", this->bandCenterFreqs.data(), leftChOut.data(), leftChOut.size(), 1);

    //    ImPlot::EndPlot();
    //}

    //ImGui::EndChild();


    //ImGui::SameLine();

    //// Right Channel graph
    //ImGui::BeginChild("RightChannelSpectrumSection", ImVec2(0, -1));
    //ImGui::Text("Right Channel Spectrum");
    //ImGui::Separator();

    //// plot Right Channel data
    //if (ImPlot::BeginPlot("RightChannelSpectrumPlot", ImVec2(-1, -1))) {
    //    ImPlot::SetupAxesLimits(16, ALB_Obj.ALB_captureSampleRate / 2, -100, 0);
    //    ImPlot::SetupAxes("Frequency (Hz)", "Amplitude (dB)", 0, ImPlotAxisFlags_Invert);
    //    ImPlot::SetupAxisScale(ImAxis_X1, ImPlotScale_SymLog);
    //    //ImPlot::SetupAxisScale(ImAxis_Y1, ImPlotScale_Log10);

    //    ImPlot::PlotBars("RightAmplitudeCurve", this->bandCenterFreqs.data(), rightChOut.data(), rightChOut.size(), 1);

    //    ImPlot::EndPlot();
    //}

    //ImGui::EndChild();
    //ImGui::End();
}

void App::run()
{
    // ALB start
    ALB_Obj.ALB_start();

    // main loop
    while (!glfwWindowShouldClose(Window))
    {
        glfwPollEvents();
        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        update();
        // Rendering
        this->mesh.draw();
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(Window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(ClearColor.x, ClearColor.y, ClearColor.z, ClearColor.w);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(Window);
    }

    // ALB finish
    ALB_Obj.ALB_finish();
}

ImVec2 App::GetWindowSize() const
{
    int w, h;
    glfwGetWindowSize(Window, &w, &h);
    return ImVec2(w, h);
}

void App::framebuffer_size_callback(GLFWwindow* window, int w, int h)
{
    glViewport(0, 0, w, h);
}
