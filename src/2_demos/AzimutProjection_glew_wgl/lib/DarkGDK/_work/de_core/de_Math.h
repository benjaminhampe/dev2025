#pragma once
#include <de_core/de_Logger.h>
#include <de_core/de_ForceInline.h>
#include <de_glm.hpp>
#include <ctime> // for randimze()

// Min():
template < typename T > T dbMin( T a, T b ) { return std::min( a, b ); }
template < typename T > T dbMin( T a, T b, T c ) { return std::min( a, std::min( b, c ) ); }
template < typename T > T dbMin( T a, T b, T c, T d ) { return std::min( std::min( a, b ), std::min( c, d ) ); }

// Max():
template < typename T > T dbMax( T a, T b ) { return std::max( a, b ); }
template < typename T > T dbMax( T a, T b, T c ) { return std::max( a, std::max( b, c ) ); }
template < typename T > T dbMax( T a, T b, T c, T d ) { return std::max( std::max( a, b ), std::max( c, d ) ); }

// Abs():
DE_FORCE_INLINE float dbAbs( float v ) { return std::fabs( v ); }
DE_FORCE_INLINE double dbAbs( double v ) { return std::fabs( v ); }

// Sin():
DE_FORCE_INLINE float dbSin( float v ) { return ::sinf( v ); }
DE_FORCE_INLINE double dbSin( double v ) { return ::sin( v ); }

// Cos():
DE_FORCE_INLINE float dbCos( float v ) { return ::cosf( v ); }
DE_FORCE_INLINE double dbCos( double v ) { return ::cos( v ); }

// Atan2():
DE_FORCE_INLINE float dbAtan2( float a, float b ) { return ::atan2f( a,b ); }
DE_FORCE_INLINE double dbAtan2( double a, double b ) { return ::atan2( a,b ); }

// Arcsin():
DE_FORCE_INLINE float dbAsin( float v ) { return ::asinf( v ); }
DE_FORCE_INLINE double dbAsin( double v ) { return ::asin( v ); }

// Arccos():
DE_FORCE_INLINE float dbAcos( float v ) { return ::acosf( v ); }
DE_FORCE_INLINE double dbAcos( double v ) { return ::acos( v ); }

// SQRT():
DE_FORCE_INLINE float dbSqrt( float v ) { return ::sqrtf( v ); }
DE_FORCE_INLINE double dbSqrt( double v ) { return ::sqrt( v ); }

// isInfinite():
DE_FORCE_INLINE float dbIsInf( float v ) { return std::isinf( v ) || std::isnan( v ); }
DE_FORCE_INLINE double dbIsInf( double v ) { return std::isinf( v ) || std::isnan( v ); }

// DEG: ( convert radians to degrees )
DE_FORCE_INLINE float dbDEG( float radians ) { return radians * float(180.0 / 3.1415926535897932384626433832795028841971693993751); }
DE_FORCE_INLINE double dbDEG( double radians ) { return radians * double(180.0 / 3.1415926535897932384626433832795028841971693993751); }

template < typename T > glm::tvec2< T > dbDEG( glm::tvec2< T > const & v ) { return { dbDEG( v.x ), dbDEG( v.y ) }; }
template < typename T > glm::tvec3< T > dbDEG( glm::tvec3< T > const & v ) { return { dbDEG( v.x ), dbDEG( v.y ), dbDEG( v.z ) }; }
template < typename T > glm::tvec4< T > dbDEG( glm::tvec4< T > const & v ) { return { dbDEG( v.x ), dbDEG( v.y ), dbDEG( v.z ), dbDEG( v.w ) }; }

// RAD: ( convert degrees to radians )
DE_FORCE_INLINE float dbRAD( float degrees ) { return degrees * float(3.1415926535897932384626433832795028841971693993751 / 180.0); }
DE_FORCE_INLINE double dbRAD( double degrees ) { return degrees * double(3.1415926535897932384626433832795028841971693993751 / 180.0); }

template < typename T > glm::tvec2< T > dbRAD( glm::tvec2< T > const & v ) { return { dbRAD( v.x ), dbRAD( v.y ) }; }
template < typename T > glm::tvec3< T > dbRAD( glm::tvec3< T > const & v ) { return { dbRAD( v.x ), dbRAD( v.y ), dbRAD( v.z ) }; }
template < typename T > glm::tvec4< T > dbRAD( glm::tvec4< T > const & v ) { return { dbRAD( v.x ), dbRAD( v.y ), dbRAD( v.z ), dbRAD( v.w ) }; }

// Clamp:
DE_FORCE_INLINE int32_t dbClamp( int32_t v, int32_t vmin, int32_t vmax ) { return std::clamp( v, vmin, vmax ); }
DE_FORCE_INLINE float dbClamp( float v, float vmin, float vmax ) { return std::clamp( v, vmin, vmax ); }
DE_FORCE_INLINE double dbClamp( double v, double vmin, double vmax ) { return std::clamp( v, vmin, vmax ); }

DE_FORCE_INLINE int32_t dbClampi( int32_t v, int32_t vmin, int32_t vmax ) { return std::clamp( v, vmin, vmax ); }
DE_FORCE_INLINE float dbClampf( float v, float vmin, float vmax ) { return std::clamp( v, vmin, vmax ); }
DE_FORCE_INLINE double dbClampd( double v, double vmin, double vmax ) { return std::clamp( v, vmin, vmax ); }

