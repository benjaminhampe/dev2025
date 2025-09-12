#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>

namespace learnopengl {
// Equals:
inline bool dbEquals( float a, float b, float eps = 1.0e-6f ) { return std::abs( b-a ) <= eps; }
inline bool dbEquals( double a, double b, double eps = 1.0e-9 ) { return std::abs( b-a ) <= eps; }
// SQRT():
inline float dbSqrt( float v ) { return ::sqrtf( v ); }
inline double dbSqrt( double v ) { return ::sqrt( v ); }
// DEG: ( convert radians to degrees )
inline float dbDEG( float radians ) { return radians * float(180.0 / 3.1415926535897932384626433832795028841971693993751); }
inline double dbDEG( double radians ) { return radians * double(180.0 / 3.1415926535897932384626433832795028841971693993751); }
// RAD: ( convert degrees to radians )
inline float dbRAD( float degrees ) { return degrees * float(3.1415926535897932384626433832795028841971693993751 / 180.0); }
inline double dbRAD( double degrees ) { return degrees * double(3.1415926535897932384626433832795028841971693993751 / 180.0); }
// Abs():
inline float dbAbs( float v ) { return std::fabs( v ); }
inline double dbAbs( double v ) { return std::fabs( v ); }
// Sin():
inline float dbSin( float v ) { return ::sinf( v ); }
inline double dbSin( double v ) { return ::sin( v ); }
// Cos():
inline float dbCos( float v ) { return ::cosf( v ); }
inline double dbCos( double v ) { return ::cos( v ); }

template < typename T >
struct Quat
{
   constexpr static double PI64 = 3.1415926535897932384626433832795028841971693993751;

   constexpr static T PI = T(PI64);
   constexpr static T TWOPI = T(2.0 * PI64);

   //      typedef glm::tmat3x3< T > M3;
//      typedef glm::tmat4x4< T > M4;
//      typedef glm::tvec2< T > V2;
   typedef glm::tvec3< T > V3;
//      typedef glm::tvec4< T > V4;

   T a;     // Skalar
   T b,c,d; // Vektor

   Quat()
      : a( 1 ), b( 0 ), c( 0 ), d( 0 ) // Default is identity quat.
   {}

   Quat( T skalar, T vecX, T vecY, T vecZ )
      : a( skalar ), b( vecX ), c( vecY ), d( vecZ )
   {}

   Quat( Quat const & o )
      : a( o.a ), b( o.b ), c( o.c ), d( o.d )
   {}

   Quat( T const & skalar, V3 const & vector )
      : a( skalar ), b( vector.x ), c( vector.y ), d( vector.z )
   {}

   Quat& operator=( Quat const & o )
   {
      a = o.a; b = o.b; c = o.c; d = o.d;
      return *this;
   }

   // Build quaternion from 'axis and angle', angle in radians.
   static Quat
   fromAngleAxis( T angle_in_rad, T x, T y, T z  )
   {
      while ( angle_in_rad < dbRAD( -180.0 ) )
      {
         //DE_DEBUG("Correct angle ", in.angle )
         angle_in_rad += TWOPI;
      }

      while ( angle_in_rad > dbRAD( 180.0 ) )
      {
         //DE_DEBUG("Correct angle ", in.angle )
         angle_in_rad -= TWOPI;
      }

      // in.axis = glm::normalize( in.axis ); // happens in ctr of Angle
      T const h = angle_in_rad * T( 0.5 );
      T const s = dbSin( h );
      T const c = dbCos( h );
      Quat quat;
      quat.a = c;       // cos( a/2 )
      quat.b = x * s;   // sin( a/2 ) * axis
      quat.c = y * s;   // sin( a/2 ) * axis
      quat.d = z * s;   // sin( a/2 ) * axis
      return quat;
   }

   // (new) quat = (this) quat * (other) quat ( other = rhs = right-hand-side )
   Quat
   operator*( Quat const & rhs ) const
   {
      T const & s = rhs.a;
      T const & t = rhs.b;
      T const & u = rhs.c;
      T const & v = rhs.d;
      return Quat(
         (a*s) - (b*t) - (c*u) - (d*v),
         (a*t) + (b*s) + (c*v) - (d*u),   // i
         (a*u) - (b*v) + (c*s) + (d*t),   // j
         (a*v) + (b*u) - (c*t) + (d*s) ); // k
   }

   // We rotate this quat by given quat q
   Quat
   rotate( Quat const & quat ) const
   {
      Quat const & q = (*this);
      Quat const & q_conj = Quat< T >( a, -b, -c, -d );
      return (q * quat) * q_conj; // 2 reflections = 1 rotation.
//      Quat const & p = (*this);
//      Quat const & q = this->conj();
//      Quat o = (p * in) * q;
//      DE_DEBUG( "in = ", in )
//      DE_DEBUG( "q = ", p )
//      DE_DEBUG( "q'= ", q )
//      DE_DEBUG( "qiq'= ", o )
//      return o;
   }

