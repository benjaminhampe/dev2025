#pragma once
#include <cstdint>
#include <cstdlib>
#include <cstdio>
#include <string>
#include <sstream>
#include <vector>
#include <functional>
#include <de_glm.hpp>
//#include <tinyxml2/tinyxml2.h>

#include <de_core/de_Math.h>
//#include <de_core/de_Logger.h>

namespace de {

template < typename T >
// =======================================================================
struct Math3D
// =======================================================================
{
   constexpr static const T DEG2RAD = T( 180.0 / M_PI );
   constexpr static const T RAD2DEG = T( M_PI / 180.0 );
   constexpr static const T TWO_PI = T( M_PI * 2.0 );
   constexpr static const T HALF_PI = T( M_PI / 2.0 );
};


template < typename T >
// =======================================================================
struct TRS
// =======================================================================
{
   // typedef double T;
   typedef glm::tmat4x4< T > M4;
   typedef glm::tvec2< T > V2;
   typedef glm::tvec3< T > V3;
   typedef glm::tvec4< T > V4;

   DE_CREATE_LOGGER("de.gpu.TRS")
   V3 m_T; // Relative translation of the scene node.
   V3 m_R; // Relative rotation (euler angles) of the scene node.
   V3 m_S; // Relative scale of the scene node.

   M4 m_TRS;

   TRS() : m_T(0,0,0), m_R(0,0,0), m_S(1,1,1), m_TRS( T(1) ) // All identity
   {}

   void reset() { m_T = V3(0,0,0); m_R = V3(0,0,0); m_S = V3(1,1,1); m_TRS = M4( T(1) ); }


   M4 const & getModelMatrix() const { return m_TRS; }
   M4 const & getTRS() const { return m_TRS; }

   V3 const & getT() const { return m_T; }
   T const & getTX() const { return m_T.x; }
   T const & getTY() const { return m_T.y; }
   T const & getTZ() const { return m_T.z; }

   V3 const & getR() const { return m_R; }
   T const & getRX() const { return m_R.x; }
   T const & getRY() const { return m_R.y; }
   T const & getRZ() const { return m_R.z; }

   V3 const & getS() const { return m_S; }
   T const & getSX() const { return m_S.x; }
   T const & getSY() const { return m_S.y; }
   T const & getSZ() const { return m_S.z; }

   void setT( V3 const & v ) { m_T = v; updateTRS(); }
   void setT( T x, T y, T z ) { setT( V3( x,y,z ) ); }
   void setTX( T x ) { m_T.x = x; updateTRS(); }
   void setTY( T y ) { m_T.y = y; updateTRS(); }
   void setTZ( T z ) { m_T.z = z; updateTRS(); }

   void setR( V3 const & v ) { m_R = v; updateTRS(); }
   void setR( T x, T y, T z ) { setR( V3( x,y,z ) ); }
   void setRX( T x ) { m_R.x = x; updateTRS(); }
   void setRY( T y ) { m_R.y = y; updateTRS(); }
   void setRZ( T z ) { m_R.z = z; updateTRS(); }

   void setS( V3 const & v ) { m_S = v; updateTRS(); }
   void setS( T x, T y, T z ) { setS( V3( x,y,z ) ); }
   void setSX( T x ) { m_S.x = x; updateTRS(); }
   void setSY( T y ) { m_S.y = y; updateTRS(); }
   void setSZ( T z ) { m_S.z = z; updateTRS(); }

   void addT( T x, T y, T z ) { m_T += V3( x,y,z ); updateTRS(); }
   void addR( T x, T y, T z ) { m_R += V3( x,y,z ); updateTRS(); }
   void addS( T x, T y, T z ) { m_S += V3( x,y,z ); updateTRS(); }

   void updateTRS()
   {
      // if (m_S != V3(1,1,1) )
      // {

      // }
      // if (m_R != V3(0,0,0) )
      // {
      M4 t = glm::translate( M4( T(1) ), m_T );
      M4 rx = glm::rotate( M4( T(1) ), glm::radians( m_R.x ), { 1., 0., 0. } );
      M4 ry = glm::rotate( M4( T(1) ), glm::radians( m_R.y ), { 0., 1., 0. } );
      M4 rz = glm::rotate( M4( T(1) ), glm::radians( m_R.z ), { 0., 0., 1. } );
      M4 r = rz * ry * rx;
      M4 s = glm::scale( M4(1), m_S );
      m_TRS = t * (r * s);

      //decomposeT( m_TRS );
   }

   //Box3f
   //getAbsoluteBoundingBox() const { return getBoundingBox().transformBox( m_AbsoluteTransform ); }

   // Gets the absolute position of the node in world coordinates.
   static V3
   decomposeT( M4 const & trs )
   {
      T const * const m = glm::value_ptr( trs );
      T x = m[ 12 ];
      T y = m[ 13 ];
      T z = m[ 14 ];
      //DE_DEBUG("AbsolutePosition = (",x,",",y,",",z,"), m_Translation")
      return { x, y, z };
   }

};

typedef TRS< float > TRSf;
typedef TRS< double > TRSd;


//=============================================================================
/// @brief Direction
//=============================================================================
template < typename T >
struct Direction3 // Another name for 'direction' that can be moved parallel without change.
{
   typedef glm::tmat4x4< T > M4;
   typedef glm::tvec2< T > V2;
   typedef glm::tvec3< T > V3;
   typedef glm::tvec4< T > V4;

