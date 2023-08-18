#include "input.h"

Signal<void(int, int)> Input::framebufferResizeSignal;
Signal<void(KeyCode, KeyState)> Input::mouseButtonSignal;
Signal<void(double, double)> Input::mouseMoveSignal;
Signal<void(double, double)> Input::scrollSignal;
Signal<void(KeyCode, KeyState)> Input::keyboardSignal;
std::array<KeyState, INPUT_NUM> Input::inputState{};
bool Input::guiWantCaptureMouse = false;
int Input::framebufferWidth;
int Input::framebufferHeight;

namespace {
KeyCode getGLFWKeyCode(int scancode);
}

void Input::onKeyBoard(int key, int scancode, int action, int mods) {
    auto keycode = getGLFWKeyCode(scancode);
    auto keyState = STATE_RELEASE;

    if (keycode >= 0) {
        switch (action) {
            case GLFW_PRESS:
                keyState = inputState[keycode] = STATE_PRESS;
                break;
            case GLFW_RELEASE:
                keyState = inputState[keycode] = STATE_RELEASE;
                break;
        }
    }

    keyboardSignal(keycode, keyState);
}
void Input::onMouseButton(int button, int action, int mods) {
    auto keycode = getGLFWKeyCode(button);
    auto keyState = STATE_RELEASE;
    if (keycode >= 0) {
        switch (action) {
            case GLFW_PRESS:
                keyState = inputState[keycode] = STATE_PRESS;
                break;
            case GLFW_RELEASE:
                keyState = inputState[keycode] = STATE_RELEASE;
                break;
        }
    }
    mouseButtonSignal(keycode, keyState);
}
void Input::onMouseMove(double xpos, double ypos) { mouseMoveSignal(xpos, ypos); }
void Input::onFramebufferResize(int width, int height) {
    framebufferWidth = width;
    framebufferHeight = height;
    framebufferResizeSignal(width, height);
}

