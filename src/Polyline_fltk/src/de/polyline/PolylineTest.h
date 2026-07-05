//////////////////////////////////////////////////////////////////////
/// @file   Polyline.hpp
/// @brief  SVG Miter PolyLine (miter line joins, fastest polyline)
/// @author Copyright (c) Benjamin Hampe <benjaminhampe@gmx.de>
///
///	The author grants permission of free use, free distribution and
/// free usage for commercial applications even for modified sources.
///
///	What is not allowed is to remove the original authors name or claim
/// any intellectual rights. This original code was written in my spare time
/// and is distributed publicly as free software to anyone over GitLab and GitHub.
///
//////////////////////////////////////////////////////////////////////

#pragma once
#include <de/polyline/Polyline.h>
#include <de/gpu/VideoDriver.h>

namespace de {
namespace polyline {

constexpr bool USE_POINT_LOGGING = false;
constexpr uint32_t POINT_COUNT = 32*32;
constexpr float SINC_MIN = -65.0f;
constexpr float SINC_MAX = 65.0f;

struct PolylineTest
{
    typedef glm::vec2 V2;
    typedef glm::vec3 V3;

static void
testPolylineImpl(
    de::smesh::SMeshBuffer & dc,
    const V3& offset,
    const std::vector< V2 >& points,
    const std::string& name,
    const float lineWidth = 2.5f,
    const LineCap::ELineCap lineCap = LineCap::Square,
    const LineJoin::ELineJoin lineJoin = LineJoin::Bevel )
{
    DE_DEBUG(name,"( pos:",offset,", width:",lineWidth,", cap:",lineCap,", join:",lineJoin," )")

    auto getPointCount = [&]() -> uint32_t { return points.size(); };
    auto getPoint = [&]( uint32_t index ) -> V2 { return points[ index ]; };
    auto getVertexCount = [&]() -> uint32_t { return dc.getVertexCount(); };
    auto addFace = [&]( uint32_t a, uint32_t b, uint32_t c )
    {
        dc.addIndexedTriangle( a, b, c );
        DE_TRACE(name,"addFace( ",a,", ",b,", ",c," )")
    };
    auto addVertex = [&](const V2& pos, const V2& normal,
                         const uint32_t color, const V2& tex)
    {
        V2 p2 = pos + (normal * lineWidth);
        V3 p3 = offset + V3(p2,0.f);
        V3 n3 = V3{0,1,0};
        dc.addVertex( { p3, n3, color, tex } );
        DE_TRACE(name,".addVertex( pos{",p3,"}, normal{",normal,"}, color{",color,"}, tex{",tex,"} )")
    };

    /// [OUTLINE]
    dc.setPrimitiveType( de::gpu::PrimitiveType::Triangles );
    Polyline::asTriangles( getPointCount, getPoint, getVertexCount, addVertex, addFace, lineCap, lineJoin );
    dc.upload(true);
}

static void
test_TurnLeft( de::smesh::SMeshBuffer & dc, const V3& offset )
{
    float lineWidth = 2.5f;
    LineCap::ELineCap lineCap = LineCap::Square;
    LineJoin::ELineJoin lineJoin = LineJoin::Bevel;
    std::vector< V2 > points;
    points.push_back( V2( 10,0 ) );
    points.push_back( V2( 10,10 ) );
    points.push_back( V2( 0,10 ) );
    points.push_back( V2( 0,0 ) );
    testPolylineImpl( dc, offset, points, "TurnLeft", lineWidth, lineCap, lineJoin );
}

static void
test_TurnRight( de::smesh::SMeshBuffer & dc, const V3& offset )
{
    float lineWidth = 2.5f;
    LineCap::ELineCap lineCap = LineCap::Square;
    LineJoin::ELineJoin lineJoin = LineJoin::Bevel;
    std::vector< V2 > points;
    points.push_back( V2( 0,0 ) );
    points.push_back( V2( 0,10 ) );
    points.push_back( V2( 10,10 ) );
    points.push_back( V2( 10,0 ) );
    testPolylineImpl( dc, offset, points, "TurnRight", lineWidth, lineCap, lineJoin );
}

static std::string
toString( const std::vector< V2 >& points )
{
    std::ostringstream o;
    o << "points.size() = " << points.size() << "\n";
    for ( size_t i = 0; i < points.size(); ++i )
    {
        o << "points[" << i << "] = " << points[ i ] << "\n";
    }

    return o.str();
}

static void
sinc( std::vector< V2 > & points,
      size_t n,
      V2 pos,
      V2 size,
      float phi_start,
      float phi_end )
{
    size_t const s0 = points.size();
    points.reserve( s0 + n );
    points.resize( s0 );

    float const dx = float( size.x ); // 10 Umdrehungen in 1000 Schritten
    float const dy = float( size.y ); // 10 Umdrehungen in 1000 Schritten
    float const phi_delta = phi_end - phi_start; // 10 Umdrehungen in 1000 Schritten
    float const phi_step = phi_delta / float( n ); // 10 Umdrehungen in 1000 Schritten

    float phi;
    for ( size_t i = 0; i < n; ++i )
    {
        phi = phi_start + phi_step * i;
        float x = dx * ( phi - phi_start ) / phi_delta;

        // prevent div by zero
        if ( std::fabs( phi ) <= 0.0001f )
            phi = 0.0001f;

        float y = 0.2f * dy * sin( phi ) / phi;

        points.emplace_back( pos + V2( x, y ) );

        phi += phi_step;
    }
}

static void
test_PolyLine_asTriangles(
    de::smesh::SMeshBuffer & dc,
    const std::vector<V2> & points,
    const V3 & offset,
    const float lineWidth,
    const uint32_t color,
    const LineCap::ELineCap lineCap,
    const LineJoin::ELineJoin lineJoin )
{
    DE_DEBUG("( cap:",lineCap,", join:",lineJoin,", lineWidth:",lineWidth," )")

    auto getPointCount = [&] () -> uint32_t { return points.size(); };
    auto getPoint = [&] ( uint32_t const index ) { return points[ index ]; };
    auto getVertexCount = [&] () { return dc.getVertexCount(); };
    auto addVertex = [&] ( V2 const & pos, const V2& nrm, uint32_t color, const V2 & tex )
    {
        V2 p2 = pos + (nrm * lineWidth);
        V3 p3 = offset + V3(p2,0);
        dc.addVertex( { p3, V3{0,1,0}, color, tex} );
    };
    auto addFace = [ & ] ( uint32_t a, uint32_t b, uint32_t c )
    {
        dc.addIndexedTriangle( a, b, c );
    };

    dc.setPrimitiveType( de::gpu::PrimitiveType::Triangles );
    Polyline::asTriangles( getPointCount, getPoint, getVertexCount, addVertex, addFace, lineCap, lineJoin );
    dc.upload(true);
}


static void
test_PolyLine_asWireframe(
    de::smesh::SMeshBuffer & dc,
    const std::vector<V2> & points,
    const V3 & offset,
    const float lineWidth,
    const uint32_t color,
    const LineCap::ELineCap lineCap,
    const LineJoin::ELineJoin lineJoin )
{
    DE_DEBUG("( cap:",lineCap,", join:",lineJoin,", lineWidth:",lineWidth," )")

    auto getPointCount = [&] () -> uint32_t { return points.size(); };
    auto getPoint = [&] ( uint32_t const index ) { return points[ index ]; };
    auto getVertexCount = [&] () { return dc.getVertexCount(); };
    auto addVertex = [&] ( const V2& pos, const V2& nrm, uint32_t color, const V2& tex )
    {
        V2 p2 = pos + (nrm * lineWidth);
        V3 p3 = offset + V3(p2, 0);
        dc.addVertex( {p3, V3{0,1,0}, color, tex} );
    };
    auto addFace = [&] ( uint32_t a, uint32_t b, uint32_t c )
    {
        dc.addIndexedLineTriangle(a,b,c);
    };

    /// [OUTLINE]
    dc.setPrimitiveType( de::gpu::PrimitiveType::Lines );
    Polyline::asTriangles( getPointCount, getPoint, getVertexCount, addVertex, addFace, lineCap, lineJoin );
    dc.upload(true);
}

};


struct PolylineRenderer
{
    typedef glm::vec2 V2;
    typedef glm::vec3 V3;

