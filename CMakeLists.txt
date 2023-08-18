cmake_minimum_required(VERSION 3.16)

get_filename_component(FolderName ${CMAKE_SOURCE_DIR} NAME)

set(ProjectName ${FolderName})
project(${ProjectName})

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED on)
set(CMAKE_CXX_EXTENSIONS off)

set(MYOUTPUT_DIR ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_DEBUG_POSTFIX d)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${MYOUTPUT_DIR})
# set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${MYOUTPUT_DIR})
# set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${MYOUTPUT_DIR})

configure_file(config.h.in config.h)

add_subdirectory(${CMAKE_SOURCE_DIR}/3rdparty/glew/build/cmake)
add_subdirectory(${CMAKE_SOURCE_DIR}/3rdparty/glfw)

find_package(OpenMP REQUIRED)

#============imgui lib==========
file(GLOB imguifile
imgui/*.cpp
)
add_library(imgui STATIC ${imguifile})
target_include_directories(imgui PRIVATE
$ENV{WORKSPACE}/3rdparty/glfw/include
)

#============common lib==========
file(GLOB commonfile
common/*.cpp 
)

add_library(common STATIC ${commonfile})
target_include_directories(common PUBLIC
${CMAKE_SOURCE_DIR}
${CMAKE_SOURCE_DIR}/common
${CMAKE_BINARY_DIR}
${CMAKE_SOURCE_DIR}/3rdparty/glm
${CMAKE_SOURCE_DIR}/3rdparty/stb
${CMAKE_SOURCE_DIR}/3rdparty/glew/include
${CMAKE_SOURCE_DIR}/3rdparty/glfw/include
${CMAKE_SOURCE_DIR}/3rdparty/tinyobjloader
imgui
opengl32.lib
)

#==========example template=======
function(newexample examplename)
message(${examplename})
add_executable(${examplename} examples/${examplename}.cpp)

target_include_directories(${examplename} PRIVATE 
${CMAKE_SOURCE_DIR}
${CMAKE_SOURCE_DIR}/common
${CMAKE_BINARY_DIR}
${CMAKE_SOURCE_DIR}/3rdparty/glm
${CMAKE_SOURCE_DIR}/3rdparty/stb
${CMAKE_SOURCE_DIR}/3rdparty/glew/include
${CMAKE_SOURCE_DIR}/3rdparty/glfw/include
${CMAKE_SOURCE_DIR}/3rdparty/tinyobjloader
imgui
)

if(MSVC)
target_link_libraries(${examplename} PUBLIC
glew
glfw
opengl32.lib
common
imgui
)
endif()

add_dependencies(${examplename} common)

endfunction()
newexample(test)

#===========install =======================