   V3 m_Dir;

   Direction3() : m_Dir() {}
   Direction3( V3 const & dir ) : m_Dir( dir ) {}

   V3 const &
   getDir() const { return m_Dir; }

   V3
   toEulerAngles() const { return toEulerAngles( m_Dir ); }

   static Direction3
   fromEulerAngles( V3 const & euler )
   {
      return Direction3( Vector3_from_EulerAngles( euler ) );
   }

   static Direction3
   fromEulerAngles( V3 const & euler, V3 const & axis )
   {
      return Direction3( Vector3_from_EulerAngles( euler, axis ) );
   }

   ///@brief DirectionVector to EulerAngles
   ///@return A rotation vector containing the Y (pitch) and Z (yaw) rotations (in rad) that when applied to a
   /// direction vector(0,0,1) would make it point in the same direction as this vector.
   /// The X (roll) rotation is always 0, since two Euler rotations are sufficient to any given direction.
   static V3
   toEulerAngles( V3 dir )
   {
      dir = glm::normalize( dir );
      V3 euler( 0,0,0 );
      euler.y = dbAtan2( dir.x, dir.z );
      T const TWOPI = T( M_PI * 2.0 );
      T const HALF_PI64 = T( M_PI * 0.5 );
      //T const DEG2RAD = T( M_PI / 180.0 );
      //T const RAD2DEG = T( 180.0 / M_PI );
      if ( euler.y < T( 0 ) ) euler.y += TWOPI;
      if ( euler.y >= TWOPI ) euler.y -= TWOPI;
      euler.x = dbAtan2( dbSqrt( dir.x*dir.x + dir.z*dir.z ), dir.y ) - T( HALF_PI64 );
      if ( euler.x < T( 0 ) ) euler.x += TWOPI;
      if ( euler.x >= TWOPI ) euler.x -= TWOPI;
      return dbDEG( euler );
   }

   // EulerAngles(rad) -> DirectionVector on unit sphere
   static V3
   Vector3_from_EulerAngles( V3 euler )
   {
      T const cr = dbCos( euler.x );
      T const sr = dbSin( euler.x );
      T const cp = dbCos( euler.y );
      T const sp = dbSin( euler.y );
      T const cy = dbCos( euler.z );
      T const sy = dbSin( euler.z );
      T const crsp = cr*sp;
      return { crsp*cy+sr*sy, crsp*sy-sr*cy, cr*cp };
   }

