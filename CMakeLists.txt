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
        Tephryte/src/Debug.h)




add_executable(Tephryte
        ${IMGUI_SRC}
        ${SRC_DIR}/main.cpp
        ${SRC_DIR}/Log.h
        ${SRC_DIR}/VkBackend.cpp
        ${SRC_DIR}/VkBackend.h
        ${SRC_DIR}/Application.cpp
        ${SRC_DIR}/Application.h)

target_link_libraries(${PROJECT_NAME} glfw Vulkan::Vulkan)
target_compile_definitions(${PROJECT_NAME} PUBLIC -DImTextureID=ImU64)