//template < typename T > static T dbClamp( T v, T vmin, T vmax )
//{
//   if ( vmax < vmin ) { std::swap( vmin, vmax ); }
//   return std::min( std::max( v, vmin ), vmax );
//}
//DE_FORCE_INLINE int32_t dbClampi( int32_t v, int32_t vmin, int32_t vmax ) { if ( vmax < vmin ) { std::swap( vmin, vmax ); } return std::min( std::max( v, vmin ), vmax ); }
//DE_FORCE_INLINE float dbClampf( float v, float vmin, float vmax ) { if ( vmax < vmin ) { std::swap( vmin, vmax ); } return std::min( std::max( v, vmin ), vmax ); }
//DE_FORCE_INLINE double dbClampd( double v, double vmin, double vmax ) { if ( vmax < vmin ) { std::swap( vmin, vmax ); } return std::min( std::max( v, vmin ), vmax ); }

//==============================================================================
DE_FORCE_INLINE bool dbIsPowerOfTwo( uint64_t k )
{
   if (k < 2) return false;
   return 0 == (k & (k - 1));
}

DE_FORCE_INLINE bool dbIsPowerOfTwo( uint32_t k )
{
   if (k < 2) return false;
   return 0 == (k & (k - 1));
}

// Returns the smallest PowerOf2 greater or equal than the given integral value.
DE_FORCE_INLINE uint32_t dbNextPowerOf2( uint32_t k ) noexcept
{
   if (k < 2) return 2;
   --k;
   k |= (k >> 1);
   k |= (k >> 2);
   k |= (k >> 4);
   k |= (k >> 8);
   k |= (k >> 16);
   return k + 1;
}

// Returns the smallest PowerOf2 greater or equal than the given integral value.
DE_FORCE_INLINE uint64_t dbNextPowerOf2( uint64_t k ) noexcept
{
   if (k < 2) return 2;
   --k;
   k |= (k >> 1);
   k |= (k >> 2);
   k |= (k >> 4);
   k |= (k >> 8);
   k |= (k >> 16);
   k |= (k >> 32);
   return k + 1;
}

// Reciprocal:
DE_FORCE_INLINE float dbReciprocal( float v ) { float i = 1.0f / v; if ( std::isinf( i ) || std::isnan( i ) ) { return 0.0f; } return i; }
DE_FORCE_INLINE double dbReciprocal( double v ) { double i = 1.0 / v; if ( std::isinf( i ) || std::isnan( i ) ) { return 0.0; } return i; }

// Equals:
DE_FORCE_INLINE bool dbEquals( float a, float b, float eps = 1.0e-6f ) { return std::abs( b-a ) <= eps; }
DE_FORCE_INLINE bool dbEquals( double a, double b, double eps = 1.0e-9 ) { return std::abs( b-a ) <= eps; }
DE_FORCE_INLINE bool dbEquals( glm::vec3 const & a, glm::vec3 const & b, float eps = 1.0e-6f )
{
   return dbEquals( a.x,b.x,eps ) && dbEquals( a.y,b.y,eps ) && dbEquals( a.z,b.z,eps );
}
DE_FORCE_INLINE bool dbEquals( glm::dvec3 const & a, glm::dvec3 const & b, double eps = 1e-6 )
{
   return dbEquals( a.x, b.x, eps ) && dbEquals( a.y, b.y, eps ) && dbEquals( a.z, b.z, eps );
}

// IsZero:
DE_FORCE_INLINE float dbIsZero( float v ) { return dbEquals( v, 0.0f ); }
DE_FORCE_INLINE double dbIsZero( double v ) { return dbEquals( v, 0.0, 1.0e-15 ); }

// Round:
DE_FORCE_INLINE float dbRound( float v ) { return std::round( v ); }
DE_FORCE_INLINE double dbRound( double v ) { return std::round( v ); }

// Round32:
DE_FORCE_INLINE int32_t dbRound32( float v ) { return int32_t( std::round( v ) ); }
DE_FORCE_INLINE int32_t dbRound32( double v ) { return int32_t( std::round( v ) ); }

// Round64:
DE_FORCE_INLINE int64_t dbRound64( float v ) { return int64_t( std::round( v ) ); }
DE_FORCE_INLINE int64_t dbRound64( double v ) { return int64_t( std::round( v ) ); }

// SafeModulo with negative numbers
DE_FORCE_INLINE int32_t dbModSafe( int32_t a, int32_t b )
{
   if ( a == 0 || b == 0 ) { return 0; }
   return std::abs( a ) % std::abs( b );
}

// Byte2float
DE_FORCE_INLINE float dbByte2float( uint8_t b_0_255 )
{
   return float( b_0_255 ) / 255.0f;
}

// Float2byte
DE_FORCE_INLINE uint8_t dbFloat2byte( float f_0_1 )
{
   return uint8_t( dbClamp( int( f_0_1 * 255.0f ), 0, 255 ) );
}

namespace de {

/// @author Benjamin Hampe <benjaminhampe@gmx.de>
/// @class de.Math
struct Math
{
   DE_CREATE_LOGGER( "de.Math" )

