#include "device.h"

#include <iostream>
#include <sstream>
#include <string>

template <class... Args>
constexpr void myprint(std::ostream &os, Args &&...args) noexcept {
    ((os << std::forward<Args>(args) << " "), ...);
}

#ifdef NDEBUG
#define GL_LOG(...)
#define GL_LOG_VAR(str)
#else
#define GL_LOG(...)                                       \
    {                                                     \
        std::cout << __FILE__ << " " << __LINE__ << ": "; \
        myprint(std::cout, __VA_ARGS__);                  \
        std::cout << std::endl;                           \
    }
#define GL_LOG_VAR(...)                                                          \
    {                                                                            \
        std::cout << __FILE__ << " " << __LINE__ << " " << #__VA_ARGS__ << ": "; \
        myprint(std::cout, __VA_ARGS__);                                         \
        std::cout << std::endl;                                                  \
    }
// std::cout << __FILE__ << " " << __LINE__ << ":  " << #str << ": " << str << std::endl;
#endif

#define THROW(...)                            \
    {                                         \
        std::stringstream ss;                 \
        myprint(ss, __FILE__, __LINE__, ":"); \
        myprint(ss, __VA_ARGS__);             \
        throw std::runtime_error(ss.str());   \
    }
#define CHECK_VK_RESULT(x)                                            \
    {                                                                 \
        auto res = x;                                                 \
        if (res != VK_SUCCESS) THROW(#x " failed, error code:", res); \
    }

namespace GL {
PipelineHandle::~PipelineHandle() {
    glDeleteVertexArrays(1, &vao);
    for (auto e : programs) glDeleteProgram(e);
    glDeleteProgramPipelines(1, &pipeline);
}

static void APIENTRY DebugOutputCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length,
                                         const GLchar *message, const void *userParam) {
    /*
id: 131204
Source: API :0x33350
Type: Other :0x33361
Severity: low :0x37192
Texture state usage warning: The texture object (N) bound to texture image unit M does not have a defined base level and
cannot be used for texture mapping.

id: 131076
Source: API :0x8246
Type: Other :0x8251
Severity: low :0x9148
Usage warning: Generic vertex attribute array N uses a pointer with a small value (0x0000000000000000).Is this intended
to be used as an offset into a buffer object?

id: 131185
Source: API :0x8246
Type: Other :0x8251
Severity: notification :0x826b
Buffer detailed info: Buffer object N (bound to GL_UNIFORM_BUFFER (1), and GL_UNIFORM_BUFFER_EXT, usage hint is
GL_DYNAMIC_DRAW) has been mapped WRITE_ONLY in SYSTEM HEAP memory (fast).

id:131169
Source: API :0x33350
Type: Other :0x33361
Severity: low :0x37192
Framebuffer detailed info: The driver allocated storage for renderbuffer N.
*/
    if (id == 131204 || id == 131076 || id == 131185 || id == 131169)
        // if (id == 131169 || id == 131185 || id == 131218 || id == 131204 || id == 131076)
        return;  // ignore these non-significant error codes

    std::string str;
    str.reserve(512);
    str += "\nDebug message id:";
    str += std::to_string(id);
    str += "\n";
    switch (type) {
        case GL_DEBUG_TYPE_ERROR:
            str += "Type: Error :0x";
            str += std::to_string(GL_DEBUG_TYPE_ERROR);
            break;
        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
            str += "Type: Deprecated Behaviour :0x";
            str += std::to_string(GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR);
            break;
        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
            str += "Type: Undefined Behaviour :0x";
            str += std::to_string(GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR);
            break;
        case GL_DEBUG_TYPE_PORTABILITY:
            str += "Type: Portability :0x";
            str += std::to_string(GL_DEBUG_TYPE_PORTABILITY);
            break;
        case GL_DEBUG_TYPE_PERFORMANCE:
            str += "Type: Performance :0x";
            str += std::to_string(GL_DEBUG_TYPE_PERFORMANCE);
            break;
        case GL_DEBUG_TYPE_MARKER:
            str += "Type: Marker :0x";
            str += std::to_string(GL_DEBUG_TYPE_MARKER);
            break;
        case GL_DEBUG_TYPE_PUSH_GROUP:
            str += "Type: Push Group :0x";
            str += std::to_string(GL_DEBUG_TYPE_PUSH_GROUP);
            break;
        case GL_DEBUG_TYPE_POP_GROUP:
            str += "Type: Pop Group :0x";
            str += std::to_string(GL_DEBUG_TYPE_POP_GROUP);
            break;
        case GL_DEBUG_TYPE_OTHER:
            str += "Type: Other :0x";
            str += std::to_string(GL_DEBUG_TYPE_OTHER);
            break;
    }
    str += "\n";
    switch (severity) {
        case GL_DEBUG_SEVERITY_HIGH:
            str += "Severity: high :0x";
            str += std::to_string(GL_DEBUG_SEVERITY_HIGH);
            break;
        case GL_DEBUG_SEVERITY_MEDIUM:
            str += "Severity: medium :0x";
            str += std::to_string(GL_DEBUG_SEVERITY_MEDIUM);
            break;
        case GL_DEBUG_SEVERITY_LOW:
            str += "Severity: low :0x";
            str += std::to_string(GL_DEBUG_SEVERITY_LOW);
            break;
        case GL_DEBUG_SEVERITY_NOTIFICATION:
            str += "Severity: notification :0x";
            str += std::to_string(GL_DEBUG_SEVERITY_NOTIFICATION);
            break;
    }
    str += "\n";
    switch (source) {
        case GL_DEBUG_SOURCE_API:
            str += "Source: API :0x";
            str += std::to_string(GL_DEBUG_SOURCE_API);
            break;
        case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
            str += "Source: Window System :0x";
            str += std::to_string(GL_DEBUG_SOURCE_WINDOW_SYSTEM);
            break;
        case GL_DEBUG_SOURCE_SHADER_COMPILER:
            str += "Source: Shader Compiler :0x";
            str += std::to_string(GL_DEBUG_SOURCE_SHADER_COMPILER);
            break;
        case GL_DEBUG_SOURCE_THIRD_PARTY:
            str += "Source: Third Party :0x";
            str += std::to_string(GL_DEBUG_SOURCE_THIRD_PARTY);
            break;
        case GL_DEBUG_SOURCE_APPLICATION:
            str += "Source: Application :0x";
            str += std::to_string(GL_DEBUG_SOURCE_APPLICATION);
            break;
        case GL_DEBUG_SOURCE_OTHER:
            str += "Source: Other :0x";
            str += std::to_string(GL_DEBUG_SOURCE_OTHER);
            break;
    }
    str += "\nlength:";
    str += std::to_string(length);
    str += "\nmessage:";
    str += message;
    GL_LOG(str)
}
void EnableDebugOutput(const void *userParam) {
    // enable OpenGL debug context if context allows for debug context
    GLint flags;
    glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
    if (flags & GL_CONTEXT_FLAG_DEBUG_BIT) {
        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);  // makes sure errors are displayed
                                                // synchronously
        if (GLEW_VERSION_4_3) {
            glDebugMessageCallback(GL::DebugOutputCallback, userParam);
            glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, NULL, GL_TRUE);
        } else {
#ifdef GL_ARB_debug_output
            glDebugMessageCallbackARB(GL::DebugOutputCallback, userParam);
            glDebugMessageControlARB(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, NULL, GL_TRUE);
#else
            THROW("current opengl version do not support debug output");
#endif
        }
    } else {
        THROW("current context do not support debug output");
    }
}

