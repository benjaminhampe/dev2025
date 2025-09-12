#pragma once
#include <DarkImage.hpp>
#include <cstdint>
#include <cstdlib>
#include <cstdio>
#include <string>
#include <sstream>
#include <vector>
#include <functional>
#include <de_glm.hpp>
#include <tinyxml2/tinyxml2.h>

#include <de_opengles32.hpp>
#include <de/GL_Validate.hpp>

#ifndef SAFE_DELETE
#define SAFE_DELETE(x) if ( (x) ) { delete (x); (x) = nullptr; }
#endif

namespace de {

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
      m_Pos = Math::transformVector( modelMatrix, m_Pos );
      m_Dir = Math::transformVector( modelMatrix, m_Dir );
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
// EulerAngles
// =======================================================================
template < typename T >
struct EulerAngles
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
   toDegrees() const { return V3{ yaw, pitch, roll } * T(Math::RAD2DEG64); }

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
      T const Pi2 = T(Math::TWOPI64);
      T const PiH = T(Math::HALF_PI64);

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
