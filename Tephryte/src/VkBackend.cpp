//
// Created by timha on 7/9/2024.
//
#include "core/Debug.h"
#include "VkBackend.h"
#define GLFW_INCLUDE_VULKAN
#include <array>

#include "GLFW/glfw3.h"

#include <bitset>
#include <cstring>
#include <fstream>

#define WIDTH 1000
#define HEIGHT 1000

namespace Tephryte {
    VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT, VkDebugUtilsMessageTypeFlagsEXT, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void*)
    {
        TPR_LOG_STREAM << "[VulkanValidation] " << pCallbackData->pMessage << "\n";
        return VK_FALSE;
    }

    struct QueueFamilyInfo {
        uint32_t queueFamilyGeneralIndex;
        uint32_t queueFamilyGeneralCount;
        uint32_t queueFamilyTransferIndex;
        uint32_t queueFamilyTransferCount;
        uint32_t queueFamilyComputeIndex;
        uint32_t queueFamilyComputeCount;
    };

    static std::vector<char> readFile(const std::string& filename) {
        std::ifstream file(filename, std::ios::ate | std::ios::binary);
        TPR_ASSERT(file.is_open(), "Failed to open file")
        uint64_t file_size = file.tellg();
        std::vector<char> buffer(file_size);
        file.seekg(0);
        file.read(buffer.data(), file_size);

        file.close();

        return buffer;
    }



    VkBackend::VkBackend(const GraphicsSettings& settings, GLFWwindow* window)
    : extensions(settings.extensions), layers(settings.layers) {
        VkResult err;
//
//         VkApplicationInfo app_info { };
//         app_info.sType =                VK_STRUCTURE_TYPE_APPLICATION_INFO;
//         app_info.pApplicationName =     settings.appName;
//         app_info.pEngineName =          "Tephryte";
//         app_info.engineVersion =        VK_MAKE_API_VERSION(0, 1, 0, 0);
//         app_info.apiVersion =           VK_API_VERSION_1_3;
//
//
//         VkInstanceCreateInfo inst_info { };
//         inst_info.sType =               VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
//         inst_info.pApplicationInfo =    &app_info;
//
//         // Add debug utilities
// #ifdef TPR_DEBUG
//         VkDebugUtilsMessengerCreateInfoEXT debug_info { };
//         debug_info.sType =              VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
//         debug_info.messageSeverity =    settings.debugLevel;
//         debug_info.messageType =        settings.debugTypes;
//         debug_info.pfnUserCallback =    debugCallback;
//
//         inst_info.pNext = &debug_info;
//
//         layers.push_back("VK_LAYER_KHRONOS_validation");
//         extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
// #endif //TPR_DEBUG
//
//         // Get required extensions
//         uint32_t glfw_extension_count;
//         const char** glfw_extensions = glfwGetRequiredInstanceExtensions(&glfw_extension_count);
//         extensions.insert(extensions.end(), glfw_extensions, glfw_extensions + glfw_extension_count);
//
//         // Validate layers
//         uint32_t layer_count;
//         vkEnumerateInstanceLayerProperties(&layer_count, nullptr);
//         VkLayerProperties layer_properties[layer_count];
//         vkEnumerateInstanceLayerProperties(&layer_count, layer_properties);
//
//         for (const char* layer : layers) {
//             bool available = false;
//             for (int i = 0; i < layer_count; ++i) {
//                 if (strcmp(layer, layer_properties[i].layerName) == 0){
//                     available = true;
//                     break;
//                 }
//             }
//             TPR_ENGINE_ASSERT(available ,"Requested vulkan layer \"", layer, "\" not available")
//         }
//         inst_info.enabledLayerCount = static_cast<uint32_t>(layers.size());
//         inst_info.ppEnabledLayerNames = layers.data();
//
//         // Validate extensions
//         uint32_t extension_count;
//         vkEnumerateInstanceExtensionProperties(nullptr, &extension_count, nullptr);
//         VkExtensionProperties extension_properties[extension_count];
//         vkEnumerateInstanceExtensionProperties(nullptr, &extension_count, extension_properties);
//
//         for (const char* ext : extensions) {
//             bool available = false;
//             for (int i = 0; i < extension_count; ++i) {
//                 if (strcmp(ext, extension_properties[i].extensionName) == 0){
//                     available = true;
//                     break;
//                 }
//             }
//             TPR_ENGINE_ASSERT(available ,"Requested vulkan extension \"", ext, "\" not available")
//         }
//         inst_info.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
//         inst_info.ppEnabledExtensionNames =  extensions.data();
//
//         //Create instance
//         err = vkCreateInstance(&inst_info, allocator, &instance);
//         TPR_ENGINE_ASSERT(err == VK_SUCCESS, "VkResult code: ", err);
//
//         // Setup vulkan debug messenger
// #ifdef TPR_DEBUG
//         auto create_debug_func = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT"));
//         TPR_ENGINE_ASSERT(create_debug_func != nullptr, "Vulkan debug messenger could not be created")
//         err = create_debug_func(instance, &debug_info, allocator, &debugMessenger);
//         TPR_ENGINE_ASSERT(err == VK_SUCCESS, "VkResult code: ", err);
// #endif //TPR_DEBUG
//
//         // Select GPU
//         uint32_t device_count = 0;
//         vkEnumeratePhysicalDevices(instance, &device_count, nullptr);
//         TPR_ENGINE_ASSERT(device_count > 0, "No gpu with vulkan support found")
//         VkPhysicalDevice physical_devices[device_count];
//         vkEnumeratePhysicalDevices(instance, &device_count, physical_devices);
//
//         gpu = physical_devices[0];
//         for (VkPhysicalDevice physical_device : physical_devices) {
//             VkPhysicalDeviceProperties properties;
//             vkGetPhysicalDeviceProperties(physical_device, &properties);
//         }
        // // Create Surface
        // err = glfwCreateWindowSurface(instance, window, allocator, &surface);
        // TPR_ENGINE_ASSERT(err == VK_SUCCESS, "Failed to create vulkan surface")
        //
        // // Find queue families
        // uint32_t queue_family_count = 0;
        // vkGetPhysicalDeviceQueueFamilyProperties(gpu, &queue_family_count, nullptr);
        // VkQueueFamilyProperties queue_family_properties[queue_family_count];
        // vkGetPhysicalDeviceQueueFamilyProperties(gpu, &queue_family_count, queue_family_properties);
        //
        // uint32_t graphics_queue_family = -1;
        // int index = 0;
        // for (VkQueueFamilyProperties& props : queue_family_properties) {
        //     VkBool32 presentSupport = false;
        //     vkGetPhysicalDeviceSurfaceSupportKHR(gpu, index, surface, &presentSupport);
        //     //TPR_ENGINE_INFO(presentSupport, " ", std::bitset<32>(props.queueFlags))
        //     if ((props.queueFlags & VK_QUEUE_GRAPHICS_BIT) == VK_QUEUE_GRAPHICS_BIT) {
        //         graphics_queue_family = index;
        //     }
        //     // if ((props.queueFlags & ~VK_QUEUE_SPARSE_BINDING_BIT) == VK_QUEUE_TRANSFER_BIT) {
        //     //
        //     // }
        //     // if ((props.queueFlags & (VK_QUEUE_COMPUTE_BIT | VK_QUEUE_GRAPHICS_BIT)) == VK_QUEUE_COMPUTE_BIT) {
        //     //
        //     // }
        //     ++index;
        // }
        //
        // float priority = 1.0f;
        // VkDeviceQueueCreateInfo queue_info{ };
        // queue_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        // queue_info.queueFamilyIndex = graphics_queue_family;
        // queue_info.queueCount = 1;
        // queue_info.pQueuePriorities = &priority;
        //
        // VkPhysicalDeviceFeatures device_features{ };
        // const std::vector<const char*> device_extensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME, VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME };
        //
        // // Validate device extensions
        // uint32_t device_extension_count;
        // vkEnumerateDeviceExtensionProperties(gpu, nullptr, &device_extension_count, nullptr);
        // VkExtensionProperties device_extension_properties[device_extension_count];
        // vkEnumerateDeviceExtensionProperties(gpu, nullptr, &device_extension_count, device_extension_properties);
        //
        // for (const char* ext : device_extensions) {
        //     bool available = false;
        //     for (VkExtensionProperties& props : device_extension_properties) {
        //         if (strcmp(ext, props.extensionName) == 0){
        //             available = true;
        //             break;
        //         }
        //     }
        //     TPR_ENGINE_ASSERT(available, "Requested vulkan device extension \"", ext, "\" not available")
        // }
        //
        // // Create device
        // VkPhysicalDeviceDynamicRenderingFeatures dynamic_rendering_feature{};
        // dynamic_rendering_feature.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DYNAMIC_RENDERING_FEATURES;
        // dynamic_rendering_feature.dynamicRendering = VK_TRUE;
        //
        // VkDeviceCreateInfo device_info{ };
        // device_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        // device_info.queueCreateInfoCount = 1;
        // device_info.pQueueCreateInfos = &queue_info;
        // device_info.pEnabledFeatures = &device_features;
        // device_info.enabledExtensionCount = device_extensions.size();
        // device_info.ppEnabledExtensionNames = device_extensions.data();
        // device_info.pNext = &dynamic_rendering_feature;
        //
        // err = vkCreateDevice(gpu, &device_info, allocator, &device);
        // TPR_ENGINE_ASSERT(err == VK_SUCCESS, "VkResult code: ", err);

        // vkGetDeviceQueue(device, graphics_queue_family, 0, &queue);

        // Get surface capablities
        VkSurfaceCapabilitiesKHR capabilities;
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(gpu, surface, &capabilities);

        // // Query supported surface formats
        // uint32_t surface_format_count;
        // vkGetPhysicalDeviceSurfaceFormatsKHR(gpu, surface, &surface_format_count, nullptr);
        // VkSurfaceFormatKHR surface_formats[surface_format_count];
        // vkGetPhysicalDeviceSurfaceFormatsKHR(gpu, surface, &surface_format_count, surface_formats);
        //
        // // Query supported present modes
        // uint32_t present_mode_count;
        // vkGetPhysicalDeviceSurfacePresentModesKHR(gpu, surface, &present_mode_count, nullptr);
        // VkPresentModeKHR present_modes[present_mode_count];
        // vkGetPhysicalDeviceSurfacePresentModesKHR(gpu, surface, &present_mode_count, present_modes);

        //CHANGE LATER
        VkSurfaceFormatKHR surface_format = {VK_FORMAT_B8G8R8A8_SRGB, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR};
        VkPresentModeKHR present_mode = VK_PRESENT_MODE_FIFO_KHR;
        VkExtent2D swap_extent{WIDTH, HEIGHT};
        uint32_t image_count = 3;

        //Create swapchain
        VkSwapchainCreateInfoKHR swap_info{ };
        swap_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        swap_info.surface = surface;
        swap_info.minImageCount = image_count;
        swap_info.imageFormat = surface_format.format;
        swap_info.imageColorSpace = surface_format.colorSpace;
        swap_info.imageExtent = swap_extent;
        swap_info.imageArrayLayers = 1;
        swap_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
        swap_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE; // CHANGE IF MULTIPLE QUEUES
        swap_info.preTransform = capabilities.currentTransform;
        swap_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        swap_info.presentMode = present_mode;
        swap_info.clipped = VK_TRUE;

        err = vkCreateSwapchainKHR(device, &swap_info, allocator, &swapchain);
        TPR_ENGINE_ASSERT(err == VK_SUCCESS, "VkResult code: ", err);

        swapchainExtent = swap_extent;
        swapchainImageFormat = surface_format.format;

        //Get swapchain images
        vkGetSwapchainImagesKHR(device, swapchain, &image_count, nullptr);
        swapchainImages.resize(image_count);
        vkGetSwapchainImagesKHR(device, swapchain, &image_count, swapchainImages.data());

        swapchainImageCount = image_count;

        // Create image views
        swapchainImageViews.resize(swapchainImageCount);
        for (int i = 0; i < swapchainImageCount; ++i) {
            VkImageViewCreateInfo view_info{};
            view_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            view_info.image = swapchainImages[i];
            view_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
            view_info.format = swapchainImageFormat;
            view_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            view_info.subresourceRange.baseMipLevel = 0;
            view_info.subresourceRange.levelCount = 1;
            view_info.subresourceRange.baseArrayLayer = 0;
            view_info.subresourceRange.layerCount = 1;
            err = vkCreateImageView(device, &view_info, allocator, &swapchainImageViews[i]);
            TPR_ENGINE_ASSERT(err == VK_SUCCESS, "VkResult code: ", err);
        }

        // Create graphics pipeline
        auto vert_shader_code = readFile("shaders/base_UI.vert.spv");
        auto frag_shader_code = readFile("shaders/base_UI.frag.spv");

        VkShaderModule vert_shader_module = createShaderModule(vert_shader_code);
        VkShaderModule frag_shader_module = createShaderModule(frag_shader_code);

        VkPipelineShaderStageCreateInfo vert_stage_info{};
        vert_stage_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        vert_stage_info.stage = VK_SHADER_STAGE_VERTEX_BIT;
        vert_stage_info.module = vert_shader_module;
        vert_stage_info.pName = "main";

        VkPipelineShaderStageCreateInfo frag_stage_info{};
        frag_stage_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        frag_stage_info.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
        frag_stage_info.module = frag_shader_module;
        frag_stage_info.pName = "main";

        VkPipelineShaderStageCreateInfo shader_stages[] = {vert_stage_info, frag_stage_info};

        std::vector dynamic_states = {
            VK_DYNAMIC_STATE_VIEWPORT,
            VK_DYNAMIC_STATE_SCISSOR
        };

        VkPipelineDynamicStateCreateInfo dynamic_state{};
        dynamic_state.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
        dynamic_state.dynamicStateCount = dynamic_states.size();
        dynamic_state.pDynamicStates = dynamic_states.data();

        VkPipelineVertexInputStateCreateInfo vertex_input_info{};
        vertex_input_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        vertex_input_info.vertexBindingDescriptionCount = 0;
        vertex_input_info.vertexAttributeDescriptionCount = 0;

        VkPipelineInputAssemblyStateCreateInfo input_assembly{};
        input_assembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        input_assembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        input_assembly.primitiveRestartEnable = VK_FALSE;

        VkViewport viewport{};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = static_cast<float>(swapchainExtent.width);
        viewport.height = static_cast<float>(swapchainExtent.height);
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;

        VkRect2D scissor{};
        scissor.offset = {0, 0};
        scissor.extent = swapchainExtent;

        VkPipelineViewportStateCreateInfo viewport_state{};
        viewport_state.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        viewport_state.viewportCount = 1;
        viewport_state.pViewports = &viewport;
        viewport_state.scissorCount = 1;
        viewport_state.pScissors = &scissor;

        VkPipelineRasterizationStateCreateInfo rasterizer_info{};
        rasterizer_info.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        rasterizer_info.depthClampEnable = VK_FALSE;
        rasterizer_info.rasterizerDiscardEnable = VK_FALSE;
        rasterizer_info.polygonMode = VK_POLYGON_MODE_FILL;
        rasterizer_info.lineWidth = 1.0f;
        rasterizer_info.cullMode = VK_CULL_MODE_BACK_BIT;
        rasterizer_info.frontFace = VK_FRONT_FACE_CLOCKWISE;
        rasterizer_info.depthBiasEnable = VK_FALSE;

        VkPipelineMultisampleStateCreateInfo multisample_info{};
        multisample_info.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        multisample_info.sampleShadingEnable = VK_FALSE;
        multisample_info.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

        VkPipelineColorBlendAttachmentState color_blend_attachment{};
        color_blend_attachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
        color_blend_attachment.blendEnable = VK_FALSE;

        VkPipelineColorBlendStateCreateInfo color_blend{};
        color_blend.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        color_blend.logicOpEnable = VK_FALSE;
        color_blend.attachmentCount = 1;
        color_blend.pAttachments = &color_blend_attachment;

        VkPipelineLayoutCreateInfo pipeline_layout_info{}; // shader uniform stuff
        pipeline_layout_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        err = vkCreatePipelineLayout(device, &pipeline_layout_info, allocator, &pipelineLayout);
        TPR_ENGINE_ASSERT(err == VK_SUCCESS, "VkResult code: ", err);

        VkPipelineRenderingCreateInfo pipeline_rendering_info{};
        pipeline_rendering_info.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO;
        pipeline_rendering_info.colorAttachmentCount = 1;
        pipeline_rendering_info.pColorAttachmentFormats = &swapchainImageFormat;

        VkGraphicsPipelineCreateInfo pipeline_info{};
        pipeline_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        pipeline_info.pNext = &pipeline_rendering_info;
        pipeline_info.stageCount = 2;
        pipeline_info.pStages = shader_stages;
        pipeline_info.pVertexInputState = &vertex_input_info;
        pipeline_info.pInputAssemblyState = &input_assembly;
        pipeline_info.pViewportState = &viewport_state;
        pipeline_info.pRasterizationState = &rasterizer_info;
        pipeline_info.pMultisampleState = &multisample_info;
        pipeline_info.pColorBlendState = &color_blend;
        pipeline_info.pDynamicState = &dynamic_state;
        pipeline_info.layout = pipelineLayout;
        err = vkCreateGraphicsPipelines(device, nullptr, 1, &pipeline_info, allocator, &graphicsPipeline);
        TPR_ENGINE_ASSERT(err == VK_SUCCESS, "VkResult code: ", err);

        vkDestroyShaderModule(device, frag_shader_module, allocator);
        vkDestroyShaderModule(device, vert_shader_module, allocator);

        //Create command pool
        VkCommandPoolCreateInfo command_pool_info{};
        command_pool_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        command_pool_info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
        vkCreateCommandPool(device, &command_pool_info, allocator, &commandPool);
        TPR_ENGINE_ASSERT(err == VK_SUCCESS, "VkResult code: ", err);

        commandBuffers.resize(swapchainImageCount);
        VkCommandBufferAllocateInfo cmd_bufffer_allocate_info{};
        cmd_bufffer_allocate_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        cmd_bufffer_allocate_info.commandPool = commandPool;
        cmd_bufffer_allocate_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        cmd_bufffer_allocate_info.commandBufferCount = swapchainImageCount;
        vkAllocateCommandBuffers(device, &cmd_bufffer_allocate_info, commandBuffers.data());
        TPR_ENGINE_ASSERT(err == VK_SUCCESS, "VkResult code: ", err);

        imageAvailableSemaphores.resize(swapchainImageCount);
        renderFinishedSemaphores.resize(swapchainImageCount);
        inFlightFences.resize(swapchainImageCount);

        VkSemaphoreCreateInfo semaphoreInfo{};
        semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

        VkFenceCreateInfo fenceInfo{};
        fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT; // so it does not block first frame rendering

        for (size_t i = 0; i < swapchainImageCount; i++) {
            err = vkCreateSemaphore(device, &semaphoreInfo, allocator, &imageAvailableSemaphores[i]);
            TPR_ENGINE_ASSERT(err == VK_SUCCESS, "VkResult code: ", err);
            err = vkCreateSemaphore(device, &semaphoreInfo, allocator, &renderFinishedSemaphores[i]);
            TPR_ENGINE_ASSERT(err == VK_SUCCESS, "VkResult code: ", err);
            err = vkCreateFence(device, &fenceInfo, allocator, &inFlightFences[i]);
            TPR_ENGINE_ASSERT(err == VK_SUCCESS, "VkResult code: ", err);

        }
    }

    VkBackend::~VkBackend() {
        for (size_t i = 0; i < swapchainImageCount; i++) {
            vkDestroySemaphore(device, renderFinishedSemaphores[i], allocator);
            vkDestroySemaphore(device, imageAvailableSemaphores[i], allocator);
            vkDestroyFence(device, inFlightFences[i], allocator);
        }
        for (auto image_view : swapchainImageViews) {
            vkDestroyImageView(device, image_view, allocator);
        }
        vkDestroyCommandPool(device, commandPool, allocator);
        vkDestroyPipeline(device, graphicsPipeline, allocator);
        vkDestroyPipelineLayout(device, pipelineLayout, allocator);
        vkDestroySwapchainKHR(device, swapchain, allocator);
        vkDestroySurfaceKHR(instance, surface, allocator);
        vkDestroyDevice(device, allocator);

        #ifdef TPR_DEBUG
        auto destroy_debug_func = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT"));
        if(destroy_debug_func != nullptr) {
            destroy_debug_func(instance, debugMessenger, allocator);
        }
        #endif //TPR_DEBUG

        vkDestroyInstance(instance, allocator);
    }

    VkShaderModule VkBackend::createShaderModule(const std::vector<char> &code) {
        VkShaderModuleCreateInfo shader_info{};
        shader_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        shader_info.codeSize = code.size();
        shader_info.pCode = reinterpret_cast<const uint32_t*>(code.data());

        VkShaderModule shader_module;
        VkResult err = vkCreateShaderModule(device, &shader_info, allocator, &shader_module);
        TPR_ENGINE_ASSERT(err == VK_SUCCESS, "VkResult code: ", err);
        return shader_module;
    }

    void VkBackend::draw() {
        VkResult err;

        vkWaitForFences(device, 1, &inFlightFences[swapCurrentFrame], VK_TRUE, UINT64_MAX);
        vkResetFences(device, 1, &inFlightFences[swapCurrentFrame]);

        uint32_t imageIndex;
        vkAcquireNextImageKHR(device, swapchain, UINT64_MAX, imageAvailableSemaphores[swapCurrentFrame], nullptr, &imageIndex);
        VkCommandBuffer command_buffer = commandBuffers[swapCurrentFrame];
        vkResetCommandBuffer(command_buffer, 0);

        VkCommandBufferBeginInfo cmd_buffer_begin_info{};
        cmd_buffer_begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        err = vkBeginCommandBuffer(command_buffer, &cmd_buffer_begin_info);
        TPR_ENGINE_ASSERT(err == VK_SUCCESS, "VkResult code: ", err);

        VkImageSubresourceRange subresource_range{};
        subresource_range.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        subresource_range.baseMipLevel = 0;
        subresource_range.levelCount = 1;
        subresource_range.baseArrayLayer = 0;
        subresource_range.layerCount = 1;

        VkImageMemoryBarrier image_memory_barrier_1{};
        image_memory_barrier_1.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        image_memory_barrier_1.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
        image_memory_barrier_1.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        image_memory_barrier_1.newLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        image_memory_barrier_1.image = swapchainImages[swapCurrentFrame];
        image_memory_barrier_1.subresourceRange = subresource_range;

        vkCmdPipelineBarrier(
            command_buffer,
            VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
            VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
            0,
            0,
            nullptr,
            0,
            nullptr,
            1,
            &image_memory_barrier_1);

        VkRenderingAttachmentInfo color_attachment_info{ };
        color_attachment_info.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
        color_attachment_info.clearValue.color = {0, 0, 0 ,0};
        color_attachment_info.imageLayout = VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL;
        color_attachment_info.imageView = swapchainImageViews[swapCurrentFrame];
        color_attachment_info.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        color_attachment_info.storeOp = VK_ATTACHMENT_STORE_OP_STORE;

        VkRenderingInfo render_info{ };
        render_info.sType = VK_STRUCTURE_TYPE_RENDERING_INFO;
        render_info.colorAttachmentCount = 1;
        render_info.layerCount = 1;
        render_info.pColorAttachments = &color_attachment_info;
        render_info.pDepthAttachment = nullptr;
        render_info.renderArea = VkRect2D{{0, 0}, {swapchainExtent.width, swapchainExtent.height}};

        vkCmdBeginRendering(command_buffer, &render_info);

        VkViewport viewport{0, 0, HEIGHT, WIDTH};
        vkCmdSetViewport(command_buffer, 0, 1, &viewport);

        VkRect2D scissor{};
        scissor.extent = swapchainExtent;
        scissor.offset = {0,0};
        vkCmdSetScissor(command_buffer, 0, 1, &scissor);

        vkCmdBindPipeline(command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);

        vkCmdDraw(command_buffer, 3, 1, 0, 0);

        vkCmdEndRendering(command_buffer);
        VkImageMemoryBarrier image_memory_barrier_2 {};
        image_memory_barrier_2.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        image_memory_barrier_2.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
        image_memory_barrier_2.oldLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        image_memory_barrier_2.newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
        image_memory_barrier_2.image = swapchainImages[swapCurrentFrame];
        image_memory_barrier_2.subresourceRange = subresource_range;

        vkCmdPipelineBarrier(
            command_buffer,
            VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
            VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
            0,
            0,
            nullptr,
            0,
            nullptr,
            1,
            &image_memory_barrier_2);

        err = vkEndCommandBuffer(command_buffer);
        TPR_ENGINE_ASSERT(err == VK_SUCCESS, "VkResult code: ", err);

        VkSemaphore wait_semaphores[] = {imageAvailableSemaphores[swapCurrentFrame]};
        VkSemaphore signal_semaphores[] = {renderFinishedSemaphores[swapCurrentFrame]};
        VkPipelineStageFlags wait_stages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};

        VkSubmitInfo submit_info{};
        submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submit_info.waitSemaphoreCount = 1;
        submit_info.pWaitSemaphores = wait_semaphores;
        submit_info.pWaitDstStageMask = wait_stages;
        submit_info.commandBufferCount = 1;
        submit_info.pCommandBuffers = &command_buffer;
        submit_info.signalSemaphoreCount = 1;
        submit_info.pSignalSemaphores = signal_semaphores;

        err = vkQueueSubmit(queue, 1, &submit_info, inFlightFences[swapCurrentFrame]);
        TPR_ENGINE_ASSERT(err == VK_SUCCESS, "VkResult code: ", err);

        VkPresentInfoKHR present_info{};
        present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        present_info.waitSemaphoreCount = 1;
        present_info.pWaitSemaphores = signal_semaphores;
        VkSwapchainKHR swapChains[] = {swapchain};
        present_info.swapchainCount = 1;
        present_info.pSwapchains = swapChains;
        present_info.pImageIndices = &imageIndex;
        present_info.pResults = nullptr;

        vkQueuePresentKHR(queue, &present_info);

        swapCurrentFrame = (swapCurrentFrame + 1) % swapchainImageCount;
    }
}

