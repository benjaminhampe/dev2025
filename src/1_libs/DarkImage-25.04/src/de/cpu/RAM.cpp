#include <de/cpu/RAM.h>
#include <de/Core.h>

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>

#include <sysinfoapi.h> // CPU info
#include <psapi.h> // Memory info

// Include NVIDIA API for GPU temperature
//#include <nvapi.h>
// #include <fstream> // Reading temp files

namespace de {
namespace sys {

/*
// Function to get CPU core count and thread count
void cpuInfo()
{
    SYSTEM_INFO sysInfo;
    GetSystemInfo(&sysInfo);
    
    DE_DEBUG("CPU Cores: ",sysInfo.dwNumberOfProcessors)
}
*/

// Function to get system memory info (RAM)
RamInfo
ramInfo()
{
    RamInfo dat;
	
    MEMORYSTATUSEX mem;
    mem.dwLength = sizeof(MEMORYSTATUSEX);
    GlobalMemoryStatusEx(&mem);

    dat.total = mem.ullTotalPhys;
    dat.avail = mem.ullAvailPhys;
    dat.used = dat.total - dat.avail;
	
    DE_DEBUG("Total RAM: ", dat.total / (1024 * 1024), " MB")
    DE_DEBUG("Avail RAM: ", dat.avail / (1024 * 1024), " MB")

    return dat;
}

// Function to get storage info (HDD & SSD)

DriveInfos
driveInfos()
{
    DriveInfos drives;

	const uint32_t bpg = 1024 * 1024 * 1024; // bytes per giga byte
	
    char tmpStr[1024];
    GetLogicalDriveStringsA(sizeof(tmpStr), tmpStr);
    
    for (char* drive = tmpStr; *drive; drive += strlen(drive) + 1)
	{
        ULARGE_INTEGER availBytes, totalBytes, dummyBytes;
        DWORD driveType = GetDriveTypeA(drive);
        
		// HDD or SSD
        if (driveType == DRIVE_FIXED) 
		{ 
			// BOOL GetDiskFreeSpaceExA(
			  // LPCSTR          lpDirectoryName,  // Directory or drive (e.g., "C:\\")
			  // PULARGE_INTEGER lpFreeBytesAvailable, // Free bytes available to the user
			  // PULARGE_INTEGER lpTotalNumberOfBytes, // Total size of the drive
			  // PULARGE_INTEGER lpTotalNumberOfFreeBytes // Total free space on the drive
			// );

            GetDiskFreeSpaceExA(drive, &availBytes, &totalBytes, &dummyBytes);
			
            DriveInfo dat;
            dat.name = drive;
            dat.total = totalBytes.QuadPart;
            dat.avail = availBytes.QuadPart;
            dat.used = dat.total - dat.avail;
			
            DE_DEBUG("Drive name(",dat.name,"), "
                     "Total(", dat.total / bpg, " GB), "
                     "Used(", dat.used / bpg, " GB), "
                     "Avail(", dat.avail / bpg, " GB)")

            drives.push_back(dat);
        }
    }

    return drives;
}

// Function to read CPU and GPU temperatures using OpenHardwareMonitor
float cpuTemp()
{
    // std::ifstream file("C:\\Program Files\\OpenHardwareMonitor\\OpenHardwareMonitorReport.txt");
    // std::string line;
    // while (std::getline(file, line)) {
        // if (line.find("CPU Package") != std::string::npos) {
            // return std::stof(line.substr(line.find(":") + 2, line.length() - 1));
        // }
    // }
	
    return -1; // If unable to read temperature
}

// Function to read GPU temperature using NVAPI
int gpuTemp()
{
    // NvAPI_Initialize();
    // NvAPI_ShortString gpuName;
    // NvAPI_GPU_GetFullName(NvAPI_GetGPUHandles()[0], gpuName);
    // NvAPI_GPU_GetThermalSettings(NvAPI_GetGPUHandles()[0], 0, &thermalSettings);

    // return thermalSettings.sensor[0].currentTemp; // Return GPU temperature
	return -1;
}

} // end namespace sys
} // end namespace de

/*
int main() {
    std::cout << "System Information:" << std::endl;
    
    getCPUInfo();
    getRAMInfo();
    getStorageInfo();
    
    std::cout << "CPU Temperature: " << getCPUTemp() << "°C" << std::endl;
    std::cout << "GPU Temperature: " << getGPUTemp() << "°C" << std::endl;

    return 0;
}
*/
