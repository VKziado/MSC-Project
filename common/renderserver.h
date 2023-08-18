#pragma once
#include "scene.h"
#include "singleton.h"

enum PostProcessType {
    POST_PROCESS_NONE,
    POST_PROCESS_SSAO,
    POST_PROCESS_NUM,
};

struct RenderTarget {
    GL::FramebufferHandle fbo;
    std::vector<GL::ImageHandle> images;
    ~RenderTarget();
};

class PostProcess {
protected:
    std::unique_ptr<Technique> renderTechnique;

    std::unique_ptr<Technique> technique;

    GLint resultTexWidth, resultTexHeight;

    bool isInit{false};

    void init();
    void postProcess();

    virtual void initImpl() {}
    virtual void postProcessImpl(){}
    virtual void renderSetting() {}

public:
    GL::ImageHandle inColorTexture{};

    PostProcess();
    virtual ~PostProcess();
    void run();
};

class PostProcessSSAO : public PostProcess, public Singleton<PostProcessSSAO> {
    void initImpl() override;
    void postProcessImpl() override;
    void renderSetting() override;

    GL::ImageHandle resultAO;

    GL::BufferHandle uboPara;
    GL::BufferHandle uboShadingPara;

public:
    GL::ImageHandle inDepth;
    GL::ImageHandle inNormal;

    struct Paras
    {
        int aoType;
        int sampleStep{16};
        int sampleStepNum{2};
        int sampleDirections{4};
        float maxRadius0{10};
        float maxRadius1{15};
    }params{};

    struct ShadingParams {
        int shadingMode;
    } shadingParams{};

    void updateAOParams();
    void updateShadingParams();

    PostProcessSSAO();
    ~PostProcessSSAO();
};

class RenderServer : public Singleton<RenderServer> {
    std::unique_ptr<Node> _gridNode;

    static std::array<std::unique_ptr<Technique>, POST_PROCESS_NUM> techniques;

    RenderTarget _defaultRenderTarget;

    void createDefaultFBO();
    void recreateDefaultFBO();

    void initPostProcessTechniques();

public:
    RenderServer();
    ~RenderServer() {}

    void renderScene(Scene* scene);

    PostProcessType postProcessType{};

    bool showGrid = true;

    void onFramebufferResize(int width, int height);
};