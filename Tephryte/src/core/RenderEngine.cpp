//
// Created by Timothy on 8/6/2024.
//

#include "Debug.h"
#include "RenderEngine.h"


namespace Tephryte {

    void RenderEngine::init(const Config& config)
    {
        std::vector<const char*> requiredInstanceExtensions = { };
        std::vector<const char*> requiredDeviceExtensions = { "VK_KHR_swapchain", "VK_KHR_dynamic_rendering" };

        for (auto ext: config.window->getRequiredVulkanInstanceExtensions()) {
            requiredInstanceExtensions.push_back(ext);
        }

        for (auto ext: config.instanceExtensions) {
            requiredInstanceExtensions.push_back(ext);
        }
        for (auto ext: config.deviceExtensions) {
            requiredDeviceExtensions.push_back(ext);
        }

        instance = VulkanImpl::createVulkanInstance(
            requiredInstanceExtensions,
            config.layers,
            allocator,
            &debugMessenger);

        std::vector<VkPhysicalDevice> gpus = VulkanImpl::getPhysicalDevices(
            instance,
            config.features,
            requiredDeviceExtensions);

        if (gpus.empty()) {
            vkDestroyInstance(instance, allocator);
            return;
        }

        if (config.useSpecificGpu) {
            gpu = VulkanImpl::getPhysicalDeviceByID(gpus, config.specificGpuID);
        }
        else {
            gpu = VulkanImpl::getPhysicalDeviceByPreferredType(gpus, config.preferredGpuType);
        }

        gpu = VulkanImpl::getPhysicalDeviceByPreferredType(gpus, config.preferredGpuType);
        device = VulkanImpl::createDevice(gpu, requiredDeviceExtensions, config.features, allocator, queues);

        surface = config.window->getSurface(instance);
        VkSurfaceCapabilitiesKHR capabilities;
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(gpu, surface, &capabilities);

        VkExtent2D display_resolution = capabilities.currentExtent;

        swapchain = VulkanImpl::createSwapchain(device, gpu, surface, display_resolution,
            config.swapchainConfig.vSyncMode, config.swapchainConfig.imageFormat, config.swapchainConfig.colorSpace,
            config.swapchainConfig.minImageCount, allocator, swapImages);

        swapImageCount = swapImages.size();

        swapImageViews.resize(swapImageCount);
        for (int i = 0; i < swapImageCount; ++i) {
            swapImageViews[i] = VulkanImpl::createImageView(
                device,
                swapImages[i],
                VK_IMAGE_VIEW_TYPE_2D,
                VK_IMAGE_ASPECT_COLOR_BIT,
                1,
                config.swapchainConfig.imageFormat,
                allocator);
        }

        renderResolution = config.renderResolution;

        viewport = {
            .x = 0.0f,
            .y = 0.0f,
            .width = static_cast<float>(renderResolution.width),
            .height = static_cast<float>(renderResolution.height),
            .minDepth = 0.0f,
            .maxDepth = 1.0f
        };

        MSAASamples = config.MSAASamples;
        scissor = {{0,0}, renderResolution};

        frameInfos.resize(concurrentFrames);
        for (int i = 0; i < concurrentFrames; ++i) {
            Frame& frame = frameInfos[i];
            frame.swapImageAvailable = VulkanImpl::createSemaphore(device, allocator);
            frame.frameAvailable = VulkanImpl::createFence(device, true, allocator);

            frame.cmdPool = VulkanImpl::createCommandPool(device, 0, queues.graphicsFamilyIndex, allocator);
            frame.cmdBuffer = VulkanImpl::createCommandBuffer(device, frame.cmdPool, VK_COMMAND_BUFFER_LEVEL_PRIMARY);
        }

        internalCmdPool = VulkanImpl::createCommandPool(device, 0, queues.graphicsFamilyIndex, allocator);
        createSceneBuffer(config);

        blendInfo = {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
            .logicOpEnable = VK_FALSE,
            .logicOp = VK_LOGIC_OP_COPY,
            .attachmentCount = 1,
            .pAttachments = &sceneBuffer.blendInfo,
        };

        presentCmdPool = VulkanImpl::createCommandPool(device, 0, queues.graphicsFamilyIndex, allocator);
        createPresentCommandBuffers(config);

        vkDestroyDebugUtilsMessengerEXT = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT"));

        freeUsedBuffers();
    }

