#pragma once
#include <DarkImage.h>
#include <de/gpu/Camera.h>
#include <de/gpu/State.h>

namespace de {
namespace gpu {

/*
#pragma pack( push )
#pragma pack( 1 )

// ===========================================================================
struct S3DVertex // FVF_POSITION_XYZ | FVF_NORMAL_XYZ | FVF_RGBA | FVF_TEXCOORD0
// ===========================================================================
{
    glm::vec3 pos;
    glm::vec3 normal;
    uint32_t color;
    glm::vec2 tex;    // 12 + 12 + 4 + 8 = 36 Bytes if packed
public:
    S3DVertex()
        : pos(), normal( 0, 0, 1 ), color( 0xFFFF00FF ), tex()
    {}
    S3DVertex( float x, float y, float z, float nx, float ny, float nz, uint32_t crgba, float u, float v )
        : pos( x,y,z ), normal( nx,ny,nz ), color( crgba ), tex( u,v )
    {}
    S3DVertex( glm::vec3 const & p, glm::vec3 const & nrm, uint32_t crgba, glm::vec2 const & tex0 )
        : pos( p ), normal( nrm ), color( crgba ), tex( tex0 )
    {}

    void
    flipV()
    {
        tex.y = glm::clamp( 1.0f - tex.y, 0.f, 1.0f ); // Invert v for OpenGL
    }

    bool operator==( const S3DVertex& other ) const
    {
        return std::tie(other.pos,other.normal,other.color,other.tex) == std::tie(pos,normal,color,tex);
    }
};

#pragma pack( pop )
*/

inline uint32_t
glRGBA( uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255 )
{
    return ( uint32_t( a ) << 24 ) | ( uint32_t( b ) << 16 )
    | ( uint32_t( g ) << 8 ) | uint32_t( r );
}

inline std::string
GT_getShaderHeaderVersionString()
{
    return "#version 330 core\n";
}

void
GT_init();

std::string
GT_getShaderTypeStr( uint32_t shaderType );

uint32_t
GT_compileShader(
    const std::string& shaderName,
    uint32_t shaderType,
    const std::string& srcText);

uint32_t
GT_createShader(
    const std::string& shaderName,
    const std::string& vsText,
    const std::string& fsText );



// ===========================================================================
struct Texture
// ===========================================================================
{
    uint32_t m_id;     // texture id.
    uint32_t m_target; // texture target GL_TEXTURE_2D|.
    /*
    +-------------------------------+---------------------------------------+-------------------------------------------------------------------------------+
    |   glBindTexture(target,id)    |      glGetIntegerv(target, &id)       |                            Description                                        |
    +-------------------------------+---------------------------------------+-------------------------------------------------------------------------------+
    | GL_TEXTURE_1D (0x0DE0)        | GL_TEXTURE_BINDING_1D (0x8068)        | 1D textures, useful for gradients or lookup tables.                           |
    | GL_TEXTURE_2D (0x0DE1)        | GL_TEXTURE_BINDING_2D (0x8069)        | 2D textures, the most common type for images, patterns, etc.                  |
    | GL_TEXTURE_3D (0x806F)        | GL_TEXTURE_BINDING_3D (0x806A)        | 3D textures, typically used for volumetric data.                              |
    | GL_TEXTURE_1D_ARRAY (0x8C18)  | GL_TEXTURE_BINDING_1D_ARRAY (0x8C1C)  | Array of 1D textures for indexed rendering.                                   |
    | GL_TEXTURE_2D_ARRAY (0x8C1A)  | GL_TEXTURE_BINDING_2D_ARRAY (0x8C1D)  | Array of 2D textures, useful in complex rendering scenarios like shadow maps. |
    | GL_TEXTURE_RECTANGLE (0x84F5) | GL_TEXTURE_BINDING_RECTANGLE (0x84F6) | Non-power-of-two (NPOT) 2D textures, often used for framebuffers.             |
    | GL_TEXTURE_CUBE_MAP (0x8513)  | GL_TEXTURE_BINDING_CUBE_MAP (0x8514)  | Cube map textures, typically used for environment mapping.                    |
    | GL_TEXTURE_CUBE_MAP_ARRAY (0x9009) | GL_TEXTURE_BINDING_CUBE_MAP_ARRAY (0x900A) | Array of cube maps for advanced graphics effects.                   |
    | GL_TEXTURE_BUFFER (0x8C2A)    | GL_TEXTURE_BINDING_BUFFER (0x8C2A)    | Buffer textures, used for accessing buffer data in a shader.                  |
    | GL_TEXTURE_2D_MULTISAMPLE (0x9100) | GL_TEXTURE_BINDING_2D_MULTISAMPLE (0x9100)   | Multisample 2D textures for anti-aliasing.                        |
    | GL_TEXTURE_2D_MULTISAMPLE_ARRAY (0x9102) | GL_TEXTURE_BINDING_2D_MULTISAMPLE_ARRAY (0x9102) | Array of multisample 2D textures.                       |
    +-------------------------------+----------+----------------------------+---------------------+---------------------------------------------------------+
    */
    uint32_t m_w;      // texture width.
    uint32_t m_h;      // texture height.
    uint32_t m_layer;  // texture layer.
    int32_t  m_unit;   // {-1} = unbound, {0 ... N-1} = bound to specific tex unit.
    uint32_t m_preset; // internalFormat = GL_RED;
    uint32_t m_format; // intCategory = GL_RGB;
    uint32_t m_dataType;// intDataType = GL_FLOAT|GL_UNSIGNED_BYTE|etc...;
    SamplerOptions m_so;
    PixelFormat m_fmt;  // PixelFormat
    std::string m_name;

    Texture();

    void set( uint32_t preset, uint32_t format, uint32_t dataType);

    std::string str() const;

    const std::string& name() const { return m_name; }
    uint32_t id() const { return m_id; }
    uint32_t target() const { return m_target; }
    uint32_t w() const { return m_w; }
    uint32_t h() const { return m_h; }
    uint32_t layer() const { return m_layer; }
    int32_t unit() const { return m_unit; }
    PixelFormat pixelFormat() const { return m_fmt; }
    std::string pixelFormatStr() const { return PixelFormat::getString( m_fmt ); }
    const SamplerOptions & so() const { return m_so; }
    SamplerOptions & so() { return m_so; }
};

/*
uint32_t
GT_createTexHandle();

void
GT_deleteTexHandle( uint32_t & texId );

// returns valid stage (texunit) of bound texture on success.
// returns -2 on fail.
// if stage is -1, then no glActiveTexture is called
int
GT_bindTex2D( uint32_t texId, int stage = 0 );

void
GT_unbindTex2D( int stage = 0 );

void
GT_applyTex2DOptions( SamplerOptions const & so );

bool
GT_uploadTex2D( Texture & tex, int w, int h, void const* const pixels, PixelFormat fmt,
               SamplerOptions const & so = SamplerOptions() );

bool
GT_uploadTex2D( Texture & tex, Image const & img, SamplerOptions const & so = SamplerOptions() );

bool
GT_createTexture2D( Texture & tex, Image const & img, SamplerOptions const & so = SamplerOptions() );

bool
GT_loadTexture2D( Texture & tex, std::string const & uri, SamplerOptions const & so = SamplerOptions(), ImageLoadOptions const & lo = ImageLoadOptions() );
*/

// ===========================================================================
struct TexRef
// ===========================================================================
{
    Texture* tex;     // 8B, valid when tex->id != 0.
    uint32_t layer;   // 4B 3d tex layer
    uint32_t miplayer;// 4B 3d mipmap tex layer
    uint8_t dummy;    // 1B (reserved)
    PixelFormat fmt;  // 1B 1:1 Format support Image <-> Tex (ideally,desired).
    SamplerOptions so;// 2B ? really useful?
    float intensity;  // 4B e.g. float u_bumpMapHeightScaleFactor,
    float gamma;      // 4B
    Recti rect;       // 16B {x,y,w,h} = rect
    glm::vec4 coords; // 16B {x,y} = uv-offset, {z,w} = uv-size
    glm::vec2 repeat; // 8B  {x,y[,z]} = uvw-repeat
    std::string name; // Can differ from tex->name, for image atlas chunks
    glm::mat2 transform;
    TexRef();
    TexRef(Texture* tex_);
    TexRef(Texture* tex_, const Recti& pos_);
    TexRef(Texture* tex_, const Recti& pos_, const glm::vec2& repeat_);
    void reset(Texture* tex_, uint32_t layer_ = 0 );
    uint32_t w() const;
    uint32_t h() const;
    std::string str() const;
    bool empty() const;
    void setTransform( float x, float y, float w, float h );
    const glm::vec4& getTransform() const;
    static glm::vec4 computeCoords( Texture* tex, Recti _pos);
};

// ============================================================================
struct TexManager
// ============================================================================
{
    uint32_t m_numTexUnits;
    uint32_t m_maxTex2DSize;
    float m_maxAnisotropy;
    //std::unordered_map< std::string, TexRef > m_refs;
    std::vector< Texture* > m_texCache; // All big memory pages ( atlas textures )
    std::vector< Texture* > m_texTrash; // Deleted on next call to updateTextures().
    //std::unordered_map< std::string, TexRef > m_refs;
    //std::vector< TexRef > m_texUnits; // Physical avail texture Units

    TexManager();
    ~TexManager();

    void init();
    void deinit();
    uint32_t        getMaxTex2DSize() const { return m_maxTex2DSize; }
    uint32_t        getNumTexUnits() const { return m_numTexUnits; }

    void            dumpTextures() const;
    void            updateTextures();
    void            removeTextures();
    void            removeTexture( Texture* &tex );
    void            removeTexture( const std::string& name );
    Texture*        getTexture( const std::string& name ) const;
    Texture*        loadTexture2D( const std::string& name, const SamplerOptions& so = SamplerOptions(), const ImageLoadOptions& ilo = ImageLoadOptions() );
    //Texture*      createTexture2D( const std::string& name );
    Texture*        createTexture2D( const std::string& name, int w, int h, void const* const pixels, PixelFormat fmt, const SamplerOptions& so = SamplerOptions() );
    Texture*        createTexture2D( const std::string& name, const Image& img, const SamplerOptions& so = SamplerOptions() );

    void            applySamplerOptions(SamplerOptions &so);
    bool            uploadTexture2D( Texture* &tex, int w, int h, void const* const pixels, PixelFormat fmt, const SamplerOptions& so );
    bool            uploadTexture2D( Texture* &tex, const Image& img, const SamplerOptions& so = SamplerOptions() )
    {
        return uploadTexture2D(tex,img.w(),img.h(),img.data(),img.pixelFormat(), so);
    }
};

typedef std::vector<TexRef> TexRefs;

// ===========================================================================
struct Light
// ===========================================================================
{
    glm::vec3 pos = glm::vec3(0.f,500.f,0.f);
    glm::vec3 color = glm::vec3(1.f,1.f,.8f);
    int typ = 0;
    float maxdist = 1000.0f; // distance
    //glm::vec3 siz;   // world size of light source
    //glm::vec3 dir;
    //float power;      // distance
    //float radius = 500.0f;
    //glm::vec3 clipPos; // for Shader, computed by CPU

    //SMeshBuffer mesh;
};

typedef std::vector< Light > Lights;

} // end namespace gpu.
} // end namespace de.


/*
#pragma pack( push )
#pragma pack( 1 )

// ===========================================================================
struct ColorVertex3D // FVF_POSITION_XYZ | FVF_COLOR_RGBA
// ===========================================================================
{
    glm::vec3 pos;
    uint32_t color;

    ColorVertex3D()
        : pos()
        //, normal( 0, 0, 1 )
        , color( 0xFFFF00FF )
    //, tex()
    {}

    ColorVertex3D( float x, float y, float z, uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255 )
        : pos( x,y,z )
        //, normal( nx,ny,nz )
        , color( glRGBA(r,g,b,a) )
    //, tex( u,v )
    {}

    ColorVertex3D( float x, float y, float z, uint32_t crgba )
        : pos( x,y,z )
        //, normal( nx,ny,nz )
        , color( crgba )
    //, tex( u,v )
    {}
};

#pragma pack( pop )

// ===========================================================================
struct ColorVertex3D_MeshBuffer
// ===========================================================================
{
    PrimitiveType PrimType = PrimitiveType::Points;
    uint32_t VAO = 0;
    uint32_t VBO = 0;
    uint32_t IBO = 0;
    std::vector< ColorVertex3D > Vertices;
    std::vector< uint32_t > Indices;

    void destroy();
    void upload( bool bForceUpload = false );
    void draw();
};

// ===========================================================================
struct ColorVertex3D_Shader
// ===========================================================================
{
    uint32_t m_shaderId = 0;

    void init();
    void useShader( Camera* pCamera, bool bBlend );
    void deinit();
};
*/
