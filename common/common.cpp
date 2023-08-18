#include "common.h"

#include <unordered_map>

std::string shaderPath(const char *shaderFilename) { return std::string(SHADER_DIR) + "/" + shaderFilename; }
std::string shaderSpvPath(const char *shaderFilename) {
    return std::string(SHADER_SPV_PATH) + "/" + shaderFilename + ".spv";
}

void listGLInfo() {
    // if (glewIsSupported("GL_EXT_memory_object"))
    //{
    //	int NUM_DEVICE_UUIDS_EXT;
    //	glGetIntegerv(GL_NUM_DEVICE_UUIDS_EXT, &NUM_DEVICE_UUIDS_EXT);
    //	LOG_VAR(NUM_DEVICE_UUIDS_EXT);
    //	GLint64 DEVICE_UUID_EXT{};
    //	for (int i = 0; i < NUM_DEVICE_UUIDS_EXT; ++i)
    //	{
    //		LOG(i);
    //		glGetInteger64i_v(GL_DEVICE_UUID_EXT, i, &DEVICE_UUID_EXT);
    //		LOG_VAR(DEVICE_UUID_EXT);
    //	}
    //	GLint64 DRIVER_UUID_EXT;
    //	glGetInteger64v(GL_DRIVER_UUID_EXT, &DRIVER_UUID_EXT);
    //	LOG_VAR(DRIVER_UUID_EXT);
    // }

    // check opengl informations
    LOG_VAR(glGetString(GL_VENDOR));
    LOG_VAR(glGetString(GL_RENDERER));
    LOG_VAR(glGetString(GL_VERSION));
    LOG_VAR(glGetString(GL_SHADING_LANGUAGE_VERSION));

    // int numShadingLanguageVersions{};
    // glGetIntegerv(GL_NUM_SHADING_LANGUAGE_VERSIONS,
    // &numShadingLanguageVersions); LOG_VAR(numShadingLanguageVersions); for (int
    // i = 0; i < numShadingLanguageVersions; ++i)
    //	LOG_VAR(glGetStringi(GL_SHADING_LANGUAGE_VERSION, i));

    // int samples;
    // glGetIntegerv(GL_SAMPLES, &samples);
    // LOG_VAR(samples);

    // int maxFramebufferWidth{}, maxFramebufferHeight{};
    // glGetIntegerv(GL_MAX_FRAMEBUFFER_WIDTH, &maxFramebufferWidth);
    // glGetIntegerv(GL_MAX_FRAMEBUFFER_HEIGHT, &maxFramebufferHeight);
    // LOG_VAR(maxFramebufferWidth);
    // LOG_VAR(maxFramebufferHeight);

    if (GLEW_VERSION_4_3) {
        // framebuffer parameter
        GLenum list0[]{//		GL_FRAMEBUFFER_DEFAULT_WIDTH,
                       //		GL_FRAMEBUFFER_DEFAULT_HEIGHT,
                       //		GL_FRAMEBUFFER_DEFAULT_LAYERS,
                       //		GL_FRAMEBUFFER_DEFAULT_SAMPLES,
                       //		GL_FRAMEBUFFER_DEFAULT_FIXED_SAMPLE_LOCATIONS,

                       GL_DOUBLEBUFFER,
                       GL_IMPLEMENTATION_COLOR_READ_FORMAT,
                       GL_IMPLEMENTATION_COLOR_READ_TYPE,
                       GL_SAMPLES,
                       GL_SAMPLE_BUFFERS,
                       GL_STEREO};
        const char *list1[]{"GL_DOUBLEBUFFER",
                            "GL_IMPLEMENTATION_COLOR_READ_FORMAT",
                            "GL_IMPLEMENTATION_COLOR_READ_TYPE",
                            "GL_SAMPLES",
                            "GL_SAMPLE_BUFFERS",
                            "GL_STEREO"};

        // default framebuffer
        for (int i = 0, len = sizeof list0 / sizeof list0[0]; i < len; ++i) {
            GLint a{};
            glGetFramebufferParameteriv(GL_FRAMEBUFFER, list0[i], &a);
            LOG(list1[i], " ", a);
        }
    }

    // GLenum list2[]{GL_FRONT_LEFT, GL_FRONT_RIGHT, GL_BACK_LEFT, GL_BACK_RIGHT, GL_DEPTH, GL_STENCIL};
    // const char *list3[]{"GL_FRONT_LEFT", "GL_FRONT_RIGHT", "GL_BACK_LEFT", "GL_BACK_RIGHT", "GL_DEPTH", "GL_STENCIL"};
    // GLenum list4[]{GL_FRAMEBUFFER_ATTACHMENT_RED_SIZE,       GL_FRAMEBUFFER_ATTACHMENT_GREEN_SIZE,
    //                GL_FRAMEBUFFER_ATTACHMENT_BLUE_SIZE,      GL_FRAMEBUFFER_ATTACHMENT_ALPHA_SIZE,
    //                GL_FRAMEBUFFER_ATTACHMENT_DEPTH_SIZE,     GL_FRAMEBUFFER_ATTACHMENT_STENCIL_SIZE,
    //                GL_FRAMEBUFFER_ATTACHMENT_COMPONENT_TYPE, GL_FRAMEBUFFER_ATTACHMENT_COLOR_ENCODING};
    // const char *list5[]{"GL_FRAMEBUFFER_ATTACHMENT_RED_SIZE",       "GL_FRAMEBUFFER_ATTACHMENT_GREEN_SIZE",
    //                     "GL_FRAMEBUFFER_ATTACHMENT_BLUE_SIZE",      "GL_FRAMEBUFFER_ATTACHMENT_ALPHA_SIZE",
    //                     "GL_FRAMEBUFFER_ATTACHMENT_DEPTH_SIZE",     "GL_FRAMEBUFFER_ATTACHMENT_STENCIL_SIZE",
    //                     "GL_FRAMEBUFFER_ATTACHMENT_COMPONENT_TYPE", "GL_FRAMEBUFFER_ATTACHMENT_COLOR_ENCODING"};
    // for (int i = 0; i < 6; ++i) {
    //     GLint a{};
    //     glGetFramebufferAttachmentParameteriv(GL_FRAMEBUFFER, list2[i], GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE, &a);
    //     LOG(list3[i], " ", a);
    //     if (a != GL_NONE) {
    //         for (int j = 0, len = sizeof list4 / sizeof list4[0]; j < len; ++j) {
    //             glGetFramebufferAttachmentParameteriv(GL_FRAMEBUFFER, list2[i], list4[j], &a);
    //             LOG(list5[j], " ", a);
    //         }
    //     }
    // }

#if 0
	// opengl extensions
	GLint aa;
	glGetIntegerv(GL_NUM_EXTENSIONS, &aa);
	for (GLint i = 0; i < aa; ++i)
		LOG(glGetStringi(GL_EXTENSIONS, static_cast<GLuint>(i)));
#endif

    constexpr GLenum glCapabilityEnum[] = {
        GL_BLEND,
        GL_COLOR_LOGIC_OP,
        GL_CULL_FACE,
        GL_DEPTH_CLAMP,
        GL_DEBUG_OUTPUT,
        GL_DEBUG_OUTPUT_SYNCHRONOUS,
        GL_DEPTH_TEST,
        GL_DITHER,
        GL_FRAMEBUFFER_SRGB,
        GL_LINE_SMOOTH,
        GL_MULTISAMPLE,
        GL_POLYGON_SMOOTH,
        GL_POLYGON_OFFSET_FILL,
        GL_POLYGON_OFFSET_LINE,
        GL_POLYGON_OFFSET_POINT,
        GL_PROGRAM_POINT_SIZE,
        GL_PRIMITIVE_RESTART,
        GL_SAMPLE_ALPHA_TO_COVERAGE,
        GL_SAMPLE_ALPHA_TO_ONE,
        GL_SAMPLE_COVERAGE,
        GL_SAMPLE_MASK,
        GL_SCISSOR_TEST,
        GL_STENCIL_TEST,
        GL_TEXTURE_CUBE_MAP_SEAMLESS,
    };
    const char *glCapabilityEnumNames[]{
        "GL_BLEND",
        "GL_COLOR_LOGIC_OP",
        "GL_CULL_FACE",
        "GL_DEPTH_CLAMP",
        "GL_DEBUG_OUTPUT",
        "GL_DEBUG_OUTPUT_SYNCHRONOUS",
        "GL_DEPTH_TEST",
        "GL_DITHER",
        "GL_FRAMEBUFFER_SRGB",
        "GL_LINE_SMOOTH",
        "GL_MULTISAMPLE",
        "GL_POLYGON_SMOOTH",
        "GL_POLYGON_OFFSET_FILL",
        "GL_POLYGON_OFFSET_LINE",
        "GL_POLYGON_OFFSET_POINT",
        "GL_PROGRAM_POINT_SIZE",
        "GL_PRIMITIVE_RESTART",
        "GL_SAMPLE_ALPHA_TO_COVERAGE",
        "GL_SAMPLE_ALPHA_TO_ONE",
        "GL_SAMPLE_COVERAGE",
        "GL_SAMPLE_MASK",
        "GL_SCISSOR_TEST",
        "GL_STENCIL_TEST",
        "GL_TEXTURE_CUBE_MAP_SEAMLESS",
    };

    for (int i = 0, len = sizeof(glCapabilityEnum) / sizeof(glCapabilityEnum[0]); i < len; ++i) {
        LOG(glCapabilityEnumNames[i], bool(glIsEnabled(glCapabilityEnum[i])));
    }

    int a;
    glGetInternalformativ(GL_TEXTURE_2D_ARRAY, GL_BGR, GL_INTERNALFORMAT_SUPPORTED, 1, &a);
    LOG("GL_BGR supported: ", a);

    // constexpr GLenum aggregateLimits[]{
    //	GL_MIN_PROGRAM_TEXEL_OFFSET,
    //	GL_MAX_PROGRAM_TEXEL_OFFSET,
    //	GL_MAX_UNIFORM_BUFFER_BINDINGS,
    //	GL_MAX_UNIFORM_BLOCK_SIZE,
    //	GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT,
    //	GL_MAX_COMBINED_UNIFORM_BLOCKS,
    //	GL_MAX_VARYING_COMPONENTS,
    //	GL_MAX_VARYING_VECTORS,
    //	GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS,
    //	GL_MAX_SUBROUTINES,
    //	GL_MAX_SUBROUTINE_UNIFORM_LOCATIONS,
    //	GL_MAX_UNIFORM_LOCATIONS,
    //	GL_MAX_ATOMIC_COUNTER_BUFFER_BINDINGS,
    //	GL_MAX_ATOMIC_COUNTER_BUFFER_SIZE,
    //	GL_MAX_COMBINED_ATOMIC_COUNTER_BUFFERS,
    //	GL_MAX_COMBINED_ATOMIC_COUNTERS,
    //	GL_MAX_SHADER_STORAGE_BUFFER_BINDINGS,
    //	GL_MAX_SHADER_STORAGE_BLOCK_SIZE,
    //	GL_MAX_COMBINED_SHADER_STORAGE_BLOCKS,
    //	GL_SHADER_STORAGE_BUFFER_OFFSET_ALIGNMENT,
    //	GL_MAX_IMAGE_UNITS,
    //	GL_MAX_COMBINED_SHADER_OUTPUT_RESOURCES,
    //	GL_MAX_IMAGE_SAMPLES,
    //	GL_MAX_VERTEX_IMAGE_UNIFORMS,
    //	GL_MAX_TESS_CONTROL_IMAGE_UNIFORMS,
    //	GL_MAX_TESS_EVALUATION_IMAGE_UNIFORMS,
    //	GL_MAX_GEOMETRY_IMAGE_UNIFORMS,
    //	GL_MAX_FRAGMENT_IMAGE_UNIFORMS,
    //	GL_MAX_COMBINED_IMAGE_UNIFORMS,
    //	GL_MAX_COMBINED_VERTEX_UNIFORM_COMPONENTS,
    //	GL_MAX_COMBINED_GEOMETRY_UNIFORM_COMPONENTS,
    //	GL_MAX_COMBINED_TESS_CONTROL_UNIFORM_COMPONENTS,
    //	GL_MAX_COMBINED_TESS_EVALUATION_UNIFORM_COMPONENTS,
    //	GL_MAX_COMBINED_FRAGMENT_UNIFORM_COMPONENTS,
    // };
    // std::string aggregateLimitNames[]{
    //	"GL_MIN_PROGRAM_TEXEL_OFFSET",
    //	"GL_MAX_PROGRAM_TEXEL_OFFSET",
    //	"GL_MAX_UNIFORM_BUFFER_BINDINGS",
    //	"GL_MAX_UNIFORM_BLOCK_SIZE",
    //	"GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT",
    //	"GL_MAX_COMBINED_UNIFORM_BLOCKS",
    //	"GL_MAX_VARYING_COMPONENTS",
    //	"GL_MAX_VARYING_VECTORS",
    //	"GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS",
    //	"GL_MAX_SUBROUTINES",
    //	"GL_MAX_SUBROUTINE_UNIFORM_LOCATIONS",
    //	"GL_MAX_UNIFORM_LOCATIONS",
    //	"GL_MAX_ATOMIC_COUNTER_BUFFER_BINDINGS",
    //	"GL_MAX_ATOMIC_COUNTER_BUFFER_SIZE",
    //	"GL_MAX_COMBINED_ATOMIC_COUNTER_BUFFERS",
    //	"GL_MAX_COMBINED_ATOMIC_COUNTERS",
    //	"GL_MAX_SHADER_STORAGE_BUFFER_BINDINGS",
    //	"GL_MAX_SHADER_STORAGE_BLOCK_SIZE",
    //	"GL_MAX_COMBINED_SHADER_STORAGE_BLOCKS",
    //	"GL_SHADER_STORAGE_BUFFER_OFFSET_ALIGNMENT",
    //	"GL_MAX_IMAGE_UNITS",
    //	"GL_MAX_COMBINED_SHADER_OUTPUT_RESOURCES",
    //	"GL_MAX_IMAGE_SAMPLES",
    //	"GL_MAX_VERTEX_IMAGE_UNIFORMS",
    //	"GL_MAX_TESS_CONTROL_IMAGE_UNIFORMS",
    //	"GL_MAX_TESS_EVALUATION_IMAGE_UNIFORMS",
    //	"GL_MAX_GEOMETRY_IMAGE_UNIFORMS",
    //	"GL_MAX_FRAGMENT_IMAGE_UNIFORMS",
    //	"GL_MAX_COMBINED_IMAGE_UNIFORMS",
    //	"GL_MAX_COMBINED_VERTEX_UNIFORM_COMPONENTS",
    //	"GL_MAX_COMBINED_GEOMETRY_UNIFORM_COMPONENTS",
    //	"GL_MAX_COMBINED_TESS_CONTROL_UNIFORM_COMPONENTS",
    //	"GL_MAX_COMBINED_TESS_EVALUATION_UNIFORM_COMPONENTS",
    //	"GL_MAX_COMBINED_FRAGMENT_UNIFORM_COMPONENTS",
    // };
}

