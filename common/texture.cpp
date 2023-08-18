#include "texture.h"

#include <cmath>
#include <iostream>

Texture::Texture(Image *image) : _pImageSrc(image) { image->load(); }
Texture::~Texture() {
    glDeleteTextures(1, &_image);
    glDeleteTextures(1, &_imageView);
}
void Texture::upload() {
    if (_image) return;
    _uploaded = true;

    GL::Extent3D extent{(uint32_t)(_pImageSrc->_width), (uint32_t)(_pImageSrc->_height), 1};

    GL::createImage(GL::ImageCreateInfo{0, GL::IMAGE_TYPE_2D, _pImageSrc->_format, extent, 0, GL::SAMPLE_COUNT_1_BIT},
                    &_image);

    updateImageSubData(
        _image, GL::IMAGE_TYPE_2D,
        GL::ImageSubData{_pImageSrc->_baseFormat, _pImageSrc->_dataType, 0, {{}, extent}, _pImageSrc->_data});
    // generate mipmap
    auto target = Map(GL::IMAGE_TYPE_2D, false);
    glBindTexture(target, _image);
    glGenerateMipmap(target);
    glBindTexture(target, 0);

    _mipmapLevel = std::floor(std::log2(std::max(extent.width, extent.height)));

    GL::createImageView(
        GL::ImageViewCreateInfo{_image,
                                GL::ImageViewType::IMAGE_VIEW_TYPE_2D,
                                _pImageSrc->_format,
                                {},
                                {GL::ImageAspectFlagBits::IMAGE_ASPECT_COLOR_BIT, 0, _mipmapLevel, 0, 1}},
        false, &_imageView);
    _target = Map(GL::ImageViewType::IMAGE_VIEW_TYPE_2D, false);
}

void Texture::bind(uint32_t binding) {
    if (!_uploaded) upload();
    glActiveTexture(GL_TEXTURE0 + binding);
    glBindTexture(_target, _imageView);
}

Texture *TextureManager::createTexture(Image *pImage) {
    auto it = _textures.find(pImage->getId());
    if (it != _textures.end()) return it->second.get();
    return _textures.emplace(pImage->getId(), std::make_unique<Texture>(pImage)).first->second.get();
}
void TextureManager::removeTexture(Texture const *texture) {
    auto id = texture->getImage()->getId();
    auto it = _textures.find(id);
    if (it == _textures.end()) return;
    _textures.erase(it);
}