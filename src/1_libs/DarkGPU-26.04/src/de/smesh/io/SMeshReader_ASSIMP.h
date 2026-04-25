#pragma once
#include <de/smesh/SMeshIO.h>

#ifdef DE_MESH_READER_ASSIMP_ENABLED

namespace de {
namespace gpu {
    struct VideoDriver;
} // end namespace gpu.
} // end namespace de.

namespace de {
namespace smesh {

// ===========================================================================
struct SMeshReaderASSIMP : public IReader
// ===========================================================================
{
    ~SMeshReaderASSIMP() override {}

    std::vector< std::string >
    getSupportedExtensions() const override {
        return { "obj", "x", "fbx" }; }

    bool
    load( gpu::VideoDriver* driver, SMesh & m,
         std::string const & uri, const uint8_t* p, size_t n,
         LoadOptions const & opt = LoadOptions() ) override;

    // static bool
    // load( SMesh & out,
    //      std::string const & uri,
    //      gpu::VideoDriver* driver,
    //      LoadOptions const & options = LoadOptions() );
};

} // end namespace smesh.
} // end namespace de.

#endif