   constexpr static double INV_3 = 1.0/3.0;
   constexpr static double INV_6 = 1.0/6.0;
   constexpr static double INV_12 = 1.0/12.0;
   constexpr static double INV_24 = 1.0/24.0;
   constexpr static double INV_60 = 1.0/60.0;
   constexpr static double INV_288 = 1.0/288.0;
   constexpr static double INV_360 = 1.0/360.0;
   constexpr static double INV_1024 = 1.0/1024.0;
   constexpr static double INV_1440 = 1.0/1440.0; // used by astro::VDI2067
   constexpr static double INV_3600 = 1.0/3600.0;
   constexpr static double INV_86400 = 1.0/86400.0;
   constexpr static double CONST_1_OVER_PI = 1.0/M_PI;
   constexpr static double CONST_2_OVER_PI = 2.0/M_PI;
   constexpr static double CONST_1_OVER_2PI = 0.5/M_PI;
   constexpr static double PI64 = 3.1415926535897932384626433832795028841971693993751;
   constexpr static double TWOPI64 = 2.0 * PI64;
   constexpr static double RAD2DEG64 = 180.0 / PI64;
   constexpr static double DEG2RAD64 = PI64 / 180.0;
   constexpr static float PI = float( PI64 );
   constexpr static float TWOPI = float( TWOPI64 );
   constexpr static float RAD2DEG = float( RAD2DEG64 );
   constexpr static float DEG2RAD = float( DEG2RAD64 );
   /// @brief 32-bit constant for reciprocal (1 / Pi).
   constexpr static float const RECIPROCAL_PI = float( 1.0 / PI64 );
   constexpr static float const HALF_PI = float( 0.5 * PI64 );
   constexpr static double const HALF_PI64 = PI64 * 0.5;

   /// @author Benjamin Hampe <benjaminhampe@gmx.de>
   /// @brief Rotate vector
   static glm::dvec3
   transformVector( glm::dmat4 const & modelMat, glm::vec3 const & vec )
   {
      //   auto a = glm::tvec3< T >( trs * glm::tvec4< T >(tri.A,T(1)) );
      //   auto b = glm::tvec3< T >( trs * glm::tvec4< T >(tri.B,T(1)) );
      //   auto c = glm::tvec3< T >( trs * glm::tvec4< T >(tri.C,T(1)) );
      return glm::dvec3( modelMat * glm::dvec4( vec, 1.0 ) );
   }

   // /// @author Benjamin Hampe <benjaminhampe@gmx.de>
   // // Safe Modulo with negative numbers
   // static int32_t
   // safeMod( int32_t a, int32_t b )
   // {
       // if ( a == 0 || b == 0 ) { return 0; }
       // return std::abs( a ) % std::abs( b );
   // }


   // /// @author Benjamin Hampe <benjaminhampe@gmx.de>
   // template < typename T >
   // static T
   // clampT( T value, T minValue, T maxValue )
   // {
      // if ( maxValue < minValue ) { std::swap( minValue, maxValue ); }
      // return std::min( std::max( value, minValue ), maxValue );
   // }

   // /// @author Benjamin Hampe <benjaminhampe@gmx.de>
   // static int32_t
   // clampi( int32_t value, int32_t minValue, int32_t maxValue )
   // {
      // if ( maxValue < minValue ) { std::swap( minValue, maxValue ); }
      // return std::min( std::max( value, minValue ), maxValue );
   // }

   // /// @author Benjamin Hampe <benjaminhampe@gmx.de>
   // static float
   // clampf( float value, float minValue, float maxValue )
   // {
      // if ( maxValue < minValue ) { std::swap( minValue, maxValue ); }
      // return std::min( std::max( value, minValue ), maxValue );
   // }

   // /// @author Benjamin Hampe <benjaminhampe@gmx.de>
   // static double
   // clampd( double value, double minValue, double maxValue )
   // {
      // if ( maxValue < minValue ) { std::swap( minValue, maxValue ); }
      // return std::min( std::max( value, minValue ), maxValue );
   // }

   // /// @brief Compile time helper
   // constexpr static float toDeg ( float radians ) { return radians * RAD2DEG; }
   // constexpr static double toDeg ( double radians ) { return radians * RAD2DEG64; }

   // constexpr static float toRad ( float degrees ) { return degrees * DEG2RAD; }
   // constexpr static double toRad ( double degrees ) { return degrees * DEG2RAD64; }

   // /// @author Benjamin Hampe <benjaminhampe@gmx.de>
   // template < typename T >
   // static T
   // min3( T a, T b, T c )
   // {
      // return std::min( std::min( a,b ), c );
   // }

   // /// @author Benjamin Hampe <benjaminhampe@gmx.de>
   // template < typename T >
   // static T
   // min4( T a, T b, T c, T d )
   // {
      // return std::min( std::min( a,b ), std::min( c,d ) );
   // }

   // /// @author Benjamin Hampe <benjaminhampe@gmx.de>
   // template < typename T >
   // static T
   // max3( T a, T b, T c )
   // {
      // return std::max( std::max( a,b ), c );
   // }

