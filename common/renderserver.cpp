#include "renderserver.h"

std::array<std::unique_ptr<Technique>, POST_PROCESS_NUM> RenderServer::techniques;

PostProcess::PostProcess() { renderTechnique = std::make_unique<TechniquePostProcessRender>(); }

PostProcess::~PostProcess() {
}
void PostProcess::init() {
    glBindTexture(GL_TEXTURE_2D, inColorTexture);
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &resultTexWidth);
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &resultTexHeight);
    glBindTexture(GL_TEXTURE_2D, 0);

    initImpl();
}
void PostProcess::postProcess() {
    if (!isInit) {
        init();
        isInit = true;
    }
    postProcessImpl();
}
void PostProcess::run() {
    postProcess();

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    renderTechnique->bind();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, inColorTexture);
    renderSetting();
    glDrawArrays(GL_TRIANGLES, 0, 3);
}

PostProcessSSAO::PostProcessSSAO() { technique = std::make_unique<TechniqueSSAO>(); }
PostProcessSSAO::~PostProcessSSAO() {}

void PostProcessSSAO::initImpl() 
{
    //create paremeter ubo
    GL::createBuffer(GL::BufferCreateInfo{{},
                                          sizeof(Paras),
                                          GL::BUFFER_STORAGE_MAP_COHERENT_BIT | GL::BUFFER_STORAGE_MAP_PERSISTENT_BIT |
                                              GL::BUFFER_STORAGE_MAP_WRITE_BIT},
                     &params, &uboPara);
    GL::createBuffer(GL::BufferCreateInfo{{},
                                          sizeof(ShadingParams),
                                          GL::BUFFER_STORAGE_MAP_COHERENT_BIT | GL::BUFFER_STORAGE_MAP_PERSISTENT_BIT |
                                              GL::BUFFER_STORAGE_MAP_WRITE_BIT},
                     &shadingParams, &uboShadingPara);

    // create result image
    glGenTextures(1, &resultAO);
    glBindTexture(GL_TEXTURE_2D, resultAO);
    glTexStorage2D(GL_TEXTURE_2D, 1, GL_R32F, resultTexWidth, resultTexHeight);
    glBindTexture(GL_TEXTURE_2D, 0);
}
void PostProcessSSAO::renderSetting() {
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, resultAO);
    glBindBufferBase(GL_UNIFORM_BUFFER, 0, uboShadingPara);
}
void PostProcessSSAO::updateAOParams()
{
    glBindBuffer(GL_UNIFORM_BUFFER, uboPara);
    void *data = glMapBuffer(GL_UNIFORM_BUFFER, GL_WRITE_ONLY);
    memcpy(data, &params, sizeof(Paras));
    glUnmapBuffer(GL_UNIFORM_BUFFER);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}
void PostProcessSSAO::updateShadingParams()
{
    glBindBuffer(GL_UNIFORM_BUFFER, uboShadingPara);
    void *data = glMapBuffer(GL_UNIFORM_BUFFER, GL_WRITE_ONLY);
    memcpy(data, &shadingParams, sizeof(ShadingParams));
    glUnmapBuffer(GL_UNIFORM_BUFFER);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}
void PostProcessSSAO::postProcessImpl() {
    technique->bind();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, inDepth);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, inNormal);
    glBindBufferBase(GL_UNIFORM_BUFFER, 0, uboPara);
    glBindImageTexture(0, resultAO, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_R32F);
    glDispatchCompute(resultTexWidth * resultTexHeight / 1024 + 1, 1, 1);
}

//===============================================================

RenderTarget::~RenderTarget() {
    glDeleteTextures(images.size(), images.data());
    glDeleteFramebuffers(1, &fbo);
}

RenderServer::RenderServer() {
    _gridNode = std::make_unique<Node>();
    _gridNode->addComponent<Grid>();
    Input::framebufferResizeSignal.Connect(&RenderServer::onFramebufferResize, this);
    createDefaultFBO();
    initPostProcessTechniques();
}
void RenderServer::initPostProcessTechniques() {}

void RenderServer::createDefaultFBO() {
    _defaultRenderTarget.images.resize(3);

    // color attachment
    auto image = &_defaultRenderTarget.images[0];
    glGenTextures(1, image);
    glBindTexture(GL_TEXTURE_2D, *image);
    glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, (GLsizei)Input::framebufferWidth, (GLsizei)Input::framebufferHeight);
    glBindTexture(GL_TEXTURE_2D, 0);

    // output normal
    image = &_defaultRenderTarget.images[1];
    glGenTextures(1, image);
    glBindTexture(GL_TEXTURE_2D, *image);
    glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGB8, (GLsizei)Input::framebufferWidth, (GLsizei)Input::framebufferHeight);
    glBindTexture(GL_TEXTURE_2D, 0);

    // create depth
    image = &_defaultRenderTarget.images[2];
    glGenTextures(1, image);
    glBindTexture(GL_TEXTURE_2D, *image);
    glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH24_STENCIL8, (GLsizei)Input::framebufferWidth,
                   (GLsizei)Input::framebufferHeight);
    glBindTexture(GL_TEXTURE_2D, 0);

    //
    glGenFramebuffers(1, &_defaultRenderTarget.fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, _defaultRenderTarget.fbo);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, _defaultRenderTarget.images[0], 0);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, _defaultRenderTarget.images[1], 0);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, _defaultRenderTarget.images[2], 0);

    GLenum a[]{GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1};
    glDrawBuffers(std::size(a), a);

    auto status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (status != GL_FRAMEBUFFER_COMPLETE) {
        THROW("framebuffer is not complete");
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
void RenderServer::recreateDefaultFBO() {
    _defaultRenderTarget.~RenderTarget();
    createDefaultFBO();
}
void RenderServer::renderScene(Scene *scene) {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    scene->environment->render();

    if (postProcessType != POST_PROCESS_NONE) {
        glBindFramebuffer(GL_FRAMEBUFFER, _defaultRenderTarget.fbo);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }
    if (showGrid) _gridNode->getComponent<Grid>()->draw();

    scene->_editorCameraNode->getComponent<Camera>()->bind();

    std::vector<Renderer *> renderers;
    scene->_root->getComponents(renderers, true);

    std::vector<Light *> lights;
    scene->_root->getComponents(lights, true);

    for (auto e : lights) {
        e->bind(2);
        for (auto e : renderers) {
            e->draw();
        }
    }

    //
    if (postProcessType != POST_PROCESS_NONE) {
        PostProcessSSAO::getSingleton().inColorTexture= _defaultRenderTarget.images[0];
        PostProcessSSAO::getSingleton().inNormal = _defaultRenderTarget.images[1];
        PostProcessSSAO::getSingleton().inDepth = _defaultRenderTarget.images[2];
        scene->_editorCameraNode->getComponent<Camera>()->bind();
        PostProcessSSAO::getSingleton().run();
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
void RenderServer::onFramebufferResize(int width, int height) { recreateDefaultFBO(); }