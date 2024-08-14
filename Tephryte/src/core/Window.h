//
// Created by Timothy on 8/12/2024.
//

#ifndef WINDOW_H
#define WINDOW_H

#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"
#include <vector>
#include "vulkan/vulkan.h"

namespace Tephryte {
    class Window {
    public:
        Window(uint32_t width, uint32_t height, const char* title);

        ~Window();

        friend class RenderEngine;

    private:
        std::vector<const char*> getRequiredVulkanInstanceExtensions();
        VkSurfaceKHR getSurface(VkInstance instance);
        GLFWwindow* windowHandle;
    };
}


#endif //WINDOW_H
