#pragma once
#include <irrlicht.h>
#include <sstream>

// #include <glm/glm.hpp>

// #include <windows.h>
// #include <time.h>

#if 0

inline std::string
toString( irr::core::dimension2du const & u )
{
    std::stringstream s;
    s << u.Width << ", " << u.Height;
    return s.str();
}

inline std::string
toString( irr::core::vector3df const & v )
{
    std::stringstream s;
    s << v.X << ", " << v.Y << ", " << v.Z;
    return s.str();
}

inline std::string
toString( glm::ivec2 const & v )
{
    std::stringstream s;
    s << v.x << ", " << v.y;
    return s.str();
}

inline std::string
toString( glm::vec2 const & v )
{
    std::stringstream s;
    s << v.x << ", " << v.y;
    return s.str();
}

inline std::string
toString( glm::vec3 const & v )
{
    std::stringstream s;
    s << v.x << ", " << v.y << ", " << v.z;
    return s.str();
}

inline float32_t
convertToNormalized( int16_t value )
{
    // ( make all symmetric around +0.5f )
    if ( value == 0 )
        return 0.5f;
    else if ( value > 0 )
        return 0.5f + static_cast< float32_t >(value)/65534.0f;
    else
        return 0.5f + static_cast< float32_t >(value)/65536.0f;
}

// inline int64_t
// getPerfTimer()
// {
    // LARGE_INTEGER li;
    // QueryPerformanceCounter( &li );
    // return static_cast< int64_t >( li.QuadPart );
// }

// inline int64_t
// getPerfTimerFrequency()
// {
    // LARGE_INTEGER li;
    // QueryPerformanceFrequency( &li );
    // return static_cast< int64_t >( li.QuadPart );
// }

// inline std::string
// getPerfTimerString()
// {
    // std::stringstream s;
    // s   << getPerfTimer() << ", f=" << getPerfTimerFrequency() << " Hz\n";
    // return s.str();
// }

// inline std::string
// getLocalTimeString()
// {
    // tm * t = localtime( nullptr );
    // std::stringstream s;
    // s   << t->tm_year << "-" << t->tm_mon << "-" << t->tm_mday << " "
        // << t->tm_hour << ":" << t->tm_min << ":" << t->tm_sec;
    // return s.str();
// }


inline std::string
getLocalTimeString()
{
    SYSTEMTIME t;
    GetLocalTime( &t );
    std::stringstream s;
    s   << t.wYear << "-" << t.wMonth << "-" << t.wDay << " "
        << t.wHour << ":" << t.wMinute << ":" << t.wSecond << "." << t.wMilliseconds;
    return s.str();
}


inline std::string
getSystemTimeString()
{
    SYSTEMTIME t;
    GetSystemTime( &t );
    std::stringstream s;
    s   << t.wYear << "-" << t.wMonth << "-" << t.wDay << " "
        << t.wHour << ":" << t.wMinute << ":" << t.wSecond << "." << t.wMilliseconds;
    return s.str();
}

class irrBackend
{
public:
    irrBackend();

    static void
    closeDevice( irr::IrrlichtDevice* & device )
    {
        if (device)
        {
            device->closeDevice();
            device->drop();
            device = nullptr;
        }
    }

    static inline irr::IrrlichtDevice*
    createNullDevice()
    {
        irr::IrrlichtDevice * device = irr::createDevice( irr::video::EDT_NULL );
        return device;
    }

    static inline irr::video::IImage*
    createImage( irr::video::IVideoDriver* driver, int32_t w, int32_t h, irr::video::SColor color = irr::video::SColor(255,0,200,0) )
    {
        irr::video::IImage* img = driver->createImage( irr::video::ECF_A8R8G8B8, irr::core::dimension2du(w,h) );
        img->fill( color );
        return img;
    }

    static inline irr::video::IImage*
    loadImage( irr::video::IVideoDriver* driver, std::string fileName )
    {
        irr::video::IImage* img = driver->createImageFromFile( fileName.c_str() );
        return img;
    }

    static inline bool
    saveImage( irr::video::IVideoDriver* driver, irr::video::IImage * img, std::string fileName )
    {
        return driver->writeImageToFile( img, fileName.c_str(), 0 );
    }


