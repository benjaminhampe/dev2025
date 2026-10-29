#include "H3_TerrainTileRenderer.h"
#include <H3/H3_Game.h>
#include <de_opengl.h>

namespace {

void createSMeshTile( de::smesh::SMeshBuffer & o, de::gpu::VideoDriver* driver,
    glm::vec3 tileSize, de::Image bumpMap, const de::gpu::TexRef& diffuseMap )
{
    DE_BENNI("Loaded bumpMap = ", dbStrBytes(bumpMap.computeMemoryConsumption()) )

    de::gpu::BumpMapUtil::saturateHeightmap( bumpMap );
    DE_BENNI("Saturated bumpMap = ", bumpMap.str() )

    bumpMap = de::ImageScaler::resizeBilinear( bumpMap, 118,118 );
    DE_BENNI("Resized bumpMap = ", bumpMap.str() )

    //Image filterMap = createHexagonFilter( bumpMap.w() );
    //filterGrey( bumpMap, filterMap );

    //std::string uriSave1 = FileSystem::fileBase( uriBumpMap ) + ".png";
    //dbSaveImage(bumpMap, uriSave1 );

    // =============== Mesh Creation & Clipping: =============================

    de::gpu::BumpMapUtil::Triangles in_triangles;
    de::gpu::BumpMapUtil::createTriangles( in_triangles, bumpMap, tileSize );
    de::gpu::BumpMapUtil::centerTrianglesXZ( in_triangles );

    de::gpu::BumpMapUtil::BBox bbox = de::gpu::BumpMapUtil::computeBBox( in_triangles );
    DE_BENNI("BBox Min(",bbox.minEdge,"), Max(",bbox.maxEdge,")")

    de::gpu::BumpMapUtil::Triangles out_triangles;
    de::gpu::BumpMapUtil::clipTrianglesHexagon( in_triangles, out_triangles, tileSize );

    de::gpu::BumpMapUtil::repairWinding( out_triangles );
    DE_OK("Before SMeshBuffer has ", out_triangles.size() * 3, " vertices")
    DE_OK("Before SMeshBuffer has ", out_triangles.size() * 3, " indices")

    o.setTexture( 0, diffuseMap );

    //auto tex = driver->createTexture2D("img4x4", imgDiffuseMap);
    //o.setTexture( 0, tex );
    o.name = de::FileSystem::fileBase(bumpMap.uri());
    o.material.Lighting = 0;
    o.material.FogEnable = false;
    o.material.state = de::gpu::State();
    o.material.state.blend = de::gpu::Blend::alphaBlend();
    // o.material.state.culling = de::gpu::Culling::disabled();
    de::gpu::BumpMapUtil::addTriangles( o, out_triangles );

    DE_OK("After SMeshBuffer has ", o.vertices.size(), " vertices")
    DE_OK("After SMeshBuffer has ", o.indices.size(), " indices")

    de::gpu::BumpMapUtil::windowVertices( o );

    //de::smesh::SMeshBufferTool::computeNormals( o );

    //o.upload();

    DE_OK( o.str() )
}

}

H3_TerrainTileRenderer::H3_TerrainTileRenderer()
{
}

H3_TerrainTileRenderer::~H3_TerrainTileRenderer()
{
    destroy();
}

void H3_TerrainTileRenderer::destroy()
{
    m_hmeshTileDesert.destroy();
    m_hmeshTileA.destroy();
    m_hmeshTileB.destroy();
    m_hmeshTileC.destroy();
    m_hmeshTileD.destroy();
    m_hmeshTileE.destroy();
}

