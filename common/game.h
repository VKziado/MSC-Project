#pragma once
#include "prerequisites.h"

class AppBase;
class Game {
    AppBase* _appBase{};

    std::string _name;

public:
    Game(std::string_view name = "") : _name(name) {}

    void setParent(AppBase* parent) { _appBase = parent; }

    AppBase* getParent() const { return _appBase; }

    virtual ~Game() {}

    virtual void init() {}

    virtual void render(float dt) {}
};