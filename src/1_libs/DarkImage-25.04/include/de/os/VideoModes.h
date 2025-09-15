#pragma once
#include <de/Core.h>

int dbDesktopWidth();
int dbDesktopHeight();

namespace de {

// int getClientWidth() const;
// int getClientHeight() const;

//===================================================================
struct VideoMode
//===================================================================
{
    int32_t width;
    int32_t height;
    int32_t bpp;
    float freq;
    std::string name;

    std::string str() const
    {
        std::ostringstream o;
        if ( name.size() ) o << name << ", ";
        o << width << ", " << height << ", " << bpp << ", " << freq << "Hz";
        return o.str();
    }
};

std::vector< VideoMode >
getVideoModes();

inline void
dumpVideoModes()
{
    const auto videoModes = getVideoModes();

    DE_TRACE("VideoModes = ", videoModes.size() )
    for ( size_t i = 0; i < videoModes.size(); ++i )
    {
        DE_TRACE("VideoMode[",i,"] = ", videoModes[ i ].str() )
    }
}

} // end namespace de.
