#include "camera.h"

#include "transform.h"
#include "input.h"
#include "node.h"

Camera::Camera(Node *parent) : Component(parent) { init(); }
Camera::Camera(Node *parent, OrthogonalDescription orth, float n, float f)
    : Component(parent), _near(n), _far(f), _extraDesc(orth) {
    init();
}
Camera::Camera(Node *parent, PerspectiveDescription persp, float n, float f)
    : Component(parent), _near(n), _far(f), _extraDesc(persp) {
    init();
}
void Camera::init() {
    glm::mat4 a[2];
    GL::createBuffer(
        GL::BufferCreateInfo{0, sizeof(UBO),
        GL::BUFFER_STORAGE_MAP_COHERENT_BIT | 
        GL::BUFFER_STORAGE_MAP_WRITE_BIT |
        GL::BUFFER_STORAGE_MAP_PERSISTENT_BIT},
                     a, &_pvBuffer);
    updateProjectionMatrix();
}
void Camera::updatePVBuffer() {
    UBO a{
        getViewMatrix(), getProjectionMatrix(),
        _parent->getComponent<Transform>()->getGlobalPosition(), _near, _far};

    glBindBuffer(GL_UNIFORM_BUFFER, _pvBuffer);
    glm::mat4 *data = (glm::mat4 *)glMapBufferRange(
        GL_UNIFORM_BUFFER, 0, sizeof(UBO),
        GL::Map((GL::BufferMapFlagBits)(GL::BUFFER_MAP_COHERENT_BIT | GL::BUFFER_MAP_PERSISTENT_BIT |
                                        GL::BUFFER_MAP_WRITE_BIT)));
    memcpy(data, &a, sizeof(UBO));
    glUnmapBuffer(GL_UNIFORM_BUFFER);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}
Camera *Camera::setOrthogonal(float xmag, float ymag) {
    _extraDesc = OrthogonalDescription{xmag, ymag};
    updateProjectionMatrix();
    return this;
}
Camera *Camera::setPerspective(float fovy, float aspect) {
    _extraDesc = PerspectiveDescription{fovy, aspect};
    updateProjectionMatrix();
    return this;
}
Camera *Camera::setNearPlane(float n) {
    _near = n;
    updateProjectionMatrix();
    return this;
}
Camera *Camera::setFarPlane(float f) {
    _far = f;
    updateProjectionMatrix();
    return this;
}
glm::mat4 Camera::getViewMatrix() {
    return glm::inverse(_parent->getComponent<Transform>()->getGlobalTransformMatrix());
}
void Camera::updateProjectionMatrix() {
    if (auto p = std::get_if<PerspectiveDescription>(&_extraDesc)) {
      
        _perspectiveMatrix = glm::perspectiveRH_NO(p->fovy, p->aspect, _near, _far);
        auto tanHalfFovy = glm::tan(p->fovy / 2);
        auto yn = tanHalfFovy * _near;
        auto yf = tanHalfFovy * _far;
        auto xn = yn / p->aspect;
        auto xf = yf / p->aspect;

        _nearVerticesInCameraSpace = {
            glm::vec3{-xn, -yn, -_near},
            glm::vec3{xn, -yn, -_near},
            glm::vec3{-xn, yn, -_near},
            glm::vec3{xn, yn, -_near},
        };
        _farVerticesInCameraSpace = {
            glm::vec3{-xf, -yf, -_far},
            glm::vec3{xf, -yf, -_far},
            glm::vec3{-xf, yf, -_far},
            glm::vec3{xf, yf, -_far},
        };
    } else {
        auto p2 = std::get_if<OrthogonalDescription>(&_extraDesc);
        auto x = p2->xmag / 2;
        auto y = p2->ymag / 2;

       
        _perspectiveMatrix = glm::orthoRH_NO(-x, x, -y, y, _near, _far);

        _nearVerticesInCameraSpace = {
            glm::vec3{-x, -y, -_near},
            glm::vec3{x, -y, -_near},
            glm::vec3{-x, y, -_near},
            glm::vec3{x, y, -_near},
        };
        _farVerticesInCameraSpace = {
            glm::vec3{-x, -y, -_far},
            glm::vec3{x, -y, -_far},
            glm::vec3{-x, y, -_far},
            glm::vec3{x, y, -_far},
        };
    }
    // make screen y coordinate downwards
    //_perspectiveMatrix[1][1] *= -1;
    updatePVBuffer();
}
void Camera::update() { updatePVBuffer(); }
void Camera::bind(uint32_t binding) { glBindBufferRange(GL_UNIFORM_BUFFER, 1, _pvBuffer, 0, sizeof(UBO)); }