    de::gpu::VideoDriver* m_driver{ nullptr };
    de::smesh::SMeshBuffer m_l;
    de::smesh::SMeshBuffer m_r;
    de::smesh::SMeshBuffer m_a;
    de::smesh::SMeshBuffer m_b;

    void init(de::gpu::VideoDriver* driver)
    {
        Polyline::testMath();
        m_driver = driver;
        auto pos = V3(0,0,0);
        PolylineTest::test_TurnLeft( m_l, pos + V3( 0, 0, 0 ) );
        PolylineTest::test_TurnRight( m_r, pos + V3( 50, 0, 0 ) );

        glm::vec2 size( 600, 1600 );
        // float outlineWidth = 2.5f;
        // float inlineWidth = 2.1f;
        float inlineWidth = 2.5f;
        float outlineWidth = 3.0f;
        LineCap::ELineCap lineCap = LineCap::Square;
        LineJoin::ELineJoin lineJoin = LineJoin::Bevel;

        std::vector< V2 > points;
        PolylineTest::sinc(
            points,
            POINT_COUNT,
            V2(0,0),
            V2(size.x, size.y),
            SINC_MIN,
            SINC_MAX );

        if ( USE_POINT_LOGGING )
        {
            DE_DEBUG("points.size(",points.size(),")")
            for ( size_t i = 0; i < points.size(); ++i )
            {
                DE_DEBUG("points[",i,"] = ",points[ i ])
            }
        }

        PolylineTest::test_PolyLine_asTriangles(
            m_a,
            points,
            pos + V3( 0,  50, 0 ),
            outlineWidth,
            0xFF0000FF,
            lineCap,
            lineJoin );

        PolylineTest::test_PolyLine_asWireframe(
            m_b,
            points,
            pos + V3( 0,  50, 0 ),
            outlineWidth,
            0xFF0080CF,
            lineCap,
            lineJoin );

       //test_PolyLine_asTriangles( dc, pos + V3( 100,50, 0 ) );

    }

