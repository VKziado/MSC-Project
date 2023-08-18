#pragma once
#include <string>
#include <string_view>

#include "device.h"
#include "gl/glew.h"
#include "idObject.h"
#include "image.h"

class Texture : public IdObject {
    Image *_pImageSrc;
    GL::ImageHandle _image{};
    GL::ImageViewHandle _imageView{};

    uint32_t _mipmapLevel{};
    GLenum _target{GL_TEXTURE_2D};
    bool _uploaded{false};

public:
    Texture(Image *image);
    ~Texture();

    constexpr Image *getImage() const { return _pImageSrc; }

    void upload();

    void bind(uint32_t binding);
};

class TextureManager : public Singleton<TextureManager> {
    // texture Id,
    std::unordered_map<IdType , std::unique_ptr<Texture>> _textures;

public:
    TextureManager() {}
    ~TextureManager() {}

    Texture* createTexture(Image *pImage);

    void removeTexture(Texture const *texture);
};