   // /// @author Benjamin Hampe <benjaminhampe@gmx.de>
   // template < typename T >
   // static T
   // max4( T a, T b, T c, T d )
   // {
      // return std::max( std::max( a,b ), std::max( c,d ) );
   // }

   // /// @author Benjamin Hampe <benjaminhampe@gmx.de>
   // static int32_t
   // round32( double v ) { return std::round( v ); }

   // /// @author Benjamin Hampe <benjaminhampe@gmx.de>
   // static int32_t
   // round32( float v ) { return std::round( v ); }

   // /// @author Benjamin Hampe <benjaminhampe@gmx.de>
   // static bool
   // equals( float a, float b, float epsilon = 1.0e-6 ) { return std::abs( b-a ) <= epsilon; }

   // /// @author Benjamin Hampe <benjaminhampe@gmx.de>
   // static bool
   // equals( double a, double b, double epsilon = 1.0e-9 ) { return std::abs( b-a ) <= epsilon; }

   // /// @author Benjamin Hampe <benjaminhampe@gmx.de>
   // static bool
   // iszero( double val ) { return equals( val, 0.0, 1.0e-15 ); }

   // /// @author Benjamin Hampe <benjaminhampe@gmx.de>
   // static bool
   // iszero( float val ) { return equals( val, 0.0f ); }

   // /// @author Benjamin Hampe <benjaminhampe@gmx.de>
   // static bool
   // equals( glm::vec3 const & a, glm::vec3 const & b, float epsilon = 1.0e-6 )
   // {
      // return equals( a.x,b.x,epsilon ) && equals( a.y,b.y,epsilon )
         // && equals( a.z,b.z,epsilon );
   // }

   // /// @author Benjamin Hampe <benjaminhampe@gmx.de>
   // static double
   // reciprocal( double val )
   // {
      // double inv = 1.0 / val;
      // if ( std::isinf( inv ) )
      // {
         // DE_DEBUG("Got 1.0 / ", val, " = infinity, returns 1.0." )
         // return 1.0;
      // }

      // if ( std::isnan( inv ) )
      // {
         // DE_DEBUG("Got 1.0 / ", val, " = NaN, returns 1.0." )
         // return 1.0;
      // }

      // return inv;
   // }

   // /// @author Benjamin Hampe <benjaminhampe@gmx.de>
   // static float
   // reciprocal( float val )
   // {
      // float inv = 1.0f / val;

      // if ( std::isinf( inv ) )
      // {
         // DE_DEBUG("Got 1.0f / ", val, "f = infinity, returns 1.0f." )
         // return 1.0f;
      // }

      // if ( std::isnan( inv ) )
      // {
         // DE_DEBUG("Got 1.0 / ", val, " = NaN, returns 1.0f." )
         // return 1.0f;
      // }

      // return inv;
   // }

   /// @author Benjamin Hampe <benjaminhampe@gmx.de>
   // Normalize to [0,360):
   template < class T >
   static T
   wrapAngle(T x)
   {
      while (x < T(0)) x += T(360);
      while (x > T(360)) x -= T(360);
      return x;
   }

   /// @author Benjamin Hampe <benjaminhampe@gmx.de>
   // Normalize to [-180,180):
   template < class T >
   static T
   wrapAngle180(T x)
   {
      while (x < T(180)) { x += T(360); }
      while ((x > T(180)) || (std::abs(x - T(180)) < 1.0e-9)) { x -= T(360); }
      return x;
   }

   /// @author Benjamin Hampe <benjaminhampe@gmx.de>
   static float
   wrapAngleToRange0_2pi( float radians )
   {
      while( radians < 0.0f ) radians += TWOPI;
      while( radians >= TWOPI ) radians -= TWOPI;
      return radians;
   }

   /// @author Benjamin Hampe <benjaminhampe@gmx.de>
   static uint8_t
   float2byte( float float_in_range_0_1 )
   {
      return uint8_t( dbClamp( int( float_in_range_0_1 * 255.0f ), 0, 255 ) );
   }

   /// @author Benjamin Hampe <benjaminhampe@gmx.de>
   static float
   byte2float( uint8_t byte_in_range_0_255 )
   {
      return float( byte_in_range_0_255 ) / 255.0f;
   }


   /// @author Benjamin Hampe <benjaminhampe@gmx.de>
   // dirty round with cast to int32_t between source and target var-type
   template < class T >
   static int32_t
   iPart( T value )
   {
      return static_cast< int32_t >( value );
   }

   /// @author Benjamin Hampe <benjaminhampe@gmx.de>
   // difference between value and dirty rounded
   template < class T >
   static T
   fPart( T value )
   {
      return value - static_cast< T >( static_cast< int32_t >( value ) );
   }

   /// @author Benjamin Hampe <benjaminhampe@gmx.de>
   // static uint32_t _nextPow2_loop( uint32_t n );
   // static uint32_t _next_power_of_2( uint32_t value );
   // static uint32_t _nextPow2( uint32_t x );
   // static uint32_t _nextPow2_IEEE( uint32_t n );
   // static int32_t s32_pow(int32_t x, int32_t p);
   // static int32_t sign( float value );
   // static int32_t sign( double value );
   // Rundet auf Anzahl Stellen nach dem Komma
   //-> Laufzeitoptimiert, 2.Parameter nur von 0 bis 8!!!, sonst Absturz!!!
   // static float Round(float zahl, int32_t stellen);
   // Rundet auf Anzahl Stellen nach dem Komma
   //-> Laufzeitoptimiert, 2.Parameter nur von 0 bis 8!!!, sonst Absturz!!!
   // static double Round(double zahl, int32_t stellen);

