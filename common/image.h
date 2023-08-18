#pragma once
#include <fstream>
#include <iostream>

#include "idObject.h"
#include "prerequisites.h"

// struct ImageDescription
// {
// 	bool hdr{false};
// 	int width;
// 	int height;
// 	int componentNum;
// 	int stride = -1;
// 	GL::DataType componentType;
// 	unsigned char *data;
// 	size_t size()
// 	{
// 		return width * height * (stride > 0 ? stride :
// GL::getDataTypeSize(componentType) * componentNum);
// 	}
// };

/**
 * @brief
 *
 * @param path
 * @param imageType  PNG, JPG
 * @param width
 * @param height
 * @param componentNum
 * @param data
 * @return true
 * @return false
 */
bool saveImage(std::string_view path, std::string_view imageType, int width, int height, int componentNum, void *data);

class Image;

class ImageLoader {
public:
    // virtual ImageDescription getInfo(std::iostream const &ss) = 0;
    // virtual ImageDescription load(std::istream *inStream) = 0;

    virtual bool load(Image *pImage) = 0;
    virtual void unload(Image *pImage) = 0;
    virtual ~ImageLoader() {}
};

class ImageLoaderSTB : public ImageLoader {
public:
    bool load(Image *pImage) override;
    void unload(Image *pImage) override;
    ~ImageLoaderSTB() {}
};

class ImageManager;

class Image : public IdObject {
    friend class ImageManger;

public:
    std::string _path;
    ImageLoader *_loader{};

    bool _loaded{false};

    void *_data{};
    int _width;
    int _height;
    int _componentNum;
    bool _isSRGB = false;
    GL::Format _format;
    GL::Format _baseFormat;
    GL::DataType _dataType;

    ImageManager *_creator;

    /**
     * @brief create image from file
     *
     * @param name
     * @param path
     */
    Image(std::string_view path, ImageLoader *loader);

    Image(std::string_view path, uint32_t width, uint32_t height, GL::Format format, GL::Format baseFormat,
          GL::DataType dataType, size_t size, void *data);

    ~Image();

    void load();

    void unload();

    // ImageDescription const &getDescription() const { return _desc; }
};

class ImageManager : public Singleton<ImageManager> {
    std::unordered_map<std::string, std::unique_ptr<Image>> _images;
    std::unordered_map<std::string, std::unique_ptr<ImageLoader>> _imageLoaders;

public:
    ImageManager();

    Image *create(std::string_view path, std::string_view loaderName = "default");

    Image *create(std::string_view path, uint32_t width, uint32_t height, GL::Format format, GL::Format baseFormat,
                  GL::DataType dataType, size_t size, void *data);

    Image *getImage(std::string_view name) const { return _images.at(std::string(name)).get(); }

    ImageLoader *getImageLoader(std::string_view name) const;

    template <typename Loader_T>
    void registerImageLoader(std::string_view loaderName) {
        _imageLoaders.emplace(loaderName, std::make_unique<Loader_T>());
    }
};
