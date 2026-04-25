#pragma once
#include <de/Math3D.h>
// #include <cstdint>
// #include <cstdlib>
// #include <cstdio>
// #include <string>
// #include <sstream>
// #include <vector>
// #include <functional>
// #include <de_glm.hpp>
// #include <tinyxml2/tinyxml2.h>

// #include <de_opengles32.hpp>
// #include <de/GL_Validate.hpp>

// #ifndef SAFE_DELETE
// #define SAFE_DELETE(x) if ( (x) ) { delete (x); (x) = nullptr; }
// #endif

namespace de {

// =======================================================================
// AngleAxis = PrincipalRotationParameters = PRV
// =======================================================================
// angle in radians + axis (x,y,z)
template < typename T >
struct AngleAxis
{
   typedef glm::tvec3< T > V3;
   typedef glm::tmat3x3< T > M3;

   T angle; // in radians
   V3 axis;
   AngleAxis()
      : angle(1), axis(0,0,1) // default is 0° about arbitrary axis
   {}
   AngleAxis( T radians, V3 achse )
      : angle(radians), axis( glm::normalize( achse ) )
   {}
   AngleAxis( T radians, T x, T y, T z )
      : angle(radians), axis( glm::normalize( V3(x,y,z) ) )
   {}

   std::string
   toString() const
   {
      std::stringstream s;
      s << "Angle:" << dbDEG( angle ) << "°|Axis:" << axis;
      return s.str();
   }

   /// @brief Create Rotation Matrix 3x3 (row-major).
   //
   //           [xxQ + c   yxQ + zs  zxQ - ys ]
   // DCM [C] = [xyQ - zs  yyQ + c   zyQ + xs ]
   //           [xzQ + ys  yzQ - xs  zzQ + c ]
   //
   // with c = cos( phi ) AND s = sin( phi ) AND Q = (1-c);
   //
   //           [xx(1-c) + c   yx(1-c) + zs  zx(1-c) - ys ]
   // DCM [C] = [xy(1-c) - zs  yy(1-c) + c   zy(1-c) + xs ]
   //           [xz(1-c) + ys  yz(1-c) - xs  zz(1-c) + c ]
   //
   M3
   toMat3() const // benni row-major
   {
      T const & w = angle;
      T const & x = axis.x;
      T const & y = axis.y;
      T const & z = axis.z;

      T s = sin( w );
      T c = cos( w );
      T Q = T(1) - c;

      M3 out( T(1) );
      T * m = glm::value_ptr( out );
      m[0] = x*x*Q + c;    // M11
      m[1] = x*y*Q + z*s;  // M12
      m[2] = x*z*Q - y*s;  // M13

      m[3] = y*x*Q - z*s;  // M21
      m[4] = y*y*Q + c;    // M22
      m[5] = y*z*Q + x*s;  // M23

      m[6] = z*x*Q + y*s;  // M31
      m[7] = z*y*Q - x*s;  // M32
      m[8] = z*z*Q + c;  // M33

      return m;
   }

   // trace( M ) = 1 + 2*cos( phi );
   // phi = acos( 0.5 * trace( M ) - 0.5 );
   // nx = 1/(2sin(phi)) * ( M23 - M32 )
   // ny = 1/(2sin(phi)) * ( M31 - M13 )
   // nz = 1/(2sin(phi)) * ( M12 - M21 )
   static AngleAxis
   fromMat3( M3 const & m33 )
   {
      // Has 2 solutions, second p' = 2pi - p

      T const * const m = glm::value_ptr( m33 );
      T trace = m[0] + m[4] + m[8]; // M11 + M22 + M33
      T phi = dbAcos( 0.5 * trace - 0.5 );

      V3 n = V3( m[5] - m[7], m[6] - m[2], m[1] - m[3] );
      T scale = T(1)/( T(2)* dbSin( phi ) );
      return AngleAxis( phi, n * scale );
   }
};

typedef AngleAxis< float > AngleAxisf;
typedef AngleAxis< double > AngleAxisd;

// =======================================================================
template < typename T > struct EulerAngles
// =======================================================================
{
   typedef glm::tvec3< T > V3;

   T yaw, pitch, roll; // in radians

   EulerAngles()
      : yaw(0), pitch(0), roll(0)
   {}
   EulerAngles( T y, T p, T r )
      : yaw(y), pitch(p), roll(r)
   {}

   static EulerAngles
   fromDegrees( T y, T p, T r )
   {
      return EulerAngles( dbRAD( y ), dbRAD( p ), dbRAD( r ) );
   }

   V3
   toDegrees() const { return V3{ yaw, pitch, roll } * Math3D<T>::RAD2DEG; }

   std::string
   toString() const
   {
      std::stringstream s;
      s << "Yaw:" << dbDEG( yaw ) << "|Pitch:" << dbDEG( pitch ) << "|Roll:" << dbDEG( roll );
      return s.str();
   }
};

typedef EulerAngles< float > EulerAnglesf;
typedef EulerAngles< double > EulerAnglesd;

//=============================================================================
/// @brief YawPitchRoll
//=============================================================================
template < typename T >
struct YawPitchRoll
{
   typedef glm::tvec3< T > V3;

   T yaw, pitch, roll;

   YawPitchRoll( T y = T(0), T p = T(0), T r = T(0) )
      : yaw( y ), pitch( p ), roll( r )
   {
      // DE_DEBUG("Constructor ", toString() )
   }


   std::string
   toString() const
   {
      std::stringstream s; s
      << "yaw = " << dbDEG( yaw ) << ","
      << "pitch = " << dbDEG( pitch ) << ","
      << "roll = " << dbDEG( roll );
      return s.str();
   }

   static YawPitchRoll
   fromRadians( T y = T(0), T p = T(0), T r = T(0) )
   {
      return YawPitchRoll( y, p, r );
   }

   static YawPitchRoll
   fromDegrees( T y = T(0), T p = T(0), T r = T(0) )
   {
      return YawPitchRoll( dbRAD( y ), dbRAD( p ), dbRAD( r ) );
   }

   /// @author Benjamin Hampe <benjaminhampe@gmx.de>
   /// @brief DirectionVector to EulerAngles
   /// @return A rotation vector containing the Y (pitch) and Z (yaw) rotations (in rad) that when applied to a
   /// direction vector(0,0,1) would make it point in the same direction as this vector.
   /// The X (roll) rotation is always 0, since two Euler rotations are sufficient to any given direction.
   static YawPitchRoll
   fromDirection( V3 const & dir )
   {
      V3 d( glm::normalize( dir ) );
      T const Pi2 = Math3D<T>::TWO_PI;
      T const PiH = Math3D<T>::HALF_PI;

      T pitch = dbAtan2( d.x, d.z );
      while ( pitch < 0.0f ) pitch += Pi2;
      while ( pitch >= Pi2 ) pitch -= Pi2;

      T yaw = dbAtan2( dbSqrt( d.x*d.x + d.z*d.z ), d.y ) - PiH;
      while ( yaw < T(0) ) yaw += Pi2;
      while ( yaw >= Pi2 ) yaw -= Pi2;

      return YawPitchRoll( yaw, pitch, T(0) );
   }