   // EulerAngles(rad) -> DirectionVector on unit sphere
   static V3
   Vector3_from_EulerAngles( V3 euler, V3 dir /* = V3( 0,0,1 ) */ )
   {
      T const cr = dbCos( euler.x );
      T const sr = dbSin( euler.x );
      T const cp = dbCos( euler.y );
      T const sp = dbSin( euler.y );
      T const cy = dbCos( euler.z );
      T const sy = dbSin( euler.z );
      T const srsp = sr*sp;
      T const crsp = cr*sp;
      T const C0 = cp*cy;
      T const C1 = cp*sy;
      T const C2 = -sp;
      T const C3 = srsp*cy-cr*sy;
      T const C4 = srsp*sy+cr*cy;
      T const C5 = sr*cp;
      T const C6 = crsp*cy+sr*sy;
      T const C7 = crsp*sy-sr*cy;
      T const C8 = cr*cp;
      return {dir.x * C0 + dir.y * C3 + dir.z * C6,
              dir.x * C1 + dir.y * C4 + dir.z * C7,
              dir.x * C2 + dir.y * C5 + dir.z * C8 };
   }

};

typedef Direction3< float > Direction3f;
typedef Direction3< double > Direction3d;

//=============================================================================
template < typename T, glm::precision P = glm::defaultp >
struct Ray3
//=============================================================================
{
   constexpr static T const NoInit = std::numeric_limits<T>::infinity();
   typedef glm::tmat4x4< T, P > M4;
   typedef glm::tvec3< T, P > V3;
   typedef glm::tvec4< T, P > V4;

   V3 m_Pos;
   V3 m_Dir;

   Ray3() : m_Pos( 0, 0, 0 ), m_Dir( 0, 0, 1 ) {}
   Ray3( V3 const & pos, V3 const & dir ) : m_Pos( pos ), m_Dir( dir ) {}
   void setPos( V3 const & pos ) { m_Pos = pos; }
   void setDir( V3 const & dir ) { m_Dir = dir; }
   V3 const & getPos() const { return m_Pos; }
   V3 const & getDir() const { return m_Dir; }


   template < typename B >
   Ray3< B >
   convert() const
   {
      return Ray3< B >( m_Pos, m_Dir );
   }

   Ray3 &
   transform( glm::dmat4 const & modelMatrix )
   {
      m_Pos = glm::dvec3( modelMatrix * glm::dvec4( m_Pos, 1.0 ) );
      m_Dir = glm::dvec3( modelMatrix * glm::dvec4( m_Dir, 1.0 ) );
      return *this;
   }

   std::string
   toString() const
   {
      std::stringstream s; s << "pos:" << m_Pos << ", dir:" << m_Dir;
      return s.str();
   }

   bool
   isValid() const
   {
      if ( std::isinf( m_Pos.x ) || std::isnan( m_Pos.x ) ) return false;
      if ( std::isinf( m_Pos.y ) || std::isnan( m_Pos.y ) ) return false;
      if ( std::isinf( m_Pos.z ) || std::isnan( m_Pos.z ) ) return false;
      if ( std::isinf( m_Dir.x ) || std::isnan( m_Dir.x ) ) return false;
      if ( std::isinf( m_Dir.y ) || std::isnan( m_Dir.y ) ) return false;
      if ( std::isinf( m_Dir.z ) || std::isnan( m_Dir.z ) ) return false;
      return true;
   }

   void reset()
   {
      // We always have bird/top view from north direction as default view.
      m_Pos = V3( 0, 0, 0 ); // origin, earth-plane is image-plane (x,y), z is height above x,y ground.
      m_Dir = V3( 0, 0, -1 ); // -z, looks from heaven (+z) to ground (0 and below)
   }

   struct HitResult
   {
      std::vector< V3 > hitPositions;
      bool empty() const { return hitPositions.empty(); }
   };


   /// @author Benjamin Hampe <benjaminhampe@gmx.de>
   /// @brief Picking something at mousePos(mx,my) on screen into 3D World space
   // Most efficient:
   // ViewProjectionMatrix is already given as inverse
   /// @param mx The mouse pos x on screen
   /// @param my The mouse pos y on screen
   /// @param w The screen width ( of this camera )
   /// @param h The screen height ( of this camera )
   /// @param Inverse camera ViewProjectionMatrix (VP_inv).
   static Ray3
   compute3DWorldRayFrom2DScreen( int mx, int my, int w, int h, M4 const & viewProjMatInv )
   {
      // MousePos to NDC
      T x = T( mx ) / T( w ) * 2.0 - 1.0;
      T y = T( h - 1 - my ) / T( h ) * 2.0 - 1.0; // flips y for OpenGL ES
      // Make a ray that goes through entire frustum from z=near to far.
      V4 ndcA( x, y, -1.0, 1.0 ); // z=-1 means ray starts at near plane
      V4 ndcB( x, y,  1.0, 1.0 ); // z=1 means ray ends at far plane
      // NDC to World, perspective correction done by (div by w)
      V4 tmpA = viewProjMatInv * ndcA;
      V4 tmpB = viewProjMatInv * ndcB;
      // After the perspective division xyz/w, worldspace pos is correct.
      V3 posA = V3( tmpA ) / tmpA.w;
      V3 posB = V3( tmpB ) / tmpB.w;
      return Ray3( posA, glm::normalize( posB - posA ) );
   }

};

typedef Ray3< float > Ray3f;
typedef Ray3< double > Ray3d;


//=============================================================================
template < typename T, glm::precision P = glm::defaultp >
struct Plane3
//=============================================================================
{
   typedef glm::tvec3< T, P > V3;
   typedef glm::tvec4< T, P > V4;
   typedef glm::tmat4x4< T, P > M4;

   V3 n; // x=nx,y=ny,z=nz,w=distance to origin
   T dist;

   Plane3( ) : n( 0,0,1 ), dist( 0 ) {}
   Plane3( T x, T y, T z, T distToOrigin = T( 0.0 ) ) : n( x,y,z ), dist( distToOrigin ) {}
   Plane3( V3 const & nrm = V3(0,0,1), T distToOrigin = T( 0.0 ) ) : n( nrm), dist( distToOrigin ) {}

   V3 getNormal() const { return n; }

   // double p =  glm::dot( rayDir, n );
   // Ebene: (x-A)*nABC = 0
   // Ebene: A = 0 + dist * n;
   // Ray: rayStart + r * rayDir
   // R in E:

   //(dist * n - rayStart - r * rayDir)o n = 0;
   //((dist * n) o n - rayStart o n - r * rayDir o n = 0;
   //((dist * n) o n - rayStart o n = r * rayDir o n;
   // r = (dist * (n,n) - rayStart*n) / (rayDir*n);
   bool
   intersectRay( V3 rayStart, V3 rayDir, V3 & hitPosition, T* out_r = nullptr ) const
   {
      T r = ( (glm::dot(n,n) * dist) - glm::dot( rayStart, n ) ) / glm::dot( rayDir, n );
      if ( std::isinf( r ) ) { return false; }
      if ( std::isnan( r ) ) { return false; }
      if ( out_r ) { *out_r = r; }
      hitPosition = rayStart + rayDir * r;
      return true;
   }

   std::string
   toString() const
   {
      std::stringstream s;
      s << n << "," << dist;
      return s.str();
   }
};

typedef Plane3< float > Plane3f;
typedef Plane3< double > Plane3d;


//=============================================================================
/// @brief Triangle3
//=============================================================================
template < typename T, glm::precision P = glm::defaultp >
struct Triangle3
{
   typedef glm::tvec3< T,P > V3;
   typedef glm::tvec4< T,P > V4;
   typedef glm::tmat4x4< T,P > M4;
   V3 A,B,C;

