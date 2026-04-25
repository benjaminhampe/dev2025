#pragma once
#include <de/Math.h>
// #include <cstdint>
// #include <cstdlib>
// #include <cstdio>
// #include <string>
// #include <sstream>
// #include <vector>
// #include <functional>
// #include <tinyxml2/tinyxml2.h>
// #include <de_core/de_Math.h>
// #include <de_core/de_Logger.h>

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

/*
// =====================================================================
struct H3_TRS
// =====================================================================
{
    glm::vec3 pos = glm::vec3(0,0,0);
    glm::vec3 rotate = glm::vec3(0,0,0);
    glm::vec3 scale = glm::vec3(1,1,1);
    glm::mat4 trs;

    void update()
    {
        // Translation
        const auto t = glm::translate(glm::mat4(1.0f), pos);
        // Create individual rotation matrices
        const auto rx = glm::rotate(glm::mat4(1.0f), rotate.x, glm::vec3(1.0f, 0.0f, 0.0f));
        const auto ry = glm::rotate(glm::mat4(1.0f), rotate.y, glm::vec3(0.0f, 1.0f, 0.0f));
        const auto rz = glm::rotate(glm::mat4(1.0f), rotate.z, glm::vec3(0.0f, 0.0f, 1.0f));
        // Combine rotations (order matters: XYZ or YXZ depending on your use case)
        const auto r = rz * (ry * rx);
        // Scale
        const auto s = glm::scale(glm::mat4(1.0f), scale);
        // Combined matrix
        trs = t * (r * s);
    }

    std::string toString() const
    {
        std::ostringstream o; o <<
            "pos(" << pos.x << ","<< pos.y << ","<< pos.z << "), "
            "rotate(" << rotate.x << ","<< rotate.y << ","<< rotate.z << "), "
            "scale(" << scale.x << ","<< scale.y << ","<< scale.z << ")";
        return o.str();
    }
};
*/

