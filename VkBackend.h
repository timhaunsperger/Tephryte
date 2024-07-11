//
// Created by timha on 7/9/2024.
//

#ifndef TEPHRYTE_VKBACKEND_H
#define TEPHRYTE_VKBACKEND_H

#include <vulkan/vulkan.h>
#include <vector>
#include <optional>
#include "GLFW/glfw3.h"



VkResult createDebugUtilsMessengerEXT(VkInstance, const VkDebugUtilsMessengerCreateInfoEXT*, VkDebugUtilsMessengerEXT*);
void destroyDebugUtilsMessengerEXT(VkInstance, VkDebugUtilsMessengerEXT);

class VkBackend {

    std::vector<const char*> extensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
    std::vector<const char*> layers = { };

    VkInstance               instance;
    VkPhysicalDevice         gpu;
    VkDevice                 device;
    VkQueue                  queue;
    VkAllocationCallbacks*   allocator;

    const char** getExtensions(uint32_t*);
    const char** getLayers(uint32_t*);

public:
    VkInstance init();

};

#endif //TEPHRYTE_VKBACKEND_H