   Triangle3() : A( 0, 0, 0 ), B( 0, 1, 0 ), C( 1, 1, 0 )
   {}
   Triangle3( V3 a, V3 b, V3 c ) : A( a ), B( b ), C( c )
   {}
   std::string
   toString() const
   {
      std::stringstream s;
      s << "A(" << A << "), B(" << B << "), C(" << C << ")";
      return s.str();
   }

   // nABC = (AB x AC) / |AB x AC|.
   V3
   computeNormal() const
   {
      return glm::normalize( glm::cross( B - A, C - B ) );
   }

   T
   computeDistToOrigin() const
   {
      T d = std::min( std::min( glm::length2( A ), glm::length2( B ) ), glm::length2( C ) );
      return dbSqrt( d );
   }

   Plane3< T >
   computePlane() const
   {
      V3 n = computeNormal();
      T d = computeDistToOrigin();
      return Plane3< T >( n, d );
   }

   Triangle3 &
   transform( M4 const & m )
   {
      A = V3( m * V4( A, T(1) ) );
      B = V3( m * V4( B, T(1) ) );
      C = V3( m * V4( C, T(1) ) );
      return *this;
   }

   // Benni - Mueller - Trumbore algorithm
   bool
   intersectRay( V3 rayStart, V3 rayDir, V3 & hitPosition ) const
   {
      T const EPSILON = T( 0.0000001 );
      V3 AB = B - A;
      V3 AC = C - A;
      // alias R = rayStart, D = rayDir
      V3 nDAC = glm::cross( rayDir, AC );
      // 'a' is projection of 'AB' on 'nDAC'
      T a = glm::dot( AB, nDAC );
      // if 'a' == 0, then ray is parallel to this triangle.
      if ( std::abs( a ) < EPSILON )
      {
//         DE_TRACE("ray is parallel, a = ", a, ", "
//                  "rayStart(", rayStart, "), rayDir(", rayDir, "), "
//                  "A =", A, ", B =", B,", C =", C )
         return false;
      }

      T f = T( 1.0 ) / a;

      if ( std::isinf( f ) )
      {
//         DE_DEBUG("f is inf, a = ", a, ", "
//                  "rayStart(", rayStart, "), rayDir(", rayDir, "), "
//                  "A =", A, ", B =", B,", C =", C )
         return false;
      }

      if ( std::isnan( f ) )
      {
//         DE_DEBUG("f is nan, a = ", a, ", "
//                  "rayStart(", rayStart, "), rayDir(", rayDir, "), "
//                  "A =", A, ", B =", B,", C =", C )
         return false;
      }

      V3 AR = rayStart - A;
      T u = f * glm::dot( AR, nDAC );
      if ( u < 0.0 || u > 1.0 )
      {
//         DE_DEBUG("u = ", u, ", a = ", a, ", "
//                  "rayStart(", rayStart, "), rayDir(", rayDir, "), "
//                  "A =", A, ", B =", B,", C =", C )
         return false;
      }

      V3 nARxAB = glm::cross( AR, AB );
      T v = f * glm::dot( rayDir, nARxAB );
      if ( v < 0.0 || u + v > 1.0 )
      {
//         DE_DEBUG("u = ", u, ", v = ", v, ", a = ", a, ", "
//                  "rayStart(", rayStart, "), rayDir(", rayDir, "), "
//                  "A =", A, ", B =", B,", C =", C )
         return false;
      }

      // At this stage we can compute t to find out where the intersection point is on the line.
      T t = f * glm::dot( AC, nARxAB );
      if ( t > EPSILON ) // ray intersection
      {
         hitPosition = rayStart + rayDir * t;
//         DE_DEBUG("Got intersection, hitPos = ", hitPosition, ", "
//                  "rayStart(", rayStart, "), rayDir(", rayDir, "), "
//                  "A =", A, ", B =", B,", C =", C )
         return true;
      }
      else
      {
         return false;
      }
   }
};

typedef Triangle3< float > Triangle3f;
typedef Triangle3< double > Triangle3d;


//=============================================================================
/// @brief BoundingBox3
//=============================================================================
//
//        F-------G (Max)           tex atlas infos         +----+----+---------+
//       /|      /|                                         |    |    |         |
//      / |  +Y / |                                         |    | +Z |         |
//     /  |    /  |                                         |    |    |         |
//    /   E---/---H   lookat = -X for plane PX              +----+----+----+----+
//   /-X /   / X /    <-------P  EyePos is +X for plane PX  |    |    |    |    |
//  /   /   / + /                                           | -X | -Y | +X | +Y |
// B-------C   /     +z                                     |    |    |    |    |
// |  /    |  /      |  +y                                  +----+----+----+----+
// | / -Y  | /       | /                                    |    |    |         |
// |/      |/        |/                                     |    | -Z |         |
// A-------D         0------> +x                            |    |    |         |
// Min:
template < typename T, glm::precision P = glm::defaultp >
struct Box3
{
   typedef glm::tvec3< T,P > V3;
   typedef glm::tvec4< T,P > V4;
   typedef glm::tmat4x4< T,P > M4;