    void RenderEngine::createSceneBuffer(const Config& config) {
        sceneBuffer.sceneRenderComplete = VulkanImpl::createSemaphore(device, allocator);
        sceneBuffer.sceneTransfered = VulkanImpl::createSemaphore(device, allocator);

        sceneBuffer.colorImageFormat = config.swapchainConfig.imageFormat;
        sceneBuffer.depthImageFormat = VK_FORMAT_D24_UNORM_S8_UINT;

        sceneBuffer.blendInfo = {
            .blendEnable = config.colorBlending,
            .srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA,
            .dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
            .colorBlendOp = VK_BLEND_OP_ADD,
            .srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE,
            .dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO,
            .alphaBlendOp = VK_BLEND_OP_ADD,
            .colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT
        };

        sceneBuffer.colorImage = VulkanImpl::createAttachmentImage(
            device,
            gpu,
            sceneBuffer.colorImageFormat,
            config.renderResolution,
            config.MSAASamples,
            VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
            allocator,
            sceneBuffer.colorMemory);

        transitionImageLayout(sceneBuffer.colorImage, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);

        sceneBuffer.depthImage = VulkanImpl::createAttachmentImage(
            device,
            gpu,
            sceneBuffer.depthImageFormat,
            config.renderResolution,
            config.MSAASamples,
            VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
            allocator,
            sceneBuffer.colorMemory);

        sceneBuffer.colorImageView = VulkanImpl::createImageView(
            device,
            sceneBuffer.colorImage,
            VK_IMAGE_VIEW_TYPE_2D,
            VK_IMAGE_ASPECT_COLOR_BIT,
            1,
            sceneBuffer.colorImageFormat,
            allocator);

        sceneBuffer.depthImageView = VulkanImpl::createImageView(
            device,
            sceneBuffer.depthImage,
            VK_IMAGE_VIEW_TYPE_2D,
            VK_IMAGE_ASPECT_DEPTH_BIT,
            1,
            sceneBuffer.depthImageFormat,
            allocator);

        sceneBuffer.colorAttachmentInfo = {
            .sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO,
            .imageView = sceneBuffer.colorImageView,
            .imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
            .resolveMode = VK_RESOLVE_MODE_NONE,
            .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
            .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
        };
        sceneBuffer.colorAttachmentInfo.clearValue.color = config.clearColor;

        sceneBuffer.depthAttachmentInfo = {
            .sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO,
            .imageView = sceneBuffer.depthImageView,
            .imageLayout = VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL,
            .resolveMode = VK_RESOLVE_MODE_NONE,
            .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
            .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
        };
        sceneBuffer.depthAttachmentInfo.clearValue.depthStencil = { 1.0f, 0 };

    }

    VkShaderModule RenderEngine::createShaderModule (std::vector<char> shaderCode) {
        VkShaderModuleCreateInfo info = {
            .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
            .codeSize = shaderCode.size(),
            .pCode = reinterpret_cast<const uint32_t*>(shaderCode.data())
        };

        VkShaderModule module;
        VkResult err = vkCreateShaderModule(device, &info, allocator, &module);
        CHECK_VK_RESULT(err)

        return module;
    }

