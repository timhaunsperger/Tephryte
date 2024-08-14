//
// Created by timha on 7/9/2024.
//

#ifndef TEPHRYTE_APPLICATION_H
#define TEPHRYTE_APPLICATION_H

#include "Debug.h"
#include "RenderEngine.h"
#include "Window.h"

namespace Tephryte {

    class Application {
        Window*      window;
        RenderEngine renderer;
        VkPipeline renderPipeline;

    public:
        Application();

        void run();

        virtual ~Application();

        Application* CreateApplication();

    };

}
#endif //TEPHRYTE_APPLICATION_H
