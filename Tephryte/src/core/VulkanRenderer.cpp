//
// Created by Timothy on 8/6/2024.
//

#include "VulkanRenderer.h"

#include <glm/detail/func_vector_relational.inl>

namespace Tephryte {
    VulkanRenderer::VulkanRenderer(
    const std::vector<const char *>& instanceExtensions,
    const std::vector<const char *>& deviceExtensions,
    const std::vector<const char *>& layers,
    const VkPhysicalDeviceFeatures& features){
        instance = createVulkanInstance(instanceExtensions, layers, allocator, &debugMessenger);
        gpu = selectPhysicalDevice(instance, VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU, features, true);
        queues = { };
        device = createDevice(gpu, deviceExtensions, features, allocator, &queues);
    }
}
