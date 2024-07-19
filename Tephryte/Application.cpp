//
// Created by timha on 7/9/2024.
//

#include <cstdio>
#include <cstdlib>
#include "Application.h"
#include "GLFW/glfw3.h"
namespace Tephryte {
    Application::Application() {

        // Create Window
        glfwInit();
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, true);

        window = glfwCreateWindow(800, 600, "Tephryte", nullptr, nullptr);
        if (window == nullptr) {
            fprintf(stderr, "FAILED TO CREATE WINDOW");
            glfwTerminate();
            exit(1);
        }
    }

    void Application::run() {
        while (true) {

        }
    }

    Application::~Application() {

    }
}