   // static void
   // Randomize();

   // static uint32_t
   // Random(uint32_t seed);

   // template<class A,class B,class C>
   // static void
   // Fmod( A const & f_value, B & i_part, C & f_part )
   // {
      // f_part = (C)abs( modf( f_value, *i_part ) );
   // }

   //	void Math::Split( float r, float* i, float* f)
   //	{
   //		*f = fabs( fmodf( r,*i ) );
   //	}

   //	void Math::Split( double r, double* i, double* f)
   //	{
   //		*f = core::Math::abs<double>( modf( r,i ) );
   //	}



   /// @author Benjamin Hampe <benjaminhampe@gmx.de>
   template < typename T >
   static glm::tvec2< T >
   getOrtho2D( glm::tvec2< T > const & dir ) { return { dir.y, -dir.x }; }

   /// @author Benjamin Hampe <benjaminhampe@gmx.de>
   template < typename T >
   static glm::tvec2< T >
   getNormal2D( glm::tvec2< T > const & dir )
   {
      return glm::normalize( glm::tvec2< T >{ dir.y, -dir.x } ); // swizzle does not change original len.
   }

   /// @author Benjamin Hampe <benjaminhampe@gmx.de>
   template < typename T >
   static glm::tvec3< T >
   getNormal3D( glm::tvec3< T > const & a, glm::tvec3< T > const & b )
   {
      return glm::normalize( glm::cross( a, b ) );
   }

   /// @author Benjamin Hampe <benjaminhampe@gmx.de>
   template < typename T >
   static glm::tvec3< T >
   getNormal3D( glm::tvec3< T > const & a, glm::tvec3< T > const & b, glm::tvec3< T > const & c )
   {
      return glm::normalize( glm::cross( b-a, c-b ) );
   }

   /// @author Benjamin Hampe <benjaminhampe@gmx.de>
   static float
   convertToNormalized( int16_t value )
   {
      // ( make all symmetric around +0.5f )
      if ( value == 0 )
      return 0.5f;
      else if ( value > 0 )
      return 0.5f + static_cast< float >(value)/65534.0f;
      else
      return 0.5f + static_cast< float >(value)/65536.0f;
   }

   /// @author Benjamin Hampe <benjaminhampe@gmx.de>
   /// @brief Get internal time as full hours + full minutes + full seconds
   static void
   millisecondsToHMS( int32_t ms, int32_t & out_hh, int32_t & out_mm,
                                  int32_t & out_ss, int32_t & out_ms )
   {
      int32_t hh = ms / 3600000;
      ms -= hh * 3600000;
      int32_t mm = ms / 60000;
      ms -= mm * 60000;
      int32_t ss = ms / 1000;
      ms -= ss * 1000;
      out_hh = hh;
      out_mm = mm;
      out_ss = ss;
      out_ms = ms;
   }

   /// @author Benjamin Hampe <benjaminhampe@gmx.de>
   template <class T>
   static T
   ggT( T a, T b)
   {
      T tmp;
      while ( b != 0 )
      {
         tmp=a%b;
         a=b;
         b=tmp;
      };
      return a;
   }

   /// @author Benjamin Hampe <benjaminhampe@gmx.de>
   template <class T>
   static T
   ggT2( T a,T b)
   {
      for(;;)
      {
         if(a==0) return b;
         b%=a;
         if(b==0) return a;
         a%=b;
      }
   }

   /// @author Benjamin Hampe <benjaminhampe@gmx.de>
   /// @brief Compute matrix (diagonal,diagonal) dot product = trace
   static float
   trace( glm::mat3 const & mat )
   {
      // Diagonal element sum = trace of a matrix trace(AB) = trace(BA) -> associative.
      float const * const m = glm::value_ptr( mat );
      return m[ 0 ] * m[ 0 ]
            + m[ 4 ] * m[ 4 ]
            + m[ 8 ] * m[ 8 ];
   }

   /// @author Benjamin Hampe <benjaminhampe@gmx.de>
   /// @brief Compute matrix (diagonal,diagonal) dot product = trace
   static float
   trace( glm::mat4 const & mat )
   {
      // Diagonal element sum = trace of a matrix trace(AB) = trace(BA) -> associative.
      float const * const m = glm::value_ptr( mat );
      return m[ 0 ] * m[ 0 ]
            + m[ 5 ] * m[ 5 ]
            + m[ 10 ] * m[ 10 ]
            + m[ 15 ] * m[ 15 ];
   }

   // ----------------------------------------------------------------------------------------
   /// @author Benjamin Hampe <benjaminhampe@gmx.de>
   /// @brief Rotate vector
   static glm::vec3
   rotateVectorX( glm::vec3 const & v, float rad )
   {
      if ( dbEquals(rad,0.0f) ) return v;
      float c = dbCos( rad );
      float s = dbSin( rad );
      return glm::vec3( v.x, float( v.y*c - v.z*s ), float( v.y*s + v.z*c ) );
   }

