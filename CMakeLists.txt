cmake_minimum_required(VERSION 3.26)
project(Tephryte)
set(CMAKE_CXX_STANDARD 23)

find_package(Vulkan REQUIRED)

option(GLFW_BUILD_EXAMPLES "Build the GLFW example programs" OFF)
option(GLFW_BUILD_TESTS "Build the GLFW test programs" OFF)
option(GLFW_BUILD_DOCS "Build the GLFW documentation" OFF)
option(GLFW_INSTALL "Generate installation target" OFF)
option(GLFW_DOCUMENT_INTERNALS "Include internals in documentation" OFF)

set(LIB_DIR ${PROJECT_SOURCE_DIR}/Tephryte/vendor)
set(SRC_DIR ${PROJECT_SOURCE_DIR}/Tephryte/src)
set(SHADER_SRC_DIR ${SRC_DIR}/graphics/shaders/src)
set(SHADER_BIN_DIR ${SRC_DIR}/graphics/shaders/bin)
set(GLFW_DIR ${LIB_DIR}/glfw)
set(IMGUI_DIR ${LIB_DIR}/ImGui)
set(GLM_DIR ${LIB_DIR}/glm)

add_subdirectory(${GLFW_DIR})

include_directories(${GLFW_DIR}/include ${GLFW_DIR}/deps)
include_directories(${IMGUI_DIR} ${IMGUI_DIR}/backends)
include_directories(${GLM_DIR})


set(IMGUI_SRC
        ${IMGUI_DIR}/backends/imgui_impl_glfw.cpp
        ${IMGUI_DIR}/backends/imgui_impl_vulkan.cpp
        ${IMGUI_DIR}/imgui.cpp
        ${IMGUI_DIR}/imgui_draw.cpp
        ${IMGUI_DIR}/imgui_demo.cpp
        ${IMGUI_DIR}/imgui_tables.cpp
        ${IMGUI_DIR}/imgui_widgets.cpp
        Tephryte/src/Tephryte.h
        Tephryte/src)

add_executable(Tephryte
        ${IMGUI_SRC}
        ${SRC_DIR}/main.cpp
        ${SRC_DIR}/core/Log.h
        ${SRC_DIR}/core/Debug.h
        ${SRC_DIR}/VkBackend.cpp
        ${SRC_DIR}/VkBackend.h
        ${SRC_DIR}/Application.cpp
        ${SRC_DIR}/Application.h
        ${SRC_DIR}/core/VulkanImpl.cpp
        ${SRC_DIR}/core/VulkanImpl.h
        ${SRC_DIR}/core/VulkanRenderer.h
        ${SRC_DIR}/core/VulkanRenderer.cpp
        ${SRC_DIR}/core/TephryteCore.h)

target_link_libraries(${PROJECT_NAME} glfw Vulkan::Vulkan)
target_compile_definitions(${PROJECT_NAME} PUBLIC -DImTextureID=ImU64)


file(GLOB_RECURSE GLSL_SOURCE_FILES
        ${SHADER_SRC_DIR}/*.frag
        ${SHADER_SRC_DIR}/*.vert
        ${SHADER_SRC_DIR}/*.comp
        ${SHADER_SRC_DIR}/*.tese
        ${SHADER_SRC_DIR}/*.tesc
)

foreach (SHADER ${GLSL_SOURCE_FILES})
    get_filename_component(FILE_NAME ${SHADER} NAME)
    set(SPIRV_OUT "${SHADER_BIN_DIR}/${FILE_NAME}.spv")
    add_custom_command(
            OUTPUT ${SPIRV_OUT}
            COMMAND "$ENV{VULKAN_SDK}/Bin/glslangValidator.exe" -V ${SHADER} -o ${SPIRV_OUT}
            DEPENDS ${SHADER})
    list(APPEND SPIRV_BINARY_FILES ${SPIRV_OUT})
endforeach ()

add_custom_target(
        Shaders
        DEPENDS ${SPIRV_BINARY_FILES})
add_dependencies(Tephryte Shaders)