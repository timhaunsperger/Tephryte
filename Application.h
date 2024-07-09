//
// Created by timha on 7/9/2024.
//

#ifndef TEPHRYTE_APPLICATION_H
#define TEPHRYTE_APPLICATION_H

#include <string>
#include "GLFW/glfw3.h"

namespace Tephryte {

    struct AppOptions {
        std::string Name;
    };

    class Application {
        GLFWwindow* window;
    public:
        Application();

        void run();

        virtual ~Application();

        Application *CreateApplication();

    };

}
#endif //TEPHRYTE_APPLICATION_H
