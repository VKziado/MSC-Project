#include "environment.h"
Environment::Environment() {}
Environment::~Environment() {}
void Environment::prepare() {
    // if (_prepared) return;
    // _prepared = true;

    // GL::createBuffer(GL::BufferCreateInfo{0, sizeof(UBO),
    //                                       GL::BUFFER_STORAGE_MAP_COHERENT_BIT | GL::BUFFER_STORAGE_MAP_WRITE_BIT |
    //                                           GL::BUFFER_STORAGE_MAP_PERSISTENT_BIT},
    //                  &_uboData, &_uboBuffer);
}
void Environment::update() {
    glBindBuffer(GL_UNIFORM_BUFFER, _uboBuffer);
    void *data =
        glMapBufferRange(GL_UNIFORM_BUFFER, 0, sizeof(UBO),
                         GL::Map((GL::BufferMapFlagBits)(GL::BUFFER_MAP_COHERENT_BIT | GL::BUFFER_MAP_PERSISTENT_BIT |
                                                         GL::BUFFER_MAP_WRITE_BIT)));
    memcpy(data, &_uboData, sizeof(UBO));
    glUnmapBuffer(GL_UNIFORM_BUFFER);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}
void Environment::bind(uint32_t binding) {
    _technique->bind();
    glBindBufferRange(GL_UNIFORM_BUFFER, binding, _uboBuffer, 0, sizeof(UBO));
}
void Environment::render() { glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); }