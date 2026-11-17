#pragma once
#include <de/gpu/Texture.h>

namespace de {
namespace gpu {

class VideoDriver;

// ===========================================================================
struct RT_Attachment
// ===========================================================================
{
    enum eAttachmentType
    {
        eAT_Color,
        eAT_Depth,
        eAT_Stencil,
        eAT_DepthStencil,
        eAT_Unknown
    };

    uint32_t view; // OpenGL 4.3+ glTextureView source.
    uint32_t attach; // GL_COLOR_ATTACHMENT0, GL_DEPTH_ATTACHMENT, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER
    PixelFormat fmt;
    Texture* tex;

    RT_Attachment()
        : attach(eAT_Color), fmt(PixelFormat::Unknown), tex(nullptr)
    {}

    RT_Attachment( PixelFormat _fmt, Texture* _tex )
        : attach(eAT_Color), fmt(_fmt), tex(_tex)
    {}

    RT_Attachment( Texture* _tex )
        : attach(eAT_Color), fmt(PixelFormat::Unknown), tex(_tex)
    {
        if (tex) { fmt = tex->pixelFormat(); }
    }
};

// ===========================================================================
struct IRenderTarget
// ===========================================================================
{
    virtual ~IRenderTarget() {}
    virtual const std::string& name() const = 0;
    virtual uint32_t fbo() const = 0;
    virtual void setFbo( uint32_t fbo ) = 0;
    // Viewport
    //virtual const Recti& pos() const = 0;
    // virtual int32_t x() const = 0;
    // virtual int32_t y() const = 0;
    virtual int32_t w() const = 0;
    virtual int32_t h() const = 0;
    virtual void init( VideoDriver* driver, int w, int h ) = 0;
    //virtual PixelFormat pixFormat() const = 0;
    //virtual Texture* tex() = 0;
    virtual void bind() = 0;
    virtual void unbind() = 0;
    virtual void clear( const glm::vec4& color ) = 0;
    virtual bool toImage(Image & img) = 0;

    virtual uint32_t colorAttachmentCount() const = 0;
    virtual uint32_t depthAttachmentCount() const = 0;
    virtual uint32_t stencilAttachmentCount() const = 0;

    virtual const RT_Attachment& colorAttachment( int i = 0 ) const = 0;
    virtual const RT_Attachment& depthAttachment( int i = 0 ) const = 0;
    virtual const RT_Attachment& stencilAttachment( int i = 0 ) const = 0;
};

// ===========================================================================
struct RT_RGB : public IRenderTarget
// ===========================================================================
{
    VideoDriver* m_driver;
    uint32_t m_fbo;
    uint32_t m_depthView;
    uint32_t m_stencilView;

    //Recti m_pos;
    int32_t m_w;
    int32_t m_h;
    RT_Attachment m_color; // DiffuseColor RGBA 32 or RGBA_128
    //RT_Attachment m_depthStencil; // DepthStencil D24_S8
    RT_Attachment m_depth; // D24
    RT_Attachment m_stencil; // Stencil S8
    std::string m_name;

    RT_RGB();
    ~RT_RGB() override;
    const std::string& name() const override { return m_name; }
    uint32_t fbo() const override { return m_fbo; }
    void setFbo( uint32_t fbo ) override { m_fbo = fbo; }

    void init( VideoDriver* driver, int w, int h ) override;
    void bind() override;
    void unbind() override;
    void clear( const glm::vec4& color ) override;
    bool toImage(Image & img) override;

    //const Recti& pos() const override { return m_pos; }
    int32_t w() const override { return m_w; }
    int32_t h() const override { return m_h; }

    uint32_t colorAttachmentCount() const override { return 1; }
    uint32_t depthAttachmentCount() const override { return 1; }
    uint32_t stencilAttachmentCount() const override { return 1; }

    const RT_Attachment& colorAttachment( int i = 0 ) const override { return m_color; }
    const RT_Attachment& depthAttachment( int i = 0 ) const override { return m_depth; }
    const RT_Attachment& stencilAttachment( int i = 0 ) const override { return m_stencil; }
    // const RT_Attachment& depthAttachment( int i = 0 ) const override { return m_depthStencil; }
    // const RT_Attachment& stencilAttachment( int i = 0 ) const override { return m_depthStencil; }
};

// ===========================================================================
struct RT_HDR : public IRenderTarget
// ===========================================================================
{
    VideoDriver* m_driver;
    uint32_t m_fbo;
    //Recti m_pos;
    int32_t m_w;
    int32_t m_h;
    RT_Attachment m_color; // GL_COLOR_ATTACHMENT0 GL_TEXTURE_2D GL_RGBA16F GL_RGBA GL_FLOAT
                           // GL_COLOR_ATTACHMENT0 GL_TEXTURE_2D GL_RGBA32F GL_RGBA GL_FLOAT
    RT_Attachment m_depth; // GL_DEPTH_ATTACHMENT GL_TEXTURE_2D GL_DEPTH_COMPONENT32 GL_DEPTH_COMPONENT GL_FLOAT;
    RT_Attachment m_stencil; // GL_STENCIL_ATTACHMENT GL_TEXTURE_2D GL_STENCIL_INDEX8 GL_STENCIL_INDEX GL_UNSIGNED_BYTE;
    std::string m_name;

    RT_HDR();
    ~RT_HDR() override;
    const std::string& name() const override { return m_name; }
    uint32_t fbo() const override { return m_fbo; }
    void setFbo( uint32_t fbo ) override { m_fbo = fbo; }

    void init( VideoDriver* driver, int w, int h ) override;
    void bind() override;
    void unbind() override;
    void clear( const glm::vec4& color ) override;
    bool toImage(Image & img) override;

    //const Recti& pos() const override { return m_pos; }
    int32_t w() const override { return m_w; }
    int32_t h() const override { return m_h; }

    uint32_t colorAttachmentCount() const override { return 1; }
    uint32_t depthAttachmentCount() const override { return 1; }
    uint32_t stencilAttachmentCount() const override { return 1; }

    const RT_Attachment& colorAttachment( int i = 0 ) const override { return m_color; }
    const RT_Attachment& depthAttachment( int i = 0 ) const override { return m_depth; }
    const RT_Attachment& stencilAttachment( int i = 0 ) const override { return m_stencil; }
};

} // end namespace gpu.
} // end namespace de.