std::string readFile(const std::string &filename) { return readFile(filename.data()); }
std::string readFile(const char *filename) {
    std::fstream file(filename, std::ios::in | std::ios::ate | std::ios::binary);
    if (!file.is_open()) throw std::runtime_error("failed to open file");
    size_t size0 = static_cast<size_t>(file.tellg());
    std::string ret;
    ret.resize(size0);
    file.seekg(0);
    file.read(const_cast<char *>(ret.data()), size0);
    file.close();
    return std::move(ret);
}

// void createFramebufferamer(const FramebufferCreateInfo &createInfo,
// FramebufferHandle &framebuffer)
//{
//	glGenFramebuffers(1, &framebuffer);
//	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
//	const auto &attachmentDesc =
//*createInfo.renderpass->GetCreateInfoPtr()->spAttachments; 	for (int i = 0, len
//= attachmentDesc.size(); i < len; ++i)
//	{
//	}
// }
// void screenshot(const Rect2D &rect)
//{
//	GLuint width = (rect.extent.width - rect.offset.x);
//	GLuint height = (rect.extent.height - rect.offset.y);

//	GLuint pbo;
//	glGenBuffers(1, &pbo);
//	glBindBuffer(GL_PIXEL_PACK_BUFFER, pbo);
//	glBufferStorage(GL_PIXEL_PACK_BUFFER, 4 * width * height, nullptr,
// GL_MAP_READ_BIT | GL_MAP_COHERENT_BIT | GL_MAP_PERSISTENT_BIT);

//	glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
//	glReadBuffer(GL_FRONT_LEFT);
//	glReadPixels(rect.offset.x, rect.offset.y, rect.extent.width,
// rect.extent.height, GL_RGBA, GL_UNSIGNED_BYTE, 0);

//	glMemoryBarrier(GL_ALL_BARRIER_BITS);
//	void *data;
//	data = glMapBuffer(GL_PIXEL_PACK_BUFFER, GL_READ_ONLY);
//	if (!stbi_write_jpg("a.jpg", width, height, 4, data, 90))
//		LOG("failed to save image");
//	glUnmapBuffer(GL_PIXEL_PACK_BUFFER);
//}