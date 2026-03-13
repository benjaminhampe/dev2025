#include <CL/cl.h>
#include <iostream>
#include <vector>
#include <string>

std::string getPlatformInfo(cl_platform_id p, cl_platform_info param) {
    size_t size = 0;
    clGetPlatformInfo(p, param, 0, nullptr, &size);
    std::string s(size, '\0');
    clGetPlatformInfo(p, param, size, s.data(), nullptr);
    return s;
}

std::string getDeviceStr(cl_device_id d, cl_device_info param) {
    size_t size = 0;
    clGetDeviceInfo(d, param, 0, nullptr, &size);
    std::string s(size, '\0');
    clGetDeviceInfo(d, param, size, s.data(), nullptr);
    return s;
}

template<typename T>
T getDeviceVal(cl_device_id d, cl_device_info param) {
    T v{};
    clGetDeviceInfo(d, param, sizeof(T), &v, nullptr);
    return v;
}

int main() {
    cl_uint numPlatforms = 0;
    clGetPlatformIDs(0, nullptr, &numPlatforms);

    if (numPlatforms == 0) {
        std::cout << "No OpenCL platforms found.\n";
        return 1;
    }

    std::vector<cl_platform_id> platforms(numPlatforms);
    clGetPlatformIDs(numPlatforms, platforms.data(), nullptr);

    std::cout << "Found " << numPlatforms << " OpenCL platform(s):\n\n";

    for (cl_uint i = 0; i < numPlatforms; i++) {
        cl_platform_id p = platforms[i];

        std::cout << "=== Platform " << i << " ===\n";
        std::cout << "Name:    " << getPlatformInfo(p, CL_PLATFORM_NAME)    << "\n";
        std::cout << "Vendor:  " << getPlatformInfo(p, CL_PLATFORM_VENDOR)  << "\n";
        std::cout << "Version: " << getPlatformInfo(p, CL_PLATFORM_VERSION) << "\n";
        std::cout << "Profile: " << getPlatformInfo(p, CL_PLATFORM_PROFILE) << "\n";
        std::cout << "Extensions:\n" << getPlatformInfo(p, CL_PLATFORM_EXTENSIONS) << "\n\n";

        cl_uint numDevices = 0;
        clGetDeviceIDs(p, CL_DEVICE_TYPE_ALL, 0, nullptr, &numDevices);

        if (numDevices == 0) {
            std::cout << "  No devices on this platform.\n\n";
            continue;
        }

        std::vector<cl_device_id> devices(numDevices);
        clGetDeviceIDs(p, CL_DEVICE_TYPE_ALL, numDevices, devices.data(), nullptr);

        for (cl_uint d = 0; d < numDevices; d++) {
            cl_device_id dev = devices[d];

            std::cout << "  --- Device " << d << " ---\n";
            std::cout << "  Name:              " << getDeviceStr(dev, CL_DEVICE_NAME) << "\n";
            std::cout << "  Vendor:            " << getDeviceStr(dev, CL_DEVICE_VENDOR) << "\n";
            std::cout << "  Driver Version:    " << getDeviceStr(dev, CL_DRIVER_VERSION) << "\n";
            std::cout << "  OpenCL Version:    " << getDeviceStr(dev, CL_DEVICE_VERSION) << "\n";

            cl_device_type type = getDeviceVal<cl_device_type>(dev, CL_DEVICE_TYPE);
            std::cout << "  Type:              ";
            if (type & CL_DEVICE_TYPE_GPU) std::cout << "GPU\n";
            else if (type & CL_DEVICE_TYPE_CPU) std::cout << "CPU\n";
            else if (type & CL_DEVICE_TYPE_ACCELERATOR) std::cout << "Accelerator\n";
            else std::cout << "Other\n";

            std::cout << "  Compute Units:     " << getDeviceVal<cl_uint>(dev, CL_DEVICE_MAX_COMPUTE_UNITS) << "\n";
            std::cout << "  Max Workgroup Size:" << getDeviceVal<size_t>(dev, CL_DEVICE_MAX_WORK_GROUP_SIZE) << "\n";

            cl_ulong mem = getDeviceVal<cl_ulong>(dev, CL_DEVICE_GLOBAL_MEM_SIZE);
            std::cout << "  Global Mem Size:   " << (mem / (1024 * 1024)) << " MB\n";

            std::cout << "  Extensions:\n" << getDeviceStr(dev, CL_DEVICE_EXTENSIONS) << "\n\n";
        }
    }

    return 0;
}
