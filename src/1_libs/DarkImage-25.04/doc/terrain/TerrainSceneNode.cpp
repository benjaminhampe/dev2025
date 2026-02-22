#include <de/gpu/terrain/TerrainSceneNode.h>
#include <de/gpu/VideoDriver.h>

namespace de {
namespace gpu {

// =====================================================================
TerrainSceneNode::TerrainSceneNode( SceneManager* env, ISceneNode* parent, u32 id, f32 x, f32 y, f32 z )
    : ISceneNode(env,parent,id,x,y,z)
{}

TerrainSceneNode::~TerrainSceneNode() {}


Box3f
TerrainSceneNode::getAbsoluteBoundingBox()
{
    auto m = getAbsoluteTransform();
    return m_bbox.transformBox(m);
}
// =====================================================================
void 			TerrainSceneNode::setMesh( PMesh mesh )
{
    m_mesh = mesh;

    for (auto & limb : m_mesh.m_limbs)
    {
        limb.markForUpload();
        limb.resetInstances( m_trs.modelMat );
        limb.upload();
    }
}
const PMesh& 	TerrainSceneNode::getMesh() const { return m_mesh; }
PMesh& 		 	TerrainSceneNode::getMesh() { return m_mesh; }
// =====================================================================
void TerrainSceneNode::draw()
{
	if (!isVisible()) { return; } // DE_WARN("Invisible, nothing todo.")

    auto driver = m_env->getVideoDriver();
    if (!driver) { DE_WARN("No driver.") return; }

    auto pmeshRenderer = driver->getPMaterialRenderer();
    if (!pmeshRenderer) { DE_WARN("No pmeshRenderer.") return; }

    auto lineRenderer = driver->getLine3DRenderer();
	if (!lineRenderer) { DE_WARN("No lineRenderer.") return; }
			
	// Draw this
    for (size_t i = 0; i < m_mesh.m_limbs.size(); ++i)
	{
        PMeshBuffer & limb = m_mesh.m_limbs[i];

        if (limb.empty()) continue;

        //DE_BENNI("Draw limb[",i,"] ", limb.m_name, "BBox(",limb.m_bbox.str(),")")

        limb.resetInstances(getAbsoluteTransform());
        limb.uploadInstances();

        pmeshRenderer->setMaterial( limb.m_material );
		limb.draw();

		// Debug 
        lineRenderer->draw3DLineBox( limb.m_bbox, randomColorRGB() );
	}

    lineRenderer->draw3DLineBox( getAbsoluteBoundingBox(), randomColorRGB() );
}

} // end namespace gpu.
} // end namespace de.
