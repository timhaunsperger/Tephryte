//
// Created by Timothy on 8/5/2024.
//

#include "VulkanImpl.h"

#include <bitset>

#include "TephryteCore.h"
#include <cstring>

#define VULKAN_DEBUG_LEVEL_BITS (\
      VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT \
    | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)
#define VULKAN_DEBUG_TYPE_BITS (\
      VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT \
    | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT)

VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT severity, VkDebugUtilsMessageTypeFlagsEXT, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void*)
{
    TPR_LOG_STREAM << "[VulkanValidation] ";
    switch (severity) {
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
            TPR_LOG_STREAM << "[VERBOSE] ";
        break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
            TPR_LOG_STREAM << "[INFO] ";
        break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
            TPR_LOG_STREAM << "[WARNING] ";
        break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
            TPR_LOG_STREAM << "[ERROR] ";
        break;
        default: ;
    }

    TPR_LOG_STREAM << pCallbackData->pMessage << "\n";
    return VK_FALSE;
}

namespace Tephryte::VulkanImpl {
    void checkVkResult(const VkResult err) {
#ifdef TPR_DEBUG
        if (err < 0) {
            TPR_ENGINE_ERROR("Vulkan function returned error code ", err)
        }
        if (err > 0) {
            TPR_ENGINE_WARN("Vulkan function returned status code ", err)
        }
#endif // TPR_DEBUG
    }

    VkInstance createVulkanInstance(std::vector<const char*> extensions, std::vector<const char*> layers,
    VkAllocationCallbacks* allocator, VkDebugUtilsMessengerEXT* debugMessenger){
        VkApplicationInfo app_info {
            .sType =            VK_STRUCTURE_TYPE_APPLICATION_INFO,
            .pEngineName =      "Tephryte",
            .apiVersion =       VK_API_VERSION_1_3
        };

        VkInstanceCreateInfo inst_info {
            .sType =            VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
            .pApplicationInfo = &app_info
        };

#ifdef TPR_DEBUG
        VkDebugUtilsMessengerCreateInfoEXT debug_info {
            .sType =            VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
            .messageSeverity =  VULKAN_DEBUG_LEVEL_BITS,
            .messageType =      VULKAN_DEBUG_TYPE_BITS,
            .pfnUserCallback =  debugCallback
        };

        inst_info.pNext = &debug_info;
        layers.push_back("VK_LAYER_KHRONOS_validation");
        extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
#endif

        bool dependancies_supported = true;
        uint32_t layer_count;
        vkEnumerateInstanceLayerProperties(&layer_count, nullptr);
        VkLayerProperties supported_layers[layer_count];
        vkEnumerateInstanceLayerProperties(&layer_count, supported_layers);

        for (const char* required_layer : layers) {
            bool available = false;
            for (auto layer: supported_layers) {
                if (strcmp(required_layer, layer.layerName) == 0){
                    available = true;
                    break;
                }
            }
            if (!available) {
                TPR_ENGINE_ERROR("Vulkan layer \"", required_layer, "\" missing")
                dependancies_supported = false;
            }
        }

        uint32_t extension_count;
        vkEnumerateInstanceExtensionProperties(nullptr, &extension_count, nullptr);
        VkExtensionProperties supported_extensions[extension_count];
        vkEnumerateInstanceExtensionProperties(nullptr, &extension_count, supported_extensions);

        for (const char* required_extension : extensions) {
            bool available = false;
            for (auto extension: supported_extensions) {
                if (strcmp(required_extension, extension.extensionName) == 0){
                    available = true;
                    break;
                }
            }
            if (!available) {
                TPR_ENGINE_ERROR("Vulkan extension \"", required_extension, "\" missing")
                dependancies_supported = false;
            }
        }

        if (!dependancies_supported) {
            TPR_EXIT
        }

        inst_info.enabledLayerCount = static_cast<uint32_t>(layers.size());
        inst_info.ppEnabledLayerNames = layers.data();
        inst_info.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
        inst_info.ppEnabledExtensionNames =  extensions.data();

        //Create instance
        VkInstance instance;
        VkResult err = vkCreateInstance(&inst_info, allocator, &instance);
        checkVkResult(err);

#ifdef TPR_DEBUG
        auto createDebugMessenger = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT"));
        TPR_ENGINE_ASSERT(createDebugMessenger != nullptr, "Vulkan debug messenger could not be created")
        err = createDebugMessenger(instance, &debug_info, allocator, debugMessenger);
        checkVkResult(err);
#endif // TPR_DEBUG

        return instance;
    }