    void draw()
    {
        if (!m_driver)
        {
            DE_ERROR("No driver")
            return;
        }

        m_driver->getSMaterialRenderer()->draw3D(m_l);
        m_driver->getSMaterialRenderer()->draw3D(m_r);

        m_driver->getSMaterialRenderer()->draw3D(m_a);
        m_driver->getSMaterialRenderer()->draw3D(m_b);
    }
};

} // end namespace polyline
} // end namespace de



/*

// =========================================================================

PrimitiveRenderer2D* painter = new PrimitiveRenderer2D( smgr, smgr->getRootSceneNode() );
PrimitiveRenderer2D & dc = *painter;
test_PolyLine( dc );


int dbInitGDK(int argc, char ** argv )
{
   irr::video::SColor clearColor( 255, 120, 120, 220 );
   srand( uint32_t( time( nullptr ) ) );
   std::cout,"---------------------------------------------------\n";
   std::cout,"BEGIN:\t\tmain_DtmGeometry_without_Bresenham()\n";
   std::cout,"---------------------------------------------------\n";
   //    std::cout,"SystemTime = ",getSystemTimeString(),"\n";
   //    std::cout,"LocalTime = ",getLocalTimeString(),"\n";
   //    std::cout,"PerfTimer = ",getPerfTimerString(),"\n";

   EventReceiver receiver;
   irr::SIrrlichtCreationParameters params;
   params.WindowSize = irr::core::dimension2du( 1200,800 );
   params.Bits = 32;
   params.AntiAlias = irr::video::EAAM_QUALITY;
   params.Doublebuffer = true;
   params.Vsync = true;
   params.DriverType = irr::video::EDT_OPENGL;
   params.HighPrecisionFPU = true;
   params.Fullscreen = false;
   params.Stencilbuffer = true;
   params.ZBufferBits = 16;
   params.EventReceiver = &receiver;
   irr::IrrlichtDevice* device = irr::createDeviceEx( params );
   if (!device)
   {
   std::cout,"ERROR: Cannot create device\n";
   return 0;
   }
   device->setResizable();
   receiver.setDevice( device );

   irr::video::IVideoDriver* driver = device->getVideoDriver();
   irr::scene::ISceneManager* smgr = device->getSceneManager();
   irr::scene::ISceneNode* root = smgr->getRootSceneNode();

   irr::SKeyMap keyMap[6];
   keyMap[0].Action = irr::EKA_MOVE_FORWARD;
   keyMap[0].KeyCode = irr::KEY_KEY_W;
   keyMap[1].Action = irr::EKA_MOVE_BACKWARD;
   keyMap[1].KeyCode = irr::KEY_KEY_S;
   keyMap[2].Action = irr::EKA_STRAFE_LEFT;
   keyMap[2].KeyCode = irr::KEY_KEY_A;
   keyMap[3].Action = irr::EKA_STRAFE_RIGHT;
   keyMap[3].KeyCode = irr::KEY_KEY_D;
   keyMap[4].Action = irr::EKA_CROUCH;
   keyMap[4].KeyCode = irr::KEY_KEY_C;
   keyMap[5].Action = irr::EKA_JUMP_UP;
   keyMap[5].KeyCode = irr::KEY_KEY_Q;

   irr::scene::ICameraSceneNode* camera = smgr->addCameraSceneNodeFPS( root, 100.0f, 0.025f, -1, keyMap, 6 );
   camera->setPosition( irr::core::vector3df( 0, 0.0f, 100.0f ) );
   camera->setTarget( irr::core::vector3df(0,0,0) );
   camera->setNearValue( 0.1f );
   camera->setFarValue( 10000.0f );

   // =========================================================================

   PrimitiveRenderer2D* painter = new PrimitiveRenderer2D( smgr, smgr->getRootSceneNode() );
   PrimitiveRenderer2D & dc = *painter;
   test_PolyLine( dc );

   // =========================================================================

   irr::ITimer* pTimer = device->getTimer();
   uint32_t t0 = pTimer->getRealTime();

   irr::gui::IGUIEnvironment* guienv = device->getGUIEnvironment();

   while (device->run())
   {
      if (device->isWindowActive())
      {
         driver->beginScene( true, true, clearColor );
         smgr->drawAll();
         if (guienv) guienv->drawAll();
         driver->endScene();

         if ( pTimer->getRealTime() - t0 > 250 )
         {
            irr::scene::ICameraSceneNode* activeCamera = smgr->getActiveCamera();

            std::wstringstream s;
            s,"FPS(",driver->getFPS(),"), ";
            if (activeCamera)
            {
               irr::core::vector3df camPos = activeCamera->getAbsolutePosition();
               irr::core::vector3df camRot = activeCamera->getAbsoluteTransformation().getRotationDegrees();
               s ,"CamPos(",int32_t(camPos.X) ,",",int32_t(camPos.Y) ,",",int32_t(camPos.Z),"), "
                 ,"CamRot(",int32_t(camRot.X) ,",",int32_t(camRot.Y) ,",",int32_t(camRot.Z),")";
               }

               device->setWindowCaption( s.str().c_str() );
               t0 = pTimer->getRealTime();
            }
      }
      else
      {
         device->yield();
      }
   }
   device->closeDevice();
   device->drop();
   return 42;
}
int main(int argc, char *argv[])
{
   dbInitGDK( argc, argv );
   return 0;
}
*/
