#include "material.h"

#include "common.h"

std::array<std::unique_ptr<Technique>, MATERIAL_Num> Material::techniques;

Technique *Material::getTechnique() {
    if (materialType == MATERIAL_BLINNPHONG) {
        if (!techniques[MATERIAL_BLINNPHONG]) techniques[MATERIAL_BLINNPHONG] = std::make_unique<TechniqueBlinnPhong>();
        return techniques[MATERIAL_BLINNPHONG].get();
    } else if (materialType == MATERIAL_UNLITCOLOR) {
        if (!techniques[MATERIAL_UNLITCOLOR]) techniques[MATERIAL_UNLITCOLOR] = std::make_unique<TechniqueUnlitColor>();
        return techniques[MATERIAL_UNLITCOLOR].get();
    }
    return techniques[MATERIAL_BLINNPHONG].get();
}

//=====================================
void MaterialUnlitColor::bind(bool bindTechinique) {
    if (!prepared) prepare();
    if (bindTechinique) getTechnique()->bind();
}

//=====================================
MaterialBlinnPhong::MaterialBlinnPhong() {}
MaterialBlinnPhong::MaterialBlinnPhong(std::string_view name) : Material(name) {}
MaterialBlinnPhong::~MaterialBlinnPhong() { glDeleteBuffers(1, &uboBuffer); 
    for(auto e:textures)
    {
        TextureManager::getSingleton();
    }
}
void MaterialBlinnPhong::prepareImpl() {
    // create ubo buffer
    GL::createBuffer(GL::BufferCreateInfo{{},
                                          sizeof(GpuData),
                                          GL::BufferStorageFlagBits::BUFFER_STORAGE_MAP_COHERENT_BIT |
                                              GL::BufferStorageFlagBits::BUFFER_STORAGE_MAP_PERSISTENT_BIT |
                                              GL::BufferStorageFlagBits::BUFFER_STORAGE_MAP_WRITE_BIT |
                                              GL::BufferStorageFlagBits::BUFFER_STORAGE_MAP_READ_BIT},
                     &gpuData, &uboBuffer);
}
void MaterialBlinnPhong::setBaseColor(glm::vec4 const &color) { gpuData.baseColor = color; }
void MaterialBlinnPhong::setShininess(float shininess) { gpuData.shininess = shininess; }
void MaterialBlinnPhong::setBaseColorImage(Image *image) {
    if (gpuData.baseColorTexIndex < 0){
        gpuData.baseColorTexIndex = textures.size();
        textures.emplace_back();
    }
    textures[gpuData.baseColorTexIndex]=TextureManager::getSingleton().createTexture(image);
}
void MaterialBlinnPhong::setNormalImage(Image *image) {
    if (gpuData.normalTexIndex < 0) {
        gpuData.normalTexIndex = textures.size();
        textures.emplace_back();
    }
    textures[gpuData.baseColorTexIndex] = TextureManager::getSingleton().createTexture(image);
}
void MaterialBlinnPhong::bind(bool bindTechinique) {
    if (!prepared) prepare();
    if (bindTechinique) getTechnique()->bind();
    glBindBufferRange(GL_UNIFORM_BUFFER, 3, uboBuffer, 0, sizeof(GpuData));
    for (size_t i = 0; i < textures.size(); ++i) textures[i]->bind(i);
}

//============================
MaterialManager::MaterialManager() {
    for (int i = 0; i < MATERIAL_Num; ++i) {
        _defaultMaterialIds[i] = createMaterial(MaterialType(i), "default")->getId();
    }
}
std::shared_ptr<Material> MaterialManager::createMaterial(MaterialType type, std::string_view name) {
    std::shared_ptr<Material> ret;
    switch (type) {
        case MATERIAL_BLINNPHONG:
            ret = std::make_shared<MaterialBlinnPhong>(name);
            break;
        case MATERIAL_UNLITCOLOR:
            ret = std::make_shared<MaterialUnlitColor>(name);
            break;
    }
    _materials.emplace(ret->getId(), ret);
    return ret;
}
std::shared_ptr<Material> MaterialManager::getMaterial(IdType id) const {
    if (id == ~0u) return getDefaultMaterial(MATERIAL_BLINNPHONG);

    auto it = _materials.find(id);
    if (it == _materials.end()) return nullptr;
    return it->second;
}