//
// Created by Timothy on 8/5/2024.
//

#ifndef VULKANIMPL_H
#define VULKANIMPL_H
#include "vulkan/vulkan.h"
#include <vector>

namespace Tephryte::VulkanImpl {
    struct Queues {
        VkQueue graphicsQueue, computeQueue, transferQueue;
        uint32_t graphicsFamilyIndex, computeFamilyIndex, transferFamilyIndex;
    };

    VkInstance createVulkanInstance(
        std::vector<const char*>                extensions,
        std::vector<const char*>                layers,
        VkAllocationCallbacks*                  allocator,
        VkDebugUtilsMessengerEXT*               debugMessenger);

    std::vector<VkPhysicalDevice> getPhysicalDevices(
        VkInstance                              instance,
        VkPhysicalDeviceFeatures                requiredFeatures,
        const std::vector<const char*>&         requiredExtensions);

    VkPhysicalDevice getPhysicalDeviceByPreferredType(
        const std::vector<VkPhysicalDevice>&    physical_devices,
        VkPhysicalDeviceType                    type);

    VkPhysicalDevice getPhysicalDeviceByID(
        const std::vector<VkPhysicalDevice>&    physical_devices,
        uint32_t                                id);

    VkDevice createDevice(
        VkPhysicalDevice                physicalDevice,
        const std::vector<const char*>& extensions,
        VkPhysicalDeviceFeatures        features,
        VkAllocationCallbacks           *allocator,
        Queues&                 queues);

    VkSwapchainKHR createSwapchain(
        VkDevice                        device,
        VkPhysicalDevice                physical_device,
        VkSurfaceKHR                    surface,
        VkExtent2D                      resolution,
        VkPresentModeKHR                vSyncMode,
        VkFormat                        imageFormat,
        VkColorSpaceKHR                 colorSpace,
        uint32_t                        minImageCount,
        VkAllocationCallbacks*          allocator,
        std::vector<VkImage>&           swapchainImages);

    VkImage createImage2D(
        VkDevice                        device,
        VkPhysicalDevice                physical_device,
        VkFormat                        imageFormat,
        VkExtent2D                      resolution,
        uint32_t                        mipmapLevels,
        VkImageUsageFlags               usages,
        VkMemoryPropertyFlags           memProps,
        VkAllocationCallbacks*          allocator,
        VkDeviceMemory&                 imageMemory);

    VkImage createImage3D(
        VkDevice                        device,
        VkPhysicalDevice                physical_device,
        VkFormat                        imageFormat,
        VkExtent3D                      resolution,
        VkImageUsageFlags               usages,
        VkMemoryPropertyFlags           memProps,
        VkAllocationCallbacks*          allocator,
        VkDeviceMemory&                 imageMemory);

    VkImage createAttachmentImage(
        VkDevice                        device,
        VkPhysicalDevice                physical_device,
        VkFormat                        imageFormat,
        VkExtent2D                      resolution,
        VkSampleCountFlagBits           MSAASamples,
        VkImageUsageFlags               usages,
        VkMemoryPropertyFlags           memProps,
        VkAllocationCallbacks*          allocator,
        VkDeviceMemory&                 imageMemory);

    VkImageView createImageView(
        VkDevice                        device,
        VkImage                         image,
        VkImageViewType                 type,
        VkImageAspectFlagBits           imageAspects,
        uint32_t                        imageMipmapLevels,
        VkFormat                        imageFormat,
        VkAllocationCallbacks*          allocator
    );

    VkSemaphore createSemaphore(
        VkDevice                        device,
        VkAllocationCallbacks*          allocator
    );

    VkFence createFence(
        VkDevice                        device,
        bool                            startSignaled,
        VkAllocationCallbacks*          allocator
    );

    VkCommandPool createCommandPool(
        VkDevice                        device,
        VkCommandPoolCreateFlags        flags,
        uint32_t                        queueFamilyIndex,
        VkAllocationCallbacks*          allocator);

    VkCommandBuffer createCommandBuffer(
        VkDevice                        device,
        VkCommandPool                   pool,
        VkCommandBufferLevel            level);


};



#endif //VULKANIMPL_H
