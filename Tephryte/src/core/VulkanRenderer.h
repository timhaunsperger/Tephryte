//
// Created by Timothy on 8/6/2024.
//

#ifndef VULKANRENDERER_H
#define VULKANRENDERER_H
#include "TephryteCore.h"


namespace Tephryte {
    using namespace VulkanImpl;
    class VulkanRenderer {
        VkAllocationCallbacks*      allocator = nullptr;
        VkDebugUtilsMessengerEXT    debugMessenger = nullptr;
        VkInstance                  instance;
        VkPhysicalDevice            gpu;
        VkDevice                    device;
        GraphicsQueues              queues;

        VulkanRenderer(
            const std::vector<const char *> &instance_extensions,
            const std::vector<const char *> &device_extensions,
            const std::vector<const char *> &layers,
            const VkPhysicalDeviceFeatures &features);
    };
}

#endif //VULKANRENDERER_H
