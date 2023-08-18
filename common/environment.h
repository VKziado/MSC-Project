#pragma once
#include <memory>

#include "device.h"
#include "glm/glm.hpp"
#include "material.h"

class Environment {
public:
    struct UBO {
        glm::vec3 ambientColor{0.1, 0.1, 0.1};
        float ambientIntensity{1};
    };

    Environment();
    ~Environment();

    void setAmbientColor(glm::vec3 const &color) {
        _uboData.ambientColor = color;
        update();
    }
    void setClearColor(float r, float g, float b, float a) { glClearColor(r, g, b, a); }
    void setClearDepthStencil(float d = 1, int s = 0) {
        glClearDepth(d);
        glClearStencil(s);
    }

    void setClearColor() {}
    void prepare();

    void update();

    void bind(uint32_t binding);

    void render();

protected:
    bool _prepared{false};

    UBO _uboData;

    GL::BufferHandle _uboBuffer;

    std::unique_ptr<TechniqueEnv> _technique;
};