   // Same as ECubeFace
   enum EPlane { NEG_X = 0, NEG_Y, NEG_Z, POS_X, POS_Y, POS_Z, EPlaneCount };

   V3 m_Min;
   V3 m_Max;

   Box3()
      : m_Min( T(0), T(0), T(0) )
      , m_Max( T(0), T(0), T(0) )
   {}

   Box3( T minX, T minY, T minZ, T maxX, T maxY, T maxZ )
      : m_Min( minX, minY, minZ )
      , m_Max( maxX, maxY, maxZ )
   {}

   Box3( V3 const & minEdge, V3 const & maxEdge )
      : m_Min( minEdge )
      , m_Max( maxEdge )
   {}

   V3 getA() const { return { m_Min.x, m_Min.y, m_Min.z }; }
   V3 getB() const { return { m_Min.x, m_Max.y, m_Min.z }; }
   V3 getC() const { return { m_Max.x, m_Max.y, m_Min.z }; }
   V3 getD() const { return { m_Max.x, m_Min.y, m_Min.z }; }
   V3 getE() const { return { m_Min.x, m_Min.y, m_Max.z }; }
   V3 getF() const { return { m_Min.x, m_Max.y, m_Max.z }; }
   V3 getG() const { return { m_Max.x, m_Max.y, m_Max.z }; }
   V3 getH() const { return { m_Max.x, m_Min.y, m_Max.z }; }