    VkPipeline RenderEngine::createGraphicsPipeline(PipielineInfo info)
    {
        VkPipelineShaderStageCreateInfo vertex_info = {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
            .stage = VK_SHADER_STAGE_VERTEX_BIT,
            .module = info.vertexShader,
            .pName = "main",
            .pSpecializationInfo = info.vertexConstants
        };

        VkPipelineShaderStageCreateInfo fragment_info = {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
            .stage = VK_SHADER_STAGE_FRAGMENT_BIT,
            .module = info.fragmentShader,
            .pName = "main",
            .pSpecializationInfo = info.fragmentConstants
        };

        std::vector<VkPipelineShaderStageCreateInfo> shader_infos(2);
        shader_infos[0] = vertex_info;
        shader_infos[1] = fragment_info;


        std::vector<VkDynamicState> dynamic_states = {
            VK_DYNAMIC_STATE_VIEWPORT,
        };

        VkPipelineDynamicStateCreateInfo dynamic_info = {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
            .dynamicStateCount = static_cast<uint32_t>(dynamic_states.size()),
            .pDynamicStates = dynamic_states.data(),
        };

        VkPipelineRenderingCreateInfo pipeline_rendering_info = {
            .sType =                            VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO,
            .colorAttachmentCount =             1,
            .pColorAttachmentFormats =          &sceneBuffer.colorImageFormat,
            .depthAttachmentFormat =            sceneBuffer.depthImageFormat
        };

        VkPipelineVertexInputStateCreateInfo vertex_input = {
            .sType =                            VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
            .vertexBindingDescriptionCount =    static_cast<uint32_t>(info.vertexBindings.size()),
            .pVertexBindingDescriptions =       info.vertexBindings.data(),
            .vertexAttributeDescriptionCount =  static_cast<uint32_t>(info.vertexAttributes.size()),
            .pVertexAttributeDescriptions =     info.vertexAttributes.data()
        };

        VkPipelineInputAssemblyStateCreateInfo input_assembly = {
            .sType =                            VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
            .topology =                         info.topology,
            .primitiveRestartEnable =           info.primitiveRestart
        };

        VkPipelineViewportStateCreateInfo viewport_state{
            .sType =                            VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
            .viewportCount =                    1,
            .pViewports =                       &viewport,
            .scissorCount =                     1,
            .pScissors =                        &scissor
        };

        VkPipelineRasterizationStateCreateInfo rasterizer_info{
            .sType =                            VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
            .depthClampEnable =                 info.depthClamp,
            .rasterizerDiscardEnable =          VK_FALSE,
            .polygonMode =                      info.polygonMode,
            .cullMode =                         info.cullMode,
            .frontFace =                        info.frontFace,
            .depthBiasEnable =                  VK_FALSE,
            .lineWidth =                        1.0f,
        };

        VkPipelineMultisampleStateCreateInfo multisample_info = {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
            .rasterizationSamples = MSAASamples,
            .sampleShadingEnable = VK_FALSE,
            .minSampleShading = VK_SAMPLE_COUNT_1_BIT,
            .pSampleMask = nullptr,
            .alphaToCoverageEnable = VK_FALSE,
            .alphaToOneEnable = VK_FALSE
        };

        VkPipelineDepthStencilStateCreateInfo depth_stencil_info = {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
            .depthTestEnable = 1,
            .depthWriteEnable = 1,
            .depthCompareOp = VK_COMPARE_OP_LESS,
            .depthBoundsTestEnable = 0,
            .stencilTestEnable = VK_FALSE,
            // .front = VK_STENCIL_OP_KEEP,
            // .back = VK_STENCIL_OP_KEEP,
            .minDepthBounds = 0.0f,
            .maxDepthBounds = 1.0f
        };


        VkPipelineLayoutCreateInfo layout_info = {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
            .setLayoutCount = static_cast<uint32_t>(info.uniformLayouts.size()),
            .pSetLayouts = info.uniformLayouts.data(),
            .pushConstantRangeCount = static_cast<uint32_t>(info.pushConstants.size()),
            .pPushConstantRanges = info.pushConstants.data()
        };
        VkPipelineLayout layout;
        VkResult err = vkCreatePipelineLayout(device, &layout_info, nullptr, &layout);
        CHECK_VK_RESULT(err);

        VkGraphicsPipelineCreateInfo pipeline_info{
            .sType =                VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
            .pNext =                &pipeline_rendering_info,
            .flags =                VK_PIPELINE_LAYOUT_CREATE_INDEPENDENT_SETS_BIT_EXT,
            .stageCount =           static_cast<uint32_t>(shader_infos.size()),
            .pStages =              shader_infos.data(),
            .pVertexInputState =    &vertex_input,
            .pInputAssemblyState =  &input_assembly,
            .pTessellationState =   nullptr,
            .pViewportState =       &viewport_state,
            .pRasterizationState =  &rasterizer_info,
            .pMultisampleState =    &multisample_info,
            .pDepthStencilState =   &depth_stencil_info,
            .pColorBlendState =     &blendInfo,
            .pDynamicState =        &dynamic_info,
            .layout =               layout
        };

        VkPipeline pipeline;
        err = vkCreateGraphicsPipelines(device, nullptr, 1, &pipeline_info, allocator, &pipeline);
        CHECK_VK_RESULT(err)

        vkDestroyShaderModule(device, info.vertexShader, allocator);
        vkDestroyShaderModule(device, info.fragmentShader, allocator);

        return pipeline;
    }