   V3
   toV3() const { return V3{ yaw, pitch, roll }; }

   V3
   toRadians() const { return V3{ yaw, pitch, roll }; }

   V3
   toDegrees() const { return glm::degrees( V3{ yaw, pitch, roll } ); }

   /// @author Benjamin Hampe <benjaminhampe@gmx.de>
   /// @brief EulerAngles -> DirectionVector
   // EulerAngles(rad) -> DirectionVector on unit sphere
   V3
   toDirection() const
   {
      //DE_DEBUG("")
#if 1
      T const pitch_s = dbSin( pitch );
      T const pitch_c = dbCos( pitch );
      T const yaw_s = dbSin( yaw );
      T const yaw_c = dbCos( yaw );
      return { pitch_s * yaw_c,
               pitch_s * yaw_s,
               pitch_c };
#else
      T const roll_s = dbSin( roll ); // 0.0
      T const roll_c = dbCos( roll ); // 1.0
      T const pitch_s = dbSin( pitch );
      T const pitch_c = dbCos( pitch );
      T const yaw_s = dbSin( yaw );
      T const yaw_c = dbCos( yaw );
      T const roll_c_pitch_s = roll_c * pitch_s;
      return { roll_c_pitch_s * yaw_c + roll_s * yaw_s,
               roll_c_pitch_s * yaw_s - roll_s * yaw_c,
               roll_c * pitch_c };
#endif
   }

   T getYaw() const { return yaw; }
   T getYawDegrees() const { return dbDEG( yaw ); }

   T getPitch() const { return pitch; }
   T getPitchDegrees() const { return dbDEG( pitch ); }

   T getRoll() const { return roll; }
   T getRollDegrees() const { return dbDEG( roll ); }

   T getMinYaw() const { return dbRAD( T(0.0) ); }
   T getMaxYaw() const { return dbRAD( T(360.0) ); }

   T getMinPitch() const { return dbRAD( T(-89.0) ); }
   T getMaxPitch() const { return dbRAD( T(89.0) ); }

   T getMinRoll() const { return dbRAD( T(-89.0) ); }
   T getMaxRoll() const { return dbRAD( T(89.0) ); }

   void Yaw( T y ) { setYaw( y + getYaw() ); }
   void Pitch( T y ) { setPitch( y + getPitch() ); }
   void Roll( T y ) { setRoll( y + getRoll() ); }

   void
   setYaw( T y )
   {
      yaw = y;
      if ( yaw < getMinYaw() )
      {
         //DE_DEBUG("Min-Limit-Yaw = ", yaw)
      }
      if ( yaw > getMaxYaw() )
      {
         //DE_DEBUG("Max-Limit-Yaw = ", yaw)
      }
   }

   void
   setPitch( T p )
   {
      pitch = p;
      if ( pitch < getMinPitch() )
      {
         //DE_DEBUG("Min-Limit-Pitch = ", pitch)
      }
      if ( pitch > getMaxPitch() )
      {
         //DE_DEBUG("Max-Limit-Pitch = ", pitch )
      }
   }

   void
   setRoll( T r )
   {
      roll = r;
      if ( roll < getMinRoll() )
      {
         //DE_DEBUG("Min-Limit-Roll = ", roll)
      }
      if ( roll > getMaxRoll() )
      {
         //DE_DEBUG("Max-Limit-Roll = ", roll)
      }
   }

/*
   ///@brief DirectionVector to EulerAngles
   ///@return A rotation vector containing the Y (pitch) and Z (yaw) rotations (in rad) that when applied to a
   /// direction vector(0,0,1) would make it point in the same direction as this vector.
   /// The X (roll) rotation is always 0, since two Euler rotations are sufficient to any given direction.
   YawPitchRoll< T >
   getRotation() const
   {
      YawPitchRoll< T > euler;
      V3 d( glm::normalize( m_Dir ) );
      euler.yaw = ::atan2( std::sqrt( d.x*d.x + d.z*d.z ), d.y ) - T(Math::HALF_PI64);
      euler.pitch = ::atan2( d.x, d.z );
//      if ( euler.x < 0.0f ) euler.x += Math::TWOPI;
//      if ( euler.x >= Math::TWOPI ) euler.x -= Math::TWOPI;
//      if ( euler.y < -Math::HALF_PI+0.0001f ) euler.y = -Math::HALF_PI+0.0001f;
//      if ( euler.y > Math::HALF_PI-0.0001f ) euler.y = Math::HALF_PI-0.0001f;
      // DE_DEBUG( "direction(", m_Dir, ") -> eulerAngles(", euler * Math::RAD2DEG, ")" )
      return euler;
   }

   ///@brief DirectionVector to EulerAngles
   ///@return A rotation vector containing the Y (pitch) and Z (yaw) rotations (in rad) that when applied to a
   /// direction vector(0,0,1) would make it point in the same direction as this vector.
   /// The X (roll) rotation is always 0, since two Euler rotations are sufficient to any given direction.
   void
   setRotation( YawPitchRoll< T > const & euler )
   {
      m_Dir = euler.toDirectionVector();
      m_IsDirtyView = true;
      update();
      // DE_DEBUG( "eulerAngles(", eulerRad * Math::RAD2DEG, ") -> direction(", m_Dir, ")" )
   }
*/
};

typedef YawPitchRoll< float > YawPitchRollf;
typedef YawPitchRoll< double > YawPitchRolld;

/*
/// @author Benjamin Hampe <benjaminhampe@gmx.de>
struct YawPitchRollTest
{
   DE_CREATE_LOGGER("de.YawPitchRollTest")

   static void
   test()
   {
      DE_DEBUG("BEGIN TESTS")

      // Test 0
      DE_DEBUG("BEGIN TEST[0]")
      {
         YawPitchRolld test0 = YawPitchRolld::fromDegrees( 0,0,0 );
         DE_DEBUG("TEST[0]: Input[0] = YawPitchRoll(", test0.toString(), ")" )
         glm::dvec3 direction = test0.toDirection();
         DE_DEBUG("TEST[0]: Output[0] Direction = ", direction )
         YawPitchRolld euler = YawPitchRolld::fromDirection( direction );
         DE_DEBUG("TEST[0]: Output[1] Back YawPitchRoll = ", euler.toString() )
      }
      DE_DEBUG("END TEST[0]")

      // Test 1
      DE_DEBUG("BEGIN TEST[1]")
      {
         YawPitchRolld test1 = YawPitchRolld::fromDegrees( 50,10,0 );
         DE_DEBUG("TEST[1]: Input[0] = YawPitchRoll(", test1.toString(), ")" )
         glm::dvec3 direction = test1.toDirection();
         DE_DEBUG("TEST[1]: Output[0] Direction = ", direction )
         YawPitchRolld euler = YawPitchRolld::fromDirection( direction );
         DE_DEBUG("TEST[1]: Output[1] Back YawPitchRoll = ", euler.toString() )
      }
      DE_DEBUG("END TEST[1]")

      DE_DEBUG("END TESTS")
   }

};
*/

// =======================================================================
// Quaternion
// =======================================================================
template < typename T >
struct Quat
{
   typedef glm::tmat3x3< T > M3;
   typedef glm::tmat4x4< T > M4;
   typedef glm::tvec2< T > V2;
   typedef glm::tvec3< T > V3;
   typedef glm::tvec4< T > V4;

