#include <de/gpu/Camera.h>

namespace de {
namespace gpu {

Camera::Camera() 
    : m_logLevel(0)
    , m_IsOrtho( false )
    , m_IsInputEnabled( true )
    , m_Name("DefaultCamera")
    , m_Pos( 671, 936, -389 )
    , m_Target( 569, 628, 109 )
    , m_Up( 0,1,0 )
    , m_Viewport( 0, 0, 640, 480 )
    , m_ZNear( 0.2 )
    , m_ZFar( 38000.0 )
    , m_Fovy( 90.0 )
    , m_MoveSpeed( T(100) )
    , m_StrafeSpeed( T(100) )
    , m_UpSpeed( T(100) )
    , m_TurnSpeed( T(100) )
    , m_ViewMatrix( 1.0 )
    , m_ProjectionMatrix( 1.0 )
    , m_VP( 1.0 )
    , m_IsDirty( true )
    , m_IsDirtyView( true )
    , m_IsDirtyProjection( true )
    // , m_IsDragging( false )
    // , m_IsLeftPressed( false )
    // , m_IsMiddlePressed( false )
    // , m_IsRightPressed( false )
    // , m_MouseX(0)
    // , m_MouseY(0)
    // , m_MouseMoveX(0)
    // , m_MouseMoveY(0)
    // , m_DragStartX(0)
    // , m_DragStartY(0)
    // , m_DragMoveX(0)
    // , m_DragMoveY(0)
    // , m_Time( 0.0 )
    // , m_LastTime( 0.0 )
{
    update();
}

// [ViewMatrix]
void Camera::setPos( V3 pos )
{
   auto delta = pos - m_Pos;
   m_Pos = pos;
   m_Target += delta;
   m_IsDirtyView = true;
   update();
}

// [ViewMatrix]
void Camera::setTarget( V3 pos )
{
    //m_Pos = pos;
    m_Target = pos;
    //m_Up = up;
    //   m_Dir = glm::normalize( m_Target - m_Pos );
    //   auto left = glm::normalize( glm::cross( up, m_Dir ) );
    //   m_Up = glm::normalize( glm::cross( dir,left ) );
    //   //m_Dir = glm::normalize( m_Target - m_Pos );
    m_IsDirtyView = true;
    update();
    /*
    auto dir = m_Target - m_Pos;
    T dist = glm::length( dir );
    if ( dist < T(1) ) dist = T(1);
    m_Pos = m_Target + dir;

    m_Target = pos;

    dir = glm::normalize( dir );
    T len = glm::length( m_Target - m_Pos );
    if ( len < T(1) ) len = T(1);
    m_Target = m_Pos + dir * len;
    m_IsDirtyView = true;

    update();
    */
}


// [ViewMatrix]
void Camera::setDir( V3 dir )
{
   dir = glm::normalize( dir );
   T len = glm::length( m_Target - m_Pos );
   if ( len < T(1) ) len = T(1);
   m_Target = m_Pos + dir * len;
   m_IsDirtyView = true;
   update();
}

// [ViewMatrix]
void Camera::setUp( V3 up )
{
   m_Up = up; // glm::normalize( up );
   m_IsDirtyView = true;
   update();
}

void Camera::setPos( T x, T y, T z ) { setPos( {x,y,z} ); }
void Camera::setDir( T x, T y, T z ) { setDir( {x,y,z} ); }
void Camera::setTarget( T x, T y, T z ) { setTarget( {x,y,z} ); }
void Camera::setUp(  T x, T y, T z ) { setUp( {x,y,z} ); }    
void Camera::setMoveSpeed( T velocity ) { m_MoveSpeed = velocity; }
void Camera::setStrafeSpeed( T velocity ) { m_StrafeSpeed = velocity; }
void Camera::setUpSpeed( T velocity ) { m_UpSpeed = velocity; }
void Camera::setTurnSpeed( T velocity ) { m_TurnSpeed = velocity; }   

// [ViewMatrix]
void Camera::move( T dt )
{
   auto delta = getDir() * (m_MoveSpeed * dt);
   m_Pos += delta;
   m_Target += delta;
   if ( m_logLevel > 1 )
   {
      // DE_DEBUG("speed(", m_MoveSpeed,"), pos(",m_Pos,"), target(",m_Target,"), up(",m_Up,")")
   }
   m_IsDirtyView = true;
   update();
}

// [ViewMatrix]
void Camera::strafe( T dt )
{
   auto delta = getRight() * (m_MoveSpeed * dt);
   m_Pos += delta;
   m_Target += delta;
   if ( m_logLevel > 1 )
   {
      // DE_DEBUG("speed(", m_MoveSpeed,"), pos(",m_Pos,"), target(",m_Target,"), up(",m_Up,")")
   }
   m_IsDirtyView = true;
   update();
}

// [ViewMatrix]
void Camera::elevate( T dt )
{
   auto delta = getUp() * (m_UpSpeed * dt);
   m_Pos += delta;
   m_Target += delta;
   if ( m_logLevel > 1 )
   {
      // DE_DEBUG("speed(", m_UpSpeed,"), pos(",m_Pos,"), target(",m_Target,"), up(",m_Up,")")
   }
   m_IsDirtyView = true;
   update();
}

// [ViewMatrix]
void Camera::yaw( T degrees )
{
   T da = dbRAD( degrees * m_TurnSpeed );
   Quat< T > rot = Quat< T >::fromAngleAxis( da, Quat<T>::V3(0,1,0) );
   auto dir = m_Target-m_Pos;
   auto len = glm::length( dir );
   dir = rot.rotate( dir );
   m_Target = m_Pos + dir * len;
   m_Up = rot.rotate( m_Up );
   if ( m_logLevel > 1 )
   {
      // DE_DEBUG("degrees(", da,"), pos(",m_Pos,"), target(",m_Target,"), up(",m_Up,")")
   }
   m_IsDirtyView = true;
   update();
}

// [ViewMatrix]
void Camera::pitch( T degrees )
{
   T da = dbRAD( degrees * m_TurnSpeed );
   Quat< T > rot = Quat< T >::fromAngleAxis( da, getRight() );
   auto dir = m_Target-m_Pos;
   auto len = glm::length( dir );
   dir = rot.rotate( dir );
   m_Target = m_Pos + dir * len;
   m_Up = rot.rotate( m_Up );
   if ( m_logLevel > 1 )
   {
      // DE_DEBUG("degrees(", da,"), pos(",m_Pos,"), target(",m_Target,"), up(",m_Up,")")
   }
   m_IsDirtyView = true;
   update();
}

// [ViewMatrix]
void Camera::roll( T degrees )
{
   T da = dbRAD( degrees * m_TurnSpeed );
   auto dir = m_Target-m_Pos;
   auto len = glm::length( dir );
   dir = glm::normalize( dir );

   Quat< T > rot = Quat< T >::fromAngleAxis( da, dir );
   dir = rot.rotate( dir );

   m_Target = m_Pos + dir * len;
   m_Up = rot.rotate( m_Up );
   if ( m_logLevel > 1 )
   {
      //DE_DEBUG("degrees(", da,"), pos(",m_Pos,"), target(",m_Target,"), up(",m_Up,")")
   }
   m_IsDirtyView = true;
   update();
}

// [ViewMatrix]
void Camera::lookAt( V3 pos, V3 target, V3 up )
{
   m_Pos = pos;
   m_Target = target;
   m_Up = up;
//   m_Dir = glm::normalize( m_Target - m_Pos );
//   auto left = glm::normalize( glm::cross( up, m_Dir ) );
//   m_Up = glm::normalize( glm::cross( dir,left ) );
//   //m_Dir = glm::normalize( m_Target - m_Pos );
   m_IsDirtyView = true;
   update();
}

Camera::V3 Camera::getPos() const { return m_Pos; }
Camera::V3 Camera::getTarget() const { return m_Target; }
Camera::V3 Camera::getUp() const { return m_Up; }
Camera::V3 Camera::getDir() const { return glm::normalize( getTarget() - getPos() ); }
Camera::V3 Camera::getRight() const { return glm::normalize( glm::cross( getUp(), getDir() ) ); }
Camera::V3 Camera::getAng() const {  return Direction3< T >( getDir() ).toEulerAngles(); }
bool Camera::isRightHanded() const { return glm::dot( glm::cross( getRight(), getUp() ), getDir() ) >= T( 0 ); }




// [ProjectionMatrix]
void Camera::setProjectionMode( bool ortho )
{
   if ( m_IsOrtho == ortho ) return;
   m_IsOrtho = ortho;
   m_IsDirtyProjection = true;
   update();
}

// [ProjectionMatrix]
void Camera::setNearValue( T nearValue )
{
   if ( dbEquals(m_ZNear,nearValue) ) return;
   m_ZNear = nearValue;
   m_IsDirtyProjection = true;
   update();
}

// [ProjectionMatrix]
void Camera::setFarValue( T farValue )
{
   if ( dbEquals(m_ZFar,farValue) ) return;
   m_ZFar = farValue;
   m_IsDirtyProjection = true;
   update();
}

// [ProjectionMatrix]
void Camera::setFOV( T fovy_degrees )
{
   if ( fovy_degrees < T(1.0) ) fovy_degrees = T(1.0);
   if ( fovy_degrees > T(179.0) ) fovy_degrees = T(179.0);
   if ( dbEquals(m_Fovy,fovy_degrees) ) return;
   m_Fovy = fovy_degrees;
   m_IsDirtyProjection = true;
   update();
}

bool Camera::isOrtho() const { return m_IsOrtho; }
Camera::T Camera::getNearValue() const { return m_ZNear; }
Camera::T Camera::getFarValue() const { return m_ZFar; }
Camera::T Camera::getFOV() const { return m_Fovy; }

    
// [Viewport]
void Camera::setViewport( Recti const & viewport )
{
   if ( m_Viewport == viewport ) { return; }
   m_Viewport = viewport;
   m_IsDirtyProjection = true;
   update();
}

Recti const & Camera::getViewport() const { return m_Viewport; }



// [RayIntersection]
Ray3d Camera::computeRay( int mx, int my ) const
{
   int w = getWidth();
   int h = getHeight();
   //DE_DEBUG("Mouse(,",mx,",",mx,"), Screen(,",w,",",h,")")
   M4 viewProjectionInv = glm::inverse( getViewProjectionMatrix() );
   return Ray3d::compute3DWorldRayFrom2DScreen( mx, my, w, h, viewProjectionInv );
}

// [RayIntersection]
Ray3f Camera::computeRayf( int mouse_x, int mouse_y ) const
{
   Ray3d r3d = computeRay( mouse_x, mouse_y );
   return r3d.convert< float >();
}

// [Update]
void Camera::update()
{
   if ( m_IsDirtyView )
   {
      //m_ViewMatrix = de::Math::buildLookAtLH( m_Pos, m_Dir, m_Up );
      auto dir = glm::normalize( m_Target - m_Pos );
      auto up = glm::normalize( m_Up );
      auto left = glm::normalize( glm::cross( up, dir ) );
      m_Up = glm::normalize( glm::cross( dir,left ) );
      //up = glm::normalize( glm::cross( dir, left ) ); // recompute normalized
      m_ViewMatrix = M4( T(1) );
      T* m = glm::value_ptr( m_ViewMatrix );
      m[ 0 ] = left.x;
      m[ 4 ] = left.y;
      m[ 8 ] = left.z;
      m[ 1 ] = m_Up.x;
      m[ 5 ] = m_Up.y;
      m[ 9 ] = m_Up.z;
      m[ 2 ] = dir.x;
      m[ 6 ] = dir.y;
      m[ 10 ] = dir.z;
      m[ 12 ] = -glm::dot( left, m_Pos );
      m[ 13 ] = -glm::dot( m_Up, m_Pos );
      m[ 14 ] = -glm::dot( dir, m_Pos );
      //DE_DEBUG("pos(",pos,",) dir(",dir,"), up(",up,", left(",left,")")
      //m_Right = glm::normalize( glm::cross( m_Dir, m_Up ) ); // RH
      //m_Right = glm::normalize( glm::cross( m_Up, m_Dir ) ); // LH
      m_IsDirtyView = false;
      m_IsDirty = true;
      // DE_DEBUG( "[View] Pos(", m_Pos, "), Dir(", m_Dir, "), Up(", m_Up, "), Right(", m_Right, ") = ViewMat(", m_ViewMatrix, ")" )
   }

   if ( m_IsDirtyProjection )
   {
      double x( getX() );
      double y( getY() );
      double w( getWidth() );
      double h( getHeight() );
      if ( m_IsOrtho )
      {
         m_ProjectionMatrix = glm::orthoLH( T(x), T(x + w), T(y), T(y + h), m_ZNear, m_ZFar );
      }
      else
      {
         m_ProjectionMatrix = glm::perspectiveLH( dbRAD( m_Fovy ), T(w)/T(h), m_ZNear, m_ZFar );
      }
      m_IsDirtyProjection = false;
      m_IsDirty = true;
      // DE_DEBUG( "[Proj] Pos(", m_Pos, "), Dir(", m_Dir, "), Up(", m_Up, "), Right(", m_Right, "), Matrix(", m_ViewMatrix, ")" )
   }

   if ( m_IsDirty )
   {
      m_VP = m_ProjectionMatrix * m_ViewMatrix;
      m_IsDirty = false;
   }
}

Camera::M4 const & Camera::getViewMatrix() const { return m_ViewMatrix; }
Camera::M4 const & Camera::getProjectionMatrix() const { return m_ProjectionMatrix; }
Camera::M4 const & Camera::getViewProjectionMatrix() const { return m_VP; }



} // end namespace gpu.
} // end namespace de.
