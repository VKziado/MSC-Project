#include "transform.h"

#include "node.h"

Transform::Transform(Node *parent) : Component(parent) { init(); }
Transform::Transform(Node *parent, std::string_view name) : Component(parent, name) { init(); }
void Transform::init() {
    GL::createBuffer(GL::BufferCreateInfo{0, sizeof(glm::mat4),
                                          GL::BUFFER_STORAGE_MAP_COHERENT_BIT | GL::BUFFER_STORAGE_MAP_WRITE_BIT |
                                              GL::BUFFER_STORAGE_MAP_PERSISTENT_BIT},
                     &_globalTransformMatrix, &_transformBuffer);
}
void Transform::updateTransformBuffer() {
    // auto m = _globalTransformMatrix * glm::scale(glm::mat4(1), _localScale);
    glm::mat4 m = _localTransformMatrix;
    if (auto p = _parent->getParent()) {
        m = p->getComponent<Transform>()->getGlobalTransformMatrix() * _localTransformMatrix;
    }
    // auto m = _globalTransformMatrix * glm::scale(glm::mat4(1), _localScale);

    glBindBuffer(GL_UNIFORM_BUFFER, _transformBuffer);
    glm::mat4 *data = (glm::mat4 *)glMapBufferRange(
        GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4),
        GL::Map((GL::BufferMapFlagBits)(GL::BUFFER_MAP_COHERENT_BIT | GL::BUFFER_MAP_PERSISTENT_BIT |
                                        GL::BUFFER_MAP_WRITE_BIT)));
    memcpy(data, &m, sizeof(glm::mat4));
    glUnmapBuffer(GL_UNIFORM_BUFFER);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}
Transform *Transform::setInitTranslation(glm::vec3 const &translation) {
    _initTranslation = translation;
    setLocalTranslation(translation);
    return this;
}
Transform *Transform::setInitScale(glm::vec3 const &scale) {
    _initScale = scale;
    setLocalScale(scale);
    return this;
}
Transform *Transform::setInitRotation(glm::quat const &rotation) {
    _initRotation = rotation;
    setLocalRotation(rotation);
    return this;
}
void Transform::reset() {
    setLocalRotation(_initRotation);
    setLocalTranslation(_initTranslation);
    setLocalScale(_initScale);
}
Transform *Transform::setLocalTranslation(glm::vec3 const &translation) {
    _localTranslation = translation;
    _parent->needUpdate();
    return this;
}
Transform *Transform::setLocalScale(glm::vec3 const &scale) {
    _localScale = scale;
    _parent->needUpdate();
    return this;
}
Transform *Transform::setLocalRotation(glm::quat const &rotation) {
    _localRotation = rotation;
    _parent->needUpdate();
    return this;
}
Transform *Transform::translate(const glm::vec3 d, TransformSpace space) {
    switch (space) {
        case TransformSpace::LOCAL:
            _localTranslation += glm::mat3(_globalTransformMatrix) * d;
            break;
        case TransformSpace::PARENT:
            _localTranslation += d;
            break;
        case TransformSpace::WORLD: {
            glm::mat4 temp{1};
            // if (_parent)
            //	temp = glm::inverse(static_cast<SceneNode
            //*>(_parent)->getGlobalTransformMatrix());
            _localTranslation += glm::vec3(temp * glm::vec4(d, 1));
            break;
        }
        default:
            return this;
    }
    _parent->needUpdate();
    return this;
}
Transform *Transform::rotate(const glm::quat &rotation, TransformSpace space) {
    switch (space) {
        case TransformSpace::LOCAL:
            _localRotation *= rotation;
            break;
        case TransformSpace::PARENT:
            _localTranslation = rotation * _localTranslation;
            _localRotation = rotation * _localRotation;
            break;
        case TransformSpace::WORLD:
            // world without translation
            //  _localRotation *= rotation;
            //_globalTransformMatrix = getGlobalTransformMatrix();
            //_localTransform.rotation =
            // glm::quat_cast(glm::inverse(_globalTransformMatrix) *
            // glm::mat4_cast(rotation) * _globalTransformMatrix) *
            //						   _localTransform.rotation;
            break;
    }
    _parent->needUpdate();
    return this;
}
Transform *Transform::rotate(glm::vec3 const axis, float rad, TransformSpace space) {
    return rotate(glm::rotate(glm::quat{1, 0, 0, 0}, rad, axis), space);
}
Transform *Transform::scale(const glm::vec3 &scale, TransformSpace space) {
    switch (space) {
        case TransformSpace::LOCAL:
            _localScale *= scale;
            break;
        case TransformSpace::PARENT:
            _localScale *= scale * _localScale;
            break;
        case TransformSpace::WORLD: {
            //_globalTransformMatrix = getGlobalTransformMatrix();
            //_localTransform.rotation =
            // glm::quat_cast(glm::inverse(_globalTransformMatrix) *
            // glm::scale(glm::mat4(1), scale) * _globalTransformMatrix) *
            //						   _localTransform.rotation;
            break;
        }
    }
    _parent->needUpdate();
    return this;
}
void Transform::update() {
    _globalRotationQuat = glm::mat4_cast(_localRotation);
    _globalTransformMatrix = glm::mat4_cast(_localRotation);  //* glm::scale(glm::mat4(1), _localScale);

    _globalTransformMatrix[3].x += _localTranslation.x;
    _globalTransformMatrix[3].y += _localTranslation.y;
    _globalTransformMatrix[3].z += _localTranslation.z;

    _localTransformMatrix = glm::mat4_cast(_localRotation) * glm::scale(glm::mat4(1), _localScale);
    _localTransformMatrix[3].x += _localTranslation.x;
    _localTransformMatrix[3].y += _localTranslation.y;
    _localTransformMatrix[3].z += _localTranslation.z;

    if (auto p = _parent->getParent()) {
        _globalTransformMatrix = p->getComponent<Transform>()->getGlobalTransformMatrix() * _globalTransformMatrix;
        _globalRotationQuat = p->getComponent<Transform>()->getGlobalRotation() * _globalRotationQuat;
    }
    // update transform buffer
    updateTransformBuffer();
}