    static irr::video::IImage*
    createImage( int32_t w, int32_t h, irr::video::SColor color = irr::video::SColor(255,0,200,0) )
    {
        irr::IrrlichtDevice * device = createNullDevice();
        irr::video::IImage* img = device->getVideoDriver()->createImage( irr::video::ECF_A8R8G8B8, irr::core::dimension2du(w,h) );
        closeDevice( device );
        img->fill( color );
        return img;
    }

    static irr::video::IImage*
    loadImage( std::string fileName )
    {
        irr::IrrlichtDevice * device = createNullDevice();
        irr::video::IImage* img = device->getVideoDriver()->createImageFromFile( fileName.c_str() );
        closeDevice( device );
        return img;
    }


    static bool
    saveImage( irr::video::IImage * img, std::string fileName )
    {
        irr::IrrlichtDevice * device = createNullDevice();
        bool const retVal = device->getVideoDriver()->writeImageToFile( img, fileName.c_str(), 0 );
        closeDevice( device );
        return retVal;
    }


    static irr::IrrlichtDevice*
    createOpenGLDevice( int32_t w, int32_t h, bool fullscreen = false )
    {
        irr::SIrrlichtCreationParameters params;
        params.WindowSize = irr::core::dimension2du(
            static_cast< int32_t >( w ),
            static_cast< int32_t >( h ) );
        params.Bits = 32;
        params.AntiAlias = irr::video::EAAM_QUALITY;
        params.Doublebuffer = true;
        params.Vsync = false;
        params.DriverType = irr::video::EDT_OPENGL;
        params.HighPrecisionFPU = true;
        params.Fullscreen = fullscreen;
        params.Stencilbuffer = true;
        params.ZBufferBits = 24;
        irr::IrrlichtDevice* device = irr::createDeviceEx( params );
        if (!device)
        {
            std::cout << "ERROR: Cannot create device\n";
        }
        return device;
    }

    static void
    runDevice( irr::IrrlichtDevice * device )
    {
        if ( !device )
        {
            return;
        }

        irr::video::IVideoDriver* driver = device->getVideoDriver();
        irr::video::SColor clearColor( 255, 200, 200, 220 );
        irr::scene::ISceneManager* smgr = device->getSceneManager();
        irr::scene::ISceneNode* root = smgr->getRootSceneNode();
        irr::scene::IMesh* mesh = smgr->getGeometryCreator()->createSphereMesh( 50.f, 33, 33 );
        irr::scene::IMeshSceneNode* sphere = smgr->addMeshSceneNode( mesh, root, -1 );
        mesh->drop();
        sphere->setMaterialFlag( irr::video::EMF_LIGHTING, false );
        sphere->setMaterialFlag( irr::video::EMF_WIREFRAME, true );

        irr::scene::ICameraSceneNode* camera = smgr->addCameraSceneNodeFPS( root, 100.0f, 0.5f, -1 );
        camera->setPosition( irr::core::vector3df( 0,0,-1000) );
        camera->setTarget( irr::core::vector3df(0,0,0) );
        camera->setNearValue( 1.0f );
        camera->setFarValue( 10000.0f );

        while (device->run())
        {
//            if (device->isWindowActive())
//            {
                driver->beginScene( true, true, clearColor );
                smgr->drawAll();
                driver->endScene();
//            }
//            else
//            {
//                device->yield();
//            }
        }
    }

    static irr::scene::IMeshSceneNode *
    createEarthSceneNode( irr::IrrlichtDevice* device )
    {
        irr::video::IVideoDriver* driver = device->getVideoDriver();
        irr::scene::ISceneManager* smgr = device->getSceneManager();
        irr::scene::ISceneNode* root = smgr->getRootSceneNode();
        irr::scene::IMesh* sphere = smgr->getGeometryCreator()->createSphereMesh( 50.f, 33, 33 );
        irr::scene::IMeshSceneNode* node = smgr->addMeshSceneNode( sphere, root, -1 );
        sphere->drop();
        node->setMaterialFlag( irr::video::EMF_FOG_ENABLE, false );
        node->setMaterialFlag( irr::video::EMF_NORMALIZE_NORMALS, true );
        node->setMaterialFlag( irr::video::EMF_LIGHTING, false );
        //node->setMaterialFlag( irr::video::EMF_WIREFRAME, true );
        irr::video::ITexture * dayTex = driver->getTexture( "../../media/earth_day_8k.jpg" );
        node->setMaterialTexture( 0, dayTex );
        return node;
    }
};

#endif // 0