   V3
   rotate( V3 const & nonunitVector ) const
   {
      Quat rot = rotate( Quat( T(0), glm::normalize( nonunitVector ) ) );
//      DE_DEBUG( "in = ", in )
//      DE_DEBUG( "p = ", p )
//      DE_DEBUG( "v = ", v )
//      DE_DEBUG( "q = ", q )
//      DE_DEBUG( "o = ", o )
      return rot.toDirection();
   }

   // Build AngleAxis from this pure rotation unit quaternion.
   V3
   toDirection() const
   {
      if ( dbEquals( T(1), a ) )
      {
         //DE_ERROR( "No angle, axis has no meaning" )
         return V3(0,0,1);
      }

      T t = T( 1 ) / dbSqrt( T( 1 ) - ( a * a ) );

      if ( std::isinf( t ) )
      {
         //DE_ERROR( "t is inf, ", toString())
         return V3(0,0,1);
      }

      if ( std::isnan( t ) )
      {
         //DE_ERROR( "t is NaN, ", toString())
         return V3(0,0,1);
      }

      V3 out;
      //out.angle = T( 2 ) * dbAcos( a ); // AngleAxis only axis for direction needed.
      out.x = b * t;  // Rotation axis x
      out.y = c * t;  // Rotation axis y
      out.z = d * t;  // Rotation axis z
      out = glm::normalize( out );
      return out;
   }

};

typedef Quat< float > Quatf;

// Defines several possible options for camera movement. Used as abstraction to stay away from window-system specific input methods
enum Camera_Movement 
{
   FORWARD,
   BACKWARD,
   LEFT,
   RIGHT
};

// Default camera values
const float YAW         = -90.0f;
const float PITCH       =  0.0f;
const float SPEED       =  2.5f;
const float SENSITIVITY =  0.1f;
const float ZOOM        =  45.0f;

// An abstract camera class that processes input and calculates the corresponding Euler Angles, Vectors and Matrices for use in OpenGL
struct Camera
{  
   // Final matrices
   glm::mat4 m_viewMat;
   glm::mat4 m_projMat;

   // View matrix
   glm::vec3 Position;
   glm::vec3 Target;
   glm::vec3 Front;
   glm::vec3 Up;
   glm::vec3 Right;
   glm::vec3 WorldUp;
   //float Yaw;
   //float Pitch;
   float MovementSpeed;
   float MouseSensitivity;
   
   // Projection matrix
   float Zoom;
   int m_screenWidth;
   int m_screenHeight;



   // Constructor with vectors
   Camera(  glm::vec3 pos = glm::vec3(0,0,0),
            glm::vec3 target = glm::vec3(0,0,1),
            glm::vec3 up = glm::vec3(0,1,0))
      : Position(pos)
      , Target(target)
      , Front(target-pos)
      , WorldUp(up)
      , MovementSpeed(SPEED)
      , MouseSensitivity(SENSITIVITY)
      , Zoom(ZOOM)
      , m_screenWidth( 1024 )
      , m_screenHeight( 768 )
   {
      //Yaw = yaw;
      //Pitch = pitch;
      updateViewMatrix();
      updateProjectionMatrix();
   }

   glm::mat4 const & getProjectionMatrix() const { return m_projMat; }
   glm::mat4 const & getViewMatrix() const { return m_viewMat; }
   glm::vec3 const & getPosition() const { return Position; }
   glm::vec3 const & getTarget() const { return Target; }
   glm::vec3 const & getFront() const { return Front; }
   glm::vec3 const & getUp() const { return Up; }
   glm::vec3 const & getRight() const { return Right; }

   void lookAt( glm::vec3 pos, glm::vec3 target )
   {
      Position = pos;
      Target = target;
      Front = Target - Position;
      updateViewMatrix();
   }

   void
   setScreenSize( int w, int h )
   {
      m_screenWidth = w;
      m_screenHeight = h;
      updateProjectionMatrix();
   }

   void
   yaw( float degrees )
   {
      float da = dbRAD( degrees );
      Quatf rotation_quaternion = Quatf::fromAngleAxis( da, 0,1,0 );
      glm::vec3 dir = Target - Position;
      //auto len = glm::length( dir );
      dir = rotation_quaternion.rotate( dir );
      Front = dir;
      Target = Position + dir; //  * len
      Up = rotation_quaternion.rotate( WorldUp );
      updateViewMatrix();
   }

   void
   pitch( float degrees )
   {
      float da = dbRAD( degrees );
      Quatf rotation_quaternion = Quatf::fromAngleAxis( da, Right.x, Right.y, Right.z );
      glm::vec3 dir = Target - Position;
      //auto len = glm::length( dir );
      dir = rotation_quaternion.rotate( dir );
      Front = dir;
      Target = Position + dir;
      Up = rotation_quaternion.rotate( WorldUp );
      updateViewMatrix();
   }

   // Processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
   void ProcessKeyboard(Camera_Movement direction, float deltaTime)
   {
      float velocity = MovementSpeed * deltaTime;
      if (direction == FORWARD)
      {
         glm::vec3 ds = Front * velocity;
         Position += ds;
         Target += ds;
         updateViewMatrix();
      }
      if (direction == BACKWARD)
      {
         glm::vec3 ds = -Front * velocity;
         Position += ds;
         Target += ds;
         updateViewMatrix();
      }
      if (direction == LEFT)
      {
         glm::vec3 ds = -Right * velocity;
         Position += ds;
         Target += ds;
         updateViewMatrix();
      }
      if (direction == RIGHT)
      {
         glm::vec3 ds = Right * velocity;
         Position += ds;
         Target += ds;
         updateViewMatrix();
      }
   }

   // Processes input received from a mouse input system.
   // Expects the offset value in both the x and y direction.
   void ProcessMouseMovement(float x, float y, GLboolean constrainPitch = true)
   {
      //std::cout << "ProcessMouseMovement(" << x << "," << y << ")" << std::endl;

      float Yaw   = x * MouseSensitivity;
      float Pitch = y * MouseSensitivity;

      if (constrainPitch)
      {
         if (Pitch > 89.0f) Pitch = 89.0f;
         if (Pitch < -89.0f) Pitch = -89.0f;
      }
      if ( Yaw != 0.0f )
      {
         yaw( Yaw );
      }
      if ( Pitch != 0.0f )
      {
         pitch( Pitch );
      }
   }

   // Processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
   void ProcessMouseScroll(float yoffset)
   {
      if (Zoom >= 1.0f && Zoom <= 45.0f) Zoom -= yoffset;
      if (Zoom <= 1.0f) Zoom = 1.0f;
      if (Zoom >= 45.0f) Zoom = 45.0f;
      updateProjectionMatrix();
   }

   void extractDataFromMatrices()
   {
      glm::mat4 const & viewMat = m_viewMat;
      glm::mat4 const & viewMatInv = glm::inverse( m_viewMat );
      //glm::vec3 camPos2 = glm::vec3(viewMatInv * glm::vec4( 0,0,0,1 ));
      glm::vec3 camPos2(viewMatInv[3][0], viewMatInv[3][1], viewMatInv[3][2]);
      //glm::vec3 camPos2( -viewMat[0][3], -viewMat[1][3], -viewMat[2][3] );
      glm::vec3 camUp2( viewMat[0][1], viewMat[1][1], viewMat[2][1] );
      glm::vec3 camRight2( viewMat[0][0], viewMat[1][0], viewMat[2][0] );
   }


private:

   /*
    * Column major glm view matrix:
    *
      float matrix[4][4] = {
       {  right[0],   right[1],   right[2],   right[3] }, // First co
       {     up[0],      up[1],      up[2],      up[3] }, // Second c
       { forward[0], forward[1], forward[2], forward[3] }, // Third co
       {position[0], position[1], position[2], position[3] } // Forth co
   };

      mat4 LookAtRH( vec3 eye, vec3 target, vec3 up )
      {
          vec3 zaxis = normal(eye - target);   // The "forward" vector.
          vec3 xaxis = normal(cross(up, zaxis));// The "right" vector.
          vec3 yaxis = cross(zaxis, xaxis);    // The "up" vector.
          // Create a 4x4 view matrix from the right, up, forward and eye po
          mat4 viewMatrix = {
              vec4(     xaxis.x,           yaxis.x,           zaxis.x,
              vec4(     xaxis.y,           yaxis.y,           zaxis.y,
              vec4(     xaxis.z,           yaxis.z,           zaxis.z,
              vec4(-dot( xaxis, eye ), -dot( yaxis, eye ), -dot( zaxis, eye
          };
          return viewMatrix;
      }
   */
   //DE_DEBUG("camUp2(",camUp2,"), camRight2(",camRight2,"), camPos2(",camPos2,")")

   void updateViewMatrix()
   {
      // Also re-calculate the Right and Up vector
      Right = glm::normalize(glm::cross(Front, WorldUp));
      // Normalize the vectors, because their length gets closer to 0
      // the more you look up or down which results in slower movement.
      Up    = glm::normalize(glm::cross(Right, Front));

      m_viewMat = glm::lookAt(Position, Target, Up);
   }

   void updateProjectionMatrix()
   {
      m_projMat = glm::perspectiveFovLH(
                           glm::radians(Zoom),
                           (float)m_screenWidth, (float)m_screenHeight, .01f, 38000.0f);
   }
};


} // end namespace learnopengl.
