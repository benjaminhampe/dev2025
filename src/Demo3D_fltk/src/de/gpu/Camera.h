#pragma once
#include <de/Math3D_Quaternion.h>
#include <de/Color.h>

namespace de {
namespace gpu {

// =======================================================================
struct Camera
// =======================================================================
{
    typedef double T;
    typedef glm::tmat4x4< T > M4;
    typedef glm::tvec2< T > V2;
    typedef glm::tvec3< T > V3;
    typedef glm::tvec4< T > V4;

    // -=== [ConfigData] ===-
    int m_logLevel;      // [Logging]
    bool m_IsOrtho;               // [Flags]
    bool m_IsInputEnabled;        // [Flags]
    bool m_IsViewportEnabled;        // [Flags]
    std::string m_Name;  // [Name]
    V3 m_Pos;      // [ViewMatrix] (0,0,1) P - Start source pos, Eye
    V3 m_Target;   // [ViewMatrix] (0,0,1) Q - Destination pos, LookAt
    V3 m_Up;       // [ViewMatrix] (0,1,0) Up vector on unit sphere around src pos
    //V3 m_Right;  // [ViewMatrix] (1,0,0) Side vector on unit sphere around src pos
    //V3 m_Dir;    // [ViewMatrix] (0,0,1) Front vector on unit sphere around src pos
    //V3 m_Ang;    // [ViewMatrix] Something computed
    Recti m_Viewport;       // [Viewport=ScreenSize+Pos]
    T m_ZNear;              // [ProjectionMatrix]
    T m_ZFar;               // [ProjectionMatrix]
    T m_Fovy;               // [ProjectionMatrix]
    T m_MoveSpeed;          // [Control] speed
    T m_StrafeSpeed;        // [Control] speed
    T m_UpSpeed;            // [Control] speed
    T m_TurnSpeed;          // [Control] speed

    // -=== [RuntimeData] ===-
    M4 m_ViewMatrix;        // [ViewMatrix]
    M4 m_ProjectionMatrix;  // [ProjectionMatrix]
    M4 m_VP;                // [ViewProjectionMatrix]
    bool m_IsDirty;               // [Flags]
    bool m_IsDirtyView;           // [Flags] ViewMatrix
    bool m_IsDirtyProjection;     // [Flags]
    // bool m_IsRightHanded;      // [Flags]
    // bool m_IsDragging;      // [Control]
    // bool m_IsLeftPressed;   // [Control] left mb pressed
    // bool m_IsMiddlePressed; // [Control]
    // bool m_IsRightPressed;  // [Control]

    // std::array< bool, 1024 > m_Keys; // Control
    // SM3< int32_t > m_MouseX;   // [Control] Mouse Pos X
    // SM3< int32_t > m_MouseY;   // [Control] Mouse Pos Y
    // int32_t m_MouseMoveX;
    // int32_t m_MouseMoveY;
    // int32_t m_DragStartX;   // [Control] Mouse Pos X at right click start
    // int32_t m_DragStartY;   // [Control] Mouse Pos Y at right click start
    // int32_t m_DragMoveX;    // [Control] Mouse Move X at right click start
    // int32_t m_DragMoveY;    // [Control] Mouse Move Y at right click start

    // double m_Time;          // Animation ?
    // double m_LastTime;      // Animation ?

    Camera();
    virtual ~Camera() = default;

    // interface [Main]
    //virtual void onAnimate( double seconds );
    //virtual void onEvent( SEvent const & event );

    // interface [Name]
    virtual std::string const & getName() const { return m_Name; }
    virtual void setName( std::string const & name ) { m_Name = name; }

    // interface [LogLevel]
    virtual int getLogLevel() const { return m_logLevel; }
    virtual void setLogLevel( int lvl ) { m_logLevel = lvl; }

    // interface [InputEnabled]
    virtual bool isViewportEnabled() const { return m_IsViewportEnabled; }
    virtual void setViewportEnabled( bool enable ) { m_IsInputEnabled = enable; }

    // interface [InputEnabled]
    virtual bool isInputEnabled() const { return m_IsInputEnabled; }
    virtual void setInputEnabled( bool enable ) { m_IsInputEnabled = enable; }

    // interface [Matrices] (used in many shaders)
    virtual void update();
    virtual M4 const & getViewMatrix() const;
    virtual M4 const & getProjectionMatrix() const;
    virtual M4 const & getViewProjectionMatrix() const;

    // interface [ViewMatrix] (used in many shaders, fog, etc..)
    // TODO: deconstruct from ViewMatrix directly
    virtual void setPos( V3 pos );
    virtual void setTarget( V3 pos );
    virtual void setDir( V3 dir );
    virtual void setUp( V3 up );
    virtual void setPos( T x, T y, T z );
    virtual void setTarget( T x, T y, T z );
    virtual void setDir( T x, T y, T z );
    virtual void setUp(  T x, T y, T z );
    virtual void setMoveSpeed( T velocity );
    virtual void setStrafeSpeed( T velocity );
    virtual void setUpSpeed( T velocity );
    virtual void setTurnSpeed( T velocity );
    virtual void move( T dt );
    virtual void strafe( T dt );
    virtual void elevate( T dt );
    virtual void yaw( T degrees );
    virtual void pitch( T degrees );
    virtual void roll( T degrees );
    virtual void lookAt( V3 pos, V3 target, V3 up = V3( 0, 1, 0) );
    virtual V3 getPos() const;
    virtual V3 getTarget() const;
    virtual V3 getUp() const;   
    virtual V3 getDir() const;
    virtual V3 getRight() const;
    virtual V3 getAng() const;
    virtual bool isRightHanded() const;

    // interface [ProjectionMatrix]
    virtual void setProjectionMode( bool ortho = false );
    virtual void setNearValue( T nearValue );
    virtual void setFarValue( T farValue );
    virtual void setFOV( T fovy_degrees );
    virtual bool isOrtho() const;
    virtual T getNearValue() const;
    virtual T getFarValue() const;
    virtual T getFOV() const;

    // interface [Viewport]
    virtual Recti const & getViewport() const;
    virtual void setViewport( Recti const & viewport );

    // interface [RayCast] convert (2d mouse screen pos) to (3d world ray)
    virtual Ray3d computeRay( int mouse_x, int mouse_y ) const;
    virtual Ray3f computeRayf( int mouse_x, int mouse_y ) const;

    // ==============================================================
    // quick-helper [Viewport]:
    // ==============================================================
    inline void setViewport( int x, int y, int w, int h ) { setViewport( Recti(x,y,w,h) ); }
    inline void setScreenSize( int32_t w, int32_t h ) { setViewport( Recti(0,0,w,h) ); }
    inline void setScreenSize( glm::ivec2 const & size ) { setViewport( Recti(0,0, size.x, size.y) ); }
    inline int32_t w() const { return getViewport().w; }    // Alias used by my shaders
    inline int32_t h() const { return getViewport().h; }  // Alias used by my shaders
    inline int32_t x() const { return getViewport().x; }
    inline int32_t y() const { return getViewport().y; }
    inline glm::ivec2 viewportSize() const
    {
        return {getViewport().w, getViewport().h};
    }
    inline int32_t getWidth() const { return getViewport().w; }    // Alias used by my shaders
    inline int32_t getHeight() const { return getViewport().h; }  // Alias used by my shaders
    inline int32_t getX() const { return getViewport().x; }
    inline int32_t getY() const { return getViewport().y; }
    inline glm::ivec2 getScreenSize() const
    {
        return {getViewport().w, getViewport().h};
    }
    inline T getAspectRatio() const { return ( h() < 1 ) ? 1.0f : T( w() ) / T( h() ); }
};
} // end namespace gpu.
} // end namespace de.
