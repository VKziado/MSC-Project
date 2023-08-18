#pragma once
#include "common.h"
#include "game.h"
#include "gui.h"
#include "image.h"
#include "imgui.h"
#include "input.h"
#include "node.h"
#include "prerequisites.h"
#include "renderserver.h"
#include "scene.h"

class AppBase {
protected:
    uint32_t FPS;
    float frameTimeInterval_ms;
    std::chrono::system_clock::time_point curTime;
    std::chrono::system_clock::time_point startTime;

    GLFWwindow *window;
    bool framebufferResized{true};

public:
    AppBase();

    virtual ~AppBase();

    void initWindow();
    void initOpengl();

    void mainLoop();

    GLFWwindow *getWindow() const { return window; }

    Signal<void(float)> renderSignal;

    template <typename T, typename... Args>
    void run(Args &&...args) {
        initWindow();
        initOpengl();

        auto game = T(args...);
        game.setParent(this);

        renderSignal.Connect(&Game::render, &game);

        game.init();
        mainLoop();
    }
};
