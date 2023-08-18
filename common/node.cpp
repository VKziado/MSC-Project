#pragma once
#include "node.h"

Node::Node(Node *parent) : _parent(parent) {}
Node::Node(std::string_view name, Node *parent) : _parent(parent), _name(name) {}
Node *Node::createChild(std::string_view name) {
    return _children.emplace_back(std::make_unique<Node>(name, this)).get();
}
void Node::addChild(Node *gameObject) {
    if (gameObject->_parent) {
        LOG("the parent of node is not null");
        return;
    }
    gameObject->_parent = this;
    _children.emplace_back(std::unique_ptr<Node>(gameObject));
}
void Node::removeChild(Node *child) {
    auto it = std::remove_if(_children.begin(), _children.end(), [child](auto &&e) { return e.get() == child; });
    _children.erase(it, _children.end());
}
void Node::setParent(Node *parent) {
    if (_parent) {
        auto it = std::find_if(_parent->_children.begin(), _parent->_children.end(),
                               [this](auto &&e) { return e.get() == this; });
        it->release();
        _parent->_children.erase(it);
        _parent = nullptr;
    }
    parent->addChild(this);
}
void Node::update() {
    if (_needUpdate) {
        updateImpl();
        for (auto &&e : _children) {
            e->update();
        }
        _needUpdate = false;
    }
}
void Node::needUpdate(bool notifyParent, bool notifyChildren) {
    _needUpdate = true;
    if (_parent && notifyParent) {
        _parent->needUpdate(true, false);
    }
    if (notifyChildren) {
        for (auto &&e : _children) {
            e->needUpdate(false, true);
        }
    }
}
void Node::updateImpl() {
    for (auto &&e : _components) {
        e->update();
    }
    updateSignal(this);
}