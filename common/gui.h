#pragma once
#include "prerequisites.h"

class Game;
class GUI {
protected:

    Game* _game;

public:
    GUI(Game* game);
    virtual ~GUI();

    // virtual void renderImpl(float dt) {}
    Signal<void()> renderGUISignal;

    virtual void render(float dt);
};
