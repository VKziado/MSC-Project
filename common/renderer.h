#pragma once
#include "component.h"

class Renderer : public Component {
public:
    Renderer(Node* parent) : Component(parent) {}

    virtual void draw(bool bindTechnique = true) = 0;
};
