//
// Created by timha on 7/9/2024.
//

#include "Application.h"

#include "IO.h"


namespace Tephryte {
    Application::Application() {
        window = new Window(1920, 1080, "App");
        RenderEngine::Config config = {.window = window};

        renderer.init(config);

        auto vertexCode = IO::readFile("shaders/base_UI.vert.spv");
        auto fragCode = IO::readFile("shaders/base_UI.frag.spv");

        VkShaderModule vertModule = renderer.createShaderModule(vertexCode);
        VkShaderModule fragModule = renderer.createShaderModule(fragCode);

        RenderEngine::PipielineInfo info = {
            .vertexShader = vertModule,
            .fragmentShader = fragModule,
        };

        renderPipeline = renderer.createGraphicsPipeline(info);

    }

    void Application::run() {
        while (true) {
            renderer.recordRenderCommandBuffer(renderPipeline);

            renderer.render();

            renderer.presentScene();
        }
    }

    Application::~Application() {
        delete window;
    }
}