   Triangle3< T >
   getTriangle( int face, bool b ) const
   {
      if ( b )
      {
         switch( face )
         {
            case NEG_Z: return { getC(), getD(), getA() }; // CDA back 2
            case POS_Z: return { getF(), getE(), getH() }; // FEH front
            case NEG_X: return { getB(), getE(), getF() }; // EFBA left
            case POS_X: return { getG(), getH(), getD() }; // DCGH right
            case NEG_Y: return { getE(), getA(), getD() }; // DHEA bottom
            case POS_Y: return { getG(), getC(), getB() }; // BFGC top
            default: return { V3(0,0,0), V3(0,0,0), V3(0,0,0) };
         }
      }
      else
      {
         switch( face )
         {
            case NEG_Z: return { getA(), getB(), getC() }; // ABC back 1
            case POS_Z: return { getH(), getG(), getF() }; // HGF front 1
            case NEG_X: return { getE(), getF(), getB() }; // EFBA left
            case POS_X: return { getD(), getC(), getG() }; // DCGH right
            case NEG_Y: return { getD(), getH(), getE() }; // DHEA bottom
            case POS_Y: return { getB(), getF(), getG() }; // BFGC top
            default: return { V3(0,0,0), V3(0,0,0), V3(0,0,0) };
         }
      }
   }

   Triangle3< T >
   getTriangle( int triangle_0_to_11 ) const
   {
      switch( triangle_0_to_11 )
      {
         case 0: return { getB(), getE(), getF() }; // NEG_X - EFBA left
         case 1: return { getE(), getF(), getB() }; // NEG_X - EFBA left
         case 2: return { getE(), getA(), getD() }; // NEG_Y - DHEA bottom
         case 3: return { getD(), getH(), getE() }; // NEG_Y - DHEA bottom
         case 4: return { getC(), getD(), getA() }; // NEG_Z - CDA back 2
         case 5: return { getA(), getB(), getC() }; // NEG_Z - ABC back 1
         case 6: return { getG(), getH(), getD() }; // POS_X - DCGH right
         case 7: return { getD(), getC(), getG() }; // POS_X - DCGH right
         case 8: return { getG(), getC(), getB() }; // POS_Y - BFGC top
         case 9: return { getB(), getF(), getG() }; // POS_Y - BFGC top
         case 10: return { getF(), getE(), getH() }; // POS_Z - FEH front
         case 11: return { getH(), getG(), getF() }; // POS_Z - HGF front 1
         default: return { V3(-10000,-10000,-10000), V3(-20000,-10000,-10000), V3(-10000,-20000,-10000) };
      }
   }

   std::string
   toString( bool withCorners = false ) const
   {
      std::stringstream s;
      s << "size(" << getSize() << "), ";
      s << "min(" << m_Min << "), ";
      s << "max(" << m_Max << ")";
      return s.str();
   }

