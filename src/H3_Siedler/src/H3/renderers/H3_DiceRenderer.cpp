#include "H3_DiceRenderer.h"
#include <H3/H3_Game.h>
#include <de_opengl.h>
#include <de/gpu/GL_debug_layer.h>

namespace {

auto addQuad = [](de::gpu::SMeshBuffer & o,
                   de::gpu::S3DVertex const & a, de::gpu::S3DVertex const & b,
                   de::gpu::S3DVertex const & c, de::gpu::S3DVertex const & d,
                   de::gpu::TexRef tex = de::gpu::TexRef(),
                   bool flip = false )
{
    o.setPrimitiveType(de::gpu::PrimitiveType::Triangles);
    o.setTexture(0, tex);
    o.setBlend(de::gpu::Blend::disabled());
    o.setLighting(0);
    o.vertices.push_back( a );
    o.vertices.push_back( b );
    o.vertices.push_back( c );
    o.vertices.push_back( d );

    const uint32_t v = o.vertices.size();
    const uint32_t ia = v-4;
    const uint32_t ib = v-3;
    const uint32_t ic = v-2;
    const uint32_t id = v-1;

    if ( flip )
    {
        // abc
        o.indices.push_back( ia );
        o.indices.push_back( ic );
        o.indices.push_back( ib );
        // acd
        o.indices.push_back( ia );
        o.indices.push_back( id );
        o.indices.push_back( ic );
    }
    else
    {
        // abc
        o.indices.push_back( ia );
        o.indices.push_back( ib );
        o.indices.push_back( ic );
        // acd
        o.indices.push_back( ia );
        o.indices.push_back( ic );
        o.indices.push_back( id );
    }
};

}

H3_DiceRenderer::H3_DiceRenderer()
{
}

H3_DiceRenderer::~H3_DiceRenderer()
{

}