   T a;     // Skalar
   T b,c,d; // Vektor


   Quat
   mul( Quat const & r ) const
   {
      T const & s = r.a;
      T const & t = r.b;
      T const & u = r.c;
      T const & v = r.d;
      return Quat(
         (a*s) - (b*t) - (c*u) - (d*v),
         (a*t) + (b*s) + (c*v) - (d*u),   // i
         (a*u) - (b*v) + (c*s) + (d*t),   // j
         (a*v) + (b*u) - (c*t) + (d*s) ); // k
   }

   // Quaternion: q = a + bi + cj + dk,    mit i^2 = j^2 = k^2 = ijk = -1
   //
   // A quaternion encodes a general rotation with an angle
   // ( 4 numbers, 1 angle + 3 direction components )
   // Any combination of many rotations result in exactly one
   // general rotation about an 'axis' (nx,ny,nz) and 'angle' phi in radians.
   //                     ->
   // A general AngleAxis(n,phi) encoded as quaternion q:
   //
   // a = cos(phi/2);
   // b = sin(phi/2)*n.x;
   // c = sin(phi/2)*n.y;
   // c = sin(phi/2)*n.z;
   // q = a + bi + cj + dk;    <- Same form from angleAxis inputs(phi,n)
   //
   // q = [cos(w),sin(w)xi,sin(w)yj,sin(w)zk) -> final stored Tupel for input(n,phi)
   //
   // Quaternionen Arithmetik:
   //
   // Addition:         q1 + q2 = (a1+a2) + (b1+b2)i + (c1+c2)j + (d1+d2)k
   //
   // Multiplikation:   q1 * q2
   //
   // q1 = ( a1 + b1i + c1j + d1k )
   // q2 = ( a2 + b2i + c2j + d2k )
   //
   // q1 * q2 =  a1 * ( a2 + b2*i + c2*j + d2*k )
   //          + b1*i*( a2 + b2*i + c2*j + d2*k )
   //          + c1*j*( a2 + b2*i + c2*j + d2*k )
   //          + d1*k*( a2 + b2*i + c2*j + d2*k )
   //
   // (q1*q2).a = a1*a2 - b1*b2 - c1*c2 - d1*d2
   // (q1*q2).b = a1*b2 + b1*a2 + c1*d2 - d1*c2
   // (q1*q2).c = a1*c2 - b1*d2 + c1*a2 + d1*b2
   // (q1*q2).d = a1*d2 + b1*c2 - c1*b2 + d1*a2
   //
   // return Quaternion(
   //         (a*s) - (b*t) - (c*u) - (d*v),
   //         (a*t) + (b*s) + (c*v) - (d*u),   // i
   //         (a*u) - (b*v) + (c*s) + (d*t),   // j
   //         (a*v) + (b*u) - (c*t) + (d*s) ); // k
   //
   // Division:         d3 = d1 / d2 = (r1 + e*t1 ) / ( r2 + e*t2 )
   //
   // q = a + bi + cj + dk;    <- The general algebraic (a,b,c,d) quaternion form.
   //                           ->
   // q = [cos(phi/2),sin(phi/2)n];    <- Same form from angleAxis inputs(phi,n)
   //
   //
   // Vektor n:
   //
   // n = (x,y,z) = (1,2,-3) -> rotation axis must be unitlen before encoding as quaternion.
   //
   // n = (x,y,z) = glm::normalize( n );
   //
   // Vektor n encoded as quaternion q: ( concept 'dir vec n' is a special 180° AngleAxis case )
   // A unit direction vector n can be directly encoded as Quaternion q.
   // The implicitly encoded angle phi=180° solves ambigouity of phi=0°
   //
   // q = Quaternion<T>::fromAngleAxis(180°,n.x,n.y,n.z);
   //
   // q = (0,n) = (0,x,y,z) = 0 + x*i + y*j + z*k, with phi=180°.
   //
   // A unit direction vector v can be directly encoded as Quaternion q
   // using phi=180° which represents the same vector but without ambigouity of phi=0°
   //

   //
   // Where a=0 represents the encoded angle of 180° = cos(90°) = 0.
   //
   // This is very convenient.
   //
   // Rotate a vector v using a quaternion(angle,axis) is the same operation as
   //
   // r = quaternion::fromAngleAxis(angle,axis);
   //
   // v' = ( r * v ) * r.conj();
   //
   // or
   //
   // v' = ( r * v ) * quaternion::conjugated( r );

   Quat()
      : a( 1 ), b( 0 ), c( 0 ), d( 0 ) // Default is identity quat.
   {}

   Quat( T a_, T b_, T c_, T d_ )
      : a( a_ ), b( b_ ), c( c_ ), d( d_ )
   {}

   Quat( Quat const & o )
      : a( o.a ), b( o.b ), c( o.c ), d( o.d )
   {}

   Quat( T const & aa, V3 const & bcd )
      : a( aa ), b( bcd.x ), c( bcd.y ), d( bcd.z )
   {}

   Quat& operator=( Quat const & o )
   {
      a = o.a; b = o.b; c = o.c; d = o.d;
      return *this;
   }

   static Quat
   slerp( Quat const & q, Quat const & p, T t )
   {
      T u = sin( (T(1) - t)* dbRAD(90.) );
      T v = sin( t * dbRAD(90.) );
      return q * u + p * v;
   }

   static V3
   slerp( V3 const & a, V3 const & b, T t )
   {
      Quat A = Quat::fromDirection( a );
      Quat B = Quat::fromDirection( b );
      return Quat< T >::slerp( A,B,t ).toDirection();
   }

   std::string
   toString() const
   {
      std::stringstream s;
      bool start = true;
      if ( dbAbs( a ) > T(1.e-14) )
      {
         start = false;
         s << a;
      }
      if ( dbAbs( b ) > T(1.e-14) )
      {
         if (!start) { if ( b > T(0) ) { s << "+"; } }
         if ( dbEquals( b, T(1) ) )
         {
            s << "i";
         }
         else if ( dbEquals( b,T(-1) ) )
         {
            s << "-i";
         }
         else
         {
            s << b << "i";
         }

         start = false;
      }
      if ( dbAbs( c ) > T(1.e-14) )
      {
         if (!start) { if ( c > T(0) ) { s << "+"; } }
         if ( dbEquals( c, T(1) ) )
         {
            s << "j";
         }
         else if ( dbEquals( c, T(-1) ) )
         {
            s << "-j";
         }
         else
         {
            s << c << "j";
         }
         start = false;
      }
      if ( dbAbs( d ) > T(1.e-14) )
      {
         if (!start) { if ( d > T(0) ) { s << "+"; } }
         if ( dbEquals( d, T(1) ) )
         {
            s << "k";
         }
         else if ( dbEquals( d, T(-1) ) )
         {
            s << "-k";
         }
         else
         {
            s << d << "k";
         }
         start = false;
      }
      return s.str();
   }

