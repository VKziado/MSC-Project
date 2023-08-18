#include "technique.h"

#include "common.h"

std::string TechniqueEnv::vertFile = "common.vert";
std::string TechniqueEnv::fragFile = "environment.frag";

TechniqueEnv::TechniqueEnv() {
    auto vertCode = readFile(shaderPath(vertFile.data()));
    auto fragCode = readFile(shaderPath(fragFile.data()));

    GL::ShaderCreateInfo vertShaderCreateInfo{GL::SHADER_STAGE_VERTEX_BIT, vertCode.size(), vertCode.data()};
    GL::ShaderCreateInfo fragShaderCreateInfo{GL::SHADER_STAGE_FRAGMENT_BIT, fragCode.size(), fragCode.data()};

    GL::GraphicsPipelineCreateInfo pipelineCI{false};
    pipelineCI.stages.resize(2);
    pipelineCI.stages[0] = {GL::SHADER_STAGE_VERTEX_BIT, 0, "main"};
    pipelineCI.stages[1] = {GL::SHADER_STAGE_FRAGMENT_BIT, 0, "main"};

    GL::createShader(vertShaderCreateInfo, &pipelineCI.stages[0].shaderHandle);
    GL::createShader(fragShaderCreateInfo, &pipelineCI.stages[1].shaderHandle);

    std::vector<GL::VertexBindingDescription> bindingDescriptions{
        {sizeof(float) * 3, 0},  // position
        {sizeof(float) * 3, 0},  // normal
        {sizeof(float) * 2, 0},  // texcoord
        {sizeof(float) * 4, 0},  // color
    };

    std::vector<GL::VertexAttributeDescription> attributeDescriptions{
        {0, 0, 3, GL::DATA_TYPE_FLOAT, false, 0},
        {1, 1, 3, GL::DATA_TYPE_FLOAT, false, 0},
        {2, 2, 2, GL::DATA_TYPE_FLOAT, false, 0},
        {3, 3, 4, GL::DATA_TYPE_FLOAT, false, 0},
    };
    pipelineCI.vertexInputState = {bindingDescriptions, attributeDescriptions};

    GL::createGraphicsPipeline(pipelineCI, &pipeline);
}
void TechniqueEnv::bind() {
    glBindProgramPipeline(pipeline.pipeline);
    glBindVertexArray(pipeline.vao);
}

std::string TechniqueUnlitColor::vertFile = "unlitColor.vert";
std::string TechniqueUnlitColor::fragFile = "unlitColor.frag";
TechniqueUnlitColor::TechniqueUnlitColor() {
    auto vertCode = readFile(shaderPath(vertFile.data()));
    auto fragCode = readFile(shaderPath(fragFile.data()));

    GL::ShaderCreateInfo vertShaderCreateInfo{GL::SHADER_STAGE_VERTEX_BIT, vertCode.size(), vertCode.data()};
    GL::ShaderCreateInfo fragShaderCreateInfo{GL::SHADER_STAGE_FRAGMENT_BIT, fragCode.size(), fragCode.data()};

    GL::GraphicsPipelineCreateInfo pipelineCI{false};
    pipelineCI.stages.resize(2);
    pipelineCI.stages[0] = {GL::SHADER_STAGE_VERTEX_BIT, 0, "main"};
    pipelineCI.stages[1] = {GL::SHADER_STAGE_FRAGMENT_BIT, 0, "main"};

    GL::createShader(vertShaderCreateInfo, &pipelineCI.stages[0].shaderHandle);
    GL::createShader(fragShaderCreateInfo, &pipelineCI.stages[1].shaderHandle);

    std::vector<GL::VertexBindingDescription> bindingDescriptions{
        {sizeof(float) * 3, 0},  // position
        {},
        {},
        {sizeof(float) * 4, 0},  // color
    };

    std::vector<GL::VertexAttributeDescription> attributeDescriptions{
        {0, 0, 3, GL::DATA_TYPE_FLOAT, false, 0},
        {3, 3, 4, GL::DATA_TYPE_FLOAT, false, 0},
    };
    pipelineCI.vertexInputState = {bindingDescriptions, attributeDescriptions};

    GL::createGraphicsPipeline(pipelineCI, &pipeline);
}
void TechniqueUnlitColor::bind() {
    glBindProgramPipeline(pipeline.pipeline);
    glBindVertexArray(pipeline.vao);
}