namespace {
KeyCode getGLFWKeyCode(int scancode) {
    switch (scancode) {
        case GLFW_KEY_SPACE:
            return INPUT_KEY_SPACE;          //
        case GLFW_KEY_APOSTROPHE:
            return INPUT_KEY_APOSTROPHE;     //
        case GLFW_KEY_COMMA:
            return INPUT_KEY_COMMA;          //
        case GLFW_KEY_MINUS:
            return INPUT_KEY_MINUS;          //
        case GLFW_KEY_PERIOD:
            return INPUT_KEY_PERIOD;         //
        case GLFW_KEY_SLASH:
            return INPUT_KEY_SLASH;          //
        case GLFW_KEY_0:
            return INPUT_KEY_0;              //
        case GLFW_KEY_1:
            return INPUT_KEY_1;              //
        case GLFW_KEY_2:
            return INPUT_KEY_2;              //
        case GLFW_KEY_3:
            return INPUT_KEY_3;              //
        case GLFW_KEY_4:
            return INPUT_KEY_4;              //
        case GLFW_KEY_5:
            return INPUT_KEY_5;              //
        case GLFW_KEY_6:
            return INPUT_KEY_6;              //
        case GLFW_KEY_7:
            return INPUT_KEY_7;              //
        case GLFW_KEY_8:
            return INPUT_KEY_8;              //
        case GLFW_KEY_9:
            return INPUT_KEY_9;              //
        case GLFW_KEY_SEMICOLON:
            return INPUT_KEY_SEMICOLON;      //
        case GLFW_KEY_EQUAL:
            return INPUT_KEY_EQUAL;          //
        case GLFW_KEY_A:
            return INPUT_KEY_A;              //
        case GLFW_KEY_B:
            return INPUT_KEY_B;              //
        case GLFW_KEY_C:
            return INPUT_KEY_C;              //
        case GLFW_KEY_D:
            return INPUT_KEY_D;              //
        case GLFW_KEY_E:
            return INPUT_KEY_E;              //
        case GLFW_KEY_F:
            return INPUT_KEY_F;              //
        case GLFW_KEY_G:
            return INPUT_KEY_G;              //
        case GLFW_KEY_H:
            return INPUT_KEY_H;              //
        case GLFW_KEY_I:
            return INPUT_KEY_I;              //
        case GLFW_KEY_J:
            return INPUT_KEY_J;              //
        case GLFW_KEY_K:
            return INPUT_KEY_K;              //
        case GLFW_KEY_L:
            return INPUT_KEY_L;              //
        case GLFW_KEY_M:
            return INPUT_KEY_M;              //
        case GLFW_KEY_N:
            return INPUT_KEY_N;              //
        case GLFW_KEY_O:
            return INPUT_KEY_O;              //
        case GLFW_KEY_P:
            return INPUT_KEY_P;              //
        case GLFW_KEY_Q:
            return INPUT_KEY_Q;              //
        case GLFW_KEY_R:
            return INPUT_KEY_R;              //
        case GLFW_KEY_S:
            return INPUT_KEY_S;              //
        case GLFW_KEY_T:
            return INPUT_KEY_T;              //
        case GLFW_KEY_U:
            return INPUT_KEY_U;              //
        case GLFW_KEY_V:
            return INPUT_KEY_V;              //
        case GLFW_KEY_W:
            return INPUT_KEY_W;              //
        case GLFW_KEY_X:
            return INPUT_KEY_X;              //
        case GLFW_KEY_Y:
            return INPUT_KEY_Y;              //
        case GLFW_KEY_Z:
            return INPUT_KEY_Z;              //
        case GLFW_KEY_LEFT_BRACKET:
            return INPUT_KEY_LEFT_BRACKET;   //
        case GLFW_KEY_BACKSLASH:
            return INPUT_KEY_BACKSLASH;      //
        case GLFW_KEY_RIGHT_BRACKET:
            return INPUT_KEY_RIGHT_BRACKET;  //
        case GLFW_KEY_GRAVE_ACCENT:
            return INPUT_KEY_GRAVE_ACCENT;   //
        case GLFW_KEY_WORLD_1:
            return INPUT_KEY_WORLD_1;        //
        case GLFW_KEY_WORLD_2:
            return INPUT_KEY_WORLD_2;        //
        case GLFW_KEY_ESCAPE:
            return INPUT_KEY_ESCAPE;         //
        case GLFW_KEY_ENTER:
            return INPUT_KEY_ENTER;          //
        case GLFW_KEY_TAB:
            return INPUT_KEY_TAB;            //
        case GLFW_KEY_BACKSPACE:
            return INPUT_KEY_BACKSPACE;      //
        case GLFW_KEY_INSERT:
            return INPUT_KEY_INSERT;         //
        case GLFW_KEY_DELETE:
            return INPUT_KEY_DELETE;         //
        case GLFW_KEY_RIGHT:
            return INPUT_KEY_RIGHT;          //
        case GLFW_KEY_LEFT:
            return INPUT_KEY_LEFT;           //
        case GLFW_KEY_DOWN:
            return INPUT_KEY_DOWN;           //
        case GLFW_KEY_UP:
            return INPUT_KEY_UP;             //
        case GLFW_KEY_PAGE_UP:
            return INPUT_KEY_PAGE_UP;        //
        case GLFW_KEY_PAGE_DOWN:
            return INPUT_KEY_PAGE_DOWN;      //
        case GLFW_KEY_HOME:
            return INPUT_KEY_HOME;           //
        case GLFW_KEY_END:
            return INPUT_KEY_END;            //
        case GLFW_KEY_CAPS_LOCK:
            return INPUT_KEY_CAPS_LOCK;      //
        case GLFW_KEY_SCROLL_LOCK:
            return INPUT_KEY_SCROLL_LOCK;    //
        case GLFW_KEY_NUM_LOCK:
            return INPUT_KEY_NUM_LOCK;       //
        case GLFW_KEY_PRINT_SCREEN:
            return INPUT_KEY_PRINT_SCREEN;   //
        case GLFW_KEY_PAUSE:
            return INPUT_KEY_PAUSE;          //
        case GLFW_KEY_F1:
            return INPUT_KEY_F1;             //
        case GLFW_KEY_F2:
            return INPUT_KEY_F2;             //
        case GLFW_KEY_F3:
            return INPUT_KEY_F3;             //
        case GLFW_KEY_F4:
            return INPUT_KEY_F4;             //
        case GLFW_KEY_F5:
            return INPUT_KEY_F5;             //
        case GLFW_KEY_F6:
            return INPUT_KEY_F6;             //
        case GLFW_KEY_F7:
            return INPUT_KEY_F7;             //
        case GLFW_KEY_F8:
            return INPUT_KEY_F8;             //
        case GLFW_KEY_F9:
            return INPUT_KEY_F9;             //
        case GLFW_KEY_F10:
            return INPUT_KEY_F10;            //
        case GLFW_KEY_F11:
            return INPUT_KEY_F11;            //
        case GLFW_KEY_F12:
            return INPUT_KEY_F12;            //
        case GLFW_KEY_F13:
            return INPUT_KEY_F13;            //
        case GLFW_KEY_F14:
            return INPUT_KEY_F14;            //
        case GLFW_KEY_F15:
            return INPUT_KEY_F15;            //
        case GLFW_KEY_F16:
            return INPUT_KEY_F16;            //
        case GLFW_KEY_F17:
            return INPUT_KEY_F17;            //
        case GLFW_KEY_F18:
            return INPUT_KEY_F18;            //
        case GLFW_KEY_F19:
            return INPUT_KEY_F19;            //
        case GLFW_KEY_F20:
            return INPUT_KEY_F20;            //
        case GLFW_KEY_F21:
            return INPUT_KEY_F21;            //
        case GLFW_KEY_F22:
            return INPUT_KEY_F22;            //
        case GLFW_KEY_F23:
            return INPUT_KEY_F23;            //
        case GLFW_KEY_F24:
            return INPUT_KEY_F24;            //
        case GLFW_KEY_F25:
            return INPUT_KEY_F25;            //
        case GLFW_KEY_KP_0:
            return INPUT_KEY_KP_0;           //
        case GLFW_KEY_KP_1:
            return INPUT_KEY_KP_1;           //
        case GLFW_KEY_KP_2:
            return INPUT_KEY_KP_2;           //
        case GLFW_KEY_KP_3:
            return INPUT_KEY_KP_3;           //
        case GLFW_KEY_KP_4:
            return INPUT_KEY_KP_4;           //
        case GLFW_KEY_KP_5:
            return INPUT_KEY_KP_5;           //
        case GLFW_KEY_KP_6:
            return INPUT_KEY_KP_6;           //
        case GLFW_KEY_KP_7:
            return INPUT_KEY_KP_7;           //
        case GLFW_KEY_KP_8:
            return INPUT_KEY_KP_8;           //
        case GLFW_KEY_KP_9:
            return INPUT_KEY_KP_9;           //
        case GLFW_KEY_KP_DECIMAL:
            return INPUT_KEY_KP_DECIMAL;     //
        case GLFW_KEY_KP_DIVIDE:
            return INPUT_KEY_KP_DIVIDE;      //
        case GLFW_KEY_KP_MULTIPLY:
            return INPUT_KEY_KP_MULTIPLY;    //
        case GLFW_KEY_KP_SUBTRACT:
            return INPUT_KEY_KP_SUBTRACT;    //
        case GLFW_KEY_KP_ADD:
            return INPUT_KEY_KP_ADD;         //
        case GLFW_KEY_KP_ENTER:
            return INPUT_KEY_KP_ENTER;       //
        case GLFW_KEY_KP_EQUAL:
            return INPUT_KEY_KP_EQUAL;       //
        case GLFW_KEY_LEFT_SHIFT:
            return INPUT_KEY_LEFT_SHIFT;     //
        case GLFW_KEY_LEFT_CONTROL:
            return INPUT_KEY_LEFT_CONTROL;   //
        case GLFW_KEY_LEFT_ALT:
            return INPUT_KEY_LEFT_ALT;       //
        case GLFW_KEY_LEFT_SUPER:
            return INPUT_KEY_LEFT_SUPER;     //
        case GLFW_KEY_RIGHT_SHIFT:
            return INPUT_KEY_RIGHT_SHIFT;    //
        case GLFW_KEY_RIGHT_CONTROL:
            return INPUT_KEY_RIGHT_CONTROL;  //
        case GLFW_KEY_RIGHT_ALT:
            return INPUT_KEY_RIGHT_ALT;      //
        case GLFW_KEY_RIGHT_SUPER:
            return INPUT_KEY_RIGHT_SUPER;    //
        case GLFW_KEY_MENU:
            return INPUT_KEY_MENU;           //
        case GLFW_MOUSE_BUTTON_LEFT:
            return INPUT_MOUSE_BUTTON_LEFT;
        case GLFW_MOUSE_BUTTON_RIGHT:
            return INPUT_MOUSE_BUTTON_RIGHT;
        case GLFW_MOUSE_BUTTON_MIDDLE:
            return INPUT_MOUSE_BUTTON_MIDDLE;
    }
    return {};
}

}  // namespace