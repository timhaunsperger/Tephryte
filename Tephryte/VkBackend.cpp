//
// Created by timha on 7/9/2024.
//

#include <cstring>
#include "VkBackend.h"

VkInstance VkBackend::init() {
    VkApplicationInfo appInfo{
        .sType =            VK_STRUCTURE_TYPE_APPLICATION_INFO,
        .pApplicationName = "Hello Engine",
        .pEngineName =      "Tephryte",
        .engineVersion =    VK_MAKE_API_VERSION(1, 0, 0, 0),
        .apiVersion =       VK_API_VERSION_1_0
    };

    // Get required extensions
    uint32_t glfwExtensionCount;
    const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
    extensions.insert(extensions.end(), glfwExtensions, glfwExtensions + glfwExtensionCount);

#ifndef NDEBUG
    // Add debug utilities
    layers.push_back("VK_LAYER_KHRONOS_validation");
    extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
#endif

    // Validate layers
    VkLayerProperties* layerProperties;
    uint32_t layerCount;
    vkEnumerateInstanceLayerProperties(&layerCount, layerProperties);

    for (const char* layer : layers) {
        bool layerAvailable = false;
        for (int i = 0; i < layerCount; ++i) {
            if (strcmp(layer, layerProperties[i].layerName) == 0){
                layerAvailable = true;
                break;
            }
        }
        if (!layerAvailable){

        }
    }


    VkInstanceCreateInfo instInfo{
        .sType =                    VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
        .pApplicationInfo =         &appInfo,
        .enabledLayerCount =        (uint32_t)layers.size(),
        .ppEnabledLayerNames =      layers.data(),
        .enabledExtensionCount =    (uint32_t)extensions.size(),
        .ppEnabledExtensionNames =  extensions.data(),
    };

}



