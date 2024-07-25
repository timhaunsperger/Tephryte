//
// Created by timha on 7/9/2024.
//

#include <cstdio>
#include <cstdlib>
#include "Tephryte.h"
#include "GLFW/glfw3.h"
namespace Tephryte {
    Application::Application() {

        VkSettings vk_settings = VkSettings{
        .appName = "Tephryte Editor",
        .extensions = { }
        };

        vkBackend = new VkBackend(vk_settings);

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

    }

    Application::~Application() {
        delete vkBackend;
        glfwTerminate();
    }
}
