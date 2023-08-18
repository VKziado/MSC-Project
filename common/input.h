#pragma once
#include "prerequisites.h"

enum KeyCode {
    INPUT_KEY_SPACE,            //              32
    INPUT_KEY_APOSTROPHE,       //         39  /* ' */
    INPUT_KEY_COMMA,            //              44  /* , */
    INPUT_KEY_MINUS,            //              45  /* - */
    INPUT_KEY_PERIOD,           //             46  /* . */
    INPUT_KEY_SLASH,            //              47  /* / */
    INPUT_KEY_0,                //                  48
    INPUT_KEY_1,                //                  49
    INPUT_KEY_2,                //                  50
    INPUT_KEY_3,                //                  51
    INPUT_KEY_4,                //                  52
    INPUT_KEY_5,                //                  53
    INPUT_KEY_6,                //                  54
    INPUT_KEY_7,                //                  55
    INPUT_KEY_8,                //                  56
    INPUT_KEY_9,                //                  57
    INPUT_KEY_SEMICOLON,        //          59  /* ; */
    INPUT_KEY_EQUAL,            //              61  /* = */
    INPUT_KEY_A,                //                  65
    INPUT_KEY_B,                //                  66
    INPUT_KEY_C,                //                  67
    INPUT_KEY_D,                //                  68
    INPUT_KEY_E,                //                  69
    INPUT_KEY_F,                //                  70
    INPUT_KEY_G,                //                  71
    INPUT_KEY_H,                //                  72
    INPUT_KEY_I,                //                  73
    INPUT_KEY_J,                //                  74
    INPUT_KEY_K,                //                  75
    INPUT_KEY_L,                //                  76
    INPUT_KEY_M,                //                  77
    INPUT_KEY_N,                //                  78
    INPUT_KEY_O,                //                  79
    INPUT_KEY_P,                //                  80
    INPUT_KEY_Q,                //                  81
    INPUT_KEY_R,                //                  82
    INPUT_KEY_S,                //                  83
    INPUT_KEY_T,                //                  84
    INPUT_KEY_U,                //                  85
    INPUT_KEY_V,                //                  86
    INPUT_KEY_W,                //                  87
    INPUT_KEY_X,                //                  88
    INPUT_KEY_Y,                //                  89
    INPUT_KEY_Z,                //                  90
    INPUT_KEY_LEFT_BRACKET,     //       91  /* [ */
    INPUT_KEY_BACKSLASH,        //          92  /* \ */
    INPUT_KEY_RIGHT_BRACKET,    //      93  /* ] */
    INPUT_KEY_GRAVE_ACCENT,     //       96  /* ` */
    INPUT_KEY_WORLD_1,          //            161 /* non-US #1 */
    INPUT_KEY_WORLD_2,          //            162 /* non-US #2 */
    INPUT_KEY_ESCAPE,           //             256
    INPUT_KEY_ENTER,            //              257
    INPUT_KEY_TAB,              //                258
    INPUT_KEY_BACKSPACE,        //          259
    INPUT_KEY_INSERT,           //             260
    INPUT_KEY_DELETE,           //             261
    INPUT_KEY_RIGHT,            //              262
    INPUT_KEY_LEFT,             //               263
    INPUT_KEY_DOWN,             //               264
    INPUT_KEY_UP,               //                 265
    INPUT_KEY_PAGE_UP,          //            266
    INPUT_KEY_PAGE_DOWN,        //          267
    INPUT_KEY_HOME,             //               268
    INPUT_KEY_END,              //                269
    INPUT_KEY_CAPS_LOCK,        //          280
    INPUT_KEY_SCROLL_LOCK,      //        281
    INPUT_KEY_NUM_LOCK,         //           282
    INPUT_KEY_PRINT_SCREEN,     //       283
    INPUT_KEY_PAUSE,            //              284
    INPUT_KEY_F1,               //                 290
    INPUT_KEY_F2,               //                 291
    INPUT_KEY_F3,               //                 292
    INPUT_KEY_F4,               //                 293
    INPUT_KEY_F5,               //                 294
    INPUT_KEY_F6,               //                 295
    INPUT_KEY_F7,               //                 296
    INPUT_KEY_F8,               //                 297
    INPUT_KEY_F9,               //                 298
    INPUT_KEY_F10,              //                299
    INPUT_KEY_F11,              //                300
    INPUT_KEY_F12,              //                301
    INPUT_KEY_F13,              //                302
    INPUT_KEY_F14,              //                303
    INPUT_KEY_F15,              //                304
    INPUT_KEY_F16,              //                305
    INPUT_KEY_F17,              //                306
    INPUT_KEY_F18,              //                307
    INPUT_KEY_F19,              //                308
    INPUT_KEY_F20,              //                309
    INPUT_KEY_F21,              //                310
    INPUT_KEY_F22,              //                311
    INPUT_KEY_F23,              //                312
    INPUT_KEY_F24,              //                313
    INPUT_KEY_F25,              //                314
    INPUT_KEY_KP_0,             //               320
    INPUT_KEY_KP_1,             //               321
    INPUT_KEY_KP_2,             //               322
    INPUT_KEY_KP_3,             //               323
    INPUT_KEY_KP_4,             //               324
    INPUT_KEY_KP_5,             //               325
    INPUT_KEY_KP_6,             //               326
    INPUT_KEY_KP_7,             //               327
    INPUT_KEY_KP_8,             //               328
    INPUT_KEY_KP_9,             //               329
    INPUT_KEY_KP_DECIMAL,       //         330
    INPUT_KEY_KP_DIVIDE,        //          331
    INPUT_KEY_KP_MULTIPLY,      //        332
    INPUT_KEY_KP_SUBTRACT,      //        333
    INPUT_KEY_KP_ADD,           //             334
    INPUT_KEY_KP_ENTER,         //           335
    INPUT_KEY_KP_EQUAL,         //           336
    INPUT_KEY_LEFT_SHIFT,       //         340
    INPUT_KEY_LEFT_CONTROL,     //       341
    INPUT_KEY_LEFT_ALT,         //           342
    INPUT_KEY_LEFT_SUPER,       //         343
    INPUT_KEY_RIGHT_SHIFT,      //        344
    INPUT_KEY_RIGHT_CONTROL,    //      345
    INPUT_KEY_RIGHT_ALT,        //          346
    INPUT_KEY_RIGHT_SUPER,      //        347
    INPUT_KEY_MENU,             //               348
    INPUT_MOUSE_BUTTON_LEFT,    //
    INPUT_MOUSE_BUTTON_RIGHT,   //
    INPUT_MOUSE_BUTTON_MIDDLE,  //
    INPUT_NUM,
};

enum KeyState {
    STATE_RELEASE,
    STATE_PRESS,
};
struct Input {
    static bool guiWantCaptureMouse;

    static int framebufferWidth;
    static int framebufferHeight;

    static std::array<KeyState, INPUT_NUM> inputState;

    static Signal<void(int, int)> framebufferResizeSignal;
    static Signal<void(KeyCode, KeyState)> mouseButtonSignal;
    static Signal<void(double, double)> mouseMoveSignal;
    static Signal<void(double, double)> scrollSignal;
    static Signal<void(KeyCode, KeyState)> keyboardSignal;

    static void onKeyBoard(int, int, int, int);
    static void onMouseButton(int, int, int);
    static void onMouseMove(double, double);
    static void onFramebufferResize(int, int);
};
