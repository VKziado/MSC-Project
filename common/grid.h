#pragma once
#include "component.h"
#include "technique.h"

class Grid : public Component {
    static std::unique_ptr<TechniqueGrid> _technique;

public:
    Grid(Node* parent);

    void draw();
};