   V3 const & getMin() const { return m_Min; }
   V3 const & getMax() const { return m_Max; }
   V3 & getMin() { return m_Min; }
   V3 & getMax() { return m_Max; }
   void setMin( V3 const & min ) { m_Min = min; }
   void setMin( T const & x, T const & y, T const & z ) { m_Min = V3(x,y,z); }
   void setMax( V3 const & max ) { m_Max = max; }
   void setMax( T const & x, T const & y, T const & z ) { m_Max = V3(x,y,z); }
   V3 getSize() const
   {
      //if ( !isValid() ) return V3();
      return m_Max - m_Min;
   }
   bool isValid() const
   {
      if ( std::isinf( m_Min.x ) || std::isnan( m_Min.x ) ) return false;
      if ( std::isinf( m_Min.y ) || std::isnan( m_Min.y ) ) return false;
      if ( std::isinf( m_Min.z ) || std::isnan( m_Min.z ) ) return false;
      if ( std::isinf( m_Max.x ) || std::isnan( m_Max.x ) ) return false;
      if ( std::isinf( m_Max.y ) || std::isnan( m_Max.y ) ) return false;
      if ( std::isinf( m_Max.z ) || std::isnan( m_Max.z ) ) return false;
      return true;
   }
   V3 getCenter() const
   {
      if ( !isValid() ) return V3();
      return m_Min + ( getSize() * 0.5f );
   }
   Box3 operator+( V3 const & value ) const
   {
      Box3 box( *this );
      box.m_Min += value;
      box.m_Max += value;
      return box;
   }
   Box3 operator-( V3 const & value ) const
   {
      Box3 box( *this );
      box.m_Min -= value;
      box.m_Max -= value;
      return box;
   }
   void reset() { m_Min = m_Max = V3( 0, 0, 0 ); }
   void reset( V3 const & pos ) { m_Min = m_Max = pos; }
   void reset( Box3 const & box ) { m_Min = box.m_Min; m_Max = box.m_Max; }
   void reset( T const & x, T const & y, T const & z ) { m_Min = m_Max = V3( x, y, z ); }

   void addInternalPoint( V3 const & p )
   {
      m_Min.x = std::min( m_Min.x, p.x );
      m_Min.y = std::min( m_Min.y, p.y );
      m_Min.z = std::min( m_Min.z, p.z );
      m_Max.x = std::max( m_Max.x, p.x );
      m_Max.y = std::max( m_Max.y, p.y );
      m_Max.z = std::max( m_Max.z, p.z );
   }

   void
   addInternalPoint( T x, T y, T z )
   {
      addInternalPoint( {x, y, z} );
   }

   void
   addInternalBox( Box3 const & o )
   {
      addInternalPoint( o.getMin() );
      addInternalPoint( o.getMax() );
   }

   V3
   getCorner( int corner ) const
   {
      switch ( corner )
      {
         case 0: return { m_Min.x, m_Min.y, m_Min.z };
         case 1: return { m_Min.x, m_Max.y, m_Min.z };
         case 2: return { m_Max.x, m_Max.y, m_Min.z };
         case 3: return { m_Max.x, m_Min.y, m_Min.z };
         case 4: return { m_Min.x, m_Min.y, m_Max.z };
         case 5: return { m_Min.x, m_Max.y, m_Max.z };
         case 6: return { m_Max.x, m_Max.y, m_Max.z };
         case 7: return { m_Max.x, m_Min.y, m_Max.z };
         default: break;
      }
   }

   Box3
   transformBox( M4 const & modelMatrix ) const
   {
      V3 const A = V3{ modelMatrix * V4( getA(), T(1.0) ) };
      V3 const B = V3{ modelMatrix * V4( getB(), T(1.0) ) };
      V3 const C = V3{ modelMatrix * V4( getC(), T(1.0) ) };
      V3 const D = V3{ modelMatrix * V4( getD(), T(1.0) ) };
      V3 const E = V3{ modelMatrix * V4( getE(), T(1.0) ) };
      V3 const F = V3{ modelMatrix * V4( getF(), T(1.0) ) };
      V3 const G = V3{ modelMatrix * V4( getG(), T(1.0) ) };
      V3 const H = V3{ modelMatrix * V4( getH(), T(1.0) ) };

      // No perspective divide by w wanted here,
      // so pls never use this very function with a projection matrix.
      // If you need to transform a unit-cube in ndc/clipSpace to a
      // camera world frustum = invViewProj * unitcube;
      // unitcube = camera frustum in ndc space.
      // So any frustum ( sheared box = Spat ) will always become
      // a unit-cube in normalized device coords in range [-1,-1,-1] to [1,1,1].
      // Thats what a projectionMatrix does - transform a camera frustum to ndc unit-cube.

      Box3 bbox( A,A);
      //bbox.addInternalPoint( A );
      bbox.addInternalPoint( B );
      bbox.addInternalPoint( C );
      bbox.addInternalPoint( D );
      bbox.addInternalPoint( E );
      bbox.addInternalPoint( F );
      bbox.addInternalPoint( G );
      bbox.addInternalPoint( H );
      return bbox;
   }