void
H3_DiceRenderer::init( H3_Game & game )
{
    m_driver = game.getDriver();
    m_renderer = m_driver->getSMaterialRenderer();

    const uint32_t color = 0xFFFFFFFF;
    const float dx = 0.5f;
    const float dy = 0.5f;
    const float dz = 0.5f;

    // =========================================================================
    //
    //        F-------G                 tex atlas infos         +----+----+---------+
    //       /|      /|                                         |    |  2 |         |
    //      / |   3 / |                                         |    | +Y |         |
    //     /  |    /  |                                         |    |    |         |
    //    /   E---/---H   lookat = -X for plane PX              +----+----+----+----+
    //   / 1 / 2 /   /    <-------P  EyePos is +X for plane PX  |  1 |  4 |  6 |  3 |
    //  /   /   / 6 /                                           | -X | -Z | +X | +Z |
    // B-------C   /     +y                                     |    |    |    |    |
    // |  /   5|  /      |  +z                                  +----+----+----+----+
    // | / 4   | /       | /                                    |    |  5 |         |
    // |/      |/        |/                                     |    | -Y |         |
    // A-------D         0------> +x                            |    |    |         |
    //
    const auto a = glm::vec3( -dx, -dy, -dz );
    const auto b = glm::vec3( -dx,  dy, -dz );
    const auto c = glm::vec3(  dx,  dy, -dz );
    const auto d = glm::vec3(  dx, -dy, -dz );
    const auto e = glm::vec3( -dx, -dy,  dz );
    const auto f = glm::vec3( -dx,  dy,  dz );
    const auto g = glm::vec3(  dx,  dy,  dz );
    const auto h = glm::vec3(  dx, -dy,  dz );

    de::gpu::SMeshBuffer m_meshNegX;
    de::gpu::SMeshBuffer m_meshPosX;
    de::gpu::SMeshBuffer m_meshNegY;
    de::gpu::SMeshBuffer m_meshPosY;
    de::gpu::SMeshBuffer m_meshNegZ;
    de::gpu::SMeshBuffer m_meshPosZ;

    {   // Top quad BFGC = Dice(2)
        de::gpu::S3DVertex B( b.x, b.y, b.z, 0,1,0, color, 0,1 );
        de::gpu::S3DVertex F( f.x, f.y, f.z, 0,1,0, color, 0,0 );
        de::gpu::S3DVertex G( g.x, g.y, g.z, 0,1,0, color, 1,0 );
        de::gpu::S3DVertex C( c.x, c.y, c.z, 0,1,0, color, 1,1 );
        addQuad( m_meshPosY,B,F,G,C, game.getTex(H3_Tex::Dice2, __func__) );
    }
    {   // Bottom quad AEHD = Dice(5)
        de::gpu::S3DVertex E( e.x, e.y, e.z, 0,-1,0, color, 0,1 );
        de::gpu::S3DVertex A( a.x, a.y, a.z, 0,-1,0, color, 0,0 );
        de::gpu::S3DVertex D( d.x, d.y, d.z, 0,-1,0, color, 1,0 );
        de::gpu::S3DVertex H( h.x, h.y, h.z, 0,-1,0, color, 1,1 );
        addQuad( m_meshNegY,E,A,D,H, game.getTex(H3_Tex::Dice5, __func__) );
    }

    {   // Front quad ABCD = Dice(4)
        de::gpu::S3DVertex A( a.x, a.y, a.z, 0,0,-1, color, 0,1 );
        de::gpu::S3DVertex B( b.x, b.y, b.z, 0,0,-1, color, 0,0 );
        de::gpu::S3DVertex C( c.x, c.y, c.z, 0,0,-1, color, 1,0 );
        de::gpu::S3DVertex D( d.x, d.y, d.z, 0,0,-1, color, 1,1 );
        addQuad( m_meshNegZ,A,B,C,D, game.getTex(H3_Tex::Dice4, __func__) );
    }
    {   // Back quad HGFE = Dice(3)
        de::gpu::S3DVertex E( e.x, e.y, e.z, 0,0,1, color, 0,1 );
        de::gpu::S3DVertex F( f.x, f.y, f.z, 0,0,1, color, 0,0 );
        de::gpu::S3DVertex G( g.x, g.y, g.z, 0,0,1, color, 1,0 );
        de::gpu::S3DVertex H( h.x, h.y, h.z, 0,0,1, color, 1,1 );
        addQuad( m_meshPosZ,H,G,F,E, game.getTex(H3_Tex::Dice3, __func__) );
    }

    {   // Left quad EFBA = Dice(1)
        de::gpu::S3DVertex E( e.x, e.y, e.z, -1,0,0, color, 0,1 );
        de::gpu::S3DVertex F( f.x, f.y, f.z, -1,0,0, color, 0,0 );
        de::gpu::S3DVertex B( b.x, b.y, b.z, -1,0,0, color, 1,0 );
        de::gpu::S3DVertex A( a.x, a.y, a.z, -1,0,0, color, 1,1 );
        addQuad( m_meshNegX,E,F,B,A, game.getTex(H3_Tex::Dice1, __func__) );
    }
    {   // Right quad DCGH = Dice(6)
        de::gpu::S3DVertex D( d.x, d.y, d.z, 1,0,0, color, 0,1 );
        de::gpu::S3DVertex C( c.x, c.y, c.z, 1,0,0, color, 0,0 );
        de::gpu::S3DVertex G( g.x, g.y, g.z, 1,0,0, color, 1,0 );
        de::gpu::S3DVertex H( h.x, h.y, h.z, 1,0,0, color, 1,1 );
        addQuad( m_meshPosX,D,C,G,H, game.getTex(H3_Tex::Dice6, __func__) );
    }

    m_cube1.addMeshBuffer( m_meshNegX );
    m_cube1.addMeshBuffer( m_meshPosX );
    m_cube1.addMeshBuffer( m_meshNegY );
    m_cube1.addMeshBuffer( m_meshPosY );
    m_cube1.addMeshBuffer( m_meshNegZ );
    m_cube1.addMeshBuffer( m_meshPosZ );

    m_cube2 = m_cube1;

    m_cube1.upload();
    m_cube2.upload();

    m_trs1.pos.x = 30.f;
    m_trs1.pos.y = 30.f;
    m_trs1.rotate.x = 14.0f;
    m_trs1.rotate.y = 45.0f;
    m_trs1.scale = glm::vec3(20,20,20);
    m_trs1.update();

    m_trs2.pos.x = 45.0f;
    m_trs2.pos.y = 30.0f;
    m_trs2.rotate.x = 64.0f;
    m_trs2.rotate.y = 135.0f;
    m_trs2.scale = glm::vec3(20,20,20);
    m_trs2.update();
}

void H3_DiceRenderer::render()
{
    if (!m_driver)
    {
        DE_ERROR("No driver")
        return;
    }
    if (!m_renderer)
    {
        DE_ERROR("No renderer")
        return;
    }

    m_camera.lookAt(glm::vec3(0,30,-60),
                    glm::vec3(0,10,10),
                    glm::vec3(0,1,0) );

    auto old_camera = m_driver->getCamera();

    m_driver->setCamera( &m_camera );

    m_driver->setModelMatrix( m_trs1.modelMat );
    m_renderer->draw3D( m_cube1 );

    m_driver->setModelMatrix( m_trs2.modelMat );
    m_renderer->draw3D( m_cube2 );

    m_trs1.rotate.x += 1.0f;
    m_trs1.rotate.y += 5.0f;
    m_trs1.update();

    m_trs2.rotate.x -= 1.0f;
    m_trs2.rotate.y += 3.0f;
    m_trs2.update();

    m_driver->setCamera( old_camera );
}
