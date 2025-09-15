#pragma once
#include <de/gpu/VideoDriver.h>
#include <de/cpu/CPU.h>
#include <de/cpu/RAM.h>
#include <de/Math3D_Intersection.h>

// #include <cstdint>
// #include <sstream>
// #include <vector>
// #include <cmath>
// #include <algorithm>
// #include <de_glm.hpp>
// #include <de/ColorGradient.h>
// #include <de/image/ImagePainter.h>

// #include <de/gpu/renderer/ScreenRenderer.h>
// #include <de/gpu/renderer/FontRenderer5x8.h>
// #include <de/gpu/renderer/FontRenderer.h>
// #include <de/gpu/renderer/SkyboxRenderer.h>
// #include <de/gpu/smesh/SMeshLibrary.h>
// #include <de/gpu/smesh/SMeshRenderer.h>
// #include <de/gpu/mtl/MTL.h>

namespace de {
	
// ===========================================================================
struct IrrlichtDevice : public IEventReceiver
// ===========================================================================
{
    IrrlichtDevice();
    ~IrrlichtDevice() override;
    bool                open(int w, int h);
    void                close();
    bool                run();
    void                requestClose();
    void                onEvent( const Event& event ) override;
    bool                getKeyState( const EKEY key ) const { return m_window ? m_window->getKeyState(key) : false; } // m_keyStates[key]
    IEventReceiver*     getEventReceiver() { return m_receiver; }
    void                setEventReceiver( IEventReceiver* receiver ) { m_receiver = receiver; }
    gpu::VideoDriver*   getVideoDriver() const { return m_driver; }
    Window*             getWindow() const { return m_window; }
    int                 getMouseX() const { return m_mouseX; }
    int                 getMouseY() const { return m_mouseY; }
    int                 getCpuCount() { return de_cpu_count(); }
    int                 getCpuCoreCount() { return de_cpu_count(); }
    int                 getCpuThreadCount() { return de_cpu_count(); }
    int                 getCpuSpeedMHz() { return de_cpu_speed_Mhz(); }
    int                 getCpuUsage() { return de_cpu_usage(); }
    sys::CpuInfo        getCpuInfo() { return sys::cpuInfo(); }
    sys::RamInfo        getRamInfo() { return sys::ramInfo(); }
    sys::DriveInfos     getDriveInfos() { return sys::driveInfos(); }
    // Camera* getCamera() { return m_videoDriver->getCamera(); }
    void                killTimers();
    void                killTimer( uint32_t id );
    uint32_t            startTimer( uint32_t ms, bool singleShot = false );
public:
    Window* m_window;
    gpu::VideoDriver* m_driver;
    IEventReceiver* m_receiver; // We relay events to this receiver.
    // Timer
    //uint32_t m_fpsTimerId;
    //uint32_t m_capTimerId;

    bool m_bShouldRun;
    //bool m_bHelpOverlay;
    bool m_bLeftPressed;
    bool m_bRightPressed;
    bool m_bMiddlePressed;
    //bool m_firstMouse;
    int m_mouseX;
    int m_mouseY;
    // int m_mouseXlast;
    // int m_mouseYlast;
    // int m_mouseMoveX;
    // int m_mouseMoveY;

    /*
    // Timer
    double m_timeStart; // in [s]
    double m_timeNow; // in [s]
    double m_timeLastCameraUpdate; // in [s]
    double m_timeLastRenderUpdate; // in [s]
    double m_timeLastWindowTitleUpdate; // in [s]
	//   int m_dummy = (0);


	
    // =======================================================================
    // struct H0_Camera // SceneNode
    // =======================================================================
	//{	
	bool m_cameraMovable = false;
	de::gpu::Camera m_camera;
	//};
	
	// =======================================================================
    // struct H0_Renderer
    // =======================================================================
	//{	
	// de::gpu::SkyboxRenderer m_skyboxRenderer;
	// de::gpu::SMaterialRenderer m_smeshRenderer;
	// de::Font m_fontAwesome20;
	// de::Font m_fontGarton32;
	// de::Font m_fontCarib48;
	// de::gpu::FontRenderer m_fontRenderer;
	// de::BillboardRenderer m_billboardRenderer;
	// de::RainbowRenderer m_rainbowRenderer;
	//};
	
	// =======================================================================
    // struct H0_Meshes
    // =======================================================================
	//{	
	// de::OBJ m_obj;
	// Needed here to compute collision triangles (collision detection)
	// TODO: Somehow move to renderer?
	// de::gpu::SMeshBuffer m_tileMesh; // Unit hexagon, used for tiles.
	// de::gpu::SMeshBuffer m_chipMesh; // Unit cylinder, used for chips and waypoints (corners, edges)
	// de::gpu::SMeshBuffer m_cubeMesh; // Unit cube, used to compose dice, roads, farms and cities.
	// de::gpu::SMeshBuffer m_coneMesh; // Unit cone, used to compose players, thief.
	// de::gpu::SMeshBuffer m_sphereMesh; // Unit sphere, used to compose players, thief, sun, mouse pointer, etc...
	// de::gpu::SMeshBuffer m_cardMesh; // Unit rounded-rect, used to compose cards.
	// de::gpu::SMeshBuffer m_roadMesh; // Composed mesh for roads.
	// de::gpu::SMeshBuffer m_farmMesh; // Composed mesh for farms.
	// de::gpu::SMeshBuffer m_cityMesh; // Composed mesh for cities.
	// de::gpu::SMeshBuffer m_thiefMesh; // Composed mesh for thief.
	//};
*/	
};

}// end namespace de

/*
// =====================================================================
struct H3_UIElem
// =====================================================================
{
    enum eType
    {
        Panel = 0,
        Image,
        Button,
        Text,
        Count
    };

    std::string name;

    u32 id = 0;
    u32 typ = 0; // Panel = 0, Image, Button, Text, Count
    u32 radius = 0; // RoundedRect Radius
    u32 border = 3;
    u32 fillColor = 0xFFFFFFFF;
    u32 textColor = 0xFF000000;
    u32 borderColor = 0xFF848484;

    bool visible = true;
    bool keepAspectWidthOverHeight = false;
    bool keepAspectHeightOverWidth = false;

    de::Align align;
    de::Font5x8 font;

    de::Rectf posf;

    de::Recti start_pos;



    de::Recti pos;
    de::gpu::TexRef ref;

    std::string msg;

    std::string str() const
    {
        std::ostringstream s;
        s << name << ", "
            "id(" << id << "), typ(" << typ << "), "
            "msg(" << msg << "), pos:" << pos.str() << ", "
            "ref:" << ref.str();
        return s.str();
    }

    void updateSize( float fScale)
    {
        pos.w = fScale * start_pos.w;
        pos.h = fScale * start_pos.h;
    }
};

// ==================================================================
struct DeviceConfig
// ==================================================================
{
   // Screen0 - Is the given frameBuffer by OS, after we asked for it.
   //           Is always the window's client rect area GetClientRect().
   // 		  	 Controlled by dragging and resizing the window on desktop.
   //	Screen1 is the actual render target where we draw our scene onto
   //			  with resolution controlled by user
   irr::core::dimension2du ScreenSize = irr::core::dimension2du( 1024, 768 );
   uint32_t AntiAlias = irr::video::EAAM_OFF;
   uint32_t BitsPerPixel = 32;
   bool DoubleBuffered = true;
   bool VSync = false;
   uint32_t FrameTimeMs = 1000 / 60; // in [ms], waittime for next render for 60Hz fps.
   uint32_t TextureQuality = 3;
};

*/
