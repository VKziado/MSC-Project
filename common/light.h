#include "component.h"

enum LightType {
    LIGHT_DIRECTIONAL,
};

struct Light : public Component {
    struct UBO {
        glm::vec3 color{1, 1, 1};
        float intensity{1};
        glm::vec3 direction{1, 1, 0};
    } uboData{};

    GL::BufferHandle uboBuffer;

    Light(Node *parent);

    void prepare() override;
    void update() override;

    void bind(uint32_t binding);
};
