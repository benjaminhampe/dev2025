#pragma once
#include <de/gpu/GPU.h>
#include <optional>
#include <sstream>

namespace de {
namespace gpu {

class Shader;
class VideoDriver;

// ===========================================================================
struct PMaterialTex // ### **Optional Texture Arguments**
// ===========================================================================
{
	typedef float T;
	typedef glm::vec2 V2;
	typedef glm::vec3 V3;
    typedef glm::vec4 V4;
	Texture* tex;	// 8B - pointer x64
	T gamma; 		// 4B - default: 1.0; `-mm gamma? brigthness gain` 	– Modify texture brightness.
	T bumpScale; 	// 4B - default: 1.0; `-bm material factor/exponent 	- map_Bump scale multiplier | or map_Ks Shininess exponent.
	V3 o;			//12B - 3D offset = .z could be layer in Array2D or Image3D.
	V2 r; 			// 8B - 2D repeat
	V3 s;			//12B - 3D scale `-s x y z`– Texture scaling.
	// 3x 16B cachelines (no 3d repeat!)

    PMaterialTex();
    PMaterialTex( Texture* tex_ );
    void setTex( Texture* tex_ );
    V4 coords() const;
    V2 repeat() const;
    std::string str() const;
    std::wstring wstr() const;
};

// ===========================================================================
struct PMaterialFog
// ===========================================================================
{
    bool enabled = false;
    uint32_t color = 0xFFFFFFFF;
    uint32_t mode = 0;
    float linearStart = 0.0f;
    float linearEnd = 1.0f;
    float expDensity = 0.001f;
    std::string str() const;
    std::wstring wstr() const;
};

// ===========================================================================
// ** Definition of (.mtl) material files for (.obj) files. **
// These members are used in MtlObjShader.
// ===========================================================================
struct PMaterial // list of all known `.mtl` file parameters
// ===========================================================================
{
	typedef float T;
	typedef uint32_t C3;
	typedef uint32_t C4;
	typedef glm::vec3 V3;
	typedef glm::ivec3 I3;
	
	// ### **Material Color and Illumination**
    std::wstring name;	// - `newmtl [name]` – Defines a new material with the given name.

	// 	Illumination Properties that are turned on in the model Property Editor
	// 	0 Color on and Ambient off
	// 	1 Color on and Ambient on
	// 	2 Highlight on
	// 	3 Reflection on and Ray trace on
	// 	4 Transparency: Glass on //      Reflection: Ray trace on
	// 	5 Reflection: Fresnel on and Ray trace on
	// 	6 Transparency: Refraction on  //      Reflection: Fresnel off and Ray trace on
	// 	7 Transparency: Refraction on //      Reflection: Fresnel on and Ray trace on
	// 	8 Reflection on and Ray trace off
	// 	9 Transparency: Glass on //      Reflection: Ray trace off
	// 	10 Casts shadows onto invisible surfaces
	int illum; // = 1;	// - `illum [model]` – Illumination model (`0` to `10`, defining shading behavior).
	T d; // = 1.0; // - `d [value]` – Opacity = Dissolve factor (`0.0` = fully transparent, `1.0` = fully opaque).
	T Ni; // = 1.0; // - `Ni [value]` – Optical density (`0.001` to `10.0`, affects refraction).
    T Ns; // = 30.0; // - `Ns [value]` – Specular exponent (`0` to `1000`, controls shininess).
    T shininess_strength = 1.5;
    //T sharpness; // = 0; // Reflectivity - `sharpness [value]` – Sharpness of reflections (`0` to `1000`).

	V3 Kd; // = glm::dvec3( 1,1,1 ); 	// - `Kd r g b` – Diffuse reflectivity (RGB values between `0.0` and `1.0`).	
	V3 Ka; // = glm::dvec3( 0,0,0 );	// - `Ka r g b` – Ambient reflectivity (RGB values between `0.0` and `1.0`).
	V3 Ks; // = glm::dvec3( 1,1,1 );	// - `Ks r g b` – Specular reflectivity (RGB values between `0.0` and `1.0`).
	//V3 Tf; // - `Tf r g b` ( 1,1,1 ) – Transmission filter (RGB values between `0.0` and `1.0`).
    V3 Ke; //( 0,0,0,0 )
    V3 Kt; //( 1,1,1,1 ) // ?
    V3 Kr; //( 1,1,1,1 ) // ?

    PMaterialTex map_Kd; // - `map_Kd [file]` – Diffuse texture map RGB.
    PMaterialTex map_Bump; 	// CombiMap: NormalMapRGB + HeightMapA;
	// ParallaxMtl_Tex map_d; // - `map_d [file]` – Transparency texture map A.
	// - `map_Bump [file]` – Alternative bump map definition.
	// - `bump [file]` – Bump map texture.
	// ParallaxMtl_Tex map_Ks; // specularColor - `map_Ks [file]` – Specular texture map.
	// ParallaxMtl_Tex map_Ns; // shininessFactor - `map_Ns [file]` – Specular exponent texture map.
	// ParallaxMtl_Tex map_Ka; // ambientColor - `map_Ka [file]` – Ambient texture map.	
	// ParallaxMtl_Tex map_disp; // displacementMap - disp [file] – displacement map texture.
	// ParallaxMtl_Tex map_decal; // decalMap - decal [file] – Decal texture map.
	// ParallaxMtl_Tex map_refl; // reflectionMap - refl -type sphere [file] – Reflection map texture.

    State state;

    std::string str() const;
    std::wstring wstr() const;
};

#pragma pack( push )
#pragma pack( 1 )

// ===========================================================================
struct PVertex
// ===========================================================================
{
	typedef float T;
	typedef uint32_t C4;
    typedef glm::vec2 V2;
    typedef glm::vec3 V3;
	
    V3 pos;		// 12 Bytes
    V3 normal;	// 12 Bytes (24)
    V3 tangent;	// 12 Bytes (36)
    // V3 bitangent; // Computed in shaders.
    C4 color;		//  4 Bytes (40)
    V3 tex;    	// 12 Bytes (52)
	V3 tex2;    	// 12 Bytes (64)
	// sum = 64B = uses 4x (16B)CacheLines per Vertex.
public:
    PVertex();

    PVertex(T x, T y, T z,    // pos
            T nx, T ny, T nz, // normal
            T tx, T ty, T tz, // tangent
            C4 colorRGBA, 	  // color
            T u1, T v1, T w1, // texcoords
            T u2, T v2, T w2 );// texcoords2

    PVertex(T x, T y, T z,    // pos
            T nx, T ny, T nz, // normal
            T tx, T ty, T tz, // tangent
            C4 c=0xFFFFFFFF,  // color
            T u1=0, T v1=0, // texcoords
            T u2=0, T v2=0 ); // texcoords2

    PVertex(const V3 & p,	// pos
            const V3 & n,	// normal
            const V3 & t,	// tangent
            const C4 c, 	// color
            const V3 & uvw1,// texcoords
            const V3 & uvw2);// texcoords2
	
    PVertex(const V3 & p,	// pos
            const V3 & n,	// normal
            const V3 & t,	// tangent
            const C4 c, 	// color
            const V2 & uv1, // texcoords
            const V2 & uv2 = V2()); // texcoords2

    std::string str() const;
    std::wstring wstr() const;
};

#pragma pack( pop )

// ===========================================================================
struct PMeshBuffer // g
// ===========================================================================
{
    typedef float T;
	typedef uint32_t C4;
    typedef glm::vec2 V2;
	typedef glm::vec3 V3;
    typedef glm::vec4 V4;
    typedef glm::mat4 M4;
	
    Box3f m_bbox;           // CPU side
    PrimitiveType m_primType;    // GPU side: Triangles, Lines, Points, etc...
    bool 	 m_bNeedUpload; // GPU side update from CPU side
    bool 	 m_bNeedInstanceUpload;      // GPU side
    C4 m_vao;               // GPU side vao handle
    C4 m_vbo_vertices;      // GPU side vbo handle
    C4 m_vbo_indices;       // GPU side vbo handle
    C4 m_vbo_instanceMat;   // GPU side vbo handle

    PMaterial m_material; // GPU side texture handle

    // CPU side
    std::string m_name;     // CPU side
    std::vector<PVertex> m_vertices; // CPU side
    std::vector<C4> m_indices;  // CPU side
    std::vector<M4> m_instanceMat;  // CPU side

    PMeshBuffer();
    bool empty() const { return m_vertices.empty(); }
    void markForUpload() { m_bNeedUpload = true; }
    void upload();
    void uploadInstances();
    void draw();
    void recalculateBoundingBox();
    uint64_t computeByteConsumption() const;
    std::string computeByteConsumptionStr() const
    {
        return dbStrBytes( computeByteConsumption() ) ;
    }


    // BKS instance:
    //       ( Right )  BKS = UCS
    //       ( Up,   )      = user coord system reference frame
    //   + M4( Front )      = x,y,z unit axis vectors + pos vector.
    //       ( Pos   )
/*
    void clearInstances()
    {
        m_instanceMat.clear();
        m_instanceMat.emplace_back( 1.0f );
        m_bNeedUpload = true;
    }
  */
    void resetInstances( const M4& modelMat = M4( T(1) ) );

    // BKS instance:
    //       ( Right )  BKS = UCS
    //       ( Up,   )      = user coord system reference frame
    //   + M4( Front )      = x,y,z unit axis vectors + pos vector.
    //       ( Pos   )
    void addInstance( const M4& modelMat );
    void addVertex( const PVertex& v );
    void addIndex( const uint32_t i );
    //
    //     B
    //    / \
    //   /   \
    //  / -Y  \
    // A-------C
    //
    void addTriangle(V3 a, V3 b, V3 c,
                     V2 uvA, V2 uvB, V2 uvC );
    //
    // B-------C
    // |       |
    // |  -Y   |
    // |       |
    // A-------D
    //
    void addQuad(V3 a, V3 b, V3 c, V3 d, T u, T v );
    std::string str() const;
    std::wstring wstr() const;
    void translate( const V3& o );
    void scale( const V3& s );
    void transform( const M4& m );
};


// ===========================================================================
struct PMesh // -o
// ===========================================================================
{
	typedef float T;
	typedef uint32_t C4;
    typedef glm::vec2 V2;
    typedef glm::vec3 V3;
    typedef glm::vec4 V4;
    typedef glm::mat4 M4;
		
    Box3f m_bbox;
    std::string m_name;
    TRSd m_trs;
    std::vector<PMeshBuffer> m_limbs;

    void recalculateBoundingBox();
    uint64_t computeByteConsumption() const;
    std::string computeByteConsumptionStr() const
    {
        return dbStrBytes( computeByteConsumption() ) ;
    }

    std::string str() const;
    std::wstring wstr() const;
    void translate( const V3& o );
    void scale( const V3& s );
    void transform( const M4& m );
    void center( const V3& o = V3() );
};

} // namespace gpu.
} // namespace de.

