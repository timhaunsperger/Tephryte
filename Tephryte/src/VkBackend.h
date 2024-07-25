//
// Created by timha on 7/9/2024.
//

#ifndef TEPHRYTE_VKBACKEND_H
#define TEPHRYTE_VKBACKEND_H

#include <vulkan/vulkan.h>
#include <vector>

namespace Tephryte {
    VkResult createDebugUtilsMessengerEXT(VkInstance, const VkDebugUtilsMessengerCreateInfoEXT*, VkDebugUtilsMessengerEXT*);
    void destroyDebugUtilsMessengerEXT(VkInstance, VkDebugUtilsMessengerEXT);

    struct VkSettings {
        const char* appName;
        std::vector<const char*> extensions = { };
        std::vector<const char*> layers = { };
        VkDebugUtilsMessageSeverityFlagsEXT debugLevel = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        VkDebugUtilsMessageTypeFlagsEXT debugTypes = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    };

    class VkBackend {

        std::vector<const char*> extensions;
        std::vector<const char*> layers;

        VkAllocationCallbacks*   allocator = nullptr;
        VkDebugUtilsMessengerEXT debugMessenger;
        VkInstance               instance;
        VkPhysicalDevice         gpu;
        VkDevice                 device;
        VkQueue                  queue;


    public:
        explicit VkBackend(const VkSettings&);

        ~VkBackend();
    };



}
#endif //TEPHRYTE_VKBACKEND_H