   /// @author Benjamin Hampe <benjaminhampe@gmx.de>
   /// @brief Rotate vector
   static glm::vec3
   rotateVectorY( glm::vec3 const & v, float rad )
   {
      if ( dbEquals(rad,0.0f) ) return v;
      float c = dbCos( rad );
      float s = dbSin( rad );
      return glm::vec3( v.x*c - v.z*s, v.y, v.x*s + v.z*c );
   }

   /// @author Benjamin Hampe <benjaminhampe@gmx.de>
   /// @brief Rotate vector
   static glm::vec3
   rotateVectorZ( glm::vec3 const & v, float rad )
   {
      if ( dbEquals(rad,0.0f) ) return v;
      float c = dbCos( rad );
      float s = dbSin( rad );
      return glm::vec3( float( v.x*c - v.y*s ), float( v.x*s + v.y*c ), v.z );
   }

   /// @author Benjamin Hampe <benjaminhampe@gmx.de>
   /// @brief Rotate vector
   static glm::vec3
   rotateVector( glm::vec3 const & v, glm::vec3 const & angles_in_rad )
   {
      glm::vec3 k = rotateVectorZ( v, angles_in_rad.z );
      k = rotateVectorY( k, angles_in_rad.y );
      k = rotateVectorX( k, angles_in_rad.x );
      return k;
   }

   /// @author Benjamin Hampe <benjaminhampe@gmx.de>
   /// @brief Create mat4
   // DCM from EulerAngles in radians
   static glm::dmat4 // , glm::precision P = glm::defaultp
   buildRotationMatrix( glm::vec3 const & radians )
   {
      double a = dbCos( double( radians.x ) );
      double b = dbSin( double( radians.x ) );
      double c = -b;
      double d = a;
      double e = dbCos( double( radians.y ) );
      double f = dbSin( double( radians.y ) );
      double g = -f;
      double h = e;
      double i = double( 1.0 );
      double k = double( 0.0 );
      double j = -k;
      double l = i;

      glm::dmat4 mat( 1.0 );
      double * m = glm::value_ptr( mat );
      m[ 0 ] = e*i;
      m[ 1 ] = a*j + c*f*i;
      m[ 2 ] = b*j + d*f*i;
      //m[ 3 ] = 0.0f;
      m[ 4 ] = e*k;
      m[ 5 ] = a*l+c*f*k;
      m[ 6 ] = b*l+d*f*k;
      //m[ 7 ] = 0.0f;
      m[ 8 ] = g;
      m[ 9 ] = c*h;
      m[ 10 ] = d*h;
      //m[ 11 ] = 0.0f;
      //m[ 12 ] = 0.0f;
      //m[ 13 ] = 0.0f;
      //m[ 14 ] = 0.0f;
      //m[ 15 ] = 1.0f;
      return mat;
   }

   /// @author Benjamin Hampe <benjaminhampe@gmx.de>
   /// @brief Create mat4
   // DCM from EulerAngles in degrees
   static glm::dmat4 // , glm::precision P = glm::defaultp
   buildRotationMatrixDEG( glm::vec3 const & degrees )
   {
      return buildRotationMatrix( degrees * Math::DEG2RAD );
   }

   /// @author Benjamin Hampe <benjaminhampe@gmx.de>
   /// @brief Create mat3
   // DCM from EulerAngles in degrees
   static glm::mat3 // , glm::precision P = glm::defaultp
   buildRotationMatrixDEG3f( glm::vec3 const & degrees )
   {
      return glm::mat3( buildRotationMatrixDEG( degrees ) );
   }

   /// @author Benjamin Hampe <benjaminhampe@gmx.de>
   /// @brief Create mat4
   // ----------------------------------------------------------------------------------------
   //      |  1  0       0       0 |
   //  M = |  0  cos(A) -sin(A)  0 |
   //      |  0  sin(A)  cos(A)  0 |
   //      |  0  0       0       1 |
   // ----------------------------------------------------------------------------------------
   static glm::mat4
   buildRotationMatrixZYX( float x_rad, float y_rad, float z_rad )
   {
      glm::mat4 mat( 1.0f );
      float * m = glm::value_ptr( mat );

      typedef float Real_t;

      Real_t a = ::cosf( Real_t( x_rad ) );
      Real_t b = ::sinf( Real_t( x_rad ) );
      Real_t c = -b;
      Real_t d = a;
      Real_t e = ::cosf( Real_t( y_rad ) );
      Real_t f = ::sinf( Real_t( y_rad ) );
      Real_t g = -f;
      Real_t h = e;
      Real_t i = ::cosf( Real_t( z_rad ) );
      Real_t k = ::sinf( Real_t( z_rad ) );
      Real_t j = -k;
      Real_t l = i;
      m[ 0 ] = e*i;
      m[ 1 ] = a*j + c*f*i;
      m[ 2 ] = b*j + d*f*i;
      //m[ 3 ] = 0.0f;
      m[ 4 ] = e*k;
      m[ 5 ] = a*l+c*f*k;
      m[ 6 ] = b*l+d*f*k;
      //m[ 7 ] = 0.0f;
      m[ 8 ] = g;
      m[ 9 ] = c*h;
      m[ 10 ] = d*h;
      //m[ 11 ] = 0.0f;
      //m[ 12 ] = 0.0f;
      //m[ 13 ] = 0.0f;
      //m[ 14 ] = 0.0f;
      //m[ 15 ] = 1.0f;
      return mat;
   }

