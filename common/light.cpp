#include "light.h"
Light::Light(Node *parent) : Component(parent) {}
void Light::prepare() {
    GL::createBuffer(
        GL::BufferCreateInfo{
            {},
            sizeof(UBO),
            GL::BufferStorageFlagBits::BUFFER_STORAGE_MAP_COHERENT_BIT |
                GL::BufferStorageFlagBits::BUFFER_STORAGE_MAP_PERSISTENT_BIT |
                GL::BufferStorageFlagBits::BUFFER_STORAGE_MAP_WRITE_BIT,
        },
        &uboData, &uboBuffer);
}
void Light::update() {
    glBindBuffer(GL_UNIFORM_BUFFER, uboBuffer);
    void *data = glMapBuffer(GL_UNIFORM_BUFFER, GL_WRITE_ONLY);
    memcpy(data, &uboData, sizeof(UBO));
    glUnmapBuffer(GL_UNIFORM_BUFFER);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}
void Light::bind(uint32_t binding) { glBindBufferRange(GL_UNIFORM_BUFFER, binding, uboBuffer, 0, sizeof(UBO)); }