    void RenderEngine::recordRenderCommandBuffer(VkPipeline pipeline) {
        Frame frame = frameInfos[currentFrame];

        vkWaitForFences(device, 1, &frame.frameAvailable, true, UINT64_MAX);
        vkResetFences(device, 1, &frame.frameAvailable);

        VkResult err = vkResetCommandPool(device, frame.cmdPool, 0);
        CHECK_VK_RESULT(err)

        VkCommandBufferBeginInfo buffer_info = { .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };
        err = vkBeginCommandBuffer(frame.cmdBuffer, &buffer_info);
        CHECK_VK_RESULT(err)

        VkRenderingInfo rendering_info = {
            .sType = VK_STRUCTURE_TYPE_RENDERING_INFO,
            .renderArea = {{0, 0}, renderResolution},
            .layerCount = 1,
            .colorAttachmentCount = 1,
            .pColorAttachments = &sceneBuffer.colorAttachmentInfo,
            .pDepthAttachment = &sceneBuffer.depthAttachmentInfo
        };
        vkCmdBeginRendering(frame.cmdBuffer, &rendering_info);

        vkCmdSetViewport(frame.cmdBuffer, 0, 1, &viewport);
        vkCmdSetScissor(frame.cmdBuffer, 0, 1, &scissor);

        vkCmdBindPipeline(frame.cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
        vkCmdDraw(frame.cmdBuffer, 3, 1, 0, 0);

        vkCmdEndRendering(frame.cmdBuffer);

        err = vkEndCommandBuffer(frame.cmdBuffer);
        CHECK_VK_RESULT(err)
    }

    void RenderEngine::render() {
        Frame frame = frameInfos[currentFrame];

        VkPipelineStageFlags wait_stage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        VkSubmitInfo submit_info = {
            .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
            .waitSemaphoreCount = 0,
            .pWaitSemaphores = nullptr,
            .pWaitDstStageMask = &wait_stage,
            .commandBufferCount = 1,
            .pCommandBuffers = &frame.cmdBuffer,
            .signalSemaphoreCount = 1,
            .pSignalSemaphores = &sceneBuffer.sceneRenderComplete
        };

        VkResult err = vkQueueSubmit(queues.graphicsQueue, 1, &submit_info, frame.frameAvailable);
        CHECK_VK_RESULT(err)
    }

    void RenderEngine::presentScene() {
        Frame frame = frameInfos[currentFrame];

        uint32_t currentSwapchainImage;
        VkResult err = vkAcquireNextImageKHR(device, swapchain, UINT64_MAX, frame.swapImageAvailable, nullptr, &currentSwapchainImage);

        if((err == VK_ERROR_OUT_OF_DATE_KHR) || (err == VK_SUBOPTIMAL_KHR)) {
            TPR_ENGINE_ERROR("aaaaa")
            TPR_EXIT
            //RELOAD SWAPCHAIN
        }

        VkSemaphore wait_semaphores[] = {frame.swapImageAvailable, sceneBuffer.sceneRenderComplete};
        VkPipelineStageFlags wait_stages = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        VkSemaphore signal_semaphores[] = {sceneBuffer.sceneTransfered};

        VkSubmitInfo submit_info = {
            .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
            .waitSemaphoreCount = 2,
            .pWaitSemaphores = wait_semaphores,
            .pWaitDstStageMask = &wait_stages,
            .commandBufferCount = 1,
            .pCommandBuffers = &presentCmdBuffers[currentSwapchainImage],
            .signalSemaphoreCount = 1,
            .pSignalSemaphores = signal_semaphores
        };

        err = vkQueueSubmit(queues.graphicsQueue, 1, &submit_info, nullptr);
        CHECK_VK_RESULT(err)

        VkPresentInfoKHR present_info = {
            .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
            .waitSemaphoreCount = 1,
            .pWaitSemaphores = &sceneBuffer.sceneTransfered,
            .swapchainCount = 1,
            .pSwapchains = &swapchain,
            .pImageIndices = &currentSwapchainImage,
            .pResults = &err

        };
        err =vkQueuePresentKHR(queues.graphicsQueue, &present_info);
        CHECK_VK_RESULT(err);

        currentFrame = (currentFrame + 1) % concurrentFrames;
    }

    void RenderEngine::createPresentCommandBuffers(const Config& config) {
        constexpr VkImageSubresourceLayers subresource_layers = {
            .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
            .mipLevel = 0,
            .baseArrayLayer = 0,
            .layerCount = 1
        };

        VkImageCopy image_copy = {
            .srcSubresource = subresource_layers,
            .srcOffset = { 0 },
            .dstSubresource = subresource_layers,
            .dstOffset = { 0 },
            .extent = { renderResolution.width, renderResolution.height, 1},
        };

        VkImageResolve image_resolve = {
            .srcSubresource = subresource_layers,
            .srcOffset = { 0 },
            .dstSubresource = subresource_layers,
            .dstOffset = { 0 },
            .extent = { renderResolution.width, renderResolution.height, 1},
        };

        constexpr VkImageSubresourceRange subresource_range = {
            .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
            .baseMipLevel = 0,
            .levelCount = 1,
            .baseArrayLayer = 0,
            .layerCount = 1
        };

        VkResult err = vkResetCommandPool(device, presentCmdPool, 0);
        CHECK_VK_RESULT(err)

        presentCmdBuffers.resize(swapImageCount);
        for (int i = 0; i < swapImageCount; ++i) {
            presentCmdBuffers[i] = VulkanImpl::createCommandBuffer(device, presentCmdPool, VK_COMMAND_BUFFER_LEVEL_PRIMARY);

            VkCommandBufferBeginInfo begin_info = {
                .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO
            };
            err = vkBeginCommandBuffer(presentCmdBuffers[i], &begin_info);
            CHECK_VK_RESULT(err)

            VkImageMemoryBarrier sceneToTransferSrcBarrier = {
                .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
                .srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
                .dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT,
                .oldLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
                .newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
                .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
                .image = sceneBuffer.colorImage,
                .subresourceRange = subresource_range
            };

            vkCmdPipelineBarrier(
                presentCmdBuffers[i],
                VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                VK_PIPELINE_STAGE_TRANSFER_BIT,
                0, 0, nullptr, 0, nullptr,
                1,
                &sceneToTransferSrcBarrier);

            VkImageMemoryBarrier swapToTransferDstBarrier = {
                .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
                .srcAccessMask = VK_ACCESS_NONE,
                .dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT,
                .oldLayout = VK_IMAGE_LAYOUT_UNDEFINED,
                .newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
                .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
                .image = swapImages[i],
                .subresourceRange = subresource_range
            };

            vkCmdPipelineBarrier(
                presentCmdBuffers[i],
                VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
                VK_PIPELINE_STAGE_TRANSFER_BIT,
                0, 0, nullptr, 0, nullptr,
                1,
                &swapToTransferDstBarrier);

            if (MSAASamples == VK_SAMPLE_COUNT_1_BIT) {
                vkCmdCopyImage(
                presentCmdBuffers[i],
                sceneBuffer.colorImage,
                VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                swapImages[i],
                VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                1, &image_copy);
            } else {
                vkCmdResolveImage(
                presentCmdBuffers[i],
                sceneBuffer.colorImage,
                VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                swapImages[i],
                VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                1, &image_resolve);
            }

            VkImageMemoryBarrier sceneToColorAttachmentBarrier = {
                .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
                .srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT,
                .dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
                .oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                .newLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
                .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
                .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
                .image = sceneBuffer.colorImage,
                .subresourceRange = subresource_range
            };

            vkCmdPipelineBarrier(
                presentCmdBuffers[i],
                VK_PIPELINE_STAGE_TRANSFER_BIT,
                VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                0, 0, nullptr, 0, nullptr,
                1,
                &sceneToColorAttachmentBarrier);

            VkImageMemoryBarrier swapToPresentBarrier = {
                .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
                .srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT,
                .oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                .newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
                .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
                .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
                .image = swapImages[i],
                .subresourceRange = subresource_range
            };

            vkCmdPipelineBarrier(
                presentCmdBuffers[i],
                VK_PIPELINE_STAGE_TRANSFER_BIT,
                VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
                0, 0, nullptr, 0, nullptr,
                1,
                &swapToPresentBarrier);

            err = vkEndCommandBuffer(presentCmdBuffers[i]);
            CHECK_VK_RESULT(err);
        }
    }

    VkCommandBuffer RenderEngine::beginSingleUseCommands() {
        VkCommandBuffer buffer = VulkanImpl::createCommandBuffer(device, internalCmdPool, VK_COMMAND_BUFFER_LEVEL_PRIMARY);

        VkCommandBufferBeginInfo begin_info = {
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
            .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT
        };
        vkBeginCommandBuffer(buffer, &begin_info);

        return buffer;
    }

    void RenderEngine::endSingleUseCommands(VkCommandBuffer buffer, VkQueue queue) {
        VkResult err = vkEndCommandBuffer(buffer);
        CHECK_VK_RESULT(err);

        VkSubmitInfo submit_info = {
            .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
            .commandBufferCount = 1,
            .pCommandBuffers = &buffer
        };
        vkQueueSubmit(queue, 1, &submit_info, nullptr);
        usedInternalCmdBuffers.push_back(buffer);
    }

    void RenderEngine::transitionImageLayout(VkImage image, VkImageLayout oldLayout, VkImageLayout newLayout) {

        constexpr VkImageSubresourceRange subresource_range = {
            .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
            .baseMipLevel = 0,
            .levelCount = 1,
            .baseArrayLayer = 0,
            .layerCount = 1
        };

        VkImageMemoryBarrier image_layout_barrier = {
            .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
            .oldLayout = oldLayout,
            .newLayout = newLayout,
            .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
            .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
            .image = image,
            .subresourceRange = subresource_range
        };

        VkPipelineStageFlags sourceStage;
        VkPipelineStageFlags destinationStage;

        if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL) {
            image_layout_barrier.srcAccessMask = VK_ACCESS_NONE;
            image_layout_barrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
            sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
            destinationStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

        } else if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL) {
            image_layout_barrier.srcAccessMask = VK_ACCESS_NONE;
            image_layout_barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
            sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
            destinationStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;

        } else if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
            image_layout_barrier.srcAccessMask = VK_ACCESS_NONE;
            image_layout_barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
            destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;

        } else {
            TPR_ENGINE_ERROR("Image layout transition dependencies not defined");
            return;
        }

        VkCommandBuffer buffer = beginSingleUseCommands();

        vkCmdPipelineBarrier(
            buffer,
            sourceStage,
            destinationStage,
            0, 0, nullptr, 0, nullptr,
            1,
            &image_layout_barrier);

        endSingleUseCommands(buffer, queues.graphicsQueue);
    }

    void RenderEngine::freeUsedBuffers() {
        vkQueueWaitIdle(queues.graphicsQueue);
        vkQueueWaitIdle(queues.computeQueue);
        vkQueueWaitIdle(queues.transferQueue);

        vkFreeCommandBuffers(device, internalCmdPool, usedInternalCmdBuffers.size(), usedInternalCmdBuffers.data());
        vkResetCommandPool(device, internalCmdPool, VK_COMMAND_POOL_RESET_RELEASE_RESOURCES_BIT);
        usedInternalCmdBuffers.clear();
    }

    RenderEngine::~RenderEngine() {
        vkDeviceWaitIdle(device);

        //ADD SYNC OBJECTS

        vkDestroyDevice(device, allocator);

        if (surface != nullptr) {
            vkDestroySurfaceKHR(instance, surface, allocator);
        }
#ifdef TPR_DEBUG
        vkDestroyDebugUtilsMessengerEXT(instance, debugMessenger, allocator);
#endif // TPR_DEBUG
        vkDestroyInstance(instance, allocator);
    }
}