void createShader(const ShaderCreateInfo &createInfo, ShaderHandle *pShader) {
    GLenum shaderType{};
    switch (createInfo.stage) {
        case SHADER_STAGE_VERTEX_BIT:
            shaderType = GL_VERTEX_SHADER;
            break;
        case SHADER_STAGE_FRAGMENT_BIT:
            shaderType = GL_FRAGMENT_SHADER;
            break;
        case SHADER_STAGE_GEOMETRY_BIT:
            shaderType = GL_GEOMETRY_SHADER;
            break;
        case SHADER_STAGE_TESS_CONTROL_BIT:
            shaderType = GL_TESS_CONTROL_SHADER;
            break;
        case SHADER_STAGE_TESS_EVALUATION_BIT:
            shaderType = GL_TESS_EVALUATION_SHADER;
            break;
        case SHADER_STAGE_COMPUTE_BIT:
            shaderType = GL_COMPUTE_SHADER;
            break;
        case SHADER_STAGE_ALL_BITS:
            break;
    }
    *pShader = glCreateShader(shaderType);
    if (*pShader) {
        glShaderSource(*pShader, 1, &createInfo.code, NULL);
        glCompileShader(*pShader);
        glReleaseShaderCompiler();
#ifndef NDEBUG
        GLint success;
        glGetShaderiv(*pShader, GL_COMPILE_STATUS, &success);
        if (!success) {
            GLint len;
            glGetShaderiv(*pShader, GL_INFO_LOG_LENGTH, &len);
            std::string log;
            log.resize(static_cast<size_t>(len));
            glGetShaderInfoLog(*pShader, len, NULL, &log[0]);
            GL_LOG(log);
            glDeleteShader(*pShader);
            throw std::runtime_error("failed to compile shader");
        }
#endif
    } else {
        throw std::runtime_error("failed to create shader");
    }
}
void createShaderBinary(const ShaderCreateInfo &createInfo, ShaderHandle *pShader) {
    GLenum shaderType{};
    switch (createInfo.stage) {
        case SHADER_STAGE_VERTEX_BIT:
            shaderType = GL_VERTEX_SHADER;
            break;
        case SHADER_STAGE_FRAGMENT_BIT:
            shaderType = GL_FRAGMENT_SHADER;
            break;
        case SHADER_STAGE_GEOMETRY_BIT:
            shaderType = GL_GEOMETRY_SHADER;
            break;
        case SHADER_STAGE_TESS_CONTROL_BIT:
            shaderType = GL_TESS_CONTROL_SHADER;
            break;
        case SHADER_STAGE_TESS_EVALUATION_BIT:
            shaderType = GL_TESS_EVALUATION_SHADER;
            break;
        case SHADER_STAGE_COMPUTE_BIT:
            shaderType = GL_COMPUTE_SHADER;
            break;
        case SHADER_STAGE_ALL_BITS:
            break;
    }

    *pShader = glCreateShader(shaderType);
    if (*pShader) {
        glShaderBinary(1, pShader, GL_SHADER_BINARY_FORMAT_SPIR_V, createInfo.code, createInfo.size);
    } else
        throw std::runtime_error("failed to create shader");
}
void createProgram(std::span<PipelineShaderStageCreateInfo> stageCreateInfos, ProgramHandle *pProgram) {
    createProgram(stageCreateInfos.size(), stageCreateInfos.data(), pProgram);
}
void createProgram(size_t count, PipelineShaderStageCreateInfo const *createInfos, ProgramHandle *pProgram) {
    *pProgram = glCreateProgram();
    for (size_t i = 0; i < count; ++i) {
        auto &stageCI = createInfos[i];
        // This will now return FALSE
        GLint status;
        glGetShaderiv(stageCI.shaderHandle, GL_COMPILE_STATUS, &status);
        GL_LOG(status);

        // equal to compilation
        glSpecializeShader(stageCI.shaderHandle, stageCI.entryName.data(),
                           stageCI.specializationInfo.constantIDs.size(), stageCI.specializationInfo.constantIDs.data(),
                           stageCI.specializationInfo.constantValues.data());

        // Specialization is equivalent to compilation.
        GLint isCompiled = 0;
        glGetShaderiv(stageCI.shaderHandle, GL_COMPILE_STATUS, &isCompiled);

#ifndef NDEBUG
        GLint maxLength = 0;
        glGetShaderiv(stageCI.shaderHandle, GL_INFO_LOG_LENGTH, &maxLength);

        // The maxLength includes the NULL character
        std::string infoLog;
        infoLog.resize(maxLength);
        glGetShaderInfoLog(stageCI.shaderHandle, maxLength, &maxLength, infoLog.data());

        GL_LOG(infoLog);
#endif
        if (isCompiled == GL_FALSE) {
            // We don't need the shader anymore.
            glDeleteShader(stageCI.shaderHandle);

            // Use the infoLog as you see fit.
            throw std::runtime_error("failed to specialize shader");
        }

        glProgramParameteri(*pProgram, GL_PROGRAM_BINARY_RETRIEVABLE_HINT, false);
        glProgramParameteri(*pProgram, GL_PROGRAM_SEPARABLE, true);

        glAttachShader(*pProgram, stageCI.shaderHandle);
    }

    glLinkProgram(*pProgram);
#ifndef NDEBUG
    GLint success;
    glGetProgramiv(*pProgram, GL_LINK_STATUS, &success);

    GLint len;
    glGetProgramiv(*pProgram, GL_INFO_LOG_LENGTH, &len);
    std::string log;
    log.resize(static_cast<size_t>(len));
    glGetProgramInfoLog(*pProgram, len, NULL, &log[0]);
    GL_LOG(log);

    if (!success) {
        glDeleteProgram(*pProgram);
        throw std::runtime_error("failed to link program");
    }
#endif
}
void createProgram2(std::vector<PipelineShaderStageCreateInfo> const &stageCreateInfos, ProgramHandle *pProgram) {
    createProgram2(stageCreateInfos.size(),stageCreateInfos.data(), pProgram);
}
void createProgram2(size_t count, PipelineShaderStageCreateInfo const *createInfos, ProgramHandle *pProgram) {
    *pProgram = glCreateProgram();
    // for (auto &&stageCI : stageCreateInfos)
    for (size_t i = 0; i < count; ++i) {
        auto &&stageCI = createInfos[i];
        ;
        // glProgramParameteri(*pProgram, GL_PROGRAM_BINARY_RETRIEVABLE_HINT, false);
        glProgramParameteri(*pProgram, GL_PROGRAM_SEPARABLE, true);
        glAttachShader(*pProgram, stageCI.shaderHandle);
    }

    glLinkProgram(*pProgram);
#ifndef NDEBUG
    GLint success;
    glGetProgramiv(*pProgram, GL_LINK_STATUS, &success);

    GLint len;
    glGetProgramiv(*pProgram, GL_INFO_LOG_LENGTH, &len);
    std::string log;
    log.resize(static_cast<size_t>(len));
    glGetProgramInfoLog(*pProgram, len, NULL, &log[0]);
    GL_LOG(log);

    if (!success) {
        glDeleteProgram(*pProgram);
        throw std::runtime_error("failed to link program");
    }
#endif
}