   bool
   operator==( Quat const & o ) const
   {
      return   dbEquals( a, o.a ) && dbEquals( b, o.b ) &&
               dbEquals( c, o.c ) && dbEquals( d, o.d );
   }

   bool
   operator!=( Quat const & o ) const
   {
      return !(*this == o);
   }

   // Skalare Addition: Quaternion + Skalar
   Quat
   operator+( T s ) const { return Quat( a+s, b+s, c+s, d+s ); }

   // Skalare Subtraktion: Quaternion - Skalar
   Quat
   operator-( T s ) const { return Quat( a-s, b-s, c-s, d-s ); }

   // Skalare Multiplikation: Quaternion * Skalar
   Quat
   operator*( T s ) const { return Quat( a*s, b*s, c*s, d*s ); }

   // Skalare Division: Quaternion / Skalar
   Quat
   operator/( T s ) const { return Quat( a/s, b/s, c/s, d/s ); }

   // Skalare Multiplikation: Quaternion *= Skalar
   Quat&
   operator*= ( T s ) { a*=s; b*=s; c*=s; d*=s; return *this; }

   // Skalare Division: Quaternion /= Skalar
   Quat&
   operator/= ( T s ) { a/=s; b/=s; c/=s; d/=s; return *this; }

   // Duale Addition: Quaternion + Quaternion
   Quat
   operator+( Quat const & o ) const { return Quat( a+o.a, b+o.b, c+o.c, d+o.d ); }

   // Duale Subtraktion: Quaternion - Quaternion
   Quat
   operator-( Quat const & o ) const { return Quat( a-o.a, b-o.b, c-o.c, d-o.d ); }

   // Duale Addition: Quaternion += Quaternion
   Quat &
   operator+=( Quat const & o ) { a += o.a; b += o.b; c += o.c; d += o.d; return *this; }

   // Duale Subtraktion: Quaternion -= Quaternion
   Quat &
   operator-=( Quat const & o ) { a -= o.a; b -= o.b; c -= o.c; d -= o.d; return *this; }

   // Duale Mul: Quaternion * Quaternion

   // q = ( a + bi + cj + dk )
   // r = ( s + ti + uj + vk )

   // q * r = ( a + bi + cj + dk ) * ( s + ti + uj + vk )
   //
   // q * r =  a * ( s + ti + uj + vk )
   //       + bi * ( s + ti + uj + vk )
   //       + cj * ( s + ti + uj + vk )
   //       + dk * ( s + ti + uj + vk )

   //
   // q * r =  as + ati + auj + avk
   //       + bsi + btii + buij + bvik
   //       + csj + ctji + cujj + cvjk
   //       + dsk + dtki + dukj + dvkk

   // q * r =  (as) + (at)i + (au)j + (av)k
   //       + (bs)i - (bt) + (bu)k - (bv)j
   //       + (cs)j - (ct)k - (cu) + (cv)i
   //       + (ds)k + (dt)j - (du)i - (dv)

   // q * r =  (as)  - (bt)  - (cu)  - (dv)
   //        + (at)i + (bs)i + (cv)i - (du)i
   //        + (au)j - (bv)j + (cs)j + (dt)j
   //        + (av)k + (bu)k - (ct)k + (ds)k

   // (q * r).a =  (as) - (bt) - (cu) - (dv)
   // (q * r).b = [(at) + (bs) + (cv) - (du)] i
   // (q * r).c = [(au) - (bv) + (cs) + (dt)] j
   // (q * r).d = [(av) + (bu) - (ct) + (ds)] k

   // qq'.a = as - bt - cu - dv
   // qq'.b = at + bs + cv - du
   // qq'.c = au - bv + cs + dt
   // qq'.d = av + bu - ct + ds

   Quat
   operator*( Quat const & r ) const
   {
      T const & s = r.a;
      T const & t = r.b;
      T const & u = r.c;
      T const & v = r.d;
      return Quat(
         (a*s) - (b*t) - (c*u) - (d*v),
         (a*t) + (b*s) + (c*v) - (d*u),   // i
         (a*u) - (b*v) + (c*s) + (d*t),   // j
         (a*v) + (b*u) - (c*t) + (d*s) ); // k
   }

   // qq' = [a,v][a',v']
   // qq' = [a + b*i + c*j + d*k][a' + b'i + c'j + d'k]
   // qq'.a = aa' - bb' - cc' - dd'
   // qq'.b = ab' + a'b + cd' - c'd
   // qq'.c = ac' + a'c + b'd - bd'
   // qq'.d = ad' + a'd + bc' - b'c

   // Benni substitutions: a' = s, b' = t, c' = u, d' = v
   // qq'.a = as - bt - cu - dv
   // qq'.b = at + sb + cv - ud
   // qq'.c = au + sc + td - bv
   // qq'.d = av + sd + bu - tc
   /*
   Quat
   operator*( Quat const & r ) const
   {
      T const & s = r.a;
      T const & t = r.b;
      T const & u = r.c;
      T const & v = r.d;
      return Quat(
         (a*s) - (b*t) - (c*u) - (d*v),
         (a*t) + (b*s) + (c*v) - (d*u),   // i
         (a*u) - (b*v) + (c*s) + (d*t),   // j
         (a*v) + (b*u) - (c*t) + (d*s) ); // k
   }
   */

// VIP 5: Div

   // Duale Div: q1 / q2 = q1*q2'/|q2||q2|

   Quat
   operator/( Quat const & r ) const
   {
      return (*this) * (this->inverse());
   }

   /// @brief This negates the vector part (b,c,d) of the Quat (a,b,c,d).
   // This creates a new instance of a Quat
   // This is for most efficiency ( on stack, no 'operator new' or some other crazy expensive allocation. )
   Quat
   conj() const { return Quat< T >( a, -b, -c, -d ); }

   Quat
   inverse() const
   {
      Quat< T > q = conj();

      T betrag = q.getLength();
      if ( betrag <= T(10)*std::numeric_limits< T >::epsilon() )
      {
         //DE_ERROR( "Betrag is too small(", betrag, "), quat(", toString(),")" )
         return q;
      }

      if ( dbEquals( betrag, T(1) ) )
      {
         return q; // q^-1 = q' for unit quaternion
      }

      //DE_WARN( "Not a unit quaternion, betrag(", betrag,"), quat(", toString(),")" )

      T betrag_inv = 1./betrag;

      if ( std::isinf( betrag_inv ) )
      {
         //DE_ERROR( "betrag_inv(", betrag_inv, ") is infinite, quat(", toString(),")" )
         return q;
      }

      if ( std::isnan( betrag_inv ) )
      {
         //DE_ERROR( "betrag_inv(", betrag_inv, ") is NaN, quat(", toString(),")" )
         return q;
      }

      return q * betrag_inv;
   }

// VIP 2:
   T
   getLengthSquared() const { return a*a + b*b + c*c + d*d; }

   T
   getLength() const { return dbSqrt( getLengthSquared() ); }

