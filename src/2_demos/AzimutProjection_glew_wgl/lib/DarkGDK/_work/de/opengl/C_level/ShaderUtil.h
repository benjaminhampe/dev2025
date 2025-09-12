#pragma once
#include <de_opengl.h>
#include <cstdint>
#include <sstream>

namespace de {

//===========================================================================
struct ShaderUtil
//===========================================================================
{
    // uint32_t programId = glCreateProgram();
    static uint32_t createShaderFromText(   
        std::string const & programName,// Program name,   e.g. "s3d+tex0+light2+bloom+fog"
        std::string const & vsText,     // Vertex shader   e.g. #version 460 core es\n int main() {}
        std::string const & fsText,     // Fragment shader e.g. #version 460 core es\n int main() {}
        std::string const & gsText = "" // Geometry shader e.g. #version 460 core es\n int main() {}
    );

    static int32_t getShaderVersionMajor();
    static int32_t getShaderVersionMinor();
    static std::string getShaderVersionHeader();
    static std::string getShaderError( uint32_t shaderId );
    static std::string getProgramError( uint32_t programId );
    static std::vector< std::string > splitText( std::string const & txt, char searchChar );
    static std::string toStringWithLineNumbers( std::string const & src );
};

} // end namespace de.