void createGraphicsPipeline(const GraphicsPipelineCreateInfo &createInfo, PipelineHandle *pPipeline) {
    if (GLEW_VERSION_4_1) {
        glGenProgramPipelines(1, &pPipeline->pipeline);
    } else {
        throw std::runtime_error("failed to create pipelines");
    }
    glBindProgramPipeline(pPipeline->pipeline);
    GLbitfield stages{0};
    pPipeline->programs.emplace_back();
    if (createInfo.binaryShader)
        createProgram(createInfo.stages.size(), createInfo.stages.data(), &pPipeline->programs.back());
    else
        createProgram2(createInfo.stages, &pPipeline->programs.back());
    for (auto &stageCI : createInfo.stages) {
        stages |= Map(stageCI.stage);
    }
    glUseProgramStages(pPipeline->pipeline, stages, pPipeline->programs.back());
    createVertexArray(createInfo.vertexInputState, &pPipeline->vao);
    glBindProgramPipeline(0);
}
void createComputePipeline(ComputePipelineCreateInfo const &createInfo, PipelineHandle *pPipeline)
{
    if (GLEW_VERSION_4_1) {
        glGenProgramPipelines(1, &pPipeline->pipeline);
    } else {
        throw std::runtime_error("failed to create compute pipelines");
    }

    glBindProgramPipeline(pPipeline->pipeline);
    pPipeline->programs.emplace_back();
    if (createInfo.binaryShader)
        createProgram(1, &createInfo.stage, &pPipeline->programs.back());
    else
        createProgram2(1, &createInfo.stage, &pPipeline->programs.back());

    GLbitfield stages = Map(createInfo.stage.stage);
    glUseProgramStages(pPipeline->pipeline, stages, pPipeline->programs.back());
    glBindProgramPipeline(0);
}
void createBuffer(const BufferCreateInfo &createInfo, const void *pData, BufferHandle *pBuffer) {
    glGenBuffers(1, pBuffer);
    // target do not matter when creating buffer
    glBindBuffer(GL_ARRAY_BUFFER, *pBuffer);
    if (createInfo.flags & BUFFER_CREATE_MUTABLE_FORMAT_BIT)
        glBufferData(GL_ARRAY_BUFFER, createInfo.size, pData, Map(createInfo.storageUsage));
    else
        //(GLVersion.major * 10 + GLVersion.minor < 44 || GL_ARB_buffer_storage)
        glBufferStorage(GL_ARRAY_BUFFER, createInfo.size, pData, createInfo.storageFlags);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}