   void
   normalize()
   {
      T betrag_inv = T( 1 ) / getLength();

      if ( dbIsInf( betrag_inv ) )
      {
         //DE_ERROR( "Invalid betrag_inv(", betrag_inv, ")" )
      }
      else
      {
         a *= betrag_inv;
         b *= betrag_inv;
         c *= betrag_inv;
         d *= betrag_inv;
      }
   }

// VIP 6: Rotate Quat

//   // We rotate this quat by given quat q
//   Quat
//   rotatedBy( Quat q ) const
//   {
//      Quat const & i = (*this);
//      Quat c = q.conj();
//      Quat o = (q * i) * c;
////      DE_DEBUG( "i = ", i )
////      DE_DEBUG( "q = ", q )
////      DE_DEBUG( "q'= ", c )
////      DE_DEBUG( "o = qiq' = ", o )
//      return o;
//   }

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
   rotate( V3 const & v3 ) const
   {
      Quat rot = rotate( Quat( T(0), glm::normalize( v3 ) ) );
//      DE_DEBUG( "in = ", in )
//      DE_DEBUG( "p = ", p )
//      DE_DEBUG( "v = ", v )
//      DE_DEBUG( "q = ", q )
//      DE_DEBUG( "o = ", o )
      return rot.toDirection();
   }

   // Build quaternion from a point, phi = 180°
   static Quat
   fromPoint( V3 const & pos ) { return Quat( 0., pos.x, pos.y, pos.z ); } // angle = 180°

   // Build quaternion from a point, phi = 180°
   static Quat
   fromPoint( T x, T y, T z ) { return Quat( 0., x, y, z ); } // angle = 180°

   // Build quaternion from 'axis', phi = 180°.
   static Quat
   fromDirection( V3 const & axis ) { return fromPoint( glm::normalize( axis ) ); }

   // Build quaternion from 'axis and angle', angle in radians.
   static Quat
   fromDirection( T x, T y, T z ) { return fromDirection( V3(x,y,z) ); }

   V3
   toDirection() const
   {
      return toAngleAxis().axis;
   }

   V3
   toPoint() const
   {
      if ( !dbEquals( a, T( 0 ) ) )
      {
         //DE_WARN("Angle not 180, a must be 0, q(a,b,c,d) = ", toString() )
         return V3(0,1,0);
      }

      return V3( b, c, d ); // must be Quat(0,x,y,z)
   }

   // Negate vector part, skalar part stays const.
   static Quat
   fromEulerAngles( EulerAngles< T > const & euler ) // yaw (Z), pitch (Y), roll (X)
   {
      // Abbreviations for the various angular functions
      T cy = dbCos( euler.yaw * 0.5 );
      T sy = dbSin( euler.yaw * 0.5 );
      T cp = dbCos( euler.pitch * 0.5 );
      T sp = dbSin( euler.pitch * 0.5 );
      T cr = dbCos( euler.roll * 0.5 );
      T sr = dbSin( euler.roll * 0.5 );

      Quat< T > q;
      q.a = cr * cp * cy + sr * sp * sy;
      q.b = sr * cp * cy - cr * sp * sy;
      q.c = cr * sp * cy + sr * cp * sy;
      q.d = cr * cp * sy - sr * sp * cy;
      return q;
   }

   static Quat
   fromEulerAngles( V3 const & ypr ) // yaw (Z), pitch (Y), roll (X)
   {
      return fromEulerAngles( EulerAngles< T >( ypr.x, ypr.y, ypr.z ) );
   }

   static Quat
   fromEulerAngles( T const & y, T const & p, T const & r ) // yaw (Z), pitch (Y), roll (X)
   {
      return fromEulerAngles( EulerAngles< T >(y,p,r) );
   }

   // Negate vector part, skalar part stays const.
   EulerAngles< T > // yaw (Z), pitch (Y), roll (X)
   toEulerAngles() const
   {
      EulerAngles< T > euler;

      T pcos_rsin = T(2) * (a * b + c * d);
      T pcos_rcos = T(1) - ( T(2) * (b * b + c * c) );

      T pcos_ysin = T(2) * (a * d + b * c);
      T pcos_ycos = T(1) - ( T(2) * (c * c + d * d) );

      // roll (z-axis rotation)
      euler.roll = dbAtan2( pcos_rsin, pcos_rcos );

      // yaw (y-axis rotation)
      euler.yaw = dbAtan2( pcos_ysin, pcos_ycos );

      // pitch (x-axis rotation)
      T sinp = T(2) * (a * c - d * b);
      if ( dbAbs( sinp ) >= T(1) )
      {
         euler.pitch = std::copysign( M_PI / 2, sinp); // use 90 degrees if out of range
      }
      else
      {
         euler.pitch = dbAsin( sinp );
      }

      return euler;
   }

   // Build quaternion from 'axis and angle', angle in radians.
   static Quat
   fromAngleAxis( T radians, V3 const & axis )
   {
      return fromRotation( AngleAxis< T >( radians, axis ) );
   }

   // Build quaternion from 'axis and angle', angle in radians.
   static Quat
   fromAngleAxis( T radians, T x, T y, T z )
   {
      return fromRotation( AngleAxis< T >( radians,x,y,z ) );
   }


   // Build AngleAxis from this pure rotation unit quaternion.
   AngleAxis< T >
   toAngleAxis() const
   {
      if ( dbEquals( T(1), a ) )
      {
         //DE_ERROR( "No angle, axis has no meaning" )
         return AngleAxis< T >(0,0,0,1);
      }

      T t = T( 1 ) / dbSqrt( T( 1 ) - ( a * a ) );

      if ( std::isinf( t ) )
      {
         //DE_ERROR( "t is inf, ", toString())
         return AngleAxis< T >(0,0,0,1);
      }

      if ( std::isnan( t ) )
      {
         //DE_ERROR( "t is NaN, ", toString())
         return AngleAxis< T >(0,0,0,1);
      }

      AngleAxis< T > out;
      out.angle = T( 2 ) * dbAcos( a ); // Angle
      out.axis.x = b * t;  // Rotation axis x
      out.axis.y = c * t;  // Rotation axis y
      out.axis.z = d * t;  // Rotation axis z
      out.axis = glm::normalize( out.axis );
      return out;
   }

   // Build quaternion from 'axis and angle', angle in radians.
   static Quat
   fromRotation( AngleAxis< T > in )
   {
      while ( in.angle < dbRAD( -180.0 ) )
      {
         //DE_DEBUG("Correct angle ", in.angle )
         in.angle += Math::TWOPI64;
      }

      while ( in.angle > dbRAD( 180.0 ) )
      {
         //DE_DEBUG("Correct angle ", in.angle )
         in.angle -= Math::TWOPI64;
      }

      // in.axis = glm::normalize( in.axis ); // happens in ctr of Angle
      T const h = in.angle * T( 0.5 );
      T const s = dbSin( h );
      T const c = dbCos( h );
      Quat quat;
      quat.a = c;
      quat.b = in.axis.x * s;
      quat.c = in.axis.y * s;
      quat.d = in.axis.z * s;
      return quat;
   }