    std::vector<VkPhysicalDevice> getPhysicalDevices(VkInstance instance) {
        uint32_t device_count = 0;
        std::vector<VkPhysicalDevice> physical_devices = { };

        vkEnumeratePhysicalDevices(instance, &device_count, nullptr);
        if (device_count == 0) {
            return physical_devices;
        }
        physical_devices.resize(device_count);
        vkEnumeratePhysicalDevices(instance, &device_count, physical_devices.data());

        return physical_devices;
    }

    VkPhysicalDevice selectPhysicalDevice(VkInstance instance, VkPhysicalDeviceType preferredType,
    VkPhysicalDeviceFeatures requiredFeatures, bool allowFallbackTypes){
        std::vector<VkPhysicalDevice> physical_devices = getPhysicalDevices(instance);
        VkPhysicalDevice fallback_device = nullptr;

        for (VkPhysicalDevice physical_device : physical_devices) {
            VkPhysicalDeviceProperties properties;
            VkPhysicalDeviceFeatures features;
            vkGetPhysicalDeviceProperties(physical_device, &properties);
            vkGetPhysicalDeviceFeatures(physical_device, &features);

            bool valid = true;

            auto* featurePtr = reinterpret_cast<VkBool32*>(&features);
            auto* requiredPtr = reinterpret_cast<VkBool32*>(&requiredFeatures);
            for (int i = 0; i < sizeof(features) / sizeof(VkBool32); ++i) {
                if (featurePtr[i] < requiredPtr[i]) {
                    valid = false;
                    break;
                }
            }
            if (!valid) {
                continue;
            }
            if (properties.deviceType == preferredType) {
                return physical_device;
            }
            if (fallback_device == nullptr) {
                fallback_device = physical_device;
            }
        }
        if (fallback_device != nullptr) {
            if (allowFallbackTypes) {
                return fallback_device;
            }
            TPR_ENGINE_ERROR("Failed to find gpu of requested type")
            return nullptr;
        }
        TPR_ENGINE_ERROR("Failed to find gpu with required features")
        return nullptr;
    }

    struct QueuePoolInfo {
        uint32_t graphicsIndex;
        uint32_t graphicsCount;
        uint32_t computeIndex;
        uint32_t computeCount;
        uint32_t transferIndex;
        uint32_t transferCount;
    };

