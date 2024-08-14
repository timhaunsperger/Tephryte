//
// Created by Timothy on 8/5/2024.
//

#include "Debug.h"
#include "VulkanImpl.h"
#include <cstring>

#define VULKAN_DEBUG_LEVEL_BITS (\
      VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT \
    | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)
#define VULKAN_DEBUG_TYPE_BITS (\
      VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT \
    | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT)

VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT severity,
VkDebugUtilsMessageTypeFlagsEXT, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void*)
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

    TPR_LOG_STREAM << pCallbackData->pMessage << "\n\n";
    return VK_FALSE;
}

namespace Tephryte::VulkanImpl {
    VkInstance createVulkanInstance(std::vector<const char*> extensions, std::vector<const char*> layers,
    VkAllocationCallbacks* allocator, VkDebugUtilsMessengerEXT* debugMessenger)
    {
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
            return nullptr;
        }

        inst_info.enabledLayerCount = static_cast<uint32_t>(layers.size());
        inst_info.ppEnabledLayerNames = layers.data();
        inst_info.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
        inst_info.ppEnabledExtensionNames =  extensions.data();

        //Create instance
        VkInstance instance;
        VkResult err = vkCreateInstance(&inst_info, allocator, &instance);
        CHECK_VK_RESULT(err);

#ifdef TPR_DEBUG
        auto createDebugMessenger = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT"));
        TPR_ENGINE_ASSERT(createDebugMessenger != nullptr, "Vulkan debug messenger could not be created")
        err = createDebugMessenger(instance, &debug_info, allocator, debugMessenger);
        CHECK_VK_RESULT(err);
#endif // TPR_DEBUG

