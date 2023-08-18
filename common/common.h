#pragma once
#include <chrono>
#include <cstdlib>
#include <exception>
#include <fstream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/hash.hpp>
#include <iostream>
#include <sstream>
#include <thread>
#include <vector>

#include "prerequisites.h"
#include "types.h"

#define SHADER_SPV_PATH SHADER_DIR "/spv/"

std::string shaderPath(const char *shaderFilename);
std::string shaderSpvPath(const char *shaderFilename);

struct Vertex {
    glm::vec3 pos;
    glm::vec3 normal;  // or color
    glm::vec2 texCoord;
    glm::vec4 color;

    static GL::VertexBindingDescription getVertexBindingDescription() { return {sizeof(Vertex), 0}; }
    static std::vector<GL::VertexAttributeDescription> getVertexAttributeDescription() {
        return {
            {0, 0, 3, GL::DATA_TYPE_FLOAT, false, offsetof(Vertex, pos)},
            {1, 0, 3, GL::DATA_TYPE_FLOAT, false, offsetof(Vertex, normal)},
            {2, 0, 2, GL::DATA_TYPE_FLOAT, false, offsetof(Vertex, texCoord)},
            {3, 0, 2, GL::DATA_TYPE_FLOAT, false, offsetof(Vertex, color)},
        };
    }
};
inline bool operator==(const Vertex &lhv, const Vertex &rhv) {
    return lhv.pos == rhv.pos && lhv.normal == rhv.normal && lhv.texCoord == rhv.texCoord;
}

namespace std {
template <>
struct hash<Vertex> {
    std::size_t operator()(Vertex const &vertex) const noexcept {
        return hash<glm::vec3>{}(vertex.pos);

        // return ((((hash<glm::vec3>{}(vertex.pos) ^
        // 		   hash<glm::vec3>{}(vertex.normal) << 1) >>
        // 		  1) ^
        // 		 (hash<glm::vec2>{}(vertex.texCoord) << 1)) >>
        // 		1) ^
        // 	   (hash<glm::vec4>{}(vertex.color));
    }
};
}  // namespace std

struct InstanceAttribute {
    glm::mat4 translation;
    static GL::VertexBindingDescription getVertexBindingDescription() { return {sizeof(InstanceAttribute), 1}; }
    static std::vector<GL::VertexAttributeDescription> getVertexAttributeDescription(uint32_t startLocation,
                                                                                     uint32_t binding) {
        return {
            {startLocation + 0, binding, 4, GL::DATA_TYPE_FLOAT, false, 0},
            {startLocation + 1, binding, 4, GL::DATA_TYPE_FLOAT, false, 16},
            {startLocation + 2, binding, 4, GL::DATA_TYPE_FLOAT, false, 32},
            {startLocation + 3, binding, 4, GL::DATA_TYPE_FLOAT, false, 48},
        };
    }
    static uint32_t getLocationCount() { return 4; }
};

void listGLInfo();

std::string readFile(const std::string &filename);
std::string readFile(const char *filename);

// void screenshot(const Rect2D &rect);