#include <de_os/de_VideoModes.h>
#include <de_os/de_win32.h>

namespace de {
   
std::vector< VideoMode >
getVideoModes()
{
   std::vector< VideoMode > videoModes;

   //TODO: EnumDisplayDevices http://msdn.microsoft.com/library/en-us/gdi/devcons_2303.asp
   /*vector<string> DisplayDevices;
   DISPLAY_DEVICE DisplayDevice;
   DisplayDevice.cb = sizeof(DISPLAY_DEVICE);
   DWORD i=0;
   while (EnumDisplayDevices(NULL, i++, &DisplayDevice, 0) {
      DisplayDevices.push_back(DisplayDevice.DeviceName);
   }*/

   // Video mode possibilities
   DEVMODE devMode;
   devMode.dmSize = sizeof(DEVMODE);

   char devName[ 1024 ];
   memset( devName, 0, 1024 );

   for ( size_t i = 0; EnumDisplaySettings( nullptr, DWORD(i), &devMode ); ++i )
   {
      VideoMode videoMode;
      videoMode.width = int32_t( devMode.dmPelsWidth );
      videoMode.height = int32_t( devMode.dmPelsHeight );
      videoMode.bpp = int32_t( devMode.dmBitsPerPel );
      videoMode.freq = int32_t( devMode.dmDisplayFrequency );

      if ( strlen(devName) > 0 )
      {
         videoMode.name = devName;
      }

      videoModes.emplace_back( std::move( videoMode ) );
   }

   return videoModes;
}

} // end namespace de