   Box3
   scale( T skalar ) const { return Box3( m_Min * skalar, m_Max * skalar ); }

   Box3
   widen( T skalar ) const
   {
      T const t = T(0.5) * skalar;
      return Box3( m_Min - t, m_Max + t );
   }

   Box3
   widenZ( float skalar ) const
   {
      T const t = T(0.5) * skalar;
      V3 m( m_Min ); m.z -= t;
      V3 n( m_Max ); n.z += t;
      return Box3( m, n );
   }


   int
   intersectRay( V3 const & rayStart, V3 const & rayDir, V3 & hitPosition ) const
   {
      T dx = T(0.5) * getSize().x;
      T dy = T(0.5) * getSize().y;
      T dz = T(0.5) * getSize().z;

      bool nx = Plane3< T >( 1,0,0,-dx ).intersectRay( rayStart, rayDir, hitPosition );
      bool ny = Plane3< T >( 0,1,0,-dy ).intersectRay( rayStart, rayDir, hitPosition );
      bool nz = Plane3< T >( 0,0,1,-dz ).intersectRay( rayStart, rayDir, hitPosition );
      bool px = Plane3< T >( -1,0,0,dx ).intersectRay( rayStart, rayDir, hitPosition );
      bool py = Plane3< T >( 0,-1,0,dy ).intersectRay( rayStart, rayDir, hitPosition );
      bool pz = Plane3< T >( 0,0,-1,dz ).intersectRay( rayStart, rayDir, hitPosition );

      int n = 0;
      if ( nx ) n++;
      if ( ny ) n++;
      if ( nz ) n++;
      if ( px ) n++;
      if ( py ) n++;
      if ( pz ) n++;

      //DE_DEBUG( "BoundingBox.IntersectCount = ",n,")" )
      return n;
   }

   T
   minDistanceTo( V3 const & pos ) const
   {
      T d = glm::length2( pos - getA() );
      d = std::min( d, glm::length2( pos - getB() ) );
      d = std::min( d, glm::length2( pos - getC() ) );
      d = std::min( d, glm::length2( pos - getD() ) );
      d = std::min( d, glm::length2( pos - getE() ) );
      d = std::min( d, glm::length2( pos - getF() ) );
      d = std::min( d, glm::length2( pos - getG() ) );
      d = std::min( d, glm::length2( pos - getH() ) );
      return dbSqrt( d );
   }

   T
   maxDistanceTo( V3 const & pos ) const
   {
      T d = glm::length2( pos - getA() );
      d = std::max( d, glm::length2( pos - getB() ) );
      d = std::max( d, glm::length2( pos - getC() ) );
      d = std::max( d, glm::length2( pos - getD() ) );
      d = std::max( d, glm::length2( pos - getE() ) );
      d = std::max( d, glm::length2( pos - getF() ) );
      d = std::max( d, glm::length2( pos - getG() ) );
      d = std::max( d, glm::length2( pos - getH() ) );
      return dbSqrt( d );
   }

   // getTriangle( )
   // back:  B,A,D,C
   // front: G,H,E,F
   // left:  F,E,A,B
   // front: G,H,E,F
   // right: C,D,H,G
   // bottom:H,D,A,E
   // top:   B,C,G,F

   constexpr uint32_t getColorA() const { return 0xFF0000FF; }
   constexpr uint32_t getColorB() const { return 0xFFFFFFFF; }
   constexpr uint32_t getColorC() const { return 0xFFFFFFFF; }
   constexpr uint32_t getColorD() const { return 0xFFFFFFFF; }
   constexpr uint32_t getColorE() const { return 0xFF00FFFF; }
   constexpr uint32_t getColorF() const { return 0xFFFFFFFF; }
   constexpr uint32_t getColorG() const { return 0xFFFFFFFF; }
   constexpr uint32_t getColorH() const { return 0xFFFFFFFF; }

};

typedef Box3< float > Box3f;
typedef Box3< double > Box3d;

} // end namespace de

template < typename T >
std::ostream& operator<< ( std::ostream& o, de::Box3< T > const & bbox )
{
   o << bbox.toString();
   return o;
}