std::string TechniqueBlinnPhong ::vertFile = "common.vert";
std::string TechniqueBlinnPhong ::fragFile = "blinnphong.frag";
TechniqueBlinnPhong::TechniqueBlinnPhong() {
    auto vertCode = readFile(shaderPath(vertFile.data()));
    auto fragCode = readFile(shaderPath(fragFile.data()));

    GL::ShaderCreateInfo vertShaderCreateInfo{GL::SHADER_STAGE_VERTEX_BIT, vertCode.size(), vertCode.data()};
    GL::ShaderCreateInfo fragShaderCreateInfo{GL::SHADER_STAGE_FRAGMENT_BIT, fragCode.size(), fragCode.data()};

    GL::GraphicsPipelineCreateInfo pipelineCI{false};
    pipelineCI.stages.resize(2);
    pipelineCI.stages[0] = {GL::SHADER_STAGE_VERTEX_BIT, 0, "main"};
    pipelineCI.stages[1] = {GL::SHADER_STAGE_FRAGMENT_BIT, 0, "main"};

    GL::createShader(vertShaderCreateInfo, &pipelineCI.stages[0].shaderHandle);
    GL::createShader(fragShaderCreateInfo, &pipelineCI.stages[1].shaderHandle);

    std::vector<GL::VertexBindingDescription> bindingDescriptions{
        {sizeof(float) * 3, 0},  // position
        {sizeof(float) * 3, 0},  // normal
        {sizeof(float) * 2, 0},  // texcoord
        {sizeof(float) * 4, 0},  // color
    };

    std::vector<GL::VertexAttributeDescription> attributeDescriptions{
        {0, 0, 3, GL::DATA_TYPE_FLOAT, false, 0},
        {1, 1, 3, GL::DATA_TYPE_FLOAT, false, 0},
        {2, 2, 2, GL::DATA_TYPE_FLOAT, false, 0},
        {3, 3, 4, GL::DATA_TYPE_FLOAT, false, 0},
    };
    pipelineCI.vertexInputState = {bindingDescriptions, attributeDescriptions};

    GL::createGraphicsPipeline(pipelineCI, &pipeline);
}
void TechniqueBlinnPhong::bind() {
    glBindProgramPipeline(pipeline.pipeline);
    glBindVertexArray(pipeline.vao);
}

//=================
std::string TechniqueGrid ::vertFile = "grid.vert";
std::string TechniqueGrid ::fragFile = "grid.frag";
TechniqueGrid::TechniqueGrid() {
    auto vertCode = readFile(shaderPath(vertFile.data()));
    auto fragCode = readFile(shaderPath(fragFile.data()));

    GL::ShaderCreateInfo vertShaderCreateInfo{GL::SHADER_STAGE_VERTEX_BIT, vertCode.size(), vertCode.data()};
    GL::ShaderCreateInfo fragShaderCreateInfo{GL::SHADER_STAGE_FRAGMENT_BIT, fragCode.size(), fragCode.data()};

    GL::GraphicsPipelineCreateInfo pipelineCI{false};
    pipelineCI.stages.resize(2);
    pipelineCI.stages[0] = {GL::SHADER_STAGE_VERTEX_BIT, 0, "main"};
    pipelineCI.stages[1] = {GL::SHADER_STAGE_FRAGMENT_BIT, 0, "main"};

    GL::createShader(vertShaderCreateInfo, &pipelineCI.stages[0].shaderHandle);
    GL::createShader(fragShaderCreateInfo, &pipelineCI.stages[1].shaderHandle);

    GL::createGraphicsPipeline(pipelineCI, &pipeline);
}
void TechniqueGrid::bind() {
    glBindProgramPipeline(pipeline.pipeline);
    glBindVertexArray(pipeline.vao);
}

//=================
std::string TechniquePostProcessRender ::vertFile = "renderintriangle.vert";
std::string TechniquePostProcessRender ::fragFile = "postprocessrender.frag";
TechniquePostProcessRender::TechniquePostProcessRender() {
    auto vertCode = readFile(shaderPath(vertFile.data()));
    auto fragCode = readFile(shaderPath(fragFile.data()));

    GL::ShaderCreateInfo vertShaderCreateInfo{GL::SHADER_STAGE_VERTEX_BIT, vertCode.size(), vertCode.data()};
    GL::ShaderCreateInfo fragShaderCreateInfo{GL::SHADER_STAGE_FRAGMENT_BIT, fragCode.size(), fragCode.data()};

    GL::GraphicsPipelineCreateInfo pipelineCI{false};
    pipelineCI.stages.resize(2);
    pipelineCI.stages[0] = {GL::SHADER_STAGE_VERTEX_BIT, 0, "main"};
    pipelineCI.stages[1] = {GL::SHADER_STAGE_FRAGMENT_BIT, 0, "main"};

    GL::createShader(vertShaderCreateInfo, &pipelineCI.stages[0].shaderHandle);
    GL::createShader(fragShaderCreateInfo, &pipelineCI.stages[1].shaderHandle);

    GL::createGraphicsPipeline(pipelineCI, &pipeline);
}
void TechniquePostProcessRender::bind() {
    glBindProgramPipeline(pipeline.pipeline);
    glBindVertexArray(pipeline.vao);
}

//===============================
std::string TechniqueSSAO ::compFile = "ssao.comp";
TechniqueSSAO::TechniqueSSAO() {
    auto compCode = readFile(shaderPath(compFile.data()));
    GL::ShaderCreateInfo compShaderCreateInfo{GL::SHADER_STAGE_COMPUTE_BIT, compCode.size(), compCode.data()};
    GL::ComputePipelineCreateInfo pipelineCI{false};
    pipelineCI.stage = {GL::SHADER_STAGE_COMPUTE_BIT, 0, "main"};
    GL::createShader(compShaderCreateInfo, &pipelineCI.stage.shaderHandle);
    GL::createComputePipeline(pipelineCI, &pipeline);
}
void TechniqueSSAO::bind() {
    glBindProgramPipeline(pipeline.pipeline);
}