//
// Created by timha on 7/9/2024.
//

#ifndef TEPHRYTE_APPLICATION_H
#define TEPHRYTE_APPLICATION_H

#include <string>

#include "VkBackend.h"
#include "GLFW/glfw3.h"

namespace Tephryte {

    struct AppOptions {
        std::string Name;
        std::vector<const char*> vkExtensions;
        std::vector<const char*> vkLayers;
    };

    class Application {
        VkBackend* vkBackend;

    public:
        Application();

        void run();

        virtual ~Application();

        Application *CreateApplication();

    };

}
#endif //TEPHRYTE_APPLICATION_H
