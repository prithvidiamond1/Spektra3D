#include "App.h"


//--------------------------------------------------------

#ifdef DESKTOP_BG_MODE
#ifdef _WIN32

// code for desktop background mode


BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam) {
    HWND p = FindWindowEx(hwnd, NULL, L"SHELLDLL_DefView", NULL);
    HWND* ret = (HWND*)lParam;

    if (p) {
        // Gets the WorkerW Window after the current one.
        *ret = FindWindowEx(NULL, hwnd, L"WorkerW", NULL);
        return false;
    }
    return true;
}

HWND get_wallpaper_window() {
    // Fetch the Progman window
    HWND progman = FindWindow(L"ProgMan", NULL);
    // Send 0x052C to Progman. This message directs Progman to spawn a 
    // WorkerW behind the desktop icons. If it is already there, nothing 
    // happens.
    SendMessageTimeout(progman, 0x052C, 0, 0, SMTO_NORMAL, 1000, nullptr);
    
    // We enumerate all Windows, until we find one, that has the SHELLDLL_DefView 
    // as a child. 
    // If we found that window, we take its next sibling and assign it to workerw.
    HWND wallpaper_hwnd = nullptr;
    EnumWindows(EnumWindowsProc, (LPARAM)&wallpaper_hwnd);
    // Return the handle you're looking for.
    return wallpaper_hwnd;
}

#endif
#endif

// -------------------------------------------

Camera App::cam;
bool App::firstMouseDetection;
float App::lastXpos;
float App::lastYpos;

float App::deltaTime;
float App::lastFrame;

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

    // Enable transparent framebuffer
    //glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, GLFW_TRUE);

#ifdef DESKTOP_BG_MODE
    // for testing desktop background mode
    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE); 
#endif

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);           // Required on Mac
#endif
    
#ifndef DESKTOP_BG_MODE
    // Create window with graphics context
    Window = glfwCreateWindow(w, h, title.c_str(), NULL, NULL);

    glfwSetWindowPos(Window, w/4, h/4);
#else 
    // for desktop background mode
    const GLFWvidmode* videoMode = glfwGetVideoMode(glfwGetPrimaryMonitor());

    // Create window with graphics context
    Window = glfwCreateWindow(videoMode->width, videoMode->height, title.c_str(), NULL, NULL);
    if (Window == NULL)
    {
        fprintf(stderr, "Failed to initialize GLFW window!\n");
        abort();
    }

    glfwSetWindowPos(Window, 0, 0);
#endif


    glfwMakeContextCurrent(Window);
    //glfwSwapInterval(1);    // Enables vsync but maybe not needed

    glfwSetWindowTitle(Window, title.c_str());

    // Set callbacks
    // disabled for desktop background mode
#ifndef DESKTOP_BG_MODE
#ifdef CAPTURE_MOUSE 
    // tell GLFW to capture our mouse
    glfwSetInputMode(Window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
#endif

    // set framebuffersizecallback for adjusting size at runtime
    glfwSetFramebufferSizeCallback(Window, framebuffer_size_callback);
#endif
    glfwSetCursorPosCallback(Window, mouse_movement_callback);
    glfwSetScrollCallback(Window, mouse_scroll_callback);

    // adding code for desktop background mode
#ifdef DESKTOP_BG_MODE
#ifdef _WIN32
    HWND glfwWindowHandle = glfwGetWin32Window(Window);
    HWND wallpaperWindowHandle = get_wallpaper_window();

    if (wallpaperWindowHandle) {
        SetParent(glfwWindowHandle, wallpaperWindowHandle);
        ShowWindow(glfwWindowHandle, SW_SHOW);

        // Get the current window style
        LONG style = GetWindowLong(glfwWindowHandle, GWL_STYLE);
        style &= ~(
            WS_CAPTION |
            WS_THICKFRAME |
            WS_SYSMENU |
            WS_MAXIMIZEBOX |
            WS_MINIMIZEBOX
            );
        style |= WS_CHILD;
        SetWindowLong(glfwWindowHandle, GWL_STYLE, style);

        // Get the current extended window style
        LONG exStyle = GetWindowLong(glfwWindowHandle, GWL_EXSTYLE);
        exStyle &= ~(
            WS_EX_DLGMODALFRAME |
            WS_EX_COMPOSITED |
            WS_EX_WINDOWEDGE |
            WS_EX_CLIENTEDGE |
            WS_EX_LAYERED |
            WS_EX_STATICEDGE |
            WS_EX_TOOLWINDOW |
            WS_EX_APPWINDOW
            );
        SetWindowLong(glfwWindowHandle, GWL_EXSTYLE, exStyle);

        DWORD dwError = GetLastError();
        if (dwError) {
            // Handle or report the error
            std::cout << "Error: " << dwError << std::endl;
        }

        //MoveWindow(glfwWindowHandle, 0, 0, videoMode->width, videoMode->height, TRUE);
        /*SetWindowPos(glfwWindowHandle, HWND_TOP, 0, 0, videoMode->width, videoMode->height,
            SWP_SHOWWINDOW | SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);*/

        SetWindowPos(glfwWindowHandle, wallpaperWindowHandle, 0, 0, 0, 0,
            SWP_SHOWWINDOW | SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);
    }
#endif
#endif
    // ---------------------------

    // Glad requires you to pass a function pointer that returns the context address
        // Note: reinterpret_cast is equivalent to a c-style cast, dont use lightly, always prefer static_cast
    if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
        unsigned int error_code = glad_glGetError();
        std::cout << "Error initializing Glad: " << error_code << std::endl; 
    }

    //Setting OpenGL Global State
    glEnable(GL_DEPTH_TEST);

    glEnable(GL_BLEND); // enables blending
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
    glEnable(GL_PROGRAM_POINT_SIZE);
    //glPointSize(10.0f);


    // Setup Dear ImGui context
    //IMGUI_CHECKVERSION();
    //ImGui::CreateContext();
    ////ImPlot::CreateContext();
    //ImGui_ImplGlfw_InitForOpenGL(Window, true);
    //ImGui_ImplOpenGL3_Init(glsl_version);

    //// App Color scheme/theme
    //ImGui::StyleColorsDark();
    //ImPlot::StyleColorsDark();

    //ClearColor = { 0.2f, 0.3f, 0.3f, 1.0f };
    ClearColor = { 0.0f, 0.0f, 0.0f, 0.0f };

    // Initializing some of our static vars
    firstMouseDetection = true;
    lastXpos = w / (float)2;
    lastYpos = h / (float)2;

    deltaTime = 0.0f;
    lastFrame = 0.0f;

    OBA_Obj.setParams(2, ALB_Obj.ALB_captureSampleRate, 3, 16, 20000);

    this->bandCenterFreqs = OBA_Obj.getCenterFreqsOfBands();
    ALB_Obj.ALB_displayVector(this->bandCenterFreqs);

    //this->mesh = new Mesh(MESH_SIZE, TILE_SIZE);
    this->mesh = new Mesh(MESH_WIDTH, MESH_LENGTH, TILE_SIZE);

    this->mesh->shaderProgram.use();

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, 0.0f, -5.0f));
    int modelLocation = glGetUniformLocation(this->mesh->shaderProgram.getID(), "model");
    glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));

    glm::mat4 view = glm::mat4(
        { 0.99999, -1.04541E-07, -0.00523, -2.73422 },
        { -0.00234, 0.89415, -0.44775, -285.31427 },
        { 0.00468, 0.44776, 0.89414, -620.26868 },
        { 0.00, 0.00, 0.00, 1.00 }
    );
    view = glm::transpose(view);
    int viewLocation = glGetUniformLocation(this->mesh->shaderProgram.getID(), "view");
    glUniformMatrix4fv(viewLocation, 1, GL_FALSE, glm::value_ptr(view));

    // setting maxHeight uniform
    int maxHeightLocation = glGetUniformLocation(this->mesh->shaderProgram.getID(), "maxHeight");
    glUniform1f(maxHeightLocation, (float)maxHeight);
}

App::~App()
{
    /*ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();*/
    //ImPlot::DestroyContext();
    //ImGui::DestroyContext();

    delete this->mesh;

    glfwDestroyWindow(Window);
    glfwTerminate();
}

void App::update()
{
    std::vector<float> leftChData, rightChData;

    ALB_Obj.ALB_getAudioData(leftChData, rightChData);

    // Average channel outputs
    std::vector<float> avgChData;
    OBA_Obj.averageChannelOutputs(std::vector<std::vector<float>>({ leftChData, rightChData }), avgChData);;

    // Process the Audio Data   
    std::vector<float> avgChOut;
    OBA_Obj.analyseFrames(avgChData, avgChOut);

    std::vector<float> newRow;

    genNewHeightRow(avgChOut, newRow, this->mesh->getMeshWidth(), this->bandCenterFreqs.size());

    if (!checkNewRowForNan(newRow)) {
        newRow.assign(this->mesh->getMeshWidth(), 0);
    }

    this->mesh->heightMap.insert(this->mesh->heightMap.begin(), newRow.begin(), newRow.end());
    this->mesh->heightMap.erase(this->mesh->heightMap.begin() + (this->mesh->getMeshLength() * this->mesh->getMeshWidth()), this->mesh->heightMap.end());
    //applyDecay(this->mesh->heightMap, this->mesh->getMeshWidth(), 0.985);
    this->mesh->updateVertexHeights();

    this->mesh->shaderProgram.use();

    glm::mat4 projection = glm::perspective(glm::radians(cam.fov), GetWindowSize().x / (float)GetWindowSize().y, 0.1f, 20000.f);
    //glm::mat4 projection = glm::perspective(glm::radians(90.0f), GetWindowSize().x / (float)GetWindowSize().y, 0.1f, 10000.f);
    int projectionLocation = glGetUniformLocation(this->mesh->shaderProgram.getID(), "projection");
    glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, glm::value_ptr(projection));

    
    //glm::mat4 view = glm::lookAt(glm::vec3(0.0f, 3.0f, 0.0f), glm::vec3(0.0f, 0.0f, -5.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    /*glm::mat4 view = glm::mat4(
        { 0.99999, -1.04541E-07, -0.00523, -2.73422 },
        {-0.00234, 0.89415, -0.44775, -285.31427},
        {0.00468, 0.44776, 0.89414, -620.26868},
        {0.00, 0.00, 0.00, 1.00}
    );
    view = glm::transpose(view);*/
    
#if !defined(DESKTOP_BG_MODE) && defined(ENABLE_CAM)
    glm::mat4 view = cam.getViewMat();
    int viewLocation = glGetUniformLocation(this->mesh->shaderProgram.getID(), "view");
    glUniformMatrix4fv(viewLocation, 1, GL_FALSE, glm::value_ptr(view));
#endif
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
        /*ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();*/
        update();
        // Other computations
        float currentFrame = (float)glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        process_keyboard_input(Window);
        // Rendering
        glClearColor(ClearColor.x, ClearColor.y, ClearColor.z, ClearColor.w);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        this->mesh->draw();
        //// Error checking
        unsigned int glErrorFlag = glGetError();
        if (glErrorFlag) {
            std::cout << "OpenGL Error Flag: " << glErrorFlag << std::endl;
        }
        const char* glfwError = nullptr;
        if (glfwGetError(&glfwError)) {
            std::cerr << "GLFW Error: " << glfwError << std::endl;
        }
        //ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(Window, &display_w, &display_h);
        //ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
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

void App::mouse_movement_callback(GLFWwindow* window, double xPosInput, double yPosInput)
{
    float xPos = (float)xPosInput;
    float yPos = (float)yPosInput;

    if (firstMouseDetection) {
        lastXpos = xPos;
        lastYpos = yPos;
        firstMouseDetection = false;
    }

    float xOffset = xPos - lastXpos;
    float yOffset = lastYpos - yPos;    // reversed since y-coordinates go from bottom to top
    lastXpos = xPos;
    lastYpos = yPos;

    cam.processMouseMovement(xOffset, yOffset);
}

void App::mouse_scroll_callback(GLFWwindow* window, double xOffset, double yOffset)
{
    cam.processMouseScroll((float)yOffset);
}

void App::process_keyboard_input(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        cam.processKeyboardInput(CamMovement::FORWARD, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        cam.processKeyboardInput(CamMovement::BACKWARD, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        cam.processKeyboardInput(CamMovement::LEFT, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        cam.processKeyboardInput(CamMovement::RIGHT, deltaTime);
    }
}

// Note that the inputVec has negative dB values. Only their absolute values must be used.
void App::genNewHeightRow(const std::vector<float>& inputVec, std::vector<float>& outputVec, int meshWidth, int bandCount)
{
    int valsPerBand = meshWidth / bandCount;

    std::function<float(float, float, float)> curveGenFunc = [](float in, float max, float a) {
        return fabsf(max)/ (float)((1 + a * in * in)); 
    };

    float a = 0.01;
    float lowEdge = 1;

    int divisionsPerHalf = valsPerBand / 2;

    for (int i = 0; i < inputVec.size(); i++){
        
        float max = mapValToRange(fabsf(inputVec[i]), 0, 100, 0, maxHeight);

        float startEdge = sqrtf(((fabsf(max) / lowEdge) - 1) / a);

        std::vector<float> evalPoints;

        float intervalStride = 2 * startEdge / (2.0f * (divisionsPerHalf - 1));

        //float startPercent = 0.0625;
        float startPercent = 0.05;
        float previousEvalPoint = 0;

        evalPoints.push_back(0);
        for (int i = 0; i < divisionsPerHalf; i++) {
            float evalPoint = previousEvalPoint + startPercent * intervalStride;
            previousEvalPoint = evalPoint;

            evalPoints.insert(evalPoints.begin(), -1 * evalPoint);
            evalPoints.insert(evalPoints.end(), evalPoint);
            startPercent *= 2;
        }

        for (int i = 0; i < evalPoints.size(); i++) {
            outputVec.push_back(curveGenFunc(evalPoints[i], max, a));
        }
    }
}

bool App::checkNewRowForNan(const std::vector<float>& newRow)
{
    bool result = true;

    for (float val : newRow) {
        if (std::isnan(val)) {
            result = false;
            break;
        }
    }

    return result;
}

float App::mapValToRange(float inputVal, float curMin, float curMax, float newMin, float newMax)
{
    return (newMax - newMin) * (inputVal / (curMax - curMin));
}

void App::applyDecay(std::vector<float>& heightMap, int meshWidth, float decayVal)
{
    for (int i = meshWidth; i < heightMap.size(); i++) {
        heightMap[i] *= decayVal;
    }
}

