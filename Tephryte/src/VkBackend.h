//
// Created by timha on 7/9/2024.
//

#ifndef TEPHRYTE_VKBACKEND_H
#define TEPHRYTE_VKBACKEND_H

#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.h>
#include <vector>

#include "GLFW/glfw3.h"

namespace Tephryte {
    VkResult createDebugUtilsMessengerEXT(VkInstance, const VkDebugUtilsMessengerCreateInfoEXT*, VkDebugUtilsMessengerEXT*);
    void destroyDebugUtilsMessengerEXT(VkInstance, VkDebugUtilsMessengerEXT);

    struct GraphicsSettings {
        const char* appName;
        std::vector<const char*> extensions = { };
        std::vector<const char*> layers = { };
        VkDebugUtilsMessageSeverityFlagsEXT debugLevel = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        VkDebugUtilsMessageTypeFlagsEXT debugTypes = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;

    };

    class VkBackend {
    public:
        explicit VkBackend(const GraphicsSettings&, GLFWwindow*);

        void draw();

        ~VkBackend();

    private:
        std::vector<const char*>    extensions;
        std::vector<const char*>    layers;
        VkAllocationCallbacks*      allocator = nullptr;
        VkDebugUtilsMessengerEXT    debugMessenger;
        VkInstance                  instance;
        VkPhysicalDevice            gpu = nullptr;
        VkDevice                    device;
        VkQueue                     queue;
        VkSurfaceKHR                surface;
        VkSwapchainKHR              swapchain;
        VkFormat                    swapchainImageFormat;
        VkExtent2D                  swapchainExtent;
        std::vector<VkImage>        swapchainImages;
        std::vector<VkImageView>    swapchainImageViews;
        uint32_t                    swapchainImageCount;
        VkPipelineLayout            pipelineLayout;
        VkPipeline                  graphicsPipeline;
        VkCommandPool               commandPool;
        std::vector<VkCommandBuffer>commandBuffers;
        std::vector<VkSemaphore>    imageAvailableSemaphores;
        std::vector<VkSemaphore>    renderFinishedSemaphores;
        std::vector<VkFence>        inFlightFences;
        uint32_t                    swapCurrentFrame = 0;

        VkShaderModule createShaderModule(const std::vector<char>& code);

    };



}
#endif //TEPHRYTE_VKBACKEND_H
