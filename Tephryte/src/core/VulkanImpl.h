//
// Created by Timothy on 8/5/2024.
//

#ifndef VULKANIMPL_H
#define VULKANIMPL_H
#include <vector>
#include <vulkan/vulkan.h>

namespace Tephryte::VulkanImpl {
    struct GraphicsQueues {
        VkQueue graphicsQueue, computeQueue, transferQueue;
    };

    void checkVkResult(VkResult err);

    VkInstance createVulkanInstance(
        std::vector<const char*> extensions,
        std::vector<const char*> layers,
        VkAllocationCallbacks* allocator,
        VkDebugUtilsMessengerEXT* debugMessenger);

    std::vector<VkPhysicalDevice> getPhysicalDevices(
        VkInstance instance);

    VkPhysicalDevice selectPhysicalDevice(
        VkInstance instance,
        VkPhysicalDeviceType preferredType,
        VkPhysicalDeviceFeatures requiredFeatures,
        bool allowFallbackTypes = true);

    std::vector<VkQueueFamilyProperties> getQueueFamilies(
        VkPhysicalDevice physical_device);

    VkDevice createDevice(
        VkPhysicalDevice physicalDevice,
        std::vector<const char*> extensions,
        VkPhysicalDeviceFeatures features,
        VkAllocationCallbacks *allocator,
        GraphicsQueues* queues);

};



#endif //VULKANIMPL_H
