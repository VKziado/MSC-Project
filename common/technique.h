#pragma once
#include "prerequisites.h"

enum MaterialType {
    MATERIAL_BLINNPHONG,
    MATERIAL_UNLITCOLOR,
    MATERIAL_Num,
};

struct Technique {
    virtual ~Technique() {}
    // bind pipeline
    virtual void bind() = 0;
};
struct TechniqueEnv : public Technique {
    static std::string vertFile;
    static std::string fragFile;
    GL::PipelineHandle pipeline;
    TechniqueEnv();
    void bind() override;
};

struct TechniqueUnlitColor : Technique {
    static std::string vertFile;
    static std::string fragFile;
    GL::PipelineHandle pipeline;
    TechniqueUnlitColor();
    void bind() override;
};

struct TechniqueBlinnPhong : Technique {
    static std::string vertFile;
    static std::string fragFile;
    GL::PipelineHandle pipeline;

    TechniqueBlinnPhong();

    void bind() override;
};

struct TechniqueGrid : Technique {
    static std::string vertFile;
    static std::string fragFile;
    GL::PipelineHandle pipeline;

    TechniqueGrid();

    void bind() override;
};
struct TechniquePostProcessRender: Technique {

    static std::string vertFile;
    static std::string fragFile;
    GL::PipelineHandle pipeline;

    TechniquePostProcessRender();

    void bind() override;
};

struct TechniqueSSAO : Technique {
    static std::string compFile;
    GL::PipelineHandle pipeline;

    TechniqueSSAO();

    void bind() override;
};