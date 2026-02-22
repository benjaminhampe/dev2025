#pragma once
#include <de/gpu/mtl/PMesh.h>
#include <de/gpu/scene/SceneManager.h>

namespace de {
namespace gpu {

// =====================================================================
struct TerrainSceneNode : public ISceneNode
// =====================================================================
{
    PMesh m_mesh;
	
public:
    TerrainSceneNode( SceneManager* env, ISceneNode* parent, u32 id, f32 x, f32 y, f32 z );
    ~TerrainSceneNode() override;

    Box3f           getAbsoluteBoundingBox() override;


    void 			setMesh( PMesh mesh );
    const PMesh& 	getMesh() const;
    PMesh&          getMesh();

    void            draw() override;
};

} // end namespace gpu.
} // end namespace de.
