#pragma once
#include <de/gpu/mtl/PMesh.h>
#include <de/gpu/smesh/SMesh.h>

namespace de {
namespace gpu {

// ===========================================================================
struct PMeshConverter
// ===========================================================================
{
    static bool
    toSMesh(SMesh & smesh, const PMesh& pmesh);
};

} // namespace gpu.
} // namespace de.