template < typename T >
// =======================================================================
struct TRS
// =======================================================================
{
    typedef glm::tquat< T > Q4;
    typedef glm::tmat3x3< T > M3;
    typedef glm::tmat4x4< T > M4;
    typedef glm::tvec2< T > V2;
    typedef glm::tvec3< T > V3;
    typedef glm::tvec4< T > V4;

    V3 pos; // 3D Position/Translation.
    V3 rotate; // 3D Rotation as EulerAngles.
    V3 scale; // 3D Scale.
    M4 modelMat; // 4D Matrix used for rendering (no Skew)
    //bool bNeedUpdate;

    TRS()
        : pos(0,0,0), rotate(0,0,0), scale(1,1,1)
        , modelMat( T(1) ) //, bNeedUpdate(false)
    {}

    TRS( V3 pos_, V3 eulerAngles_, V3 scale_)
        : pos(pos_), rotate(eulerAngles_), scale(scale_)
        , modelMat( T(1) ) //, bNeedUpdate(true)
    {
        update();
    }

    std::string str() const
    {
        std::ostringstream o;
        o << "T(" << pos << "), R(" << rotate << "), S(" << scale << ")";
        return o.str();
    }

    void reset()
    {
        pos = V3(0,0,0);
        rotate = V3(0,0,0);
        scale = V3(1,1,1);
        modelMat = M4( T(1) );
        //bNeedUpdate = false;
    }

    // Builds modelMat from TRS = Translation * Rotation * Scale, (T*(R*S))
    void update()
    {
        //if (!m_needUpdate) { return; } // Nothing todo
        // Translation
        const M4 t = glm::translate( M4( T(1) ), pos );
        // Create individual rotation matrices
        const M4 rx = glm::rotate( M4( T(1) ), glm::radians( rotate.x ), { 1., 0., 0. } );
        const M4 ry = glm::rotate( M4( T(1) ), glm::radians( rotate.y ), { 0., 1., 0. } );
        const M4 rz = glm::rotate( M4( T(1) ), glm::radians( rotate.z ), { 0., 0., 1. } );
        // Combine rotations (order matters: XYZ or YXZ depending on your use case)
        const M4 r = rz * (ry * rx);
        // Scale
        const M4 s = glm::scale( M4(1), scale );
        // Combined matrix
        modelMat = t * (r * s);
        //m_needUpdate = false;
    }

    // Combines 2 TRS into 1 TRS without doing matrix mul and such.
    // Should be numerically stable and not suffer Gimbal-Lock.
    // Could use some UnitTest!
    TRS transform( const TRS& other ) const
    {
        const auto posA = this->pos;
        const auto posB = other.pos;
        const auto rotA = glm::quat(this->rotate);
        const auto rotB = glm::quat(other.rotate);
        const auto scaleA = this->scale;
        const auto scaleB = other.scale;
        const V3 t = posA + (rotA * (scaleA * posB));
        const V3 r = glm::eulerAngles( rotA * rotB );
        const V3 s = scaleA * scaleB;
        return TRS( t, r, s );
    }

    // Probably not efficient for now.
    // Could use some UnitTest!
    TRS transform( const M4& other ) const
    {
        const M4 m = modelMat * other;
        const V3 t = decomposeT(m);
        const V3 r = decomposeEulerAngles(m);
        const V3 s = decomposeScale(m);
        return TRS( t, r, s );
    }

    // v' = M * v
    V3 transform( const V3& v ) const
    {
        return V3( modelMat * V4(v,T(1)) );
    }

    /*
    const M4& modelMatrix() const { return m_modelMat; }
    const M4& getModelMatrix() const { return m_modelMat; }

    // pos
    const V3& pos() const { return pos; }
    const T& x() const { return pos.x; }
    const T& y() const { return pos.y; }
    const T& z() const { return pos.z; }

    // euler angles
    const V3& eulerAngles() const { return rotate; }
    const T& a() const { return rotate.x; }
    const T& b() const { return rotate.y; }
    const T& c() const { return rotate.z; }

    // scale
    const V3& scale() const { return scale; }
    const T& sx() const { return scale.x; }
    const T& sy() const { return scale.y; }
    const T& sz() const { return scale.z; }

    const V3& getT() const { return pos; }
    const T& getTX() const { return pos.x; }
    const T& getTY() const { return pos.y; }
    const T& getTZ() const { return pos.z; }

    const V3& getR() const { return rotate; }
    const T& getRX() const { return rotate.x; }
    const T& getRY() const { return rotate.y; }
    const T& getRZ() const { return rotate.z; }

    const V3& getS() const { return scale; }
    const T& getSX() const { return scale.x; }
    const T& getSY() const { return scale.y; }
    const T& getSZ() const { return scale.z; }

    void setPos( const V3& v ) { pos = v; m_needUpdate = true; }
    void setPos( T x, T y, T z ) { pos = V3( x,y,z ); m_needUpdate = true; }
    void setTranslate( const V3& v ) { pos = v; m_needUpdate = true; }
    void setTranslate( T x, T y, T z ) { pos = V3( x,y,z ); m_needUpdate = true; }
    void setT( const V3& v ) { pos = v; m_needUpdate = true; }
    void setT( T x, T y, T z ) { pos = V3( x,y,z ); m_needUpdate = true; }
    void setTX( T x ) { pos.x = x; m_needUpdate = true; }
    void setTY( T y ) { pos.y = y; m_needUpdate = true; }
    void setTZ( T z ) { pos.z = z; m_needUpdate = true; }
    void setX( T x ) { pos.x = x; m_needUpdate = true; }
    void setY( T y ) { pos.y = y; m_needUpdate = true; }
    void setZ( T z ) { pos.z = z; m_needUpdate = true; }

    void setAng( const V3& v ) { rotate = v;  m_needUpdate = true;}
    void setAng( T x, T y, T z ) { rotate = V3( x,y,z );  m_needUpdate = true;}
    void setRotate( const V3& v ) { rotate = v;  m_needUpdate = true;}
    void setRotate( T x, T y, T z ) { rotate = V3( x,y,z ); m_needUpdate = true; }
    void setR( const V3& v ) { rotate = v; m_needUpdate = true; }
    void setR( T x, T y, T z ) { rotate = V3( x,y,z ); m_needUpdate = true; }
    void setRX( T x ) { rotate.x = x; m_needUpdate = true; }
    void setRY( T y ) { rotate.y = y; m_needUpdate = true; }
    void setRZ( T z ) { rotate.z = z; m_needUpdate = true; }

    void setSize( const V3& v ) { scale = v; m_needUpdate = true; }
    void setSize( T x, T y, T z ) { scale = V3( x,y,z ); m_needUpdate = true; }
    void setScale( const V3& v ) { scale = v; m_needUpdate = true; }
    void setScale( T x, T y, T z ) { scale = V3( x,y,z ); m_needUpdate = true; }
    void setS( const V3& v ) { scale = v; m_needUpdate = true; }
    void setS( T x, T y, T z ) { scale = V3( x,y,z ); m_needUpdate = true; }
    void setSX( T x ) { scale.x = x; m_needUpdate = true; }
    void setSY( T y ) { scale.y = y; m_needUpdate = true; }
    void setSZ( T z ) { scale.z = z; m_needUpdate = true; }

    // Extra
    void addT( T x, T y, T z ) { pos += V3( x,y,z ); m_needUpdate = true; }
    void addR( T x, T y, T z ) { rotate += V3( x,y,z ); m_needUpdate = true; }
    void addS( T x, T y, T z ) { scale += V3( x,y,z ); m_needUpdate = true; }
*/

    // Could use some UnitTest!
    static V3 decomposeT( const M4& a_mat )
    {
        T const * const m = glm::value_ptr( a_mat );
        T x = m[ 12 ];
        T y = m[ 13 ];
        T z = m[ 14 ];
        return { x, y, z };
    }

    // Could use some UnitTest!
    static V3 decomposePosition( const M4& m ) { return m[3]; } // Extracts translation

    // Could use some UnitTest!
    static V3 decomposeScale( const M4& m )
    {
        V3 scale;
        scale.x = glm::length(V3(m[0])); // X-axis scale
        scale.y = glm::length(V3(m[1])); // Y-axis scale
        scale.z = glm::length(V3(m[2])); // Z-axis scale
        return scale;
    }

    // Could use some UnitTest!
    static V3 decomposeEulerAngles( const M4& m )
    {
        const glm::quat a_rotationQuat = glm::quat_cast(m); // RotationQuat
        const V3 a_eulerAngles = glm::eulerAngles(a_rotationQuat); // EulerAngles
        return a_eulerAngles;
    }

    // Could use some UnitTest!
    static V3 decomposeEulerAngles( const Q4& q ) { return glm::eulerAngles( q ); } // EulerAngles

    // Could use some UnitTest!
    static Q4 decomposeRotationQuat( const M4& m )
    {
        M3 rotationMat;
        rotationMat[0] = glm::normalize(V3(m[0])); // X-axis rotation
        rotationMat[1] = glm::normalize(V3(m[1])); // Y-axis rotation
        rotationMat[2] = glm::normalize(V3(m[2])); // Z-axis rotation
        return glm::quat_cast(rotationMat);
    }

    // Could use some UnitTest!
    static M3 decomposeRotationMat( const M4& m )
    {
        M3 rotationMat;
        rotationMat[0] = glm::normalize(V3(m[0])); // X-axis rotation
        rotationMat[1] = glm::normalize(V3(m[1])); // Y-axis rotation
        rotationMat[2] = glm::normalize(V3(m[2])); // Z-axis rotation
        return rotationMat;
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
   Direction3( const V3& dir ) : m_Dir( dir ) {}
   const V3& getDir() const { return m_Dir; }
   V3 toEulerAngles() const { return toEulerAngles( m_Dir ); }

   static Direction3 fromEulerAngles( const V3& euler )
   {
      return Direction3( Vector3_from_EulerAngles( euler ) );
   }

   static Direction3 fromEulerAngles( const V3& euler, const V3& axis )
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
   Ray3( const V3& pos, const V3& dir ) : m_Pos( pos ), m_Dir( dir ) {}
   void setPos( const V3& pos ) { m_Pos = pos; }
   void setDir( const V3& dir ) { m_Dir = dir; }
   const V3& getPos() const { return m_Pos; }
   const V3& getDir() const { return m_Dir; }


   template < typename B >
   Ray3< B >
   convert() const { return Ray3< B >( m_Pos, m_Dir ); }

   Ray3 &
   transform( glm::dmat4 const & modelMatrix )
   {
      m_Pos = glm::dvec3( modelMatrix * glm::dvec4( m_Pos, 1.0 ) );
      m_Dir = glm::dvec3( modelMatrix * glm::dvec4( m_Dir, 1.0 ) );
      return *this;
   }

   std::string
   str() const { std::ostringstream o; o << "pos(" << m_Pos << "), dir(" << m_Dir << ")";
      return o.str();
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
   Plane3( const V3& nrm = V3(0,0,1), T distToOrigin = T( 0.0 ) ) : n( nrm), dist( distToOrigin ) {}

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
   str() const { std::ostringstream o; o << n << "," << dist; return o.str(); }
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
   str() const
   {
      std::ostringstream o;
      o << "A(" << A << "), B(" << B << "), C(" << C << ")";
      return o.str();
   }

   // nABC = (AB x AC) / |AB x AC|.
   V3
   computeNormal() const { return glm::normalize( glm::cross( B - A, C - B ) ); }

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
      constexpr T EPSILON = T( 0.0000001 );
      const V3 AB = B - A;
      const V3 AC = C - A;
      // alias R = rayStart, D = rayDir
      const V3 nDAC = glm::cross( rayDir, AC );
      // 'a' is projection of 'AB' on 'nDAC'
      const T a = glm::dot( AB, nDAC );
      // if 'a' == 0, then ray is parallel to this triangle.
      if ( std::abs( a ) < EPSILON )
      {
//         DE_TRACE("ray is parallel, a = ", a, ", "
//                  "rayStart(", rayStart, "), rayDir(", rayDir, "), "
//                  "A =", A, ", B =", B,", C =", C )
         return false;
      }

      const T f = T( 1.0 ) / a;

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

      const V3 AR = rayStart - A;
      const T u = f * glm::dot( AR, nDAC );
      if ( u < T(0) || u > T(1) )
      {
//         DE_DEBUG("u = ", u, ", a = ", a, ", "
//                  "rayStart(", rayStart, "), rayDir(", rayDir, "), "
//                  "A =", A, ", B =", B,", C =", C )
         return false;
      }

      const V3 nARxAB = glm::cross( AR, AB );
      const T v = f * glm::dot( rayDir, nARxAB );
      if ( v < T(0) || u + v > T(1) )
      {
//         DE_DEBUG("u = ", u, ", v = ", v, ", a = ", a, ", "
//                  "rayStart(", rayStart, "), rayDir(", rayDir, "), "
//                  "A =", A, ", B =", B,", C =", C )
         return false;
      }

      // At this stage we can compute t to find out where the intersection point is on the line.
      const T t = f * glm::dot( AC, nARxAB );
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

   Box3( const V3& minEdge, const V3& maxEdge )
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
    str() const
    {
        std::ostringstream o;
        o << "min(" << m_Min << "), ";
        o << "max(" << m_Max << "), ";
        o << "siz(" << getSize() << ")";
        return o.str();
    }


   void setMin( const V3& min ) { m_Min = min; }
   void setMin( const T& x, const T& y, const T& z ) { m_Min = V3(x,y,z); }
   void setMax( const V3& max ) { m_Max = max; }
   void setMax( const T& x, const T& y, const T& z ) { m_Max = V3(x,y,z); }

   const V3& getMin() const { return m_Min; }
   const V3& getMax() const { return m_Max; }
   V3 & getMin() { return m_Min; }
   V3 & getMax() { return m_Max; }
   V3 getSize() const { return m_Max - m_Min; }
   V3 getCenter() const
   {
       if ( !isValid() ) return V3();
       return m_Min + ( getSize() * 0.5f );
   }

   const V3& min() const { return m_Min; }
   const V3& max() const { return m_Max; }
   V3 & min() { return m_Min; }
   V3 & max() { return m_Max; }
   V3 size() const { return m_Max - m_Min; }
   V3 center() const
   {
       if ( !isValid() ) return V3();
       return m_Min + ( size() * 0.5f );
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

   Box3 operator+( const V3& value ) const
   {
      Box3 box( *this );
      box.m_Min += value;
      box.m_Max += value;
      return box;
   }
   Box3 operator-( const V3& value ) const
   {
      Box3 box( *this );
      box.m_Min -= value;
      box.m_Max -= value;
      return box;
   }
   void reset() { m_Min = m_Max = V3( 0, 0, 0 ); }
   void reset( const V3& pos ) { m_Min = m_Max = pos; }
   void reset( Box3 const & box ) { m_Min = box.m_Min; m_Max = box.m_Max; }
   void reset( const T& x, const T& y, const T& z ) { m_Min = m_Max = V3( x, y, z ); }

   void addInternalPoint( const V3& p )
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
      const auto A = V3{ modelMatrix * V4( getA(), T(1) ) };
      const auto B = V3{ modelMatrix * V4( getB(), T(1) ) };
      const auto C = V3{ modelMatrix * V4( getC(), T(1) ) };
      const auto D = V3{ modelMatrix * V4( getD(), T(1) ) };
      const auto E = V3{ modelMatrix * V4( getE(), T(1) ) };
      const auto F = V3{ modelMatrix * V4( getF(), T(1) ) };
      const auto G = V3{ modelMatrix * V4( getG(), T(1) ) };
      const auto H = V3{ modelMatrix * V4( getH(), T(1) ) };

      // No perspective divide by w wanted here,
      // so pls never use this very function with a projection matrix.
      // If you need to transform a unit-cube in ndc/clipSpace to a
      // camera world frustum = invViewProj * unitcube;
      // unitcube = camera frustum in ndc space.
      // So any frustum ( sheared box = Spat ) will always become
      // a unit-cube in normalized device coords in range [-1,-1,-1] to [1,1,1].
      // Thats what a projectionMatrix does - transform a camera frustum to ndc unit-cube.

      Box3 bbox(A,A);
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
   intersectRay( const V3& rayStart, const V3& rayDir, V3 & hitPosition ) const
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
   minDistanceTo( const V3& pos ) const
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
   maxDistanceTo( const V3& pos ) const
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

   // Checks intersection on one axis (X, Y, or Z)
   static bool
   intersect1D(T origin, T dir, T minB, T maxB, T& tNear, T& tFar)
   {
       if (std::abs(dir) < 1e-6f)
       {
           // Ray is parallel to the slab
           return (origin >= minB && origin <= maxB);
       }

       T t1 = (minB - origin) / dir;
       T t2 = (maxB - origin) / dir;
       if (t1 > t2) std::swap(t1, t2);

       // Update tNear and tFar to track overlapping intervals
       if (t1 > tNear) tNear = t1;
       if (t2 < tFar)  tFar  = t2;

       return (tNear <= tFar && tFar >= 0);
   }

   static bool
   intersectRayAABB(const Ray3<T>& ray, const Box3& box, T& tNearOut, T& tFarOut)
   {
       T tNear = -std::numeric_limits<T>::infinity();
       T tFar  =  std::numeric_limits<T>::infinity();

       // Check each axis separately using helper
       if (!intersect1D(ray.m_Pos.x, ray.m_Dir.x, box.m_Min.x, box.m_Max.x, tNear, tFar)) return false;
       if (!intersect1D(ray.m_Pos.y, ray.m_Dir.y, box.m_Min.y, box.m_Max.y, tNear, tFar)) return false;
       if (!intersect1D(ray.m_Pos.z, ray.m_Dir.z, box.m_Min.z, box.m_Max.z, tNear, tFar)) return false;

       tNearOut = tNear;
       tFarOut  = tFar;
       return true;
   }


};

typedef Box3< float > Box3f;
typedef Box3< double > Box3d;


/*
    glm::vec4 ndcCorners[8] = {
        {-1, -1, -1, 1},
        {1, -1, -1, 1},
        {-1, 1, -1, 1},
        {1, 1, -1, 1},
        {-1, -1,  1, 1},
        {1, -1,  1, 1},
        {-1, 1,  1, 1},
        {1, 1,  1, 1}
    };

    V3 right_down_near = worldCorners[1];
    V3 left_up_near    = worldCorners[2];
    V3 right_up_near   = worldCorners[3];

    V3 left_down_far   = worldCorners[4];
    V3 right_down_far  = worldCorners[5];
    V3 left_up_far     = worldCorners[6];
    V3 right_up_far    = worldCorners[7];

    static constexpr std::array<V3, 8> g_ndcCorners =
        { V3(-1,-1,-1) ; // left_down_near
        corners[1] = V4( 1,-1,-1, 1); // right_down_near
        corners[2] = V4(-1, 1,-1, 1);// left_up_near
        corners[3] = V4( 1, 1,-1, 1);// right_up_near
        corners[4] = V4(-1,-1, 1, 1);// left_down_far
        corners[5] = V4( 1,-1, 1, 1);// right_down_far
        corners[6] = V4(-1, 1, 1, 1);// left_up_far
        corners[7] = V4( 1, 1, 1, 1);// right_up_far
*/

template < typename T, glm::precision P = glm::defaultp >
struct Frustum3
{
    typedef glm::tvec3< T,P > V3;
    typedef glm::tvec4< T,P > V4;
    typedef glm::tmat4x4< T,P > M4;

    std::array< V3, 8 > corners;

    Frustum3()
        : corners{
              V3(-1,-1,-1), // left_down_near
              V3( 1,-1,-1), // right_down_near
              V3(-1, 1,-1), // left_up_near
              V3( 1, 1,-1), // right_up_near
              V3(-1,-1, 1), // left_down_far
              V3( 1,-1, 1), // right_down_far
              V3(-1, 1, 1), // left_up_far
              V3( 1, 1, 1)  // right_up_far
          }
    {}

    static Frustum3 fromBBox( const Box3<T>& bbox )
    {
        Frustum3 frustum;
        const T x = bbox.m_Min.x;
        const T y = bbox.m_Min.y;
        const T z = bbox.m_Min.z;
        const T a = bbox.m_Max.x;
        const T b = bbox.m_Max.y;
        const T c = bbox.m_Max.z;
        frustum.corners[ 0 ] = V3(x,y,z); // left_down_near
        frustum.corners[ 1 ] = V3(a,y,z); // right_down_near
        frustum.corners[ 2 ] = V3(x,b,z); // left_up_near
        frustum.corners[ 3 ] = V3(a,b,z); // right_up_near
        frustum.corners[ 4 ] = V3(x,y,c); // left_down_far
        frustum.corners[ 5 ] = V3(a,y,c); // right_down_far
        frustum.corners[ 6 ] = V3(x,b,c); // left_up_far
        frustum.corners[ 7 ] = V3(a,b,c);  // right_up_far
        return frustum;
    }

    Box3<T> boundingBox() const
    {
        Box3<T> bbox;
        bbox.reset(corners[0]);
        bbox.addInternalPoint(corners[1]);
        bbox.addInternalPoint(corners[2]);
        bbox.addInternalPoint(corners[3]);
        bbox.addInternalPoint(corners[4]);
        bbox.addInternalPoint(corners[5]);
        bbox.addInternalPoint(corners[6]);
        bbox.addInternalPoint(corners[7]);
        return bbox;
    }

    Frustum3 transform( const M4& m ) const
    {
        Frustum3 frustum;

        for (auto & corner : frustum.corners)
        {
            V4 c = m * V4(corner,1.0);
            corner = V3(c) / c.w;
        }

        return frustum;
    }

    Frustum3 transformVP( const M4& vp ) const
    {
        const auto vp_inv = glm::inverse(vp);

        Frustum3 frustum;

        for (auto & corner : frustum.corners)
        {
            V4 c = vp_inv * V4(corner,1.0);
            corner = V3(c) / c.w; // Convert from homogeneous to world space
        }

        return frustum;
    }

    /*
    void draw( const M4& vp )
    {
        glBegin(GL_LINES);
        for (int i = 0; i < 4; i++)
        {
            glVertex3fv(&worldCorners[i].x);
            glVertex3fv(&worldCorners[i + 4].x); // Connect near and far plane
        }
        int edges[12][2] = { {0,1}, {1,3}, {3,2}, {2,0},
                             {4,5}, {5,7}, {7,6}, {6,4},
                             {0,4}, {1,5}, {2,6}, {3,7} };
        for (auto& e : edges)
        {
            glVertex3fv(&worldCorners[e[0]].x);
            glVertex3fv(&worldCorners[e[1]].x);
        }
        glEnd();
    }
    */
};

} // end namespace de

template < typename T >
std::ostream& operator<< ( std::ostream& o, de::Box3< T > const & bbox )
{
   o << bbox.str();
   return o;
}