   // Returns a quaternion such that q * 'from' = to
   static Quat
   from2Vectors( V3 from, V3 to )
   {
      assert( glm::length(from) > T(0) );
      assert( glm::length(to) > T(0) );

      from = glm::normalize( from );
      to = glm::normalize( to );

      T cosPhi = glm::dot( from, to );
      if ( cosPhi < T(-0.999) )
      {
         // special case when vectors in opposite directions :
         // there is no "ideal" rotation axis
         // So guess one; any will do as long as it's perpendicular to start
         // This implementation favors a rotation around the Up axis,
         // since it's often what you want to do.
         V3 rotationAxis = glm::cross( V3(0,0,1), from );
         if ( glm::length2( rotationAxis ) < T(0.01) ) // bad luck, they were parallel, try again!
         {
            rotationAxis = glm::cross( V3(1,0,0), from );
         }

         rotationAxis = glm::normalize( rotationAxis) ;
         return fromAngleAxis( dbRAD(180.0), rotationAxis );
      }

//      T const d = glm::dot( u, v );
//      if ( std::abs( d ) >= T(1) ) // If |d| >= 1, the u = v
//      {
//         return Quat();
//      }
//      T phi = dbAcos( glm::dot(u,v)/(glm::length(u)*glm::length(v)) );
//      V3 axis = glm::normalize( glm::cross( u, v ) );
//      return fromAngleAxis( phi, axis );

      // Implementation from Stan Melax's Game Programming Gems 1 article
      V3 rotationAxis = glm::cross( from, to );
      T s = dbSqrt( T(2) * cosPhi + T(2) );
      T t = T(1) / s;
      return Quat( s * T(0.5), rotationAxis.x * t, rotationAxis.y * t, rotationAxis.z * t );
   }

   // Returns a quaternion that will make your object looking towards 'direction'.
   // Similar to RotationBetweenVectors, but also controls the vertical orientation.
   // This assumes that at rest, the object faces +Z.
   // Beware, the first parameter is a direction, not the target point !
   static Quat
   fromLookAt( V3 dir, V3 up )
   {
      if ( glm::length2( dir ) < T(0.0001) ) return Quat();

      // Recompute desiredUp so that it's perpendicular to the direction
      // You can skip that part if you really want to force desiredUp
      V3 right = glm::cross( dir, up );
      up = glm::cross( right, dir );

      // Find the rotation between the front of the object (that we assume towards +Z,
      // but this depends on your model) and the desired dir
      Quat rot1 = fromRotationBetweenVectors( V3(0,0,1), dir );
      // Because of the 1rst rotation, the up is probably completely screwed up.
      // Find the rotation between the "up" of the rotated object, and the desired up
      V3 newUp = rot1.rotate( V3( 0, 1, 0 ) );
      Quat rot2 = fromRotationBetweenVectors( newUp, up );
      // Apply them
      return rot2 * rot1; // remember, in reverse order.
   }

   // Like SLERP, but forbids rotation greater than maxAngle (in radians)
   // In conjunction to LookAt, can make your characters
   static Quat
   rotateTowards( Quat q1, Quat q2, T maxAngle )
   {
      if( maxAngle < T(0.001) )
      {
         // No rotation allowed. Prevent dividing by 0 later.
         return q1;
      }

      T cosTheta = glm::dot( q1, q2 );

      // q1 and q2 are already equal.
      // Force q2 just to be sure
      if ( cosTheta > T(0.9999) )
      {
         return q2;
      }

      // Avoid taking the long path around the sphere
      if ( cosTheta < 0 )
      {
         q1 = q1 * T(-1);
         cosTheta *= T(-1);
      }

      T angle = dbAcos( cosTheta );

      // If there is only a 2° difference, and we are allowed 5°,
      // then we arrived.
      if ( angle < maxAngle )
      {
         return q2;
      }

      // This is just like slerp(), but with a custom t
      T t = maxAngle / angle;
      angle = maxAngle;

      T f1 = dbSin( ( T(1) - t ) * angle );
      T f2 = dbSin( t * angle );
      T f3 = T(1) / dbSin( angle );
      Quat res = ( q1*f1 + q2*f2 ) * f3;
      res.normalize();
      return res;
   }


   /// @brief Create Rotation Matrix 4x4 benni|row-major.
   /// skalar a = w;
   /// vector b = x; c = y; d = z;
   ///
   //( a^2 + b^2 - c^2 - d^2     2(bc - ad)               2(bd + ac)      )
   //(      2(bc + ad)       a^2 - b^2 + c^2 - d^2        2(cd + ab)      )
   //(      2(bd - ac)           2(cd + ab)         a^2 - b^2 - c^2 + d^2 )
   M3
   toMat3() const
   {
      M3 out( T(1) );
      T * m = glm::value_ptr( out );
      m[0] = a*a + b*b - c*c - d*d;    // M11
      m[1] = T( 2 ) * ( b*c - a*d);    // M12
      m[2] = T( 2 ) * ( b*d + a*c);    // M13
      m[3] = T( 2 ) * ( b*c + a*d);    // M21
      m[4] = a*a - b*b + c*c - d*d;    // M22
      m[5] = T( 2 ) * ( c*d - a*b);    // M23
      m[6] = T( 2 ) * ( b*d - a*c);    // M31
      m[7] = T( 2 ) * ( c*d + a*b);    // M32
      m[8] = a*a - b*b - c*c + d*d;    // M33
      return out; // benni row-major
   }

   M4
   toMat4( V3 const & pos = V3() ) const
   {
      M4 out( T(1) );
      T * m = glm::value_ptr( out );
      m[0] = a*a + b*b - c*c - d*d;    // M11
      m[1] = T( 2 ) * ( b*c - a*d);    // M12
      m[2] = T( 2 ) * ( b*d + a*c);    // M13
      m[4] = T( 2 ) * ( b*c + a*d);    // M21
      m[5] = a*a - b*b + c*c - d*d;    // M22
      m[6] = T( 2 ) * ( c*d - a*b);    // M23
      m[8] = T( 2 ) * ( b*d - a*c);    // M31
      m[9] = T( 2 ) * ( c*d + a*b);    // M32
      m[10] = a*a - b*b - c*c + d*d;    // M33
      m[12] = pos.x;    // M41
      m[13] = pos.y;    // M42
      m[14] = pos.z;    // M43

      return out; // benni row-major
   }

   /// @brief Create Quat by extracting Axis and Angle from Matrix3x3
   ///
   //( a^2 + b^2 - c^2 - d^2     2(bc - ad)               2(bd + ac)      )
   //(      2(bc + ad)       a^2 - b^2 + c^2 - d^2        2(cd + ab)      )
   //(      2(bd - ac)           2(cd + ab)         a^2 - b^2 - c^2 + d^2 )
   //
   // a = +-0.5 * sqrt( M11 + M22 + M33 + 1 );
   // b = ( M23 - M32 ) / 4a;
   // c = ( M31 - M13 ) / 4a;
   // d = ( M12 - M21 ) / 4a;
   //
   // Spur( m33 ) = sum(mii) = 1 + 2*cos( ges_angle );
   // Axis = Eigenwert of m33 is the axis vector
   // -> aka. the vector that stays const after a rotation.
   // ( m33 - lambda*I33 )*x = 0

