//
// Created by Timothy on 8/12/2024.
//

#include "Window.h"

#include "Debug.h"
#include "GLFW/glfw3.h"
namespace Tephryte {
    Window::Window(uint32_t width, uint32_t height, const char* title) {
        glfwInit();
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, true);
        windowHandle = glfwCreateWindow(width, height, title, nullptr, nullptr);
    }

    Window::~Window() {
        glfwTerminate();
    }

    std::vector<const char*> Window::getRequiredVulkanInstanceExtensions() {
        uint32_t ext_count;
        const char** ext = glfwGetRequiredInstanceExtensions(&ext_count);
        std::vector<const char*> ext_vec = { };

        for (int i = 0; i < ext_count; ++i) {
            ext_vec.push_back(ext[i]);
        }

        return ext_vec;

    }

    VkSurfaceKHR Window::getSurface(VkInstance instance) {
        VkSurfaceKHR surface;
        VkResult err = glfwCreateWindowSurface(instance, windowHandle, nullptr, &surface);
        CHECK_VK_RESULT( err );

        return surface;
    }
}
