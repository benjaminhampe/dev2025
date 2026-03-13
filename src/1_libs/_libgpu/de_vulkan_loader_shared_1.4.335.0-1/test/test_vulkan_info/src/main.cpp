#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.h>

#include <iostream>
#include <vector>
#include <string>

std::string apiVersionToString(uint32_t v) {
    uint32_t major = VK_VERSION_MAJOR(v);
    uint32_t minor = VK_VERSION_MINOR(v);
    uint32_t patch = VK_VERSION_PATCH(v);
    return std::to_string(major) + "." + std::to_string(minor) + "." + std::to_string(patch);
}

int main() {
    // --- 1. Create Vulkan instance ---
    VkApplicationInfo appInfo{};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "Vulkan Info Test";
    appInfo.apiVersion = VK_API_VERSION_1_0;

    VkInstanceCreateInfo ci{};
    ci.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    ci.pApplicationInfo = &appInfo;

    VkInstance instance;
    if (vkCreateInstance(&ci, nullptr, &instance) != VK_SUCCESS) {
        std::cerr << "Failed to create Vulkan instance.\n";
        return 1;
    }

    // --- 2. Enumerate physical devices ---
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);

    if (deviceCount == 0) {
        std::cout << "No Vulkan-capable devices found.\n";
        return 1;
    }

    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

    std::cout << "Found " << deviceCount << " Vulkan device(s):\n\n";

    // --- 3. Print device info ---
    for (uint32_t i = 0; i < deviceCount; i++) {
        VkPhysicalDeviceProperties props;
        vkGetPhysicalDeviceProperties(devices[i], &props);

        std::cout << "=== Device " << i << " ===\n";
        std::cout << "Name:          " << props.deviceName << "\n";
        std::cout << "Vendor ID:     0x" << std::hex << props.vendorID << std::dec << "\n";
        std::cout << "Device ID:     0x" << std::hex << props.deviceID << std::dec << "\n";

        std::cout << "API Version:   " << apiVersionToString(props.apiVersion) << "\n";
        std::cout << "Driver Version:" << apiVersionToString(props.driverVersion) << "\n";

        std::cout << "Device Type:   ";
        switch (props.deviceType) {
            case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU: std::cout << "Integrated GPU\n"; break;
            case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:   std::cout << "Discrete GPU\n"; break;
            case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU:    std::cout << "Virtual GPU\n"; break;
            case VK_PHYSICAL_DEVICE_TYPE_CPU:            std::cout << "CPU\n"; break;
            default:                                     std::cout << "Other\n"; break;
        }

        // --- 4. Memory heaps ---
        VkPhysicalDeviceMemoryProperties memProps;
        vkGetPhysicalDeviceMemoryProperties(devices[i], &memProps);

        std::cout << "\nMemory Heaps:\n";
        for (uint32_t h = 0; h < memProps.memoryHeapCount; h++) {
            VkMemoryHeap heap = memProps.memoryHeaps[h];
            std::cout << "  Heap " << h << ": "
                      << (heap.size / (1024 * 1024)) << " MB"
                      << ((heap.flags & VK_MEMORY_HEAP_DEVICE_LOCAL_BIT) ? " (Device Local)" : "")
                      << "\n";
        }

        // --- 5. Queue families ---
        uint32_t qCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(devices[i], &qCount, nullptr);
        std::vector<VkQueueFamilyProperties> qProps(qCount);
        vkGetPhysicalDeviceQueueFamilyProperties(devices[i], &qCount, qProps.data());

        std::cout << "\nQueue Families:\n";
        for (uint32_t q = 0; q < qCount; q++) {
            std::cout << "  Queue " << q
                      << ": count=" << qProps[q].queueCount
                      << ", flags=";

            if (qProps[q].queueFlags & VK_QUEUE_GRAPHICS_BIT) std::cout << "Graphics ";
            if (qProps[q].queueFlags & VK_QUEUE_COMPUTE_BIT)  std::cout << "Compute ";
            if (qProps[q].queueFlags & VK_QUEUE_TRANSFER_BIT) std::cout << "Transfer ";
            if (qProps[q].queueFlags & VK_QUEUE_SPARSE_BINDING_BIT) std::cout << "Sparse ";

            std::cout << "\n";
        }

        std::cout << "\n";
    }

    vkDestroyInstance(instance, nullptr);
    return 0;
}
