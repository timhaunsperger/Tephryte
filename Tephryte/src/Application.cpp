//
// Created by timha on 7/9/2024.
//

#include "Tephryte.h"

#define VK_USE_PLATFORM_WIN32_KHR
#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"

namespace Tephryte {
    Application::Application() {

        GraphicsSettings vk_settings = GraphicsSettings{
        .appName = "Tephryte Editor",
        .extensions = {  }
        };

        glfwInit();
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
        GLFWwindow* window = glfwCreateWindow(1000, 1000, "a", nullptr, nullptr);
        vkBackend = new VkBackend(vk_settings, window);

        // // Create Window
        // glfwInit();
        // glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        // glfwWindowHint(GLFW_RESIZABLE, true);
        //
        // window = glfwCreateWindow(800, 600, "Tephryte", nullptr, nullptr);
        //
        // if (window == nullptr) {
        //     TPR_ENGINE_ERROR("Failed to create window");
        //     glfwTerminate();
        //     exit(1);
        // }
    }

    void Application::run() {
        while (true) {
            vkBackend->draw();
        }
    }

    Application::~Application() {
        delete vkBackend;
        glfwTerminate();
    }
}
