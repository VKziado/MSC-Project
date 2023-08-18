#pragma once
#include "idObject.h"
#include "input.h"
#include "prerequisites.h"
#include "signal.h"

class Transform;
class Node;
class Component : public IdObject {
protected:
    Node *_parent;
    std::string _name;

public:
    Component(Node *parent) : _parent(parent) {}
    Component(Node *parent, std::string_view name) : _parent(parent), _name(name) {}
    virtual ~Component() {}
    std::string_view getName() const { return _name; }
    constexpr Node *getParent() const { return _parent; }

    virtual void prepare() {}
    virtual void update() {}
};

class Behaviour : public Component {
public:
    Behaviour(Node *parent) : Component(parent) {
        Input::mouseButtonSignal.Connect(&Behaviour::onMouseButton, this);
        Input::mouseMoveSignal.Connect(&Behaviour::onMouseMove, this);
        Input::scrollSignal.Connect(&Behaviour::onScroll, this);
    }
    virtual void updatePerFrame(float frameDtMs) {}
    virtual void onScroll(double xoffset, double yoffset) {}
    virtual void onMouseMove(double xpos, double ypos) {}
    virtual void onMouseButton(KeyCode, KeyState) {}
};