//=======================
enum DirectionBits {
    LEFT = 1,
    RIGHT = 1 << 1,
    UP = 1 << 2,
    DOWN = 1 << 3,
    LEFT_UP = LEFT | UP,
    LEFT_DOWN = LEFT | DOWN,
    RIGHT_UP = RIGHT | UP,
    RIGHT_DOWN = RIGHT | DOWN,
};
glm::vec3 getDirectionVec(DirectionBits dir) {
    switch (dir) {
        case DirectionBits::LEFT:
            return glm::vec3(-1, 0, 0);
        case DirectionBits::RIGHT:
            return glm::vec3(1, 0, 0);
        case DirectionBits::UP:
            return glm::vec3(0, 0, -1);
        case DirectionBits::DOWN:
            return glm::vec3(0, 0, 1);
        case DirectionBits::LEFT_UP:
            return glm::vec3(-1, 0, -1);
        case DirectionBits::LEFT_DOWN:
            return glm::vec3(-1, 0, 1);
        case DirectionBits::RIGHT_UP:
            return glm::vec3(1, 0, -1);
        case DirectionBits::RIGHT_DOWN:
            return glm::vec3(1, 0, 1);
    }
    return glm::vec3(0, 0, 0);
}

EditCameraController::EditCameraController(Node *parent) : Behaviour(parent) {
}
void EditCameraController::prepare() { _parent->getComponent<Transform>()->translate(glm::vec3(0, 1, 5)); }
void EditCameraController::updatePerFrame(float frameDtMs) {
    // ImGuiIO &io = ImGui::GetIO();
    auto keyStateW = Input::inputState[INPUT_KEY_W];
    auto keyStateA = Input::inputState[INPUT_KEY_A];
    auto keyStateS = Input::inputState[INPUT_KEY_S];
    auto keyStateD = Input::inputState[INPUT_KEY_D];

    auto keyStateLeft = Input::inputState[INPUT_KEY_LEFT];
    auto keyStateRight = Input::inputState[INPUT_KEY_RIGHT];
    auto keyStateUp = Input::inputState[INPUT_KEY_UP];
    auto keyStateDown = Input::inputState[INPUT_KEY_DOWN];

    // bit0: left
    // bit1: right
    // bit2: up
    // bit3: down
    static int keystateBits;

    if (keyStateA == STATE_PRESS || keyStateLeft == STATE_PRESS)
        keystateBits |= 1;
    else
        keystateBits &= ~1;
    if (keyStateD == STATE_PRESS || keyStateRight == STATE_PRESS)
        keystateBits |= (1 << 1);
    else
        keystateBits &= ~(1 << 1);
    if (keyStateW == STATE_PRESS || keyStateUp == STATE_PRESS)
        keystateBits |= (1 << 2);
    else
        keystateBits &= ~(1 << 2);
    if (keyStateS == STATE_PRESS || keyStateDown == STATE_PRESS)
        keystateBits |= (1 << 3);
    else
        keystateBits &= ~(1 << 3);

    glm::vec3 dir = getDirectionVec((DirectionBits)keystateBits);
    if (dir.x != 0 || dir.y != 0 || dir.z != 0) {
        dir *= _speed * frameDtMs / 1000;
        _parent->getComponent<Transform>()->translate(dir, TransformSpace::LOCAL);
    }
}
void EditCameraController::onScroll(double xoffset, double yoffset) {
    static float c = 0.1;
    _parent->getComponent<Transform>()->translate(
        glm::vec3(0, 0, -glm::length(_parent->getComponent<Transform>()->getGlobalPosition()) * c * yoffset),
        TransformSpace::LOCAL);
}
void EditCameraController::onMouseMove(double xpos, double ypos) {
    if (Input::guiWantCaptureMouse) return;
    //=============================================================
    // auto keyStateMouseL = Shit::GetKeyState(Shit::KeyCode::KEY_LBUTTON);
    auto keyStateMouseR = Input::inputState[INPUT_MOUSE_BUTTON_RIGHT];
    auto keyStateMouseL = Input::inputState[INPUT_MOUSE_BUTTON_LEFT];
    auto keyStateMouseM = Input::inputState[INPUT_MOUSE_BUTTON_MIDDLE];

    // wheel
    // auto keyStateAltL = Shit::GetKeyState(Shit::KeyCode::KEY_LMENU);
    static bool flag = true;
    static double preCursorX, preCursorY, cursorX, cursorY;

    if (keyStateMouseL == STATE_PRESS)  //|| (keyStateMouseL.down && keyStateAltL.down))
    {
        static float c = 0.001;
        // pWindow->GetCursorPos(cursorX, cursorY);
        cursorX = xpos;
        cursorY = ypos;
        if (flag) {
            preCursorX = xpos;
            preCursorY = ypos;
            flag = false;
        }
        if (cursorX != preCursorX) {
            _parent->getComponent<Transform>()->yaw((preCursorX - cursorX) * c, TransformSpace::PARENT);
        }
        if (cursorY != preCursorY) {
            _parent->getComponent<Transform>()->pitch((preCursorY - cursorY) * c, TransformSpace::LOCAL);
        }
        preCursorX = cursorX, preCursorY = cursorY;
    } else if (keyStateMouseR == GLFW_PRESS) {
        static float c = 0.001;
        c = 0.001 * glm::length(_parent->getComponent<Transform>()->getGlobalPosition());
        cursorX = xpos;
        cursorY = ypos;
        if (flag) {
            preCursorX = xpos;
            preCursorY = ypos;
            flag = false;
        }
        if (cursorX != preCursorX) {
            _parent->getComponent<Transform>()->translate(glm::vec3((preCursorX - cursorX) * c, 0, 0),
                                                          TransformSpace::LOCAL);
        }
        if (cursorY != preCursorY) {
            _parent->getComponent<Transform>()->translate(glm::vec3(0, (cursorY - preCursorY) * c, 0),
                                                          TransformSpace::LOCAL);
        }
        preCursorX = cursorX, preCursorY = cursorY;
    } else {
        flag = true;
    }
}
void EditCameraController::onMouseButton(KeyCode, KeyState) {}

