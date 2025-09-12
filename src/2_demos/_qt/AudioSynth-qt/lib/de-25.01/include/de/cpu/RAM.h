#pragma once
#include <cstdint>
#include <vector>
#include <string>
#include <sstream>

namespace de {
namespace sys {

// Function to get CPU core count and thread count
//void dumpCPUInfo();

// Function to get system memory info (RAM)
struct RamInfo { uint64_t total, avail, used;

    std::string str() const
    {
        std::ostringstream o;
        o << "RAM total(" << total << "), avail(" << avail << "), used(" << used << ")";
        return o.str();
    }
};

RamInfo ramInfo();

// Function to get system memory info (RAM)
struct DriveInfo { std::string name; uint64_t total, avail, used;

    std::string str() const
    {
        std::ostringstream o;
        o << "SSD name(" << name << "), total(" << total << "), avail(" << avail << "), used(" << used << ")";
        return o.str();
    }
};

typedef std::vector<DriveInfo> DriveInfos;

DriveInfos driveInfos();

// Function to get storage info (HDD & SSD)
//void getStorageInfo();

// Function to read CPU and GPU temperatures using OpenHardwareMonitor
float cpuTemp();

// Function to read GPU temperature using NVAPI
int gpuTemp();

} // end namespace sys.
} // end namespace de.

/*
int main() {
    std::cout << "System Information:" << std::endl;
    
    getCPUInfo();
    RAMInfo getRAMInfo();
    getStorageInfo();
    
    std::cout << "CPU Temperature: " << getCPUTemp() << "°C" << std::endl;
    std::cout << "GPU Temperature: " << getGPUTemp() << "°C" << std::endl;

    return 0;
}
*/
