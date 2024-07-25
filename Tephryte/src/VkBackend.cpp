//
// Created by timha on 7/9/2024.
//

#include <cstring>
#include "Tephryte.h"
namespace Tephryte {
    VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
    void* pUserData) {

        TPR_LOG_STREAM << "[vulkan] " << pCallbackData->pMessage << std::endl;

        return VK_FALSE;
    }


    VkBackend::VkBackend(const VkSettings& settings)
    : extensions(settings.extensions), layers(settings.layers) {

        VkResult err;

        VkApplicationInfo app_info { };
        app_info.sType =                VK_STRUCTURE_TYPE_APPLICATION_INFO;
        app_info.pApplicationName =     settings.appName;
        app_info.pEngineName =          "Tephryte";
        app_info.engineVersion =        VK_MAKE_API_VERSION(1, 0, 0, 0);
        app_info.apiVersion =           VK_API_VERSION_1_0;


        VkInstanceCreateInfo inst_info { };
        inst_info.sType =               VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        inst_info.pApplicationInfo =    &app_info;


        // Add debug utilities
#ifdef TPR_DEBUG
        VkDebugUtilsMessengerCreateInfoEXT debug_info { };
        debug_info.sType =              VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        debug_info.messageSeverity =    settings.debugLevel;
        debug_info.messageType =        settings.debugTypes;
        debug_info.pfnUserCallback =    debugCallback;

        inst_info.pNext = &debug_info;

        layers.push_back("VK_LAYER_KHRONOS_validation");
        extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
#endif //TPR_DEBUG

        // Get required extensions
        uint32_t glfw_extension_count;
        const char** glfw_extensions = glfwGetRequiredInstanceExtensions(&glfw_extension_count);
        extensions.insert(extensions.end(), glfw_extensions, glfw_extensions + glfw_extension_count);

        // Validate layers
        uint32_t layer_count;
        vkEnumerateInstanceLayerProperties(&layer_count, nullptr);
        VkLayerProperties layer_properties[layer_count];
        vkEnumerateInstanceLayerProperties(&layer_count, layer_properties);

        for (const char* layer : layers) {
            bool available = false;
            for (int i = 0; i < layer_count; ++i) {
                if (strcmp(layer, layer_properties[i].layerName) == 0){
                    available = true;
                    break;
                }
            }
            TPR_ENGINE_ASSERT(available ,"Requested vulkan layer \"", layer, "\" not available")
        }
        inst_info.enabledLayerCount = static_cast<uint32_t>(layers.size());
        inst_info.ppEnabledLayerNames = layers.data();

        // Validate extensions
        uint32_t extension_count;
        vkEnumerateInstanceExtensionProperties(nullptr, &extension_count, nullptr);
        VkExtensionProperties extension_properties[extension_count];
        vkEnumerateInstanceExtensionProperties(nullptr, &extension_count, extension_properties);

        for (const char* ext : extensions) {
            bool available = false;
            for (int i = 0; i < extension_count; ++i) {
                if (strcmp(ext, extension_properties[i].extensionName) == 0){
                    available = true;
                    break;
                }
            }
            TPR_ENGINE_ASSERT(available ,"Requested vulkan extension \"", ext, "\" not available")
        }
        inst_info.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
        inst_info.ppEnabledExtensionNames =  extensions.data();


        err = vkCreateInstance(&inst_info, allocator, &instance);
        TPR_ENGINE_ASSERT(err == VK_SUCCESS, "Failed to create vulkan instance for ", settings.appName)

        TPR_INFO("test")

        // Setup vulkan debug messenger
#ifdef TPR_DEBUG
        auto create_debug_func = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT"));
        TPR_ENGINE_ASSERT(create_debug_func != nullptr, "Vulkan debug messenger could not be created")
        err = create_debug_func(instance, &debug_info, allocator, &debugMessenger);
        TPR_ENGINE_ASSERT(err == VK_SUCCESS, "Vulkan debug messenger could not be created")
#endif //TPR_DEBUG


    }

    VkBackend::~VkBackend() {
#ifdef TPR_DEBUG
        auto destroy_debug_func = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT"));
        if(destroy_debug_func != nullptr) {
            destroy_debug_func(instance, debugMessenger, allocator);
        }
#endif //TPR_DEBUG

        vkDestroyInstance(instance, nullptr);
    }
}


