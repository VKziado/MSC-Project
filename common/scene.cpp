#include "scene.h"

Scene::Scene() { init(); }
Scene::Scene(std::string_view name) : _name(name) { init(); }

void Scene::init() {
    _root = std::make_unique<Node>();
    _root->addComponent<Transform>();

    environment = std::make_unique<Environment>();
    // _editorCamera = std::make_unique<Camera>();

    _editorCameraNode = _root->createChild();
    _editorCameraNode->addComponent<Transform>();
    _editorCameraNode->addComponent<Camera>(PerspectiveDescription{
        glm::radians(60.f), float(800) / 600}, 0.1, 1000.f);
    _editorCameraNode->addComponent<EditCameraController>();

    Input::framebufferResizeSignal.Connect(
        std::bind(&Scene::onFramebufferResize, this, std::placeholders::_1, std::placeholders::_2));
}
void Scene::prepare() {
    std::vector<Behaviour *> behaviours;
    std::vector<Light *> lights;
    _root->getComponents(behaviours, true);
    _root->getComponents(lights, true);
    for (auto p : behaviours) p->prepare();
    for (auto p : lights) p->prepare();
}
void Scene::update(float dt) {
    _root->update();

    updateSignal(dt);

    std::vector<Behaviour *> behaviours;
    _root->getComponents(behaviours, true);
    for (auto p : behaviours) p->updatePerFrame(dt);
}
void Scene::cleanup() {}
void Scene::onFramebufferResize(int width, int height) {
    _editorCameraNode->getComponent<Camera>()->setPerspective(glm::radians(60.f), float(width) / height);
    glViewport(0, 0, width, height);
}
Node *Scene::createLight(Node *parent) {
    if (!parent) parent = _root.get();

    auto ret = parent->createChild();
    ret->addComponent<Transform>();
    ret->addComponent<Light>();
    return ret;
}
Node *Scene::createCamera(Node *parent) {
    if (!parent) parent = _root.get();
    auto ret = parent->createChild();
    ret->addComponent<Transform>();
    ret->addComponent<Camera>();
    return ret;
}
Node *Scene::addModel(Model *model, Node *parent) {
    if (!parent) parent = _root.get();

    std::vector<Node *> tempObjects;
    Node *root;
    for (auto &&e : model->nodes) {
        auto p = tempObjects.emplace_back(new Node());
        auto transform = p->addComponent<Transform>();
        transform->setInitRotation(e.rotation);
        transform->setInitTranslation(e.translaton);
        transform->setInitScale(e.scale);

        if (e.meshViewIndex >= 0) {
            auto renderer = p->addComponent<MeshRenderer>(model->meshes[model->meshviews[e.meshViewIndex].meshIndex]);
            auto material = MaterialManager::getSingleton().getMaterial(model->meshviews[e.meshViewIndex].materialId);
            renderer->setMaterial(material);
        }
    }
    for (size_t i = 0; i < model->nodes.size(); ++i) {
        auto &&e = model->nodes[i];
        if (e.parentNodeIndex == -1) {
            // root node
            root = tempObjects[i];
        } else {
            tempObjects[e.parentNodeIndex]->addChild(tempObjects[i]);
        }
    }
    parent->addChild(root);
    return root;
}