// void createVertexArray(const VertexInputStateCreateInfo &vertexInputStateCreateInfo,
//					   const std::vector<BufferHandle> &vertexBuffers,
//					   const BufferHandle indexBuffer,
//					   VertexArrayHandle *pVertexArray)
//{
//	glGenVertexArrays(1, pVertexArray);
//	if (*pVertexArray == 0)
//		throw std::runtime_error("failed to create vertex array object");
//	glBindVertexArray(*pVertexArray);

//	for (auto &attrib : vertexInputStateCreateInfo.vertexAttributeDescriptions)
//	{
//		glBindBuffer(GL_ARRAY_BUFFER, vertexBuffers[attrib.binding]);
//		glEnableVertexAttribArray(attrib.location);
//		glVertexAttribPointer(
//			attrib.location, attrib.components,
//			Map(attrib.dataType), attrib.normalized,
//			vertexInputStateCreateInfo.vertexBindingDescriptions[attrib.binding].stride,
//			(void *)attrib.offset);
//		glVertexAttribDivisor(attrib.location,
//vertexInputStateCreateInfo.vertexBindingDescriptions[attrib.binding].divisor);
//	}
//	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
//	glBindVertexArray(0);
//}
void createVertexArray(const VertexInputStateCreateInfo &vertexInputStateCreateInfo, VertexArrayHandle *vao) {
    glGenVertexArrays(1, vao);
    if (vao == 0) {
        THROW("failed to create vertex array object")
    }
    glBindVertexArray(*vao);
    for (auto &&attrib : vertexInputStateCreateInfo.vertexAttributeDescriptions) {
        glVertexAttribFormat(attrib.location, attrib.components, Map(attrib.dataType), attrib.normalized,
                             attrib.offset);
        glVertexAttribBinding(attrib.location, attrib.binding);
        // glVertexBindingDivisor(attrib.location,
        // vertexInputStateCreateInfo.vertexBindingDescriptions[attrib.binding].divisor);
        glVertexBindingDivisor(attrib.binding,
                               vertexInputStateCreateInfo.vertexBindingDescriptions[attrib.binding].divisor);
        glEnableVertexAttribArray(attrib.location);
    }
}

