//
// Created by timha on 7/9/2024.
//

#ifndef TEPHRYTE_VKBACKEND_H
#define TEPHRYTE_VKBACKEND_H

#include <vulkan/vulkan.h>
#include <vector>
#include <optional>
#include "GLFW/glfw3.h"

const std::vector<const char*> validationLayers = { "VK_LAYER_KHRONOS_validation" };
const std::vector<const char*> deviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };

VkResult createDebugUtilsMessengerEXT(VkInstance, const VkDebugUtilsMessengerCreateInfoEXT*, VkDebugUtilsMessengerEXT*);
void destroyDebugUtilsMessengerEXT(VkInstance, VkDebugUtilsMessengerEXT);

class VkBackend {

    VkAllocationCallbacks*   vkAllocator;
    VkInstance               vkInstance;
    VkPhysicalDevice         vkPhysicalDevice;
    VkDevice                 vkDevice;
    uint32_t                 vkQueueFamily;
    VkQueue                  vkQueue;
    VkDebugReportCallbackEXT vkDebugReport;
    VkPipelineCache          vkPipelineCache;
    VkDescriptorPool         vkDescriptorPool;

public:
    int Init();

    VkPhysicalDevice selectPhysicalDevice();
    void SetupVulkan(std::vector<const char*> instance_extensions);
    void SetupVulkanWindow(GLFWwindow* wd, VkSurfaceKHR surface, int width, int height);




};

#endif //TEPHRYTE_VKBACKEND_H