   Quat
   fromMat3( M3 const & m33 ) const
   {
      T const * const m = glm::value_ptr( m33 );
      Quat q;
      T betrag = T(0.5) * dbSqrt( m[ 0 ] + m[ 4 ] + m[ 8 ] + T(1) ); // (M11 + M22 + M33 + 1)
      T inv_b4 = T(1) / ( T(4) * betrag );
      q.a = betrag;
      q.b = (m[5] - m[7]) * inv_b4; // M23 - M32
      q.c = (m[6] - m[2]) * inv_b4; // M31 - M13
      q.d = (m[1] - m[3]) * inv_b4; // M12 - M21
      return q; // benni row-major
   }

   /// @brief Create Quat by extracting Axis and Angle from Matrix3x3
   ///
   //     ( M11 M12 M13 ) = ( 0 1 2 )
   // M = ( M21 M22 M23 ) = ( 3 4 5 )
   //     ( M31 M32 M33 ) = ( 6 7 8 )
   //
   // aa = 0.25 * ( 1 + trace(M) )
   // bb = 0.25 * ( 1 + 2*C11 - trace(M) )
   // cc = 0.25 * ( 1 + 2*C22 - trace(M) )
   // dd = 0.25 * ( 1 + 2*C33 - trace(M) )
   //
   // m = max( aa,bb,cc,dd )
   //
   // Compute remaining using these 6 equations:
   //
   // a*b = ( C23 - C32 ) / 4;  b*c = ( C12 + C21 ) / 4;
   // a*c = ( C31 - C13 ) / 4;  b*d = ( C13 + C31 ) / 4;
   // a*d = ( C12 - C21 ) / 4;  c*d = ( C23 + C32 ) / 4;
   Quat
   fromMat3RobustSheppard( M3 const & m33 ) const
   {
      T const * const m = glm::value_ptr( m33 );

      Quat q;
      T trace = m[ 0 ] + m[ 4 ] + m[ 8 ]; // M11 + M22 + M33

      T aa = 0.25 * trace + 0.25;
      T bb = 0.25 * (T(1) + (T(2)*m[0]) - trace);
      T cc = 0.25 * (T(1) + (T(2)*m[4]) - trace);
      T dd = 0.25 * (T(1) + (T(2)*m[8]) - trace);

      T maxi = std::max( std::max( aa, bb ), std::max( cc, dd ) );

      T a,b,c,d;

      if ( maxi == aa )
      {
         a = dbSqrt( aa );
         b = ( m[ 5 ] - m[ 7 ] ) / ( T(4)*a );  // b = ( C23 - C32 ) / (4*a);
         c = ( m[ 6 ] - m[ 2 ] ) / ( T(4)*a );  // c = ( C31 - C13 ) / (4*a);
         d = ( m[ 1 ] - m[ 3 ] ) / ( T(4)*a );  // d = ( C12 - C21 ) / (4*a);
      }
      else if ( maxi == bb )
      {
         b = dbSqrt( bb );
         a = ( m[ 5 ] - m[ 7 ] ) / ( T(4)*b );  // a = ( C23 - C32 ) / (4*b);
         c = ( m[ 6 ] - m[ 2 ] ) / ( T(4)*b );  // c = ( C12 + C21 ) / (4*b);
         d = ( m[ 1 ] - m[ 3 ] ) / ( T(4)*b );  // d = ( C13 + C31 ) / (4*b);
      }
      else if ( maxi == cc )
      {
         c = dbSqrt( cc );
         a = ( m[ 6 ] - m[ 2 ] ) / ( T(4)*c );  // a = ( C31 - C13 ) / (4*c);
         b = ( m[ 5 ] - m[ 7 ] ) / ( T(4)*c );  // b = ( C12 + C21 ) / (4*c);
         d = ( m[ 1 ] - m[ 3 ] ) / ( T(4)*c );  // d = ( C23 + C32 ) / (4*c);
      }
      else if ( maxi == dd )
      {
         d = dbSqrt( dd );
         a = ( m[ 1 ] - m[ 3 ] ) / ( T(4)*d );  // a = ( C12 - C21 ) / (4*d);
         b = ( m[ 5 ] - m[ 7 ] ) / ( T(4)*d );  // b = ( C13 + C31 ) / (4*d);
         c = ( m[ 6 ] - m[ 2 ] ) / ( T(4)*d );  // c = ( C23 + C32 ) / (4*d);
      }
      else
      {
         throw std::runtime_error( "Wtf, does max() not work or bitwise equality op?!?!?*xx");
      }

      return Quat( a,b,c,d );
   }
/*
   V3
   operator* ( V3 const & v ) const
   {
      // nVidia SDK implementation
      V3 n(b, c, d);
      V3 uv = glm::cross( n, v );
      V3 uuv = glm::cross( n, uv );
      uv *= T(2) * a;
      uuv *= T(2);
      return v + uv + uuv;
   }
*/

   // Skalarprodukt
   T
   dotP() const { return a*a + b*b + c*c + d*d; }

   T
   dotP( Quat const & other ) const { return a*other.a + b*other.b + c*other.c + d*other.d; }