void createMemory(MemoryHandle &memory) {
    /*
    server memory, cannot be accessed directly
*/
    glCreateMemoryObjectsEXT(1, &memory);

    int dedicatedMemory{};  // external memory
    glMemoryObjectParameterivEXT(memory, GL_DEDICATED_MEMORY_OBJECT_EXT, &dedicatedMemory);
    int protectedMemory{};  //
    glMemoryObjectParameterivEXT(memory, GL_PROTECTED_MEMORY_OBJECT_EXT, &protectedMemory);
}
SemaphoreHandle createSemaphore() {
    SemaphoreHandle semaphore;
    glGenSemaphoresEXT(1, &semaphore);
    return semaphore;
}

void createImage(const ImageCreateInfo &createInfo, ImageHandle *pImage) {
    uint32_t mipmapLevels = createInfo.mipLevels;
    if (mipmapLevels == 0)
        mipmapLevels = static_cast<uint32_t>(
            std::floor(std::log2(std::max(createInfo.extent.width, createInfo.extent.height))) + 1);

    if (createInfo.flags & IMAGE_CREATE_RENDER_BUFFER_BIT) {
        glGenRenderbuffers(1, pImage);
        glBindRenderbuffer(GL_RENDERBUFFER, *pImage);
        if (createInfo.samples > SAMPLE_COUNT_1_BIT) {
            glRenderbufferStorageMultisample(GL_RENDERBUFFER, createInfo.samples, createInfo.format,
                                             createInfo.extent.width, createInfo.extent.height);
        } else {
            glRenderbufferStorage(GL_RENDERBUFFER, createInfo.format, createInfo.extent.width,
                                  createInfo.extent.height);
        }
        glBindRenderbuffer(GL_RENDERBUFFER, 0);
    } else {
        glGenTextures(1, pImage);
        GLenum target = Map(createInfo.imageType, createInfo.samples > SAMPLE_COUNT_1_BIT);
        glBindTexture(target, *pImage);
        if (createInfo.flags & IMAGE_CREATE_MUTABLE_FORMAT_BIT) {
            if (createInfo.samples > SAMPLE_COUNT_1_BIT) {
                glTexImage3DMultisample(target, createInfo.samples, createInfo.format, createInfo.extent.width,
                                        createInfo.extent.height, createInfo.extent.depth, GL_FALSE);
            } else {
                switch (createInfo.imageType) {
                    case IMAGE_TYPE_1D:
                        glTexImage2D(target, 0, createInfo.format, createInfo.extent.width, createInfo.extent.height, 0,
                                     GL_BGRA, GL_UNSIGNED_BYTE, nullptr);
                        break;
                    case IMAGE_TYPE_2D:
                    case IMAGE_TYPE_3D:
                        glTexImage3D(target, 0, createInfo.format, createInfo.extent.width, createInfo.extent.height,
                                     createInfo.extent.depth, 0, GL_BGRA, GL_UNSIGNED_BYTE, nullptr);
                        break;
                }
            }
        } else {
            if (createInfo.samples > SAMPLE_COUNT_1_BIT) {
                glTexStorage3DMultisample(target, createInfo.samples, createInfo.format, createInfo.extent.width,
                                          createInfo.extent.height, createInfo.extent.depth, GL_FALSE);
            } else {
                switch (createInfo.imageType) {
                    case IMAGE_TYPE_1D:
                        glTexStorage2D(target, mipmapLevels, createInfo.format, createInfo.extent.width,
                                       createInfo.extent.height);
                        break;
                    case IMAGE_TYPE_2D:
                    case IMAGE_TYPE_3D:
                        glTexStorage3D(target, mipmapLevels, createInfo.format, createInfo.extent.width,
                                       createInfo.extent.height, createInfo.extent.depth);
                        break;
                }
            }
        }
        glBindTexture(target, 0);
    }
}