   /// @author Benjamin Hampe <benjaminhampe@gmx.de>
   /// @brief Create mat4
   template< typename Real_t >
   static glm::tmat4x4< Real_t >
   buildRotationMatrixZYX_opt_v2( Real_t x_rad, Real_t y_rad, Real_t z_rad )
   {
      glm::tmat4x4< Real_t > mat( Real_t( 1 ) );
      Real_t * m = glm::value_ptr( mat );
      Real_t a = ::cosf( Real_t( x_rad ) );
      Real_t b = ::sinf( Real_t( x_rad ) );
      Real_t c = ::cosf( Real_t( y_rad ) );
      Real_t d = ::sinf( Real_t( y_rad ) );
      Real_t e = ::cosf( Real_t( z_rad ) );
      Real_t f = ::sinf( Real_t( z_rad ) );
      m[ 0 ] = c*e;
      m[ 1 ] = -f*a - b*d*e;
      m[ 2 ] = -f*b + a*d*e;
      m[ 4 ] = c*f;
      m[ 5 ] = a*e - b*d*f;
      m[ 6 ] = b*e + a*d*f;
      m[ 8 ] = -d;
      m[ 9 ] = -b*c;
      m[ 10 ] = a*c;
      return mat;
   }

   /// @author Benjamin Hampe <benjaminhampe@gmx.de>
   /// @brief Create mat4
   template < typename T, glm::precision P = glm::defaultp >
   static glm::tmat4x4< T,P >
   buildLookAtRH( glm::tvec3< T,P > pos,
                  glm::tvec3< T,P > dir,
                  glm::tvec3< T,P > up )
   {
      dir = glm::normalize( dir );
      up = glm::normalize( up );
      glm::tvec3< T,P > right = glm::normalize( glm::cross( dir, up ) );
      up = glm::normalize( glm::cross( right, dir ) ); // recompute normalized

      glm::tmat4x4< T,P > mat( T(1) );
      T* m = glm::value_ptr( mat );
      m[ 0 ] = right.x;
      m[ 4 ] = right.y;
      m[ 8 ] = right.z;
      m[ 1 ] = up.x;
      m[ 5 ] = up.y;
      m[ 9 ] = up.z;
      m[ 2 ] = dir.x;
      m[ 6 ] = dir.y;
      m[ 10 ] = dir.z;
      m[ 12 ] = -glm::dot( right, pos );
      m[ 13 ] = -glm::dot( up, pos );
      m[ 14 ] = -glm::dot( dir, pos );
      //DE_DEBUG("pos(",pos,",) dir(",dir,"), up(",up,", left(",left,")")
      return mat;
   }

   /// @author Benjamin Hampe <benjaminhampe@gmx.de>
   /// @brief Create mat4
   template < typename T, glm::precision P = glm::defaultp >
   static glm::tmat4x4< T,P >
   buildLookAtLH( glm::tvec3< T,P > pos,
                  glm::tvec3< T,P > dir,
                  glm::tvec3< T,P > up )
   {
      dir = glm::normalize( dir );
      up = glm::normalize( up );
      glm::tvec3< T,P > left = glm::normalize( glm::cross( up, dir ) );
      up = glm::normalize( glm::cross( dir, left ) ); // recompute normalized
      glm::tmat4x4< T,P > mat( T(1) );
      T* m = glm::value_ptr( mat );
      m[ 0 ] = left.x;
      m[ 4 ] = left.y;
      m[ 8 ] = left.z;
      m[ 1 ] = up.x;
      m[ 5 ] = up.y;
      m[ 9 ] = up.z;
      m[ 2 ] = dir.x;
      m[ 6 ] = dir.y;
      m[ 10 ] = dir.z;
      m[ 12 ] = -glm::dot( left, pos );
      m[ 13 ] = -glm::dot( up, pos );
      m[ 14 ] = -glm::dot( dir, pos );
      //DE_DEBUG("pos(",pos,",) dir(",dir,"), up(",up,", left(",left,")")
      return mat;
   }



};

} // end namespace de.


/*
   /// @author Benjamin Hampe <benjaminhampe@gmx.de>
   /// @brief Create mat4
   // ----------------------------------------------------------------------------------------
   //      |  1  0       0       0 |
   //  M = |  0  cos(A) -sin(A)  0 |
   //      |  0  sin(A)  cos(A)  0 |
   //      |  0  0       0       1 |
   // ----------------------------------------------------------------------------------------
   template < typename T, glm::precision P = glm::defaultp >
   static glm::tmat4x4< T,P >
   buildXRotationMatrix( T rad )
   {
      glm::tmat4x4< T,P > xRot( 1.0f );
      T * m = glm::value_ptr( xRot );
      T s = dbSin( rad );
      T c = dbCos( rad );
      m[5] = c;
      m[6] = -s;
      m[9] = s;
      m[10] = c;
      return xRot;
   }

   /// @author Benjamin Hampe <benjaminhampe@gmx.de>
   /// @brief Create mat4
   // ----------------------------------------------------------------------------------------
   //       |  cos(A)  0   sin(A)  0 |
   //   M = |  0       1   0       0 |
   //       | -sin(A)  0   cos(A)  0 |
   //       |  0       0   0       1 |
   // ----------------------------------------------------------------------------------------
   template < typename T, glm::precision P = glm::defaultp >
   static glm::tmat4x4< T, P >
   buildYRotationMatrix( T rad )
   {
      glm::tmat4x4< T, P > yRot( 1.0f );
      T * m = glm::value_ptr( yRot );
      T s = dbSin( rad );
      T c = dbCos( rad );
      m[0] = c;
      m[2] = s;
      m[8] = -s;
      m[10] = c;
      return yRot;
   }

   /// @author Benjamin Hampe <benjaminhampe@gmx.de>
   /// @brief Create mat4
   // ----------------------------------------------------------------------------------------
   //       |  cos(A)  -sin(A)   0   0 |
   //   M = |  sin(A)   cos(A)   0   0 |
   //       |  0        0        1   0 |
   //       |  0        0        0   1 |
   // ----------------------------------------------------------------------------------------
   template < typename T, glm::precision P = glm::defaultp >
   static glm::tmat4x4< T, P >
   buildZRotationMatrix( T rad )
   {
      glm::tmat4x4< T, P > zRot( T( 1.0 ) );
      T * m = glm::value_ptr( zRot );
      T s = dbSin( rad );
      T c = dbCos( rad );
      m[0] = c;
      m[1] = -s;
      m[4] = s;
      m[5] = c;
      return zRot;
   }

   /// @author Benjamin Hampe <benjaminhampe@gmx.de>
   /// @brief EulerAngles -> DirectionVector
   // EulerAngles(rad) -> DirectionVector on unit sphere
   template < typename T, glm::precision P = glm::defaultp >
   static glm::tvec3< T, P >
   computeDirFromEulerAngles( glm::tvec3< T, P > const & eulerRad )
   {
//    T const roll_s = 0.0
//    T const roll_c = 1.0
      T const pitch_s = dbSin( eulerRad.y );
      T const pitch_c = dbCos( eulerRad.y );
      T const yaw_s = dbSin( eulerRad.z );
      T const yaw_c = dbCos( eulerRad.z );
      return { pitch_s * yaw_c,
               pitch_s * yaw_s,
               pitch_c };

//      T const roll_s = dbSin( eulerRad.x ); // 0.0
//      T const roll_c = dbCos( eulerRad.x ); // 1.0
//      T const pitch_s = dbSin( eulerRad.y );
//      T const pitch_c = dbCos( eulerRad.y );
//      T const yaw_s = dbSin( eulerRad.z );
//      T const yaw_c = dbCos( eulerRad.z );
//      T const roll_c_pitch_s = roll_c * pitch_s;
//      return { roll_c_pitch_s * yaw_c + roll_s * yaw_s,
//               roll_c_pitch_s * yaw_s - roll_s * yaw_c,
//               roll_c * pitch_c };
   }

   /// @author Benjamin Hampe <benjaminhampe@gmx.de>
   /// @brief EulerAngles -> DirectionVector
   // EulerAngles(rad) -> DirectionVector on unit sphere
   template < typename T, glm::precision P = glm::defaultp >
   static glm::tvec3< T, P >
   computeDirFromEulerAngles( T radRoll, T radPitch, T radYaw )
   {
      return computeDirFromEulerAngles< T >( { radRoll, radPitch, radYaw } );
   }

   static void
   testDir2Euler()
   {
      DE_DEBUG("BEGIN TEST")
      glm::vec3 EulerAngles = glm::vec3( 0,0,0 );
      DE_DEBUG("END TEST")
   }

   /// @author Benjamin Hampe <benjaminhampe@gmx.de>
   /// @brief DirectionVector to EulerAngles
   /// @return A rotation vector containing the Y (pitch) and Z (yaw) rotations (in rad) that when applied to a
   /// direction vector(0,0,1) would make it point in the same direction as this vector.
   /// The X (roll) rotation is always 0, since two Euler rotations are sufficient to any given direction.
   template < typename T, glm::precision P = glm::defaultp >
   static glm::tvec3< T, P >
   computeEulerAnglesFromDir( glm::tvec3< T, P > const & dir )
   {
      glm::tvec3< T,P > d( glm::normalize( dir ) );
      T const Pi2 = T(Math::TWOPI64);
      T const PiH = T(Math::HALF_PI64);

      glm::tvec3< T,P > euler( 0,0,0 );
      euler.y = dbAtan2( d.x, d.z );
      if ( euler.y < 0.0f ) euler.y += Pi2;
      if ( euler.y >= Pi2 ) euler.y -= Pi2;
      euler.x = dbAtan2( std::sqrt( d.x*d.x + d.z*d.z ), d.y ) - PiH;
      if ( euler.x < T(0) ) euler.x += Pi2;
      if ( euler.x >= Pi2 ) euler.x -= Pi2;
      return euler;
   }
*/