    std::vector<VkQueueFamilyProperties> getQueueFamilies(VkPhysicalDevice physicalDevice) {
        uint32_t queue_family_count;
        std::vector<VkQueueFamilyProperties> queue_families;
        vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queue_family_count, nullptr);
        queue_families.resize(queue_family_count);
        vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queue_family_count, queue_families.data());

        return queue_families;
    }

    QueuePoolInfo getQueuePoolInfo(VkPhysicalDevice physicalDevice) {
        QueuePoolInfo pool = { };
        std::vector<VkQueueFamilyProperties> queue_families = getQueueFamilies(physicalDevice);

        for (int i = 0; i < queue_families.size(); ++i) {
            VkQueueFamilyProperties family = queue_families[i];

            uint32_t flags = queue_families[i].queueFlags;
            flags &= ~(VK_QUEUE_SPARSE_BINDING_BIT | VK_QUEUE_PROTECTED_BIT); // mask out unimportant bits
            if ((flags & VK_QUEUE_GRAPHICS_BIT) == VK_QUEUE_GRAPHICS_BIT) {
                pool.graphicsIndex = i;
                pool.graphicsCount = family.queueCount;
                continue;
            }
            if ((flags & VK_QUEUE_COMPUTE_BIT) == VK_QUEUE_COMPUTE_BIT) {
                pool.computeIndex = i;
                pool.computeCount = family.queueCount;
                continue;
            }
            if (flags == VK_QUEUE_TRANSFER_BIT) {
                pool.transferIndex = i;
                pool.transferCount = family.queueCount;
            }
        }

        return pool;
    }

    VkDevice createDevice(VkPhysicalDevice physicalDevice, std::vector<const char*> extensions,
    VkPhysicalDeviceFeatures features, VkAllocationCallbacks *allocator, GraphicsQueues *queues) {
        VkPhysicalDeviceDynamicRenderingFeatures dynamic_rendering_feature{
            .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DYNAMIC_RENDERING_FEATURES,
            .dynamicRendering = VK_TRUE,
        };

        VkDeviceCreateInfo device_create_info{
            .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
            .pNext = &dynamic_rendering_feature,
            .queueCreateInfoCount = 1,
            .enabledExtensionCount = (uint32_t)extensions.size(),
            .ppEnabledExtensionNames = extensions.data(),
            .pEnabledFeatures = &features
        };

        // Validate device extensions
        uint32_t extension_count;
        vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extension_count, nullptr);
        VkExtensionProperties extension_properties[extension_count];
        vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extension_count, extension_properties);

        bool extensionsSupported = true;
        for (const char* ext : extensions) {
            bool available = false;
            for (VkExtensionProperties& props : extension_properties) {
                if (strcmp(ext, props.extensionName) == 0){
                    available = true;
                    break;
                }
            }
            if (!available) {
                TPR_ENGINE_ERROR("Requested vulkan device extension \"", ext, "\" not available")
                extensionsSupported = false;
            }
        }
        if (!extensionsSupported)
            return { };

        // Create queues using seperate families if possible, then try creating multiple queues in graphics family.
        QueuePoolInfo pool_info = getQueuePoolInfo(physicalDevice);

        VkDeviceQueueCreateInfo graphics_queue_info;
        std::vector<VkDeviceQueueCreateInfo> queue_create_infos = { };
        float priority[] = { 1.0f,  1.0f };

        if (pool_info.graphicsCount > 0) {
            graphics_queue_info = {
                .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
                .queueFamilyIndex = pool_info.graphicsIndex,
                .queueCount = 1,
                .pQueuePriorities = priority
            };
        } else {
            TPR_ENGINE_ERROR("Failed to create graphics queue on selected GPU")
            return { };
        }

        if (pool_info.computeCount > 0) {
            VkDeviceQueueCreateInfo compute_queue_info = {
                .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
                .queueFamilyIndex = pool_info.computeIndex,
                .queueCount = 1,
                .pQueuePriorities = priority
            };
            ++device_create_info.queueCreateInfoCount;
            queue_create_infos.push_back(compute_queue_info);
        } else if (pool_info.graphicsCount > graphics_queue_info.queueCount) {
            graphics_queue_info.queueCount = 2;
        }

        if (pool_info.transferCount > 0) {
            VkDeviceQueueCreateInfo transfer_queue_info = {
                .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
                .queueFamilyIndex = pool_info.transferIndex,
                .queueCount = 1,
                .pQueuePriorities = priority
            };
            ++device_create_info.queueCreateInfoCount;
            queue_create_infos.push_back(transfer_queue_info);
        }

        queue_create_infos.push_back(graphics_queue_info);

        // Create device
        device_create_info.pQueueCreateInfos = queue_create_infos.data();

        VkDevice device;
        VkResult err = vkCreateDevice(physicalDevice, &device_create_info, allocator, &device);
        checkVkResult(err);

        // Acquire queue handles
        queues = { };
        vkGetDeviceQueue(device, pool_info.graphicsIndex, 0, &queues->graphicsQueue);
        if (pool_info.computeCount > 0) {
            vkGetDeviceQueue(device, pool_info.computeIndex, 0, &queues->computeQueue);
        } else if(graphics_queue_info.queueCount == 2) {
            vkGetDeviceQueue(device, pool_info.graphicsIndex, 1, &queues->computeQueue);
        }
        if (pool_info.transferCount > 0) {
            vkGetDeviceQueue(device, pool_info.transferIndex, 0, &queues->transferQueue);
        }

        return device;
    }
}
