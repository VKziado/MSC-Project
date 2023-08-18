#include "model.h"

#include <array>
#include <filesystem>

#include "common.h"
#include "node.h"
#include "transform.h"

#define PI 3.141592653

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

bool ModelLoaderObj::load(Model *dstModel) {
    auto parentPath = std::filesystem::path(dstModel->path).parent_path();
    tinyobj::ObjReaderConfig reader_config;
    reader_config.mtl_search_path = parentPath.string();  // Path to material files

    tinyobj::ObjReader reader;

    if (!reader.ParseFromFile(std::string(dstModel->path), reader_config)) {
        if (!reader.Error().empty()) {
            std::cerr << "TinyObjReader: " << reader.Error();
        }
        exit(1);
    }

    if (!reader.Warning().empty()) {
        std::cout << "TinyObjReader: " << reader.Warning();
    }

    auto &attrib = reader.GetAttrib();
    auto &shapes = reader.GetShapes();
    auto &materials = reader.GetMaterials();

    std::vector<IdType> materialIds;

    // parse materials
    for (auto &&e : materials) {
        // e.
        auto material = static_cast<MaterialBlinnPhong *>(
            MaterialManager::getSingleton().createMaterial(MATERIAL_BLINNPHONG).get());
        materialIds.emplace_back(material->getId());

        // diffuse texture
        material->setBaseColor(glm::vec4(e.diffuse[0], e.diffuse[1], e.diffuse[2], 1.));
        if (!e.diffuse_texname.empty())
            material->setBaseColorImage(
                ImageManager::getSingleton().create(parentPath.string() + "/" + e.diffuse_texname));
        if (!e.normal_texname.empty())
            material->setNormalImage(ImageManager::getSingleton().create(parentPath.string() + "/" + e.normal_texname));
    }

    // parse model
    dstModel->nodes.emplace_back(Model::NodeAttribute{-1, -1});

    for (size_t i = 0; i < shapes.size(); ++i) {
        auto &&shape = shapes[i];

        dstModel->meshviews.emplace_back(
            Model::MeshView{(int)dstModel->meshes.size(), materialIds[shape.mesh.material_ids[0]]});

        auto newMesh = dstModel->meshes.emplace_back(std::make_shared<Mesh>());
        dstModel->nodes.emplace_back(Model::NodeAttribute{(int)i, 0});

        auto trianglePrimitive = newMesh->primitives.emplace_back(std::make_unique<Primitive>()).get();
        trianglePrimitive->topology = GL::PrimitiveTopology::TRIANGLE_LIST;

        size_t indexOffset = 0;

        std::unordered_map<Vertex, uint32_t> vertexIndexMap;
        Vertex tempVertex;

        // trangulated , fv is 3
        for (auto face : shape.mesh.num_face_vertices) {
            for (unsigned char v = 0; v < face; ++v, ++indexOffset) {
                auto index = shape.mesh.indices[indexOffset];
                tempVertex = {};

                auto vx = attrib.vertices[3 * index.vertex_index + 0];
                auto vy = attrib.vertices[3 * index.vertex_index + 1];
                auto vz = attrib.vertices[3 * index.vertex_index + 2];
                tempVertex.pos = {vx, vy, vz};

                if (index.normal_index >= 0) {
                    auto nx = attrib.normals[3 * index.normal_index + 0];
                    auto ny = attrib.normals[3 * index.normal_index + 1];
                    auto nz = attrib.normals[3 * index.normal_index + 2];
                    tempVertex.normal = {nx, ny, nz};
                }
                if (index.texcoord_index >= 0) {
                    auto tx = attrib.texcoords[2 * index.texcoord_index + 0];
                    auto ty = attrib.texcoords[2 * index.texcoord_index + 1];
                    tempVertex.texCoord = {tx, ty};
                }
                auto it = vertexIndexMap.find(tempVertex);
                if (it == vertexIndexMap.end()) {
                    trianglePrimitive->indices.emplace_back(vertexIndexMap[tempVertex] =
                                                                (uint32_t)trianglePrimitive->positions.size());
                    trianglePrimitive->positions.emplace_back(tempVertex.pos);
                    trianglePrimitive->normals.emplace_back(tempVertex.normal);
                    trianglePrimitive->texcoords.emplace_back(tempVertex.texCoord);
                    trianglePrimitive->colors.emplace_back(tempVertex.color);
                } else {
                    trianglePrimitive->indices.emplace_back(it->second);
                }
            }
        }
        trianglePrimitive->positions.shrink_to_fit();
        trianglePrimitive->normals.shrink_to_fit();
        trianglePrimitive->texcoords.shrink_to_fit();
        trianglePrimitive->indices.shrink_to_fit();
    }
    return true;
}
//=======================================
std::shared_ptr<Mesh> MeshFactory::createCube() {
    if (!_meshes[MESH_CUBE].expired()) return _meshes[MESH_CUBE].lock();

    auto mesh = std::make_shared<Mesh>();
    _meshes[MESH_CUBE] = mesh;
    auto primitive = mesh->primitives.emplace_back(std::make_unique<Primitive>()).get();
    primitive->topology = GL::PrimitiveTopology::TRIANGLE_LIST;
    primitive->positions = {
        {-1, -1, -1}, {1, -1, 1},  {-1, -1, 1}, {-1, -1, -1}, {1, -1, -1}, {1, -1, 1},   // bottom
        {-1, 1, -1},  {-1, 1, 1},  {1, 1, 1},   {-1, 1, -1},  {1, 1, 1},   {1, 1, -1},   // top
        {-1, -1, -1}, {1, 1, -1},  {1, -1, -1}, {-1, -1, -1}, {-1, 1, -1}, {1, 1, -1},   // back
        {-1, -1, 1},  {1, -1, 1},  {1, 1, 1},   {-1, -1, 1},  {1, 1, 1},   {-1, 1, 1},   // front
        {-1, -1, -1}, {-1, -1, 1}, {-1, 1, 1},  {-1, -1, -1}, {-1, 1, 1},  {-1, 1, -1},  // left
        {1, -1, -1},  {1, 1, 1},   {1, -1, 1},  {1, -1, -1},  {1, 1, -1},  {1, 1, 1}     // right
    };
    primitive->normals = {
        {0, -1, 0}, {0, -1, 0}, {0, -1, 0}, {0, -1, 0}, {0, -1, 0}, {0, -1, 0},  // bottom
        {0, 1, 0},  {0, 1, 0},  {0, 1, 0},  {0, 1, 0},  {0, 1, 0},  {0, 1, 0},   // top
        {0, 0, -1}, {0, 0, -1}, {0, 0, -1}, {0, 0, -1}, {0, 0, -1}, {0, 0, -1},  // back
        {0, 0, 1},  {0, 0, 1},  {0, 0, 1},  {0, 0, 1},  {0, 0, 1},  {0, 0, 1},   // front
        {-1, 0, 0}, {-1, 0, 0}, {-1, 0, 0}, {-1, 0, 0}, {-1, 0, 0}, {-1, 0, 0},  // left
        {1, 0, 0},  {1, 0, 0},  {1, 0, 0},  {1, 0, 0},  {1, 0, 0},  {1, 0, 0},   // right
    };
    primitive->colors.reserve(primitive->positions.size());
    for (auto &&e : primitive->positions) {
        primitive->colors.emplace_back(e, 1);
    }
    primitive->texcoords.resize(primitive->positions.size(), glm::vec2(1));
    return mesh;
}
std::shared_ptr<Mesh> MeshFactory::createSphere() {
    if (!_meshes[MESH_SPHERE].expired()) return _meshes[MESH_SPHERE].lock();
    auto mesh = std::make_shared<Mesh>();
    _meshes[MESH_SPHERE] = mesh;
    auto primitive = mesh->primitives.emplace_back(std::make_unique<Primitive>()).get();
    primitive->topology = GL::PrimitiveTopology::TRIANGLE_STRIP;

    const unsigned int X_SEGMENTS = 64;
    const unsigned int Y_SEGMENTS = X_SEGMENTS / 2;

    auto vertexCount = X_SEGMENTS * (Y_SEGMENTS + 1);
    primitive->positions.resize(vertexCount);
    primitive->normals.resize(vertexCount);
    primitive->texcoords.resize(vertexCount);
    primitive->colors.resize(vertexCount, glm::vec4(1, 1, 1, 1));

    for (int j = 0; j <= Y_SEGMENTS; ++j) {
        for (int i = 0; i < X_SEGMENTS; ++i) {
            float theta = float(i * 2 * PI) / X_SEGMENTS;
            float phi = float(j * PI) / Y_SEGMENTS;

            float cosTheta = glm::cos(theta);
            float sinTheta = glm::sin(theta);
            float cosPhi = -glm::cos(phi);  // from bottom to top
            float sinPhi = glm::sin(phi);

            int index = j * X_SEGMENTS + i;

            primitive->normals[index] = primitive->positions[index] = {sinPhi * cosTheta, cosPhi, sinPhi * sinTheta};
            primitive->texcoords[index] = {theta, phi};
        }
    }

    // a.indices
    primitive->indices.resize(2 * X_SEGMENTS * (Y_SEGMENTS + 1));
    for (int j = 0; j <= Y_SEGMENTS; ++j) {
        for (int i = 0; i < X_SEGMENTS - 1; ++i) {
            primitive->indices[i * 2 * (Y_SEGMENTS + 1) + 2 * j] = j * (X_SEGMENTS) + i + 1;
            primitive->indices[i * 2 * (Y_SEGMENTS + 1) + 2 * j + 1] = j * (X_SEGMENTS) + i;
        }
        primitive->indices[(X_SEGMENTS - 1) * 2 * (Y_SEGMENTS + 1) + 2 * j] = j * (X_SEGMENTS);
        primitive->indices[(X_SEGMENTS - 1) * 2 * (Y_SEGMENTS + 1) + 2 * j + 1] = j * (X_SEGMENTS) + X_SEGMENTS - 1;
    }
    return mesh;
}
std::shared_ptr<Mesh> MeshFactory::createLine(glm::vec3 const &p0, glm::vec3 const &p1) {
    if (!_meshes[MESH_LINE].expired()) return _meshes[MESH_LINE].lock();
    auto mesh = std::make_shared<Mesh>();
    _meshes[MESH_LINE] = mesh;
    auto primitive = mesh->primitives.emplace_back(std::make_unique<Primitive>()).get();
    primitive->topology = GL::PrimitiveTopology::LINE_LIST;
    primitive->positions = {p0, p1};
    primitive->colors.resize(2, glm::vec4(1));
    primitive->texcoords = {{0, 0}, {1, 0}};
    return mesh;
}
std::shared_ptr<Mesh> MeshFactory::createQuad(glm::vec2 size, uint32_t countx, uint32_t county) {
    if (!_meshes[MESH_QUAD].expired()) return _meshes[MESH_QUAD].lock();

    auto mesh = std::make_shared<Mesh>();
    _meshes[MESH_QUAD] = mesh;
    auto primitive = mesh->primitives.emplace_back(std::make_unique<Primitive>()).get();
    primitive->topology = GL::PrimitiveTopology::TRIANGLE_LIST;

    auto vertexCount = (countx + 1) * (county + 1);
    primitive->positions.resize(vertexCount);
    primitive->texcoords.resize(vertexCount);
    primitive->normals.resize(vertexCount);
    primitive->colors.resize(vertexCount);

    float hw = size.x / 2, hh = size.y / 2;
    float stepx = size.x / countx, stepy = size.y / county;
    int index;
    for (int j = 0; j <= county; ++j) {
        for (int i = 0; i <= countx; ++i) {
            index = j * (countx + 1) + i;
            primitive->positions[index] = glm::vec3{-hw + stepx * i, 0, -hh + stepy * j};
            primitive->texcoords[index] = glm::vec2(float(i) / countx, float(j) / county);
            primitive->normals[index] = glm::vec3(0, 1, 0);
            primitive->colors[index] = glm::vec4{1, 1, 1, 1};

            if (i != countx && j != county) {
                primitive->indices.emplace_back(index);
                primitive->indices.emplace_back(index + countx + 1);
                primitive->indices.emplace_back(index + countx + 2);
                primitive->indices.emplace_back(index);
                primitive->indices.emplace_back(index + countx + 2);
                primitive->indices.emplace_back(index + 1);
            }
        }
    }
    return mesh;
}
std::shared_ptr<Mesh> MeshFactory::createAxis() {
    auto mesh = std::make_shared<Mesh>();
    _meshes[MESH_AXIS] = mesh;
    auto primitive = mesh->primitives.emplace_back(std::make_unique<Primitive>()).get();
    primitive->topology = GL::PrimitiveTopology::LINE_LIST;
    primitive->positions = {{0, 0, 0}, {1, 0, 0}, {0, 0, 0}, {0, 1, 0}, {0, 0, 0}, {0, 0, 1}};
    primitive->colors = {{0, 0, 0, 1}, {1, 0, 0, 1}, {0, 0, 0, 1}, {0, 1, 0, 1}, {0, 0, 0, 1}, {0, 0, 1, 1}};
    return mesh;
}

