#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <memory>
#include <string_view>
#include <vector>

#include "idObject.h"
#include "material.h"
#include "prerequisites.h"
#include "renderer.h"
#include "singleton.h"

class Model;

class ModelLoader {
public:
    virtual bool load(Model *dstModel) = 0;
    virtual ~ModelLoader() {}
};

class ModelLoaderObj : public ModelLoader {
public:
    bool load(Model *dstModel) override;
    ~ModelLoaderObj() {}
};

//======================
class PrimitiveRenderer;
struct Primitive {
    GL::PrimitiveTopology topology;
    std::vector<glm::vec3> positions;
    std::vector<glm::vec3> normals;
    std::vector<glm::vec2> texcoords;
    std::vector<glm::vec4> colors;

    std::vector<uint32_t> indices;

    GL::BufferHandle _positionBuffer{};  // locatin 0
    GL::BufferHandle _normalBuffer{};    // locatin 1
    GL::BufferHandle _texcoordBuffer{};  // locatin 2
    GL::BufferHandle _colordBuffer{};    // locatin 3

    GL::BufferHandle _indexBuffer{};

    union {
        GL::DrawIndirectCommand _drawIndirectCmd;
        GL::DrawIndexedIndirectCommand _drawIndexedIndirectCmd;
    };

    // std::unique_ptr<PrimitiveRenderer> renderer{};
    bool _uploaded{false};

    void upload();

    void draw();
    ~Primitive();
};

struct Mesh {
    std::vector<std::unique_ptr<Primitive>> primitives;
};

enum MeshType {
    MESH_CUBE,
    MESH_SPHERE,
    MESH_LINE,
    MESH_QUAD,
    MESH_AXIS,
    MESH_Num,
};

class MeshFactory : public Singleton<MeshFactory> {
    std::array<std::weak_ptr<Mesh>, MESH_Num> _meshes;

public:
    std::shared_ptr<Mesh> createCube();
    std::shared_ptr<Mesh> createSphere();
    std::shared_ptr<Mesh> createLine(glm::vec3 const &p0, glm::vec3 const &p1);
    std::shared_ptr<Mesh> createQuad(glm::vec2 size, uint32_t countx = 1, uint32_t county = 1);
    std::shared_ptr<Mesh> createAxis();
};

struct Model : public IdObject {
    friend class ModelManager;

    std::string name;
    std::string path;

    struct MeshView {
        int meshIndex{-1};
        IdType materialId{0};
    };

    struct NodeAttribute {
        int meshViewIndex{-1};
        int parentNodeIndex{-1};
        glm::vec3 translaton{};
        glm::quat rotation{1, 0, 0, 0};
        glm::vec3 scale{1, 1, 1};
    };

    std::vector<std::shared_ptr<Mesh>> meshes;
    std::vector<MeshView> meshviews;
    std::vector<NodeAttribute> nodes;

    ModelLoader *loader{};
    bool loaded{false};

    Model(std::string_view path, ModelLoader *loader);
    Model(std::string_view name) : name(name) {}
    Model() {}

    void load();
};

class ModelManager : public Singleton<ModelManager> {
    std::unordered_map<std::string, std::unique_ptr<Model>> _models;
    std::unordered_map<std::string, std::unique_ptr<ModelLoader>> _modelLoaders;

public:
    ModelManager();

    Model *createModel(std::string_view path, std::string_view modelLoaderName = "obj");
    Model *createLine(glm::vec3 const &p0, glm::vec3 const &p1);
    Model *createQuad(glm::vec2 size = glm::vec2{1, 1}, int countx = 1, int county = 1);
    Model *createAxis();
    Model *createSphere(float radius = 1);
    Model *createCube(glm::vec3 const &size = glm::vec3(1));

    Model *createTestModel1();
    Model *createTestModel2();

    Model *getModel(std::string_view name) const { return _models.at(std::string(name)).get(); }

    ModelLoader *getModelLoader(std::string_view name) const;

    template <typename Loader_T>
    void registerModelLoader(std::string_view loaderName) {
        _modelLoaders.emplace(loaderName, std::make_unique<Loader_T>());
    }
};

//============================
class MeshRenderer : public Renderer {
    std::shared_ptr<Mesh> _mesh;
    std::shared_ptr<Material> _material{};

public:
    MeshRenderer(Node *parent, std::shared_ptr<Mesh> mesh);

    void setMaterial(uint32_t materialId);
    void setMaterial(std::shared_ptr<Material> material);

    void draw(bool bindTechnique = true) override;
};