void EditCameraController::onKey(KeyCode keyCode, KeyState keyState)
{
    float speed = 0.01;

    if (keyState == STATE_PRESS) {
        glm::vec3 translation = glm::vec3(0.0f);

        // WASD movement
        if (keyCode == INPUT_KEY_W) {
            translation.z -= speed;
        }
        else if (keyCode == INPUT_KEY_S) {
            translation.z += speed;
        }
        else if (keyCode == INPUT_KEY_A) {
            translation.x -= speed;
        }
        else if (keyCode == INPUT_KEY_D) {
            translation.x += speed;
        }

        // Space and Ctrl for vertical movement
        if (keyCode == INPUT_KEY_SPACE) {
            translation.y += speed;
        }
        else if (keyCode == INPUT_KEY_LEFT_CONTROL) {
            translation.y -= speed;
        }

        // Apply the translation
        _parent->getComponent<Transform>()->translate(translation, TransformSpace::LOCAL);
    }
}

void EditCameraController2::prepare() { _parent->getComponent<Transform>()->setInitTranslation({0, 0, 3}); }
void EditCameraController2::updatePerFrame(float frameDtMs) {}
void EditCameraController2::onScroll(double xoffset, double yoffset) {
    _parent->getComponent<Transform>()->translate(_parent->getComponent<Transform>()->getLocalRotation() *
                                                  glm::vec3{0, 0, yoffset * 0.1});
}
void EditCameraController2::onMouseMove(double xpos, double ypos) {
    if (Input::guiWantCaptureMouse) return;
    auto keyStateMouseR = Input::inputState[INPUT_MOUSE_BUTTON_RIGHT];
    auto keyStateMouseL = Input::inputState[INPUT_MOUSE_BUTTON_LEFT];
    auto keyStateMouseM = Input::inputState[INPUT_MOUSE_BUTTON_MIDDLE];

    static bool flag = false;
    static double preCursorX, preCursorY, cursorX, cursorY;

    if (keyStateMouseL == STATE_PRESS || keyStateMouseR == STATE_PRESS) {
        cursorX = xpos;
        cursorY = ypos;

        if (flag) {
            float sens = 2.f;
            auto dx = sens * (cursorX - preCursorX);
            auto dy = -sens * (cursorY - preCursorY);
            if (keyStateMouseL == GLFW_PRESS) {
                // drag rotate scene around zero
                glm::vec3 a = glm::normalize(glm::vec3(dx, dy, 0));
                glm::vec3 rotAxis = glm::cross(a, glm::vec3(0, 0, 1));
                auto dl = glm::length(glm::vec2(dx, dy));
                _parent->getComponent<Transform>()->rotate(
                    glm::rotate(glm::quat{1, 0, 0, 0}, glm::radians(dl), rotAxis), TransformSpace::LOCAL);

                _parent->getComponent<Transform>()->setLocalTranslation(
                    _parent->getComponent<Transform>()->getLocalRotation() *
                    _parent->getComponent<Transform>()->getInitTranslation());
            } else if (keyStateMouseR == GLFW_PRESS) {
                // y rotate global y
                // x rotate local x
                _parent->getComponent<Transform>()->yaw(glm::radians(dx), TransformSpace::PARENT);
                _parent->getComponent<Transform>()->pitch(glm::radians(dy), TransformSpace::LOCAL);
            }

            preCursorX = xpos;
            preCursorY = ypos;
        }
        flag = true;
    } else {
        flag = false;
    }
}
void EditCameraController2::onMouseButton(KeyCode keyCode, KeyState keyState) {
    if (keyCode == INPUT_MOUSE_BUTTON_LEFT && keyState == STATE_RELEASE) {
        _parent->getComponent<Transform>()->reset();
    }
}