#pragma once
#include "component.h"

class RigidBody : public Component {
public:
    RigidBody(Node* parent) : Component(parent) {}
};
