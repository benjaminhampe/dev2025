#pragma once
#include <de/gpu/mtl/PMesh.h>
#include <de/gpu/scene/SceneManager.h>

namespace de {
namespace gpu {

// =====================================================================
struct PMeshSceneNode : public ISceneNode
// =====================================================================
{
    PMesh m_mesh;
	
public:
    PMeshSceneNode( SceneManager* env, ISceneNode* parent, u32 id, f32 x, f32 y, f32 z );
    ~PMeshSceneNode() override;

    Box3f           getAbsoluteBoundingBox() override;


    void 			setMesh( PMesh mesh );
    const PMesh& 	getMesh() const;
    PMesh&          getMesh();

    void            draw() override;
};

} // end namespace gpu.
} // end namespace de.
