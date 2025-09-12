#pragma once
#include <de/scene/ICamera.hpp>

namespace de {
namespace scene {

using ::de::gpu::SM3;

// =======================================================================
struct Camera : public ICamera
// =======================================================================
{
   Camera();
   ~Camera() override {}
   // interface [Main]
   //void onAnimate( double seconds );
   // void onEvent( SEvent const & event );
   void update() override;

   // interface [Name]
   std::string const & getName() const override { return m_Name; }
   void setName( std::string const & name ) override { m_Name = name; }

   // interface [InputEnabled]
   bool isInputEnabled() const override { return m_IsInputEnabled; }
   void setInputEnabled( bool enable ) override { m_IsInputEnabled = enable; }

   // interface [Matrices] (used in many shaders)
   M4 const & getViewProjectionMatrix() const override { return m_VP; }
   M4 const & getViewMatrix() const override { return m_ViewMatrix; }
   M4 const & getProjectionMatrix() const override { return m_ProjectionMatrix; }

   // interface [ViewMatrix] (used in many shaders, fog, etc..)
   V3 getPos() const override { return m_Pos; }
   V3 getTarget() const override { return m_Target; }
   V3 getUp() const override { return m_Up; }
   void setPos( V3 pos ) override;
   void setDir( V3 dir ) override;
   void setUp( V3 up ) override;
   void lookAt( V3 pos, V3 target, V3 up = V3( 0, 1, 0) ) override;
   void move( T dt ) override;
   void strafe( T dt ) override;
   void elevate( T dt ) override;
   void yaw( T degrees ) override;
   void pitch( T degrees ) override;
   void roll( T degrees ) override;

   // interface [ProjectionMatrix]
   bool isOrtho() const override { return m_IsOrtho; }
   T getNearValue() const override { return m_ZNear; }
   T getFarValue() const override { return m_ZFar; }
   T getFOV() const override { return m_Fovy; }
   void setProjectionMode( bool ortho = false ) override;
   void setNearValue( T nearValue ) override;
   void setFarValue( T farValue ) override;
   void setFOV( T fovy_degrees ) override;

   // interface [Viewport]
   Recti const & getViewport() const override { return m_Viewport; }
   void setViewport( Recti const & viewport ) override;

   // interface [MoveSpeeds]
   void setMoveSpeed( T vel = T(5) ) override { m_MoveSpeed = vel; }
   void setStrafeSpeed( T vel = T(5) ) override { m_StrafeSpeed = vel; }
   void setUpSpeed( T vel = T(5) ) override { m_UpSpeed = vel; }
   void setTurnSpeed( T vel = T(5) ) override { m_TurnSpeed = vel; }

   // interface [RayCast]
   Ray3d computeRay( int mouse_x, int mouse_y ) const override;
   Ray3f computeRayf( int mouse_x, int mouse_y ) const override;

   DE_CREATE_LOGGER("de.CameraFPS_XZ")

   // -=== [ConfigData] ===-
   int m_logLevel;      // [Logging]
   std::string m_Name;  // [Name]
   bool m_IsOrtho;               // [Flags]
   bool m_IsInputEnabled;        // [Flags]
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
   bool m_IsDragging;      // [Control]
   bool m_IsLeftPressed;   // [Control] left mb pressed
   bool m_IsMiddlePressed; // [Control]
   bool m_IsRightPressed;  // [Control]

   std::array< bool, 1024 > m_Keys; // Control
   SM3< int32_t > m_MouseX;   // [Control] Mouse Pos X
   SM3< int32_t > m_MouseY;   // [Control] Mouse Pos Y
   int32_t m_MouseMoveX;
   int32_t m_MouseMoveY;
   int32_t m_DragStartX;   // [Control] Mouse Pos X at right click start
   int32_t m_DragStartY;   // [Control] Mouse Pos Y at right click start
   int32_t m_DragMoveX;    // [Control] Mouse Move X at right click start
   int32_t m_DragMoveY;    // [Control] Mouse Move Y at right click start

   double m_Time;          // Animation ?
   double m_LastTime;      // Animation ?
};

} // end namespace scene.
} // end namespace de.
