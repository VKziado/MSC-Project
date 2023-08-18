#include "image.h"

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image.h"
#include "stb_image_write.h"

bool saveImage(std::string_view path, std::string_view imageType, int width, int height, int componentNum, void *data) {
    if (imageType == "PNG")
        stbi_write_png(path.data(), width, height, componentNum, data, componentNum);
    else if (imageType == "BMP")
        stbi_write_bmp(path.data(), width, height, componentNum, data);
    else if (imageType == "TGA")
        stbi_write_tga(path.data(), width, height, componentNum, data);
    else if (imageType == "JPG")
        stbi_write_jpg(path.data(), width, height, componentNum, data, 90);
    else if (imageType == "HDR")
        stbi_write_hdr(path.data(), width, height, componentNum, (float *)data);
    else
        throw std::runtime_error("failed to save image");
}

bool ImageLoaderSTB::load(Image *pImage) {
    auto path = pImage->_path;

    int componentNum;

    stbi_set_flip_vertically_on_load(true);
    if (stbi_is_hdr(pImage->_path.c_str())) {
        pImage->_data = stbi_loadf(pImage->_path.c_str(), &pImage->_width, &pImage->_height, &pImage->_componentNum, 4);
        pImage->_format = GL_RGBA32F;
        pImage->_dataType = GL::DATA_TYPE_FLOAT;
    } else {
        pImage->_data = stbi_load(pImage->_path.c_str(), &pImage->_width, &pImage->_height, &pImage->_componentNum, 4);
        pImage->_dataType = GL::DATA_TYPE_UNSIGNED_BYTE;
        if (pImage->_isSRGB)
            pImage->_format = GL_SRGB8_ALPHA8;
        else
            pImage->_format = GL_RGBA8;
    }
    pImage->_baseFormat = GL_RGBA;
    if (pImage->_data) return true;
    return false;
}
void ImageLoaderSTB::unload(Image *pImage) { stbi_image_free(pImage->_data); }

//========================
Image::Image(std::string_view path, ImageLoader *loader) : _path(path), _loader(loader) {}
Image::Image(std::string_view path, uint32_t width, uint32_t height, GL::Format format, GL::Format baseFormat,
             GL::DataType dataType, size_t size, void *data)
    : _path(path), _width(width), _height(height), _format(format), _baseFormat(baseFormat), _dataType(dataType) {
    _data = new char[size];
    memcpy(_data, data, size);
}
Image::~Image() { unload(); }
// Image::Image(std::string_view name, ImageDescription const &desc,
// std::string_view path) 	: _name(name), _desc(desc), _loaded(true), _path(path)
// {
// }
void Image::load() {
    if (_loaded) return;
    if (_loader && !_loader->load(this)) LOG("failed to load image path ", _path);
}
void Image::unload() {
    if (!_loaded) return;
    _loaded = false;
    if (_loader) {
        _loader->unload(this);
    } else {
        delete _data;
        _data = 0;
    }
}
//==================================
ImageManager::ImageManager() { registerImageLoader<ImageLoaderSTB>("default"); }
ImageLoader *ImageManager::getImageLoader(std::string_view name) const {
    auto it = _imageLoaders.find(std::string(name));
    if (it != _imageLoaders.cend()) return it->second.get();
    return nullptr;
}
Image *ImageManager::create(std::string_view path, std::string_view loaderName) {
    if (auto loader = getImageLoader(loaderName))
        return _images.emplace(path, std::make_unique<Image>(path, loader)).first->second.get();
    LOG("cannot find image loader");
    return nullptr;
}
Image *ImageManager::create(std::string_view path, uint32_t width, uint32_t height, GL::Format format,
                            GL::Format baseFormat, GL::DataType dataType, size_t size, void *data) {
    return _images.emplace(path, std::make_unique<Image>(path, width, height, format, baseFormat, dataType, size, data))
        .first->second.get();
}