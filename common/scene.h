#pragma once
#include "camera.h"
#include "environment.h"
#include "grid.h"
#include "idObject.h"
#include "input.h"
#include "light.h"
#include "model.h"
#include "node.h"
#include "prerequisites.h"
#include "singleton.h"
#include "transform.h"

class Scene : public IdObject {
protected:
    friend class RenderServer;
    std::string _name;

    std::unique_ptr<Node> _root;

    Node* _editorCameraNode;

    void init();

public:
    std::unique_ptr<Environment> environment;

public:
    Scene();
    Scene(std::string_view name);
    virtual ~Scene() {}

    std::string_view getName() const { return _name; }

    Node* getRoot() const { return _root.get(); }

    void prepare();
    void update(float dt);
    void cleanup();

    Node* createLight(Node* parent = nullptr);
    Node* createCamera(Node* parent = nullptr);

    Node* addModel(Model* model, Node* parent = nullptr);

    void onFramebufferResize(int, int);

    Signal<void(float)> updateSignal;
};