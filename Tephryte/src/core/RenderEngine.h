//
// Created by Timothy on 8/6/2024.
//

#ifndef RENDERENGINE_H
#define RENDERENGINE_H


#include <list>

#include "VulkanImpl.h"
#include "Window.h"

namespace Tephryte {
    class RenderEngine {
    public:
        struct SwapchainConfig {
            VkPresentModeKHR            vSyncMode = VK_PRESENT_MODE_FIFO_KHR;
            VkFormat                    imageFormat = VK_FORMAT_B8G8R8A8_SRGB;
            VkColorSpaceKHR             colorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
            uint32_t                    minImageCount = 3;
        };

        struct Config {
            Window*                     window;
            std::vector<const char *>   instanceExtensions = { };
            std::vector<const char *>   deviceExtensions = { };
            std::vector<const char *>   layers = { };
            VkPhysicalDeviceFeatures    features = { };
            VkPhysicalDeviceType        preferredGpuType = VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU;
            bool                        useSpecificGpu = false;
            uint32_t                    specificGpuID = 0;
            VkSampleCountFlagBits       MSAASamples = VK_SAMPLE_COUNT_1_BIT;
            VkExtent2D                  renderResolution = {1280, 720};
            VkClearColorValue           clearColor = {0, 0, 0, 0};
            bool                        colorBlending = true;

            SwapchainConfig             swapchainConfig = { };
        };

        struct Frame {
            VkSemaphore                 swapImageAvailable;
            VkFence                     frameAvailable;

            VkCommandPool               cmdPool = nullptr;
            VkCommandBuffer             cmdBuffer = nullptr;
        };

        struct SceneBuffer {
            VkSemaphore                         sceneRenderComplete, sceneTransfered;

            VkImage                             colorImage;
            VkDeviceMemory                      colorMemory;
            VkImageView                         colorImageView;
            VkFormat                            colorImageFormat;
            VkRenderingAttachmentInfo           colorAttachmentInfo;
            VkPipelineColorBlendAttachmentState blendInfo;

            VkImage                             depthImage;
            VkDeviceMemory                      depthMemory;
            VkImageView                         depthImageView;
            VkFormat                            depthImageFormat;
            VkRenderingAttachmentInfo           depthAttachmentInfo;
        };

        struct PipielineInfo {
            VkShaderModule                                  vertexShader;
            VkShaderModule                                  fragmentShader;
            std::vector<VkVertexInputBindingDescription>    vertexBindings = { };
            std::vector<VkVertexInputAttributeDescription>  vertexAttributes = { };
            std::vector<VkDescriptorSetLayout>              uniformLayouts = { };
            std::vector<VkPushConstantRange>                pushConstants = { };
            VkSpecializationInfo*                           vertexConstants = nullptr;
            VkSpecializationInfo*                           fragmentConstants = nullptr;
            VkPrimitiveTopology                             topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
            bool                                            primitiveRestart = false;
            bool                                            depthClamp = false;
            VkPolygonMode                                   polygonMode = VK_POLYGON_MODE_FILL;
            VkCullModeFlags                                 cullMode = VK_CULL_MODE_NONE;
            VkFrontFace                                     frontFace = VK_FRONT_FACE_CLOCKWISE;
        };

        RenderEngine() = default;

        VkInstance instance;
        VkAllocationCallbacks* allocator = nullptr;

        void init(const Config& config);

        VkShaderModule createShaderModule(std::vector<char> shaderCode);

        VkPipeline createGraphicsPipeline(PipielineInfo info);

        void recordRenderCommandBuffer(VkPipeline pipeline);

        void render();

        void presentScene();

        ~RenderEngine();
        RenderEngine(const RenderEngine&) = delete;
        RenderEngine& operator= (const RenderEngine&) = delete;

    private:

        VkDebugUtilsMessengerEXT            debugMessenger = nullptr;
        VkPhysicalDevice                    gpu = nullptr;
        VkDevice                            device = nullptr;
        VkSurfaceKHR                        surface = nullptr;
        VulkanImpl::Queues                  queues = { };

        VkSwapchainKHR                      swapchain = nullptr;
        uint32_t                            swapImageCount = 0;
        std::vector<VkImage>                swapImages = { };
        std::vector<VkImageView>            swapImageViews = { };

        const uint32_t                      concurrentFrames = 1;
        std::vector<Frame>                  frameInfos = { };
        uint32_t                            currentFrame = 0;

        SceneBuffer                         sceneBuffer = { };
        VkExtent2D                          renderResolution;
        VkSampleCountFlagBits               MSAASamples;
        VkViewport                          viewport;
        VkRect2D                            scissor;
        VkPipelineColorBlendStateCreateInfo blendInfo;

        VkCommandPool                       presentCmdPool = nullptr;
        std::vector<VkCommandBuffer>        presentCmdBuffers = { };

        VkCommandPool                       internalCmdPool = nullptr;
        std::vector<VkCommandBuffer>        usedInternalCmdBuffers = { };

        PFN_vkDestroyDebugUtilsMessengerEXT vkDestroyDebugUtilsMessengerEXT;

        void createSceneBuffer(const Config& config);

        void createPresentCommandBuffers(const Config& config);

        VkCommandBuffer beginSingleUseCommands();

        void endSingleUseCommands(VkCommandBuffer buffer, VkQueue queue);

        void freeUsedBuffers();

        void transitionImageLayout(VkImage image, VkImageLayout oldLayout, VkImageLayout newLayout);
    };
}

#endif //RENDERENGINE_H