void createSampler(const SamplerCreateInfo &createInfo, SamplerHandle *pSampler) {
    glGenSamplers(1, pSampler);
    glSamplerParameteri(*pSampler, GL_TEXTURE_MAG_FILTER, Map(createInfo.magFilter));
    if (createInfo.mipmapMode == SAMPLER_MIPMAP_MODE_LINEAR) {
        if (createInfo.minFilter == FILTER_LINEAR)
            glSamplerParameteri(*pSampler, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        else
            glSamplerParameteri(*pSampler, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
    } else {
        if (createInfo.minFilter == FILTER_LINEAR)
            glSamplerParameteri(*pSampler, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
        else
            glSamplerParameteri(*pSampler, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
    }
    glSamplerParameterf(*pSampler, GL_TEXTURE_LOD_BIAS, createInfo.mipLodBias);
    glSamplerParameterf(*pSampler, GL_TEXTURE_MIN_LOD, createInfo.minLod);
    glSamplerParameterf(*pSampler, GL_TEXTURE_MAX_LOD, createInfo.maxLod);
    glSamplerParameteri(*pSampler, GL_TEXTURE_WRAP_S, Map(createInfo.wrapModeU));
    glSamplerParameteri(*pSampler, GL_TEXTURE_WRAP_T, Map(createInfo.wrapModeV));
    glSamplerParameteri(*pSampler, GL_TEXTURE_WRAP_R, Map(createInfo.wrapModeW));
    if (createInfo.compareEnable) {
        glSamplerParameteri(*pSampler, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
        glSamplerParameteri(*pSampler, GL_TEXTURE_COMPARE_FUNC, Map(createInfo.compareOp));
    }
    if (createInfo.borderColor.dataType == DATA_TYPE_INT)
        glSamplerParameterIiv(*pSampler, GL_TEXTURE_BORDER_COLOR, createInfo.borderColor.color.int32);
    else if (createInfo.borderColor.dataType == DATA_TYPE_UNSIGNED_INT)
        glSamplerParameterIuiv(*pSampler, GL_TEXTURE_BORDER_COLOR, createInfo.borderColor.color.uint32);
    else if (createInfo.borderColor.dataType == DATA_TYPE_FLOAT)
        glSamplerParameterfv(*pSampler, GL_TEXTURE_BORDER_COLOR, createInfo.borderColor.color.float32);
}

void createImageView(const ImageViewCreateInfo &createInfo, bool multisample, ImageViewHandle *pImageViewHandle) {
    GLenum target = Map(createInfo.viewType, multisample);
    glGenTextures(1, pImageViewHandle);
    if (GLEW_VERSION_4_3) {
        glTextureView(*pImageViewHandle, target, createInfo.image, createInfo.format,
                      createInfo.subresourceRange.baseMipLevel, createInfo.subresourceRange.levelCount,
                      createInfo.subresourceRange.baseArrayLayer, createInfo.subresourceRange.layerCount);
    } else if (glewIsSupported("GL_EXT_texture_view")) {
        glTextureViewEXT(*pImageViewHandle, target, createInfo.image, createInfo.format,
                         createInfo.subresourceRange.baseMipLevel, createInfo.subresourceRange.levelCount,
                         createInfo.subresourceRange.baseArrayLayer, createInfo.subresourceRange.layerCount);
    } else {
        THROW("texture view not supported");
    }
    glBindTexture(target, *pImageViewHandle);
    if (createInfo.subresourceRange.aspectMask & IMAGE_ASPECT_DEPTH_BIT)
        glTexParameteri(target, GL_DEPTH_STENCIL_TEXTURE_MODE, GL_DEPTH_COMPONENT);
    else if (createInfo.subresourceRange.aspectMask & IMAGE_ASPECT_STENCIL_BIT)
        glTexParameteri(target, GL_DEPTH_STENCIL_TEXTURE_MODE, GL_STENCIL_COMPONENTS);

    GLint swizzles[4]{
        GL_RED,
        GL_GREEN,
        GL_BLUE,
        GL_ALPHA,
    };
    if (createInfo.components.r != COMPONENT_SWIZZLE_IDENTITY) swizzles[0] = Map(createInfo.components.r);
    if (createInfo.components.g != COMPONENT_SWIZZLE_IDENTITY) swizzles[1] = Map(createInfo.components.g);
    if (createInfo.components.b != COMPONENT_SWIZZLE_IDENTITY) swizzles[2] = Map(createInfo.components.b);
    if (createInfo.components.a != COMPONENT_SWIZZLE_IDENTITY) swizzles[3] = Map(createInfo.components.a);
    glTexParameteriv(target, GL_TEXTURE_SWIZZLE_RGBA, swizzles);
    glBindTexture(target, 0);
}

void updateImageSubData(ImageHandle image, ImageType imageType, const ImageSubData &imageSubData) {
    GLenum target = Map(imageType, false);
    glBindTexture(target, image);
    switch (imageType) {
        case IMAGE_TYPE_1D:
            glTexSubImage2D(target, imageSubData.mipLevel, imageSubData.rect.offset.x, imageSubData.rect.offset.y,
                            imageSubData.rect.extent.width, imageSubData.rect.extent.height, imageSubData.format,
                            Map(imageSubData.dataType), imageSubData.data);
            break;
        case IMAGE_TYPE_2D:
        case IMAGE_TYPE_3D:
            glTexSubImage3D(target, imageSubData.mipLevel, imageSubData.rect.offset.x, imageSubData.rect.offset.y,
                            imageSubData.rect.offset.z, imageSubData.rect.extent.width, imageSubData.rect.extent.height,
                            imageSubData.rect.extent.depth, imageSubData.format, Map(imageSubData.dataType),
                            imageSubData.data);
            break;
    }
    glBindTexture(target, 0);
};
// void updateImageSubData2(ImageHandle image, ImageType imageType, const ImageSubData &imageSubData)
//{
//	GLenum target = Map(imageType, false);
//	glBindTexture(target, image);

//	glBindTexture(target, 0);
//};

void setImageSampler(const SamplerCreateInfo &createInfo, ImageViewHandle image, ImageViewType imageViewType,
                     bool multisample) {
    if (multisample == true) {
        GL_LOG("multisample texture do not support sampler parameter");
        return;
    }
    GLenum target = Map(imageViewType, false);
    glBindTexture(target, image);

    glTexParameteri(target, GL_TEXTURE_MAG_FILTER, Map(createInfo.magFilter));
    if (createInfo.mipmapMode == SAMPLER_MIPMAP_MODE_LINEAR) {
        if (createInfo.minFilter == FILTER_LINEAR)
            glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        else
            glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
    } else {
        if (createInfo.minFilter == FILTER_LINEAR)
            glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
        else
            glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
    }
    glTexParameterf(target, GL_TEXTURE_LOD_BIAS, createInfo.mipLodBias);
    glTexParameterf(target, GL_TEXTURE_MIN_LOD, createInfo.minLod);
    glTexParameterf(target, GL_TEXTURE_MAX_LOD, createInfo.maxLod);
    glTexParameteri(target, GL_TEXTURE_WRAP_S, Map(createInfo.wrapModeU));
    glTexParameteri(target, GL_TEXTURE_WRAP_T, Map(createInfo.wrapModeV));
    glTexParameteri(target, GL_TEXTURE_WRAP_R, Map(createInfo.wrapModeW));
    if (createInfo.compareEnable) {
        glTexParameteri(target, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
        glTexParameteri(target, GL_TEXTURE_COMPARE_FUNC, Map(createInfo.compareOp));
    }
    if (createInfo.borderColor.dataType == DATA_TYPE_INT)
        glTexParameterIiv(target, GL_TEXTURE_BORDER_COLOR, createInfo.borderColor.color.int32);
    else if (createInfo.borderColor.dataType == DATA_TYPE_UNSIGNED_INT)
        glTexParameterIuiv(target, GL_TEXTURE_BORDER_COLOR, createInfo.borderColor.color.uint32);
    else if (createInfo.borderColor.dataType == DATA_TYPE_FLOAT)
        glTexParameterfv(target, GL_TEXTURE_BORDER_COLOR, createInfo.borderColor.color.float32);
    glBindTexture(target, 0);
}

std::vector<GLint> getSupportedShaderBinaryFormat() {
    GLint count;
    glGetIntegerv(GL_NUM_SHADER_BINARY_FORMATS, &count);
    std::vector<GLint> formats(count);
    glGetIntegerv(GL_SHADER_BINARY_FORMATS, formats.data());
    return formats;
}
std::vector<GLint> getSupportedProgramBinaryFormat() {
    GLint count;
    glGetIntegerv(GL_NUM_PROGRAM_BINARY_FORMATS, &count);
    std::vector<GLint> formats(count);
    glGetIntegerv(GL_PROGRAM_BINARY_FORMATS, formats.data());
    return formats;
}

bool isSupportShaderBinaryFormat(ShaderBinaryFormat format) {
    auto shaderBinaryFormats = getSupportedShaderBinaryFormat();
    for (auto &shaderBinaryFormat : shaderBinaryFormats) {
        GL_LOG("shaderBinaryFormat ", shaderBinaryFormat);
        if (static_cast<GLenum>(shaderBinaryFormat) == Map(format)) return true;
    }
    return false;
}

ImageTiling findSupportedTilingType(const std::vector<ImageTiling> &candidateTilings, ImageType imageType,
                                    bool multisample, GLenum format) {
    GLenum target = Map(imageType, multisample);

    int NUM_TILING_TYPES_EXT;
    glGetInternalformativ(target, format, GL_NUM_TILING_TYPES_EXT, 1, &NUM_TILING_TYPES_EXT);
    GL_LOG("NUM_TILING_TYPES_EXT ", NUM_TILING_TYPES_EXT);

    std::vector<int> TILING_TYPES_EXT(NUM_TILING_TYPES_EXT);
    glGetInternalformativ(target, format, GL_TILING_TYPES_EXT, NUM_TILING_TYPES_EXT, TILING_TYPES_EXT.data());
    for (auto canditiling : candidateTilings) {
        for (GLenum tiling : TILING_TYPES_EXT) {
            if (Map(canditiling) == tiling) {
                GL_LOG("support tiling type is:", tiling);
                return canditiling;
            }
        }
    }
    throw std::runtime_error("failed to find supported tiling type");
}
void Draw(PrimitiveTopology topology, DrawIndirectCommand const &indirectCmd) {
    // glDrawArrays(Map(topology), indirectCmd.firstVertex, indirectCmd.vertexCount);
    // glDrawArraysInstanced(
    //	Map(topology),
    //	indirectCmd.firstVertex,
    //	indirectCmd.vertexCount,
    //	indirectCmd.instanceCount);
    glDrawArraysInstancedBaseInstance(Map(topology), indirectCmd.firstVertex, indirectCmd.vertexCount,
                                      indirectCmd.instanceCount, indirectCmd.firstInstance);
}
void DrawIndexed(PrimitiveTopology topology, DataType indexType, DrawIndexedIndirectCommand const &indexedIndirectCmd) {
    // glDrawElements(
    //	Map(topology),
    //	indexedIndirectCmd.indexCount,
    //	Map(indexType),
    //);
    auto offset = indexedIndirectCmd.firstIndex * getDataTypeSize(indexType);
    // glDrawElementsInstanced(
    //	Map(topology),
    //	indexedIndirectCmd.indexCount,
    //	Map(indexType),
    //	(void *)(GLintptr)(offset),
    //	indexedIndirectCmd.instanceCount);
    glDrawElementsInstancedBaseVertexBaseInstance(Map(topology), indexedIndirectCmd.indexCount, Map(indexType),
                                                  (void *)(GLintptr)(offset), indexedIndirectCmd.instanceCount,
                                                  indexedIndirectCmd.vertexOffset, indexedIndirectCmd.firstInstance);
}
void Clear(ImageAspectFlagBits imageAspect) { glClear(Map(imageAspect)); }

void createDescriptorSetLayout(const DescriptorSetLayoutCreateInfo &createInfo, DescriptorSetLayout &outSetLayout) {
    outSetLayout = new DescriptorSetLayout_T{createInfo};
}
void destroyDescriptorSetLayout(DescriptorSetLayout &setLayout) { delete setLayout; }

void createDescriptorSet(const std::vector<DescriptorSetLayout> &descriptorSetLayouts, DescriptorSet &descriptorSet) {
    descriptorSet = new DescriptorSet_T{descriptorSetLayouts};
}

void destroyDescriptorSet(DescriptorSet descriptorSet) { delete descriptorSet; }
void updateDescriptorSets(const std::vector<WriteDescriptorSet> &descriptorWrites,
                          const std::vector<CopyDescriptorSet> &descriptorCopy) {
    for (auto &writeSet : descriptorWrites) {
        if (writeSet.descriptorType == DESCRIPTOR_TYPE_UNIFORM_BUFFER ||
            writeSet.descriptorType == DESCRIPTOR_TYPE_STORAGE_BUFFER) {
            for (auto &buffer : writeSet.dstSet->_buffers) {
                if (buffer.binding.binding == writeSet.dstBinding) {
                    std::copy(writeSet.buffersInfo.begin(), writeSet.buffersInfo.end(),
                              buffer.buffersInfo.begin() + writeSet.dstArrayElement);
                    break;
                }
            }
        } else if (writeSet.descriptorType == DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER ||
                   writeSet.descriptorType == DESCRIPTOR_TYPE_SAMPLED_IMAGE ||
                   writeSet.descriptorType == DESCRIPTOR_TYPE_STORAGE_IMAGE ||
                   writeSet.descriptorType == DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER ||
                   writeSet.descriptorType == DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER) {
            for (auto &image : writeSet.dstSet->_images) {
                if (image.binding.binding == writeSet.dstBinding) {
                    for (int i = 0, len = writeSet.imagesInfo.size(); i < len; ++i) {
                        setImageSampler(writeSet.imagesInfo[i].samplerCreateInfo, writeSet.imagesInfo[i].imageView,
                                        writeSet.viewType, writeSet.multisample);
                        image.imageViews[i + writeSet.dstArrayElement] = writeSet.imagesInfo[i].imageView;
                        image.imageViewType = writeSet.viewType;
                        image.multisample = writeSet.multisample;
                    }
                    break;
                }
            }
        }
    }
    // for (auto &copySet : descriptorCopy)
    //{
    // }
}

}  // namespace GL