//=======================================
void Primitive::upload() {
    if (_uploaded) return;

    GL::createBuffer(GL::BufferCreateInfo{{}, positions.size() * sizeof(glm::vec3)}, positions.data(),
                     &_positionBuffer);
    if (!normals.empty())
        GL::createBuffer(GL::BufferCreateInfo{{}, normals.size() * sizeof(glm::vec3)}, normals.data(), &_normalBuffer);
    if (!texcoords.empty())
        GL::createBuffer(GL::BufferCreateInfo{{}, texcoords.size() * sizeof(glm::vec2)}, texcoords.data(),
                         &_texcoordBuffer);
    if (!colors.empty())
        GL::createBuffer(GL::BufferCreateInfo{{}, colors.size() * sizeof(glm::vec4)}, colors.data(), &_colordBuffer);
    if (!indices.empty()) {
        GL::createBuffer(GL::BufferCreateInfo{{}, indices.size() * sizeof(uint32_t)}, indices.data(), &_indexBuffer);
        _drawIndexedIndirectCmd = {(uint32_t)indices.size(), 1, 0, 0, 0};
    } else {
        _drawIndirectCmd = {(uint32_t)positions.size(), 1, 0, 0};
    }

    _uploaded = true;
}
void Primitive::draw() {
    glBindVertexBuffer(0, _positionBuffer, 0, sizeof(float) * 3);
    if (_normalBuffer) glBindVertexBuffer(1, _normalBuffer, 0, sizeof(float) * 3);
    if (_texcoordBuffer) glBindVertexBuffer(2, _texcoordBuffer, 0, sizeof(float) * 2);
    if (_colordBuffer) glBindVertexBuffer(3, _colordBuffer, 0, sizeof(float) * 4);

    if (_indexBuffer) {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _indexBuffer);
        // bind indexbuffer
        GL::DrawIndexed(topology, GL::DataType::DATA_TYPE_UNSIGNED_INT, _drawIndexedIndirectCmd);
    } else {
        GL::Draw(topology, _drawIndirectCmd);
    }
}
Primitive::~Primitive() {
    glDeleteBuffers(1, &_positionBuffer);
    glDeleteBuffers(1, &_normalBuffer);
    glDeleteBuffers(1, &_texcoordBuffer);
    glDeleteBuffers(1, &_colordBuffer);
    glDeleteBuffers(1, &_indexBuffer);
}
//=======================================
Model::Model(std::string_view path, ModelLoader *loader) : path(path), loader(loader) {
    name = std::filesystem::path(path).filename().string();
    load();
}
void Model::load() {
    if (loaded || !loader) return;
    std::fstream fs(path.data());
    if (fs.is_open()) {
        loader->load(this);
        loaded = true;
        return;
    }
    LOG("failed to load image", name, "path ", path);
}
//=======================================
ModelManager::ModelManager() { registerModelLoader<ModelLoaderObj>("obj"); }
ModelLoader *ModelManager::getModelLoader(std::string_view name) const {
    auto it = _modelLoaders.find(std::string(name));
    if (it == _modelLoaders.cend()) {
        LOG("failed to find model loader ", name);
    }
    return it->second.get();
}
Model *ModelManager::createModel(std::string_view path, std::string_view modelLoaderName) {
    auto loader = getModelLoader(modelLoaderName);
    auto model = new Model(path, loader);
    return _models.emplace(std::string(model->name), std::unique_ptr<Model>(model)).first->second.get();
}
Model *ModelManager::createLine(glm::vec3 const &p0, glm::vec3 const &p1) {
    std::string name = "line";
    auto model = new Model(name);
    auto mesh = MeshFactory::getSingleton().createLine(p0, p1);
    auto material = MaterialManager::getSingleton().getDefaultMaterial(MATERIAL_BLINNPHONG);
    model->meshes.emplace_back(mesh);
    model->meshviews = {{0, material->getId()}};
    model->nodes = {{0, -1}};
    _models.emplace(name, std::unique_ptr<Model>(model));
    return model;
}
Model *ModelManager::createQuad(glm::vec2 size, int countx, int county) {
    std::string name = "quad";
    auto model = new Model(name);
    auto mesh = MeshFactory::getSingleton().createQuad(size, countx, county);
    auto material = MaterialManager::getSingleton().getDefaultMaterial(MATERIAL_BLINNPHONG);
    model->meshes = {mesh};
    model->meshviews = {{0, material->getId()}};
    model->nodes = {{0, -1}};
    _models.emplace(name, std::unique_ptr<Model>(model));
    return model;
}
Model *ModelManager::createAxis() {
    std::string name = "axis";
    auto model = new Model(name);
    auto mesh = MeshFactory::getSingleton().createAxis();
    auto material = MaterialManager::getSingleton().getDefaultMaterial(MATERIAL_UNLITCOLOR);
    model->meshes = {mesh};
    model->meshviews = {{0, material->getId()}};
    model->nodes = {{0, -1}};
    _models.emplace(name, std::unique_ptr<Model>(model));
    return model;
}
Model *ModelManager::createSphere(float radius) {
    std::string name = "sphere";
    auto model = new Model(name);
    auto mesh = MeshFactory::getSingleton().createSphere();
    auto material = MaterialManager::getSingleton().getDefaultMaterial(MATERIAL_BLINNPHONG);
    model->meshes = {mesh};
    model->meshviews = {{0, material->getId()}};
    model->nodes = {{0, -1}};
    _models.emplace(name, std::unique_ptr<Model>(model));
    return model;
}
Model *ModelManager::createCube(glm::vec3 const &size) {
    std::string name = "cube";
    auto model = new Model(name);
    auto mesh = MeshFactory::getSingleton().createCube();
    auto material = MaterialManager::getSingleton().getDefaultMaterial(MATERIAL_BLINNPHONG);
    model->meshes = {mesh};
    model->meshviews = {{0, material->getId()}};
    model->nodes = {{0, -1}};
    _models.emplace(name, std::unique_ptr<Model>(model));
    return model;
}
Model *ModelManager::createTestModel1() {
    std::string name = "model1";
    auto model = new Model(name);

    auto mesh = MeshFactory::getSingleton().createCube();
    auto material0 = MaterialManager::getSingleton().createMaterial(MATERIAL_BLINNPHONG);
    auto material1 = MaterialManager::getSingleton().createMaterial(MATERIAL_BLINNPHONG);
    std::static_pointer_cast<MaterialBlinnPhong>(material0)->setBaseColor(glm::vec4(1, 0, 0, 1));
    std::static_pointer_cast<MaterialBlinnPhong>(material1)->setBaseColor(glm::vec4(0, 1, 0, 1));

    model->meshes = {mesh};
    model->meshviews = {{0, material0->getId()}, {0, material1->getId()}};

    model->nodes = {
        {0, -1, {0, 2, 0}, {1, 0, 0, 0}, {0.2, 2, 0.2}},
        {1, 0, {0, 3, 0}, {1, 0, 0, 0}, {0.2, 1, 0.2}},
    };
    return model;
}
Model *ModelManager::createTestModel2() {
    std::string name = "model2";
    auto model = new Model(name);

    auto mesh = MeshFactory::getSingleton().createCube();
    std::vector<std::shared_ptr<MaterialBlinnPhong>> materials{
        std::static_pointer_cast<MaterialBlinnPhong>(
            MaterialManager::getSingleton().createMaterial(MATERIAL_BLINNPHONG)),
        std::static_pointer_cast<MaterialBlinnPhong>(
            MaterialManager::getSingleton().createMaterial(MATERIAL_BLINNPHONG)),
        std::static_pointer_cast<MaterialBlinnPhong>(
            MaterialManager::getSingleton().createMaterial(MATERIAL_BLINNPHONG)),
        std::static_pointer_cast<MaterialBlinnPhong>(
            MaterialManager::getSingleton().createMaterial(MATERIAL_BLINNPHONG)),
    };
    materials[1]->setBaseColor({1, 0, 0, 1});
    materials[2]->setBaseColor({0, 1, 0, 1});
    materials[3]->setBaseColor({0, 0, 1, 1});

    model->meshes = {mesh};
    model->meshviews = {
        // body
        {0, materials[0]->getId()},
        // left arm
        {0, materials[1]->getId()},
        {0, materials[2]->getId()},
        // right arm
        {0, materials[1]->getId()},
        {0, materials[2]->getId()},
        // tail
        {0, materials[3]->getId()},
    };

    model->nodes = {
        {0, -1, {0, 0, 0}, {1, 0, 0, 0}, {1, 1, 0.5}},   {1, 0, {2, 0, 0}, {1, 0, 0, 0}, {1, 0.6, 0.6}},
        {2, 1, {2, 0, 0}, {1, 0, 0, 0}, {1, 0.3, 0.6}},  {3, 0, {-2, 0, 0}, {1, 0, 0, 0}, {1, 0.6, 0.6}},
        {4, 3, {-2, 0, 0}, {1, 0, 0, 0}, {1, 0.3, 0.6}}, {5, 0, {0, -3, 0}, {1, 0, 0, 0}, {0.5, 2, 0.5}},
        {1, 5, {0, -2, 0}, {1, 0, 0, 0}, {0.5, 1, 0.5}}, {2, 6, {0, -2, 0}, {1, 0, 0, 0}, {0.5, 1, 0.5}},
        {1, 7, {0, -2, 0}, {1, 0, 0, 0}, {0.5, 1, 0.5}},
    };

    return model;
}
//===========================================
MeshRenderer::MeshRenderer(Node *parent, std::shared_ptr<Mesh> mesh) : Renderer(parent), _mesh(mesh) {
    for (auto &&e : _mesh->primitives) {
        e->upload();
    }
}
void MeshRenderer::draw(bool bindTechnique) {
    if (_material)
        _material->bind(bindTechnique);
    else
        MaterialManager::getSingleton().getDefaultMaterial(MATERIAL_UNLITCOLOR)->bind(bindTechnique);

    glBindBufferRange(GL_UNIFORM_BUFFER, 0, _parent->getComponent<Transform>()->getTransformBufferHandle(), 0,
                      sizeof(glm::mat4));

    for (auto &&e : _mesh->primitives) {
        e->draw();
    }
}
void MeshRenderer::setMaterial(uint32_t materialId) {
    _material = MaterialManager::getSingleton().getMaterial(materialId);
}
void MeshRenderer::setMaterial(std::shared_ptr<Material> material) { _material = material; }