void H3_TerrainTileRenderer::init( H3_Game & game )
{
    m_driver = game.getDriver();

    m_hmeshRenderer.init( m_driver );
    // === Create hexagon tiles geometry ===
    de::smesh::SMeshBuffer m_meshTileDesert; // Nevada
    de::smesh::SMeshBuffer m_meshTileA; // Mt Wilder
    de::smesh::SMeshBuffer m_meshTileB; // ?
    de::smesh::SMeshBuffer m_meshTileC; // Thuringen
    de::smesh::SMeshBuffer m_meshTileD; // Italia
    de::smesh::SMeshBuffer m_meshTileE; // Alpen

    auto ts = game.m_tileSize;
    createSMeshTile( m_meshTileDesert, m_driver, ts, game.getImg(H3_Img::TileDesert_H), game.getTex(H3_Tex::TileDesert_D, "m_meshTileDesert"));
    createSMeshTile( m_meshTileA, m_driver, ts, game.getImg(H3_Img::TileA_H), game.getTex(H3_Tex::TileA_D, "m_meshTileA"));
    createSMeshTile( m_meshTileB, m_driver, ts, game.getImg(H3_Img::TileB_H), game.getTex(H3_Tex::TileB_D, "m_meshTileB"));
    createSMeshTile( m_meshTileC, m_driver, ts, game.getImg(H3_Img::TileC_H), game.getTex(H3_Tex::TileC_D, "m_meshTileC"));
    createSMeshTile( m_meshTileD, m_driver, ts, game.getImg(H3_Img::TileD_H), game.getTex(H3_Tex::TileD_D, "m_meshTileD"));
    createSMeshTile( m_meshTileE, m_driver, ts, game.getImg(H3_Img::TileE_H), game.getTex(H3_Tex::TileE_D, "m_meshTileE"));

    // de::gpu::BumpMapUtil::testNevada( m_meshTileDesert, m_driver );
    // de::gpu::BumpMapUtil::testMountWilder( m_meshTileA, m_driver );
    // de::gpu::BumpMapUtil::testLehm( m_meshTileB, m_driver );
    // de::gpu::BumpMapUtil::testThuringen( m_meshTileC, m_driver );
    // de::gpu::BumpMapUtil::testWeizen( m_meshTileD, m_driver );
    // de::gpu::BumpMapUtil::testAlpen( m_meshTileE, m_driver );

    m_hmeshTileDesert = de::gpu::HMesh::fromSMesh( m_meshTileDesert );
    m_hmeshTileA = de::gpu::HMesh::fromSMesh( m_meshTileA );
    m_hmeshTileB = de::gpu::HMesh::fromSMesh( m_meshTileB );
    m_hmeshTileC = de::gpu::HMesh::fromSMesh( m_meshTileC );
    m_hmeshTileD = de::gpu::HMesh::fromSMesh( m_meshTileD );
    m_hmeshTileE = de::gpu::HMesh::fromSMesh( m_meshTileE );
}

