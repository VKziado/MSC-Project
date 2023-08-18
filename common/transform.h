#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

#include "component.h"

enum class TransformSpace { LOCAL, PARENT, WORLD };

class Transform : public Component {
    glm::mat4 _globalTransformMatrix{1};

    glm::quat _globalRotationQuat{1, 0, 0, 0};

    glm::quat _initRotation{1, 0, 0, 0};
    glm::vec3 _initTranslation{0};
    glm::vec3 _initScale{1};

    glm::mat4 _localTransformMatrix{1};
    glm::vec3 _localTranslation{0};
    glm::vec3 _localScale{1};
    // wxyz
    glm::quat _localRotation{1, 0, 0, 0};

    GL::BufferHandle _transformBuffer{};

    void init();
    void updateTransformBuffer();

public:
    Transform(Node *parent);
    Transform(Node *parent, std::string_view name);
    ~Transform() { glDeleteBuffers(1, &_transformBuffer); }

    Transform *setInitTranslation(glm::vec3 const &translation);
    Transform *setInitScale(glm::vec3 const &scale);
    Transform *setInitRotation(glm::quat const &rotation);

    glm::vec3 getInitTranslation() const { return _initTranslation; }
    glm::vec3 getInitScale() const { return _initScale; }
    glm::quat getInitRotation() const { return _initRotation; }

    void reset();

    Transform *setLocalTranslation(glm::vec3 const &translation);
    Transform *setLocalScale(glm::vec3 const &scale);
    Transform *setLocalRotation(glm::quat const &rotation);

    glm::vec3 getLocalTranslation() const { return _localTranslation; }
    glm::vec3 getLocalScale() const { return _localScale; }
    glm::quat getLocalRotation() const { return _localRotation; }
    Transform *translate(const glm::vec3 d, TransformSpace space = TransformSpace::PARENT);
    Transform *rotate(const glm::quat &auat, TransformSpace space = TransformSpace::PARENT);
    Transform *rotate(glm::vec3 const axis, float rad, TransformSpace space = TransformSpace::PARENT);
    Transform *scale(const glm::vec3 &scale, TransformSpace space = TransformSpace::LOCAL);

    // roll yaw pitch is working in local space
    Transform *roll(float radian, TransformSpace space = TransformSpace::LOCAL) {
        return rotate(glm::quat(glm::vec3(0, 0, radian)), space);
    }
    Transform *yaw(float radian, TransformSpace space = TransformSpace::LOCAL) {
        return rotate(glm::quat(glm::vec3(0, radian, 0)), space);
    }
    Transform *pitch(float radian, TransformSpace space = TransformSpace::LOCAL) {
        return rotate(glm::quat(glm::vec3(radian, 0, 0)), space);
    }
    glm::mat4 const &getGlobalTransformMatrix() const { return _globalTransformMatrix; }

    glm::quat const &getGlobalRotation() const { return _globalRotationQuat; }

    constexpr glm::vec3 getGlobalPosition() const { return glm::vec3(_globalTransformMatrix[3]); }

    constexpr GL::BufferHandle getTransformBufferHandle() const { return _transformBuffer; }

    void update() override;
};
