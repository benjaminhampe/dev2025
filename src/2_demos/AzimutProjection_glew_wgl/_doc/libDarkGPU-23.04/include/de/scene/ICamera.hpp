#pragma once
#include <de/gpu/Material.hpp>

namespace de {
namespace scene {

// =======================================================================
struct ICamera
// =======================================================================
{
   typedef double T;
   typedef glm::tmat4x4< T > M4;
   typedef glm::tvec2< T > V2;
   typedef glm::tvec3< T > V3;
   typedef glm::tvec4< T > V4;

   virtual ~ICamera() = default;

   // interface [Main]
   //void onAnimate( double seconds );
   //void onEvent( SEvent const & event );
   virtual void update() = 0;

   // interface [Name]
   virtual std::string const & getName() const = 0;
   virtual void setName( std::string const & name ) = 0;

   // interface [InputEnabled]
   virtual bool isInputEnabled() const = 0;
   virtual void setInputEnabled( bool enable ) = 0;

   // interface [Matrices] (used in many shaders)
   virtual M4 const & getViewMatrix() const = 0;
   virtual M4 const & getProjectionMatrix() const = 0;
   virtual M4 const & getViewProjectionMatrix() const = 0;

   // interface [ViewMatrix] (used in many shaders, fog, etc..)
   // TODO: deconstruct from ViewMatrix directly
   virtual V3 getPos() const = 0; // Used atleast in fog shader!
   virtual V3 getTarget() const = 0;
   virtual V3 getUp() const = 0;
   virtual void setPos( V3 pos ) = 0;
   virtual void setDir( V3 dir ) = 0;
   virtual void setUp( V3 up ) = 0;
   virtual void lookAt( V3 pos, V3 target, V3 up = V3( 0, 1, 0) ) = 0;
   virtual void move( T dt ) = 0;
   virtual void strafe( T dt ) = 0;
   virtual void elevate( T dt ) = 0;
   virtual void yaw( T degrees ) = 0;
   virtual void pitch( T degrees ) = 0;
   virtual void roll( T degrees ) = 0;

   // interface [ProjectionMatrix]
   virtual bool isOrtho() const = 0;
   virtual T getNearValue() const = 0;
   virtual T getFarValue() const = 0;
   virtual T getFOV() const = 0;
   virtual void setProjectionMode( bool ortho = false ) = 0;
   virtual void setNearValue( T nearValue ) = 0;
   virtual void setFarValue( T farValue ) = 0;
   virtual void setFOV( T fovy_degrees ) = 0;

   // interface [Viewport]
   virtual Recti const & getViewport() const = 0;
   virtual void setViewport( Recti const & viewport ) = 0;

   // interface [MoveSpeeds]
   virtual void setMoveSpeed( T vel ) = 0;
   virtual void setStrafeSpeed( T vel ) = 0;
   virtual void setUpSpeed( T vel ) = 0;
   virtual void setTurnSpeed( T vel ) = 0;

   // interface [RayCast] convert (2d mouse screen pos) to (3d world ray)
   virtual Ray3d computeRay( int mouse_x, int mouse_y ) const = 0;
   virtual Ray3f computeRayf( int mouse_x, int mouse_y ) const = 0;

   // ==============================================================
   // quick-helper:
   // ==============================================================

   // quick-helper [ViewMatrix]
   void setPos( T x, T y, T z ) { setPos( {x,y,z} ); }
   void setDir( T x, T y, T z ) { setDir( {x,y,z} ); }
   void setUp(  T x, T y, T z ) { setUp( {x,y,z} ); }

   // quick-helper [ViewMatrix]
   V3 getDir() const
   {
      return glm::normalize( getTarget() - getPos() );
   }
   // quick-helper [ViewMatrix]
   V3 getRight() const
   {
      return glm::normalize( glm::cross( getUp(), getDir() ) );
   }
   // quick-helper [ViewMatrix]
   V3 getAng() const
   {
      //return Quat< T >::fromDirection( getDir() ).toEulerAngles().toDegrees();
      return Direction3< T >( getDir() ).toEulerAngles();
   }
   // quick-helper [ViewMatrix]
   bool isRightHanded() const
   {
      return glm::dot( glm::cross( getRight(), getUp() ), getDir() ) >= T( 0 );
   }

   // quick-helper [ProjectionMatrix]

   // quick-helper [Viewport]
   void setViewport( int32_t x, int32_t y, int32_t w, int32_t h )
   {
      setViewport( Recti(x,y,w,h) );
   }

   // quick-helper [Viewport]
   void setScreenSize( int32_t w, int32_t h )
   {
      setViewport( Recti(0,0,w,h) );
   }

   // quick-helper [Viewport]
   void setScreenSize( glm::ivec2 const & size )
   {
      setViewport( Recti(0,0, size.x, size.y) );
   }

   // quick-helper [Viewport]
   int32_t w() const { return getViewport().w(); }    // Alias used by my shaders
   int32_t h() const { return getViewport().h(); }  // Alias used by my shaders
   int32_t x() const { return getViewport().x(); }
   int32_t y() const { return getViewport().y(); }
   glm::ivec2 viewportSize() const { return getViewport().size(); }

   // quick-helper [Viewport]
   int32_t getWidth() const { return getViewport().w(); }    // Alias used by my shaders
   int32_t getHeight() const { return getViewport().h(); }  // Alias used by my shaders
   int32_t getX() const { return getViewport().x(); }
   int32_t getY() const { return getViewport().y(); }
   glm::ivec2 getScreenSize() const { return getViewport().size(); }

   // quick-helper [Viewport]
   T getAspectRatio() const
   {
      return ( h() < 1 ) ? 1.0f : T( w() ) / T( h() );
   }
};

} // end namespace scene.
} // end namespace de.
