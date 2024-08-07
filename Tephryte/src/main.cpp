#include <bitset>
#include <vector>
#include "Application.h"
#include "core/Log.h"
#include "core/VulkanRenderer.h"
#include "glm/glm.hpp"

struct test {
    int a = 1;
    long b = 2;
    bool c = true;
};

int main() {

    VkInstance instance = Tephryte::VulkanImpl::createVulkanInstance({ }, { }, nullptr);
    VkPhysicalDeviceFeatures fet = { 0 };

    VkPhysicalDevice dev = Tephryte::VulkanImpl::selectPhysicalDevice(instance, VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU, fet);

    Tephryte::VulkanImpl::DeviceInfo a = Tephryte::VulkanImpl::createDevice(dev, {}, {}, nullptr);



     // auto *app = new Tephryte::Application;
     //
     // app->run();
     //
     // delete app;

     return 0;
}









