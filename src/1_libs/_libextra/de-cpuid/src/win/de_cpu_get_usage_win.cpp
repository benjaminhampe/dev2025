#include <de_cpu_get_usage.hpp>
#include "de_win32api.hpp"

//===============================================================================================
int de_cpu_get_usage()
//===============================================================================================
{
   // RegOpenKeyExW(HKEY hKey,LPCWSTR lpSubKey,DWORD ulOptions,REGSAM samDesired,PHKEY phkResult);
   HKEY hKey = 0;
   LSTATUS ok = RegOpenKeyExW( HKEY_DYN_DATA, L"PerfStats\\StartStat", 0, KEY_ALL_ACCESS, &hKey );
   if ( ok != ERROR_SUCCESS )
   {
      return -1;
   }

   DWORD dataType = REG_NONE;
   DWORD bufSize = sizeof( DWORD );
   BYTE dummy = 0;
   if ( RegQueryValueExW( hKey, L"KERNEL\\CPUUsage", nullptr, &dataType, &dummy, &bufSize ) != ERROR_SUCCESS )
   {
      return -2;
   }
   RegCloseKey(hKey);

   // { get the value }
   if ( RegOpenKeyExW(HKEY_DYN_DATA, L"PerfStats\\StatData", 0, KEY_ALL_ACCESS, &hKey) != ERROR_SUCCESS )
   {
      return -3;
   }
   RegCloseKey(hKey);

   dataType = REG_NONE;
   bufSize = sizeof(DWORD);
   BYTE usage = 0;
   if ( RegQueryValueExW(hKey, L"KERNEL\\CPUUsage", nullptr, &dataType, &usage, &bufSize) != ERROR_SUCCESS )
   {
      return -4;
   }
   RegCloseKey(hKey);

   // { stop measuring }
   if ( RegOpenKeyExW(HKEY_DYN_DATA, L"PerfStats\\StopStat", 0, KEY_ALL_ACCESS, &hKey) != ERROR_SUCCESS )
   {
      return -5;
   }

   dataType = REG_NONE;
   bufSize = sizeof(DWORD);
   if ( RegQueryValueExW(hKey, L"KERNEL\\CPUUsage", nullptr, &dataType, &dummy, &bufSize) != ERROR_SUCCESS )
   {
      return -6;
   }

   RegCloseKey(hKey);
//	}
//	else
//	{
//		dbERROR( "Unknown System to get CPU-usage from registry\n");
//	}
   return usage;
}