void H3_TerrainTileRenderer::render()
{
    if (!m_driver)
    {
        DE_ERROR("No driver")
        return;
    }

    m_driver->beginPerf("Draw Tiles");

    // m_driver->beginPerf("TileDesert");
    m_hmeshRenderer.draw( m_hmeshTileDesert );
    // m_driver->endPerf();

    // m_driver->beginPerf("TileA");
    m_hmeshRenderer.draw( m_hmeshTileA );
    // m_driver->endPerf();

    // m_driver->beginPerf("TileB");
    m_hmeshRenderer.draw( m_hmeshTileB );
    // m_driver->endPerf();

    // m_driver->beginPerf("TileC");
    m_hmeshRenderer.draw( m_hmeshTileC );
    // m_driver->endPerf();

    // m_driver->beginPerf("TileD");
    m_hmeshRenderer.draw( m_hmeshTileD );
    // m_driver->endPerf();

    // m_driver->beginPerf("TileE");
    m_hmeshRenderer.draw( m_hmeshTileE );
    // m_driver->endPerf();

    m_driver->endPerf();

    /*
    // <Desert>
    {
        glm::mat4 m = glm::translate(glm::mat4(1.0f), glm::vec3(-150,0,0));
        m_driver->setModelMatrix( m );
        m_driver->getSMaterialRenderer()->draw3D( m_meshTileDesert );
        m_driver->getLine3DRenderer()->draw3DLineBox( m_meshTileDesert.boundingBox );
    }
    // <A>
    {
        glm::mat4 m = glm::translate(glm::mat4(1.0f), glm::vec3(0,0,0));
        m_driver->setModelMatrix( m );
        m_driver->getSMaterialRenderer()->draw3D( m_meshTileA );
        m_driver->getLine3DRenderer()->draw3DLineBox( m_meshTileA.boundingBox );
    }
    // <B>
    {
        glm::mat4 m = glm::translate(glm::mat4(1.0f), glm::vec3(150,0,0));
        m_driver->setModelMatrix( m );
        m_driver->getSMaterialRenderer()->draw3D( m_meshTileB );
        m_driver->getLine3DRenderer()->draw3DLineBox( m_meshTileB.boundingBox );
    }
    // <C>
    {
        glm::mat4 m = glm::translate(glm::mat4(1.0f), glm::vec3(-150,0,150));
        m_driver->setModelMatrix( m );
        m_driver->getSMaterialRenderer()->draw3D( m_meshTileC );
        m_driver->getLine3DRenderer()->draw3DLineBox( m_meshTileC.boundingBox );
    }
    // <D>
    {
        glm::mat4 m = glm::translate(glm::mat4(1.0f), glm::vec3(0,0,150));
        m_driver->setModelMatrix( m );
        m_driver->getSMaterialRenderer()->draw3D( m_meshTileD );
        m_driver->getLine3DRenderer()->draw3DLineBox( m_meshTileD.boundingBox );
    }
    // <E>
    {
        glm::mat4 m = glm::translate(glm::mat4(1.0f), glm::vec3(150,0,150));
        m_driver->setModelMatrix( m );
        m_driver->getSMaterialRenderer()->draw3D( m_meshTileE );
        m_driver->getLine3DRenderer()->draw3DLineBox( m_meshTileE.boundingBox );
    }
    */
}


void H3_TerrainTileRenderer::collectAndUploadInstances(const H3_Game& game)
{
    m_driver = game.getDriver();

    m_driver->beginPerf("Update Tiles");

    // ========================
    // === Create DrawCalls ===
    // ========================
    // HMesh uses f16 half floats, aka 16-bit floats
    // to render terrain (from heightmap) like hexagonal tiles.
    // The heightmap is converted to vector of 3D Triangles,
    // clipped to hexagon using math, then as cleanup vector is
    // rebuild to use only unique vertices (reduce vertices)
    // and then finally converted to HMesh for fast rendering.
    // Uses f16 vertices to increase render speed. (reduce GPU memory)
    // Indices are still uint32_t.
    auto ts = game.m_tileSize;

    auto mkHMeshDrawCall = [&]( de::gpu::HMesh & hmesh, H3_TileType tt )
    {
        hmesh.m_instanceMat.clear();

        for (const auto& t : game.m_tiles)
        {
            if (t.tt.value != tt) { continue; }

            de::TRSf trs;
            trs.pos = t.pos;
            trs.rotate = glm::vec3(0,0,0);
            trs.scale = glm::vec3(ts.x * 0.5f, ts.y, ts.z * 0.5f);
            trs.update();

            hmesh.m_instanceMat.push_back( trs.modelMat );
        }

        //DE_OK(tt.toString()," : instance = ", hmesh.m_instanceMat.size())
        hmesh.upload( true );
    };

    mkHMeshDrawCall( m_hmeshTileA, H3_TileType::TileA );
    mkHMeshDrawCall( m_hmeshTileB, H3_TileType::TileB );
    mkHMeshDrawCall( m_hmeshTileC, H3_TileType::TileC );
    mkHMeshDrawCall( m_hmeshTileD, H3_TileType::TileD );
    mkHMeshDrawCall( m_hmeshTileE, H3_TileType::TileE );
    mkHMeshDrawCall( m_hmeshTileDesert, H3_TileType::Desert );

    m_driver->endPerf();

}
