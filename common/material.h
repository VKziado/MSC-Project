#pragma once
#include <array>
#include <memory>
#include <string>
#include <vector>

#include "technique.h"
#include "prerequisites.h"
#include "texture.h"

struct Material : public IdObject {
    static std::array<std::unique_ptr<Technique>, MATERIAL_Num> techniques;

    MaterialType materialType{MATERIAL_BLINNPHONG};

    std::string name;

    bool prepared{false};

    Technique *getTechnique();

    Material(std::string_view name_) : name(name_) {}
    Material() {}

    virtual ~Material() {}

    // bind descriptorSet
    virtual void bind(bool bindTechinique = true) = 0;

    void prepare() {
        if (prepared) return;
        prepared = true;
        prepareImpl();
    }
    virtual void prepareImpl() {}
};
struct MaterialUnlitColor : Material {
    MaterialUnlitColor() { materialType = MATERIAL_UNLITCOLOR; };
    MaterialUnlitColor(std::string_view name) : Material(name) { materialType = MATERIAL_UNLITCOLOR; };
    void bind(bool bindTechinique = true);
};

struct MaterialBlinnPhong : Material {
private:
    struct GpuData {
        glm::vec4 baseColor{1, 1, 1, 1};
        float shininess{200};
        int baseColorTexIndex{-1};
        int normalTexIndex{-1};
    } gpuData{};

    GL::BufferHandle uboBuffer;

    std::vector<Texture *> textures;
    GLuint uboHandle{};

public:
    MaterialBlinnPhong();
    MaterialBlinnPhong(std::string_view name);
    ~MaterialBlinnPhong();

    void setBaseColor(glm::vec4 const& color);
    void setShininess(float shininess);

    void setBaseColorImage(Image *image);
    void setNormalImage(Image *image);

    void prepareImpl() override;
    void bind(bool bindTechinique = true) override;
};

class MaterialManager : public Singleton<MaterialManager> {
    std::unordered_map<IdType, std::shared_ptr<Material>> _materials;
    std::array<int, MATERIAL_Num> _defaultMaterialIds;

public:
    MaterialManager();

    std::shared_ptr<Material> createMaterial(MaterialType type, std::string_view name = "");
    std::shared_ptr<Material> getMaterial(IdType id) const;
    std::shared_ptr<Material> getDefaultMaterial(MaterialType materialType) const {
        return _materials.at(_defaultMaterialIds[materialType]);
    };
};