   // set quaternion to identity
   Quat&
   makeIdentity()
   {
      a = T(1); b = T(0); c = T(0); d = T(0);
      return *this;
   }


/*
   // Eigenwert lamda = 1 for a rotation matrix:
   //
   // [( m11 m12 m13 )            ( 1 0 0 )]   ( x )   ( 0 )
   // [( m21 m22 m23 ) - lambda * ( 0 1 0 )] * ( y ) = ( 0 )
   // [( m31 m32 m33 )            ( 0 0 1 )]   ( z )   ( 0 )
   //
   // But we know that one Eigenwert (lambda) is always 1 for a rotation matrix
   //
   // [( m11 m12 m13 )   ( 1 0 0 )]   ( x )   ( 0 )
   // [( m21 m22 m23 ) - ( 0 1 0 )] * ( y ) = ( 0 )
   // [( m31 m32 m33 )   ( 0 0 1 )]   ( z )   ( 0 )
   //
   // Ax = b = 0
   // ( m11-1 m12   m13   )   ( x )   ( 0 )
   // ( m21   m22-1 m23   ) * ( y ) = ( 0 )
   // ( m31   m32   m33-1 )   ( z )   ( 0 )

   // -> build inverse A'
   // use inverse
   //
   // (AA')x = (A')b
   // (I)x = A'b
   // x = A'b

   Quat
   fromMat3( M3 const & m33 ) const
   {
      // M = ( R t )
      //     ( 0 1 )
      // Angle =

      T const * const m = glm::value_ptr( m33 );

      // Spur( m33 ) = sum(mii) = 1 + 2*cos( ges_angle );
      // Axis = Eigenwert of m33 is the axis vector
      // -> aka. the vector that stays const after a rotation.
      // ( m33 - lambda*I33 )*x = 0
      m[0] = a*a + b*b - c*c - d*d;    // M11
      m[1] = T( 2 ) * ( b*c - a*d);    // M12
      m[2] = T( 2 ) * ( b*d + a*c);    // M13
      m[3] = T( 2 ) * ( b*c + a*d);    // M21
      m[4] = a*a - b*b + c*c - d*d;    // M22
      m[5] = T( 2 ) * ( c*d - a*b);    // M23
      m[6] = T( 2 ) * ( b*d - a*c);    // M31
      m[7] = T( 2 ) * ( c*d + a*b);    // M32
      m[8] = a*a - b*b - c*c + d*d;    // M33
      return m; // benni row-major
   }
*/


};

typedef Quat< float > Quatf;
typedef Quat< double > Quatd;


// =======================================================================
// QuatTest
// =======================================================================
/*
struct QuatTest
{
   DE_CREATE_LOGGER("de.QuatTest")

   static void
   test()
   {
      DE_INFO("==============================================")
      DE_INFO("[1.] TEST ======== ( KIT EXAMPLE 4 ) =========")
      DE_INFO("==============================================")
      {
         DE_INFO("Rotation des Punktes P = (1,0,9)" )
         DE_INFO(" um die Drehachse    n = (1,0,0)" )
         DE_INFO(" mit Winkel        phi = 90°" )
         DE_INFO("Expected Point       W = (1,-9,0) !!!" )
         Quatd::V3 P( 1., 0., 9. );
         Quatd::V3 n( 1., 0., 0. );
         double phi = dbRAD( 90.0 );
         Quatd p = Quatd::fromPoint( P );
         Quatd rot1 = Quatd::fromAngleAxis( phi, n );
         Quatd::V3 w = rot1.rotate( p ).toPoint();
         Quatd::V3 e(1,-9,0);
         DE_INFO("Given n = ", n )
         DE_INFO("Given phi = ", phi )
         DE_INFO("Given rot1 = ", rot1 )
         DE_INFO("Given P = (", p.toPoint(),")" )
         DE_INFO("Rotated Point = (", w, ")" )
         DE_INFO("Expected Point = (", e, ")" )

         if ( !dbEquals( w, e ) )
         {
            throw std::runtime_error("QuaterionTest (1) failed.");
         }
         else
         {
            DE_INFO("OK (0)" )
         }

      }
      DE_INFO("==============================================")
      DE_INFO("[2.] TEST ======== ( KIT EXAMPLE 4.5 ) =======")
      DE_INFO("==============================================")
      {
         AngleAxisd axis( 1.287, 1.0, -2.0, -2.0 );
         DE_INFO("AngleAxis = ", axis )
         Quatd q = Quatd::fromRotation( axis );
         DE_INFO("ExpectedQuat = 0.8 + 0.2i - 0.4j - 0.4k !!!" )
         DE_INFO("ComputedQuat = ", q )
         if ( q != Quatd(0.8,0.2,-0.4,- 0.4) )
         {
            throw std::runtime_error("QuaterionTest (2) failed.");
         }
         else
         {
            DE_INFO("OK (0)" )
         }
      }
      DE_INFO("==============================================")
      DE_INFO("[3.] TEST ========== ( Mul/Add ) =============")
      DE_INFO("==============================================")
      {
         Quatd q1 = Quatd( 0.8, 0.2, -0.4, -0.4 );
         Quatd q2 = q1.conj();
         Quatd q3 = q1.inverse();
         DE_INFO("q1 = ", q1 )
         DE_INFO("q2 = q1.conj() = ", q2 )
         DE_INFO("q3 = q1.inv() = ", q3 )
         DE_INFO("q2*q1 = ", (q2*q1) )
         DE_INFO("q1*q2 = ", (q1*q2) )
         DE_INFO("q3*q1 = ", (q3*q1) )
         DE_INFO("q1*q3 = ", (q1*q3) )
         DE_INFO("OK (0)" )
      }
      DE_INFO("==============================================")
      DE_INFO("[4.] TEST ============ 4x 90° Rotation X =====")
      DE_INFO("==============================================")
      {
         Quatd q0 = Quatd( 0.8, 0.2, -0.4, -0.4 );
         Quatd X90 = Quatd::fromAngleAxis( dbRAD( 90.0 ), 1, 0, 0 );
         DE_INFO("X90° = ", X90 )
         DE_INFO("q0°  = ", q0 )
         Quatd q90 = X90.rotate( q0 );    DE_INFO("q90° = ", q90 )
         Quatd q180 = X90.rotate( q90 );  DE_INFO("q180°= ", q180 )
         Quatd q270 = X90.rotate( q180 ); DE_INFO("q270°= ", q270 )
         Quatd q360 = X90.rotate( q270 ); DE_INFO("q360°= ", q360 )
         if ( q0 != q360 )
         {
            throw std::runtime_error("QuaterionTest (4) failed.");
         }
         else
         {
            DE_INFO("OK (0)" )
         }
      }
      DE_INFO("==============================================")
      DE_INFO("[5.] TEST ====== slerp((1,0,0),(0,1,0),0.5 ) =")
      DE_INFO("==============================================")
      {
         Quatd q1 = Quatd( 0,1,0,0 );
         Quatd q2 = Quatd( 0,0,1,0 );
         Quatd q3 = Quatd::slerp( q1, q2, 0.5 );
         DE_INFO("x = (1,0,0)" )
         DE_INFO("y = (0,1,0)" )
         DE_INFO("slerp( x, y, 0.5 )" )
         DE_INFO("Computed q1 = ", q1 )
         DE_INFO("Computed q2 = ", q2 )
         DE_INFO("Computed q3 = ", q3 )
         DE_INFO("Expected q3 = 0.7071067812i+0.7071067812j " )
         DE_INFO("Expected axis = ( 0.7071067812, 0.7071067812, 0 )" )
         DE_INFO("Computed axis = ( ",q3.toPoint(), " )" )
//         if ( q3 != q360 )
//         {
//            throw std::runtime_error("QuaterionTest (4) failed.");
//         }
         DE_INFO("OK (0)" )
      }

   }
};


// ===========================================================================
enum class LineCap
// ===========================================================================
{
   /// @brief LineCaps for Start- and EndSegment of a polyline by SVG standard
   None = 0,       // SVG lineCap = "butt"
   Round,          // SVG lineCap = "round"
   Square,         // SVG lineCap = "square"
   EnumCount,
   Default = Round,
   Butt = None
};

// ===========================================================================
enum class LineJoin
// ===========================================================================
{
   /// @brief LineJoins between two segments of a polyline by SVG standard
   None = 0,   // No visible line-join
   Bevel,      // Add one or two triangles == AUTO_DOUBLE_BEVEL
   Round,      // For now a circular, not elliptic shape, because lineWidth is const for all segments ( for now )
   Miter,      // Not used, produces too large and ugly triangles. TODO: Miter should reuse already computed intersection point
   EnumCount,	// Indicates invalid state, also counts the enum-items before
   Default = Bevel
};

*/


} // end namespace de.

template < typename T >
std::ostream& operator<< ( std::ostream& o, de::EulerAngles< T > const & euler )
{
   o << euler.toString();
   return o;
}


template < typename T >
std::ostream& operator<< ( std::ostream& o, de::AngleAxis< T > const & euler )
{
   o << euler.toString();
   return o;
}

template < typename T >
std::ostream& operator<< ( std::ostream& o, de::Quat< T > const & quat )
{
   o << quat.toString();
   return o;
}