        return instance;
    }

    std::vector<VkPhysicalDevice> getPhysicalDevices(VkInstance instance,
    VkPhysicalDeviceFeatures requiredFeatures, const std::vector<const char*>& requiredExtensions)
    {
        uint32_t device_count = 0;
        std::vector<VkPhysicalDevice> physical_devices = { };
        std::vector<VkPhysicalDevice> valid_physical_devices = { };

        vkEnumeratePhysicalDevices(instance, &device_count, nullptr);
        if (device_count == 0) {
            TPR_ENGINE_ERROR("Failed to find gpu with vulkan support")
            return valid_physical_devices;
        }
        physical_devices.resize(device_count);
        vkEnumeratePhysicalDevices(instance, &device_count, physical_devices.data());

        for (VkPhysicalDevice physical_device : physical_devices) {
            VkPhysicalDeviceProperties properties;
            VkPhysicalDeviceFeatures features;
            vkGetPhysicalDeviceProperties(physical_device, &properties);
            vkGetPhysicalDeviceFeatures(physical_device, &features);

            uint32_t extension_count;
            vkEnumerateDeviceExtensionProperties(physical_device, nullptr, &extension_count, nullptr);
            VkExtensionProperties extension_properties[extension_count];
            vkEnumerateDeviceExtensionProperties(physical_device, nullptr, &extension_count, extension_properties);

            bool valid = true;

            for (const char* ext : requiredExtensions) {
                bool available = false;
                for (VkExtensionProperties& props : extension_properties) {
                    if (strcmp(ext, props.extensionName) == 0){
                        available = true;
                        break;
                    }
                }
                if (!available) {
                    valid = false;
                }
            }

            auto* featurePtr = reinterpret_cast<VkBool32*>(&features);
            auto* requiredPtr = reinterpret_cast<VkBool32*>(&requiredFeatures);
            for (int i = 0; i < sizeof(features) / sizeof(VkBool32); ++i) {
                if (featurePtr[i] < requiredPtr[i]) {
                    valid = false;
                    break;
                }
            }
            if (valid) {
                valid_physical_devices.push_back(physical_device);
            }
        }

        if (valid_physical_devices.empty()) {
            TPR_ENGINE_ERROR("Failed to find gpu with required extension and feature support")
        }

        return valid_physical_devices;
    }

    VkPhysicalDevice getPhysicalDeviceByPreferredType(const std::vector<VkPhysicalDevice>& physical_devices,
    const VkPhysicalDeviceType type)
    {
        for (auto physical_device: physical_devices) {
            VkPhysicalDeviceProperties properties;
            vkGetPhysicalDeviceProperties(physical_device, &properties);
            if(properties.deviceType == type) {
                return physical_device;
            }
        }
        return physical_devices[0];
    }

    VkPhysicalDevice getPhysicalDeviceByID(const std::vector<VkPhysicalDevice>& physical_devices, const uint32_t id)
    {
        for (auto physical_device: physical_devices) {
            VkPhysicalDeviceProperties properties;
            vkGetPhysicalDeviceProperties(physical_device, &properties);
            if(properties.deviceType == id) {
                return physical_device;
            }
        }
        TPR_ENGINE_ERROR("No matching GPU found")
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

    static QueuePoolInfo getQueuePoolInfo(VkPhysicalDevice physicalDevice) {
        uint32_t queue_family_count;
        std::vector<VkQueueFamilyProperties> queue_families;
        vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queue_family_count, nullptr);
        queue_families.resize(queue_family_count);
        vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queue_family_count, queue_families.data());

        QueuePoolInfo pool = { };

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

    VkDevice createDevice(VkPhysicalDevice physicalDevice, const std::vector<const char*>& extensions,
    VkPhysicalDeviceFeatures features, VkAllocationCallbacks *allocator, Queues& queues)
    {
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
        CHECK_VK_RESULT(err);

        // Acquire queue handles
        vkGetDeviceQueue(device, pool_info.graphicsIndex, 0, &queues.graphicsQueue);

        queues.graphicsFamilyIndex = pool_info.graphicsIndex;
        queues.computeFamilyIndex = pool_info.graphicsIndex;
        queues.transferFamilyIndex = pool_info.graphicsIndex;

        // Use specialized queues if available
        if (pool_info.computeCount > 0) {
            vkGetDeviceQueue(device, pool_info.computeIndex, 0, &queues.computeQueue);
            queues.computeFamilyIndex = pool_info.computeIndex;
        } else if(graphics_queue_info.queueCount == 2) {
            vkGetDeviceQueue(device, pool_info.graphicsIndex, 1, &queues.computeQueue);
        }

        if (pool_info.transferCount > 0) {
            vkGetDeviceQueue(device, pool_info.transferIndex, 0, &queues.transferQueue);
            queues.transferFamilyIndex = pool_info.transferIndex;
        }

        return device;
    }

    VkSwapchainKHR createSwapchain(VkDevice device, VkPhysicalDevice physicalDevice, VkSurfaceKHR surface,
    VkExtent2D resolution, VkPresentModeKHR vSyncMode, VkFormat imageFormat, VkColorSpaceKHR colorSpace,
    uint32_t minImageCount, VkAllocationCallbacks* allocator, std::vector<VkImage>& swapchainImages)
    {
        VkSurfaceCapabilitiesKHR capabilities;
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &capabilities);

        VkSwapchainKHR swapchain;

        VkSwapchainCreateInfoKHR swap_info = {
            .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
            .surface = surface,
            .minImageCount = minImageCount,
            .imageFormat = imageFormat,
            .imageColorSpace = colorSpace,
            .imageExtent = resolution,
            .imageArrayLayers = 1,
            .imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT,
            .imageSharingMode = VK_SHARING_MODE_EXCLUSIVE,
            .preTransform = capabilities.currentTransform,
            .compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
            .presentMode = vSyncMode,
            .clipped = VK_TRUE,
        };

        VkResult err = vkCreateSwapchainKHR(device, &swap_info, allocator, &swapchain);
        CHECK_VK_RESULT(err)

        uint32_t image_count;
        vkGetSwapchainImagesKHR(device, swapchain, &image_count, nullptr);

        swapchainImages.resize(image_count);
        vkGetSwapchainImagesKHR(device, swapchain, &image_count, swapchainImages.data());

        return swapchain;
    }

    static uint32_t findMemoryType(VkPhysicalDevice physicalDevice, uint32_t typeFilter, VkMemoryPropertyFlags properties) {
        VkPhysicalDeviceMemoryProperties memoryProps;
        vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memoryProps);

        for (int i = 0; i < memoryProps.memoryTypeCount; ++i) {
            if (typeFilter & (1 << i) && (memoryProps.memoryTypes[i].propertyFlags & properties)) {
                return i;
            }
        }
        TPR_ENGINE_ERROR("Failed to find suitable gpu memory type")
        return 0;
    }

    static VkDeviceMemory allocateImageMemory(VkDevice device, VkPhysicalDevice physical_device, VkImage image,
    VkMemoryPropertyFlags memProps, VkAllocationCallbacks* allocator)
    {
        VkDeviceMemory memory;
        VkMemoryRequirements req;
        vkGetImageMemoryRequirements(device, image, &req);

        VkMemoryAllocateInfo allocate_info = {
            .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
            .allocationSize = req.size,
            .memoryTypeIndex = findMemoryType(physical_device, req.memoryTypeBits, memProps)
        };
        VkResult err = vkAllocateMemory(device, &allocate_info, allocator, &memory);
        CHECK_VK_RESULT(err);
        vkBindImageMemory(device, image, memory, 0);

        return memory;
    }

    VkImage createImage2D(VkDevice device, VkPhysicalDevice physical_device, VkFormat imageFormat, VkExtent2D resolution,
    uint32_t mipmapLevels, VkImageUsageFlags usages, VkMemoryPropertyFlags memProps, VkAllocationCallbacks* allocator,
    VkDeviceMemory& imageMemory)
    {
        VkImage image;

        VkImageCreateInfo image_info = {
            .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
            .pNext = nullptr,
            .imageType = VK_IMAGE_TYPE_2D,
            .format = imageFormat,
            .extent = VkExtent3D(resolution.width, resolution.height, 1),
            .mipLevels = mipmapLevels,
            .arrayLayers = 1,
            .samples = VK_SAMPLE_COUNT_1_BIT,
            .tiling = VK_IMAGE_TILING_OPTIMAL,
            .usage = usages,
            .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
            .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED
        };
        VkResult err = vkCreateImage(device, &image_info, allocator, &image);
        CHECK_VK_RESULT(err);

        imageMemory = allocateImageMemory(device, physical_device, image, memProps, allocator);

        return image;
    }

    VkImage createImage3D(VkDevice device, VkPhysicalDevice physical_device, VkFormat imageFormat, VkExtent3D resolution,
    VkImageUsageFlags usages, VkMemoryPropertyFlags memProps, VkAllocationCallbacks* allocator, VkDeviceMemory& imageMemory)
    {
        VkImage image;

        VkImageCreateInfo image_info = {
            .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
            .pNext = nullptr,
            .imageType = VK_IMAGE_TYPE_3D,
            .format = imageFormat,
            .extent = resolution,
            .mipLevels = 1,
            .arrayLayers = 1,
            .samples = VK_SAMPLE_COUNT_1_BIT,
            .tiling = VK_IMAGE_TILING_OPTIMAL,
            .usage = usages,
            .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
            .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED
        };
        VkResult err = vkCreateImage(device, &image_info, allocator, &image);
        CHECK_VK_RESULT(err);

        imageMemory = allocateImageMemory(device, physical_device, image, memProps, allocator);

        return image;
    }

    VkImage createAttachmentImage(VkDevice device, VkPhysicalDevice physical_device, VkFormat imageFormat,
    VkExtent2D resolution, VkSampleCountFlagBits MSAASamples, VkImageUsageFlags usages, VkMemoryPropertyFlags memProps,
    VkAllocationCallbacks* allocator, VkDeviceMemory& imageMemory)
    {
        VkImage image;

        VkImageCreateInfo image_info = {
            .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
            .pNext = nullptr,
            .imageType = VK_IMAGE_TYPE_2D,
            .format = imageFormat,
            .extent = VkExtent3D(resolution.width, resolution.height, 1),
            .mipLevels = 1,
            .arrayLayers = 1,
            .samples = MSAASamples,
            .tiling = VK_IMAGE_TILING_OPTIMAL,
            .usage = usages,
            .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
            .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED
        };
        VkResult err = vkCreateImage(device, &image_info, allocator, &image);
        CHECK_VK_RESULT(err);

        imageMemory = allocateImageMemory(device, physical_device, image, memProps, allocator);

        return image;
    }

    VkImageView createImageView(VkDevice device, VkImage image, VkImageViewType type, VkImageAspectFlagBits imageAspects,
    uint32_t imageMipmapLevels, VkFormat imageFormat, VkAllocationCallbacks* allocator)
    {
        const VkImageSubresourceRange subresource_range = {
            .aspectMask = static_cast<VkImageAspectFlags>(imageAspects),
            .baseMipLevel = 0,
            .levelCount = imageMipmapLevels,
            .baseArrayLayer = 0,
            .layerCount = 1
        };

        VkImageViewCreateInfo view_info = {
            .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
            .image = image,
            .viewType = type,
            .format = imageFormat,
            .subresourceRange = subresource_range
        };

        VkImageView image_view;
        VkResult err = vkCreateImageView(device, &view_info, allocator, &image_view);
        CHECK_VK_RESULT(err)

        return image_view;
    }

    VkSemaphore createSemaphore(VkDevice device, VkAllocationCallbacks* allocator) {
        VkSemaphoreCreateInfo semaphore_info = { .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO };

        VkSemaphore semaphore;
        vkCreateSemaphore(device, &semaphore_info, allocator, &semaphore);

        return semaphore;
    }

    VkFence createFence(VkDevice device, bool startSignaled, VkAllocationCallbacks* allocator) {
        VkFenceCreateInfo fence_info = {
            .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
            .flags = VK_FENCE_CREATE_SIGNALED_BIT * static_cast<uint32_t>(startSignaled)
        };

        VkFence fence;
        vkCreateFence(device, &fence_info, allocator, &fence);

        return fence;
    }

    VkCommandPool createCommandPool(VkDevice device, VkCommandPoolCreateFlags flags, uint32_t queueFamilyIndex,
    VkAllocationCallbacks* allocator)
    {
        VkCommandPool pool;

        VkCommandPoolCreateInfo pool_info = {
            .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
            .flags = flags,
            .queueFamilyIndex = queueFamilyIndex,
        };

        VkResult err = vkCreateCommandPool(device, &pool_info, allocator, &pool);
        CHECK_VK_RESULT(err);

        return pool;
    }

    VkCommandBuffer createCommandBuffer(VkDevice device, VkCommandPool pool, VkCommandBufferLevel level)
    {
        VkCommandBuffer buffer;

        VkCommandBufferAllocateInfo buffer_info = {
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
            .commandPool = pool,
            .level = level,
            .commandBufferCount = 1
        };
        VkResult err = vkAllocateCommandBuffers(device, &buffer_info, &buffer);
        CHECK_VK_RESULT(err)
        return buffer;
    }
}
