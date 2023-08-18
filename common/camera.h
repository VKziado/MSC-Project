#pragma once
#include <array>
#include <glm/glm.hpp>
#include <variant>

#include "component.h"

struct PerspectiveDescription {
    float fovy;    // radians
    float aspect;  // width/height
};
struct OrthogonalDescription {
    float xmag;
    float ymag;
};

class Camera : public Component {
    float _near{0.1f};
    float _far{10000.f};  // if far==0, means infinite perspective, orthogonal cannot be 0
    std::variant<PerspectiveDescription, OrthogonalDescription> _extraDesc =
        PerspectiveDescription{glm::radians(60.f), 1.777};

    glm::mat4 _perspectiveMatrix;

    std::array<glm::vec3, 4> _nearVerticesInCameraSpace;
    std::array<glm::vec3, 4> _farVerticesInCameraSpace;

    GL::BufferHandle _pvBuffer;

    struct UBO {
        glm::mat4 V;
        glm::mat4 P;
        glm::vec3 eyePos;
        float near;
        float far;
    };

    void updatePVBuffer();
    void updateProjectionMatrix();

    void init();

public:
    Camera(Node *parent);
    Camera(Node *parent, OrthogonalDescription orth, float n, float f);
    Camera(Node *parent, PerspectiveDescription persp, float n, float f);
    ~Camera() { glDeleteBuffers(1, &_pvBuffer); }

    Camera *setOrthogonal(float xmag, float ymag);
    Camera *setPerspective(float fovy, float aspect);
    Camera *setNearPlane(float n);
    Camera *setFarPlane(float f);

    glm::mat4 getViewMatrix();
    constexpr glm::mat4 const &getProjectionMatrix() const { return _perspectiveMatrix; }

    void update() override;

    constexpr GL::BufferHandle getPVBuffer() const { return _pvBuffer; }

    void bind(uint32_t binding = 1);
};

class EditCameraController : public Behaviour {
    // per second
    float _speed = 3.f;

public:
    EditCameraController(Node *node);
    ~EditCameraController() {}

    // EditCameraController(EditCameraController const &other);

    constexpr void setSpeed(float speed) { _speed = speed; }

    void prepare() override;
    void updatePerFrame(float frameDtMs) override;
    void onScroll(double xoffset, double yoffset) override;
    void onMouseMove(double xpos, double ypos) override;
    void onMouseButton(KeyCode, KeyState) override;
    // keyboard event
    void onKey(KeyCode keyCode, KeyState keyState);
};
class EditCameraController2 : public Behaviour {
    // per second
    float _speed = 3.f;

public:
    EditCameraController2(Node *node) : Behaviour(node) {}
    ~EditCameraController2() {}

    // EditCameraController(EditCameraController const &other);

    constexpr void setSpeed(float speed) { _speed = speed; }

    void prepare() override;
    void updatePerFrame(float frameDtMs) override;
    void onScroll(double xoffset, double yoffset) override;
    void onMouseMove(double xpos, double ypos) override;
    void onMouseButton(KeyCode, KeyState) override;
};