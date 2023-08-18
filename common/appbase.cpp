#include "appbase.h"

#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

static GLint glVersion{46};  // set glversion,such as 33 mean use version 33 , if 0, use latest

static void window_size_callback(GLFWwindow *window, int width, int height) {}
static void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
    Input::onFramebufferResize(width, height);
}
static void window_content_scale_callback(GLFWwindow *window, float xscale, float yscale) {
}
static void window_pos_callback(GLFWwindow *window, int xpos, int ypos) {}
static void window_iconify_callback(GLFWwindow *window, int iconified) {
    if (iconified) {
        // The window was iconified
    } else {
        // The window was restored
    }
}
static void window_maximize_callback(GLFWwindow *window, int maximized) {
    if (maximized) {
        // The window was maximized
    } else {
        // The window was restored
    }
}
static void window_focus_callback(GLFWwindow *window, int focused) {
    if (focused) {
        // The window gained input focus
    } else {
        // The window lost input focus
    }
}

void window_refresh_callback(GLFWwindow *window) {
    // draw_editor_ui(window);
    // glfwSwapBuffers(window);
}

static void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods) {
    Input::onKeyBoard(key, scancode, action, mods);
}
static void character_callback(GLFWwindow *window, unsigned int codepoint) {}
static void cursor_position_callback(GLFWwindow *window, double xpos, double ypos) {
    Input::onMouseMove(xpos, ypos);
}
static void cursor_enter_callback(GLFWwindow *window, int entered) {
    if (entered) {
        // The cursor entered the content area of the window
    } else {
        // The cursor left the content area of the window
    }
}
static void mouse_button_callback(GLFWwindow *window, int button, int action, int mods) {
    Input::onMouseButton(button, action, mods);
}
static void scroll_callback(GLFWwindow *window, double xoffset, double yoffset) {
    Input::scrollSignal(xoffset, yoffset);
}
static void joystick_callback(int jid, int event) {
    if (event == GLFW_CONNECTED) {
        // The joystick was connected
    } else if (event == GLFW_DISCONNECTED) {
        // The joystick was disconnected
    }
}
static void drop_callback(GLFWwindow *window, int count, const char **paths) {
    // int i;
    // for (i = 0;  i < count;  i++)
    //     handle_dropped_file(paths[i]);
}

//============================================================
AppBase::AppBase() {}
AppBase::~AppBase() {
    glfwDestroyWindow(window);
    glfwTerminate();
}
void AppBase::initWindow() {
    startTime = std::chrono::system_clock::now();
    auto err = glfwInit();
    if (err == GLFW_FALSE) {
        THROW("failed to init glfw,err", err);
    }

    // set gl versiont
    if (glVersion > 0) {
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, glVersion / 10);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, glVersion % 10);
    }
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifndef NDEBUG
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
#endif
    glfwWindowHint(GLFW_SRGB_CAPABLE, GL_TRUE);
    glfwWindowHint(GLFW_SAMPLES, 4);

    window = glfwCreateWindow(1200, 800, __FILE__, nullptr, nullptr);
    if (!window) {
        // char strs[10][200];
        const char *data;
        err = glfwGetError(&data);
        glfwTerminate();
        THROW("failed to create window", err, "description", data);
    }

    glfwMakeContextCurrent(window);
    glfwSetWindowUserPointer(window, this);

    glfwSetWindowSizeCallback(window, window_size_callback);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetWindowContentScaleCallback(window, window_content_scale_callback);
    glfwSetWindowPosCallback(window, window_pos_callback);
    glfwSetWindowIconifyCallback(window, window_iconify_callback);
    glfwSetWindowMaximizeCallback(window, window_maximize_callback);
    glfwSetWindowFocusCallback(window, window_focus_callback);
    glfwSetWindowRefreshCallback(window, window_refresh_callback);

    glfwSetKeyCallback(window, key_callback);
    glfwSetCharCallback(window, character_callback);
    glfwSetCursorPosCallback(window, cursor_position_callback);
    glfwSetCursorEnterCallback(window, cursor_enter_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetJoystickCallback(joystick_callback);
    glfwSetDropCallback(window, drop_callback);

    glfwSwapInterval(0);  // enable v sync

    glfwGetFramebufferSize(window, &Input::framebufferWidth, &Input::framebufferHeight);
}
void AppBase::initOpengl() {
    GLenum err = glewInit();
    if (GLEW_OK != err) {
        /* Problem: glewInit failed, something is seriously wrong. */
        fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
        THROW("failed to init glew");
    }

#ifndef NDEBUG
    listGLInfo();
    GL::EnableDebugOutput(this);
#endif

    glEnable(GL_FRAMEBUFFER_SRGB);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glFrontFace(GL_CCW);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
    glBlendEquation(GL_FUNC_ADD);
}
void AppBase::mainLoop() {
    curTime = std::chrono::system_clock::now();
    while (!glfwWindowShouldClose(window)) {
        auto temptime = std::chrono::system_clock::now();
        frameTimeInterval_ms =
            std::chrono::duration<float, std::chrono::milliseconds::period>(temptime - curTime).count();
        curTime = temptime;
        FPS = static_cast<uint32_t>(1000 / frameTimeInterval_ms);

        glfwPollEvents();

        renderSignal(frameTimeInterval_ms);

        glfwSwapBuffers(window);
    }
}