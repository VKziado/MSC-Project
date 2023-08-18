#pragma once
#include "component.h"
#include "prerequisites.h"

class Node : public IdObject {
protected:
    std::string _name;

    std::vector<std::unique_ptr<Component>> _components;

    Node *_parent{};
    std::vector<std::unique_ptr<Node>> _children;

    bool _needUpdate = true;

    virtual void updateImpl();

public:
    Node(Node *parent = nullptr);
    Node(std::string_view name, Node *parent = nullptr);

    virtual ~Node() {}

    Node *createChild(std::string_view name = {});

    void addChild(Node *gameObject);
    void removeChild(Node *child);
    void setParent(Node *parent);

    constexpr Node *getParent() const { return _parent; }

    decltype(auto) childBegin() { return _children.begin(); }
    decltype(auto) childEnd() { return _children.end(); }
    decltype(auto) childCbegin() const { return _children.cbegin(); }
    decltype(auto) childCend() const { return _children.cend(); }

    std::string_view getName() const { return _name; }

    template <typename T, typename... Args>
    T *addComponent(Args... args) {
        auto a = new T(this, args...);
        _components.emplace_back(std::unique_ptr<T>(a));
        return a;
    }

    /**
     * @brief Get the Component object
     *
     * @tparam T
     * @return T* return first component T
     */
    template <typename T>
    T *getComponent() {
        for (auto &&e : _components) {
            if (auto it = dynamic_cast<T *>(e.get())) {
                return it;
            }
        }
        return nullptr;
    }

    template <typename T>
    void getComponents(std::vector<T *> &components, bool recursive = false) {
        for (auto &&e : _components) {
            if (auto it = dynamic_cast<T *>(e.get())) {
                components.emplace_back(it);
            }
        }
        if (recursive) {
            for (auto &&e : _children) {
                e->getComponents(components, recursive);
            }
        }
    }

    decltype(auto) componentCbegin() const { return _components.cbegin(); }
    decltype(auto) componentCend() const { return _components.cend(); }
    decltype(auto) componentBegin() { return _components.begin(); }
    decltype(auto) componentEnd() { return _components.end(); }

    /**
     * @brief update all children
     *
     */
    void update();

    void needUpdate(bool notifyParent = true, bool notifyChildren = true);

    Signal<void(Node *)> updateSignal;
};
