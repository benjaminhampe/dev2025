#pragma once
#include <de/gpu/State.h>

namespace de {
namespace gpu {
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
std::string TextureFormat_str(uint32_t internalFormat);

// ===========================================================================
class ITexture
// ===========================================================================
{
public:
    virtual ~ITexture() {}

    virtual u32 id() const = 0;
    virtual u32 target() const = 0;
    virtual s32 unit() const = 0;
    virtual u32 mipLevels() const = 0;
    virtual u32 w() const = 0;
    virtual u32 h() const = 0;
    virtual u32 layers() const = 0;
    virtual u32 gpuFormat() const = 0;
    virtual u32 cpuFormat() const = 0;
    virtual u32 dataType() const = 0;
    virtual const SamplerOptions & so() const = 0;
    virtual SamplerOptions & so() = 0;
    virtual PixelFormat pixelFormat() const = 0;
    virtual std::string pixelFormatStr() const = 0;
    virtual const std::string& name() const = 0;
    virtual std::string str() const = 0;

    virtual void set_id( u32 id ) = 0;
    virtual void set_target( u32 target ) = 0;
    virtual void set_unit( s32 unit ) = 0;
    virtual void set_mipLevels( u32 mipLevels ) = 0;
    virtual void set_w( u32 w ) = 0;
    virtual void set_h( u32 h ) = 0;
    virtual void set_layers( u32 layers ) = 0;
    virtual void set_gpuFormat( u32 gpuFormat ) = 0;
    virtual void set_cpuFormat( u32 cpuFormat ) = 0;
    virtual void set_dataType( u32 dataType ) = 0;
    virtual void set_so( const SamplerOptions & so ) = 0;
    virtual void set_pixelFormat( const PixelFormat & fmt ) = 0;
    virtual void set_name(const std::string& name) = 0;

    virtual bool empty() const = 0;
    virtual void destroy() = 0;

};

// ===========================================================================
class Texture : public ITexture
// ===========================================================================
{
public:
    ~Texture() override
    {
        if (m_id != 0)
        {
            DE_ERROR("Forgot to call destroy(), ",m_name)
            //destroy();
        }
    }

    u32 id() const override { return m_id; }
    u32 target() const override { return m_target; }
    s32 unit() const override { return m_unit; }
    u32 mipLevels() const override { return m_mipLevels; }
    u32 w() const override { return m_w; }
    u32 h() const override { return m_h; }
    u32 layers() const override { return m_layers; }
    u32 gpuFormat() const override { return m_gpuFormat; }
    u32 cpuFormat() const override { return m_cpuFormat; }
    u32 dataType() const override { return m_dataType; }
    const SamplerOptions & so() const override { return m_so; }
    SamplerOptions & so() override { return m_so; }
    PixelFormat pixelFormat() const override { return m_fmt; }
    std::string pixelFormatStr() const override { return PixelFormat::getString( m_fmt ); }
    const std::string& name() const override { return m_name; }
    std::string str() const override;

    void set_id( u32 id ) override { m_id = id; }
    void set_target( u32 target ) override { m_target = target; }
    void set_unit( s32 unit ) override { m_unit = unit; }
    void set_mipLevels( u32 mipLevels ) override { m_mipLevels = mipLevels; }
    void set_w( u32 w ) override { m_w = w; }
    void set_h( u32 h ) override { m_h = h; }
    void set_layers( u32 layers ) override { m_layers = layers; }
    void set_gpuFormat( u32 gpuFormat ) override { m_gpuFormat = gpuFormat; }
    void set_cpuFormat( u32 cpuFormat ) override { m_cpuFormat = cpuFormat; }
    void set_dataType( u32 dataType ) override { m_dataType = dataType; }
    void set_so( const SamplerOptions & so ) override { m_so = so; }
    void set_pixelFormat( const PixelFormat & fmt ) override { m_fmt = fmt; }
    void set_name(const std::string& name) override { m_name = name; }

    Texture();

    bool empty() const override { return m_id == 0; }
    void destroy() override;

    void set( u32 gpuFormat, u32 cpuFormat, u32 dataType)
    {
        m_gpuFormat = gpuFormat;
        m_cpuFormat = cpuFormat;
        m_dataType = dataType;
        // DE_BENNI("cpuFormat(",StringUtil::hex(cpuFormat),"), "
        //          "cpuFormat(",StringUtil::hex(cpuFormat),"), "
        //          "dataType(",StringUtil::hex(dataType),")")
    }

protected:
    u32 m_id;          // texture id.
    u32 m_target;      // texture target GL_TEXTURE_2D|GL_TEXTURE_2D_ARRAY.
    s32 m_unit;        // {-1} = unbound, {0 ... N-1} = bound to specific tex unit.
    u32 m_mipLevels;
    u32 m_w;           // texture width.
    u32 m_h;           // texture height.
    u32 m_layers;      // texture layer.
    u32 m_gpuFormat;   // GL_RED;
    u32 m_cpuFormat;   // GL_RGB;
    u32 m_dataType;    // GL_FLOAT|GL_UNSIGNED_BYTE|etc...;
    SamplerOptions m_so;
    PixelFormat m_fmt;  // PixelFormat
    std::string m_name;
};


// ===========================================================================
class Tex2DArray : public Texture
// ===========================================================================
{
public:
    // uint32_t id;
    // int32_t w;          // Same width for all images.
    // int32_t h;          // Same height for all images.
    // int32_t layers;
    // int32_t mipLevels;
    // uint32_t gpuFormat;
    // SamplerOptions so;

    Tex2DArray();
    //~Tex2DArray() override;

    // Verschiebe-Semantik (Move)
    // Tex2DArray(Tex2DArray&& other) noexcept;
    // Tex2DArray& operator=(Tex2DArray&& other) noexcept;
    Tex2DArray(Tex2DArray&& other) = delete;
    Tex2DArray& operator=(Tex2DArray&& other) = delete;
    Tex2DArray(const Tex2DArray&) = delete;
    Tex2DArray& operator=(const Tex2DArray&) = delete;

    bool create(const std::string& name, const std::vector<const Image*>& images, const SamplerOptions& so);

    /**
     * Erstellt das Textur-Array. Berechnet automatisch die maximal möglichen Mipmap-Stufen.
     */
    // static Tex2DArray
    // create(const std::string& name, const std::vector<const Image*>& images, const SamplerOptions& so);

    /**
     * Diese Funktion wird kurz vor dem Rendern aufgerufen.
     * Sie setzt die Filteroptionen und generiert die Mipmaps NUR DANN, wenn sie gebraucht werden.
     */
    // void applySamplerOptions(const SamplerOptions& options, uint32_t slot = 0);
};

// ===========================================================================
struct TexRef
// ===========================================================================
{
    Texture* tex;     // 8B, valid when tex->id != 0.
    u32 layer;   // 4B 3d tex layer
    u32 miplayer;// 4B 3d mipmap tex layer
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
    void reset(Texture* tex_, u32 layer_ = 0 );
    u32 w() const;
    u32 h() const;
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
    u32 m_numTexUnits;
    u32 m_maxTex2DSize;
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
    u32        getMaxTex2DSize() const { return m_maxTex2DSize; }
    u32        getNumTexUnits() const { return m_numTexUnits; }

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
    bool            uploadTexture2D( Texture* &tex, int w, int h, void const* const pixels, PixelFormat fmt);
    bool            uploadTexture2D( Texture* &tex, const Image& img)
    {
        return uploadTexture2D(tex,img.w(),img.h(),img.data(),img.pixelFormat());
    }


    Tex2DArray*
    createTex2DArray(
        const std::string& name,
        const std::vector<const Image*>& images,
        const SamplerOptions& so);

};

typedef std::vector<TexRef> TexRefs;


} // end namespace gpu.
} // end namespace de.
