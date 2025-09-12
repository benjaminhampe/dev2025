#pragma once
#include <de_os/de_CreateParams.h>
#include <de_image/de_Align.h>
#include <de_image/de_Font5x8.h>

#include <de_gpu/de_Shader.h>
#include <de_gpu/de_Tex.h>
#include <de_gpu/de_Camera.h>
#include <de_gpu/de_State.h>

namespace de {

// ////////////////////////////////////
struct VideoDriver
// ////////////////////////////////////
{
   virtual ~VideoDriver() = default;

   virtual bool open( CreateParams params ) = 0;
   virtual void close() = 0;

   virtual void beginRender( bool clearColor = true,
                             bool clearDepth = true,
                             bool clearStencil = true ) = 0;

   virtual void endRender() = 0;

   virtual int getScreenWidth() const = 0;
   virtual int getScreenHeight() const = 0;

   virtual double getFPS() const = 0;

   virtual uint64_t getFrameCount() const = 0;

   virtual double getTimeInSeconds() const = 0;

   virtual void resize( int w, int h ) = 0;

   virtual void setViewport( int x, int y, int w, int h ) = 0;

   // #####################
   //    ShaderManager:
   // #####################

   virtual void clearShaders() = 0;
   virtual bool useShader( Shader* shader ) = 0;
   virtual Shader* getShader() const = 0;
   virtual Shader* getShader( std::string const & name ) const = 0;
   virtual void addShader( Shader* shader ) = 0;
   virtual Shader* createShader(
      std::string const & name,
      std::string const & vsSourceText,
      std::string const & fsSourceText,
      bool debug = false ) = 0;
   virtual Shader* createShaderFromFile(
      std::string const & name,
      std::string const & vsUri,
      std::string const & fsUri,
      bool debug = false ) = 0;

   // #####################
   //    Font5x8Renderer2D:
   // #####################

   virtual void
   draw2DText( int x, int y, std::string const & msg,
               uint32_t color = 0xFFFFFFFF,
               Align align = Align::Default,
               Font5x8 const & font = Font5x8() ) = 0;

   // ##########################
   // ###   TexUnitManager   ###
   // ##########################

   virtual uint32_t  getUnitCount() const = 0;
   virtual int32_t   findUnit( uint32_t texId ) const = 0;

   virtual bool      bindTextureId( int stage, uint32_t texId ) = 0;
   virtual bool      bindTexture( int stage, Tex* tex ) = 0;

   // Benni's more intelligent calls for raw/higher level TexClass.
   // AutoSelect a free tex unit and return its index for use in setSamplerUniform().
   virtual int       bindTextureId( uint32_t texId ) = 0;
   virtual bool      unbindTextureId( uint32_t texId ) = 0;

   virtual int       bindTexture( Tex* tex ) = 0;
   virtual bool      unbindTexture( Tex* tex ) = 0;

   // ######################
   // ###   TexManager   ###
   // ######################

   //virtual Tex*    findTexture( std::string const & name ) const = 0;
   virtual bool      hasTexture( std::string const & name ) const = 0;
   virtual Tex*      getTexture( std::string const & name ) = 0;
   virtual Tex*      loadTexture( std::string const & name, TexOptions so = TexOptions() ) = 0;
   virtual Tex*      createTexture( std::string const & name, Image const & img, TexOptions so = TexOptions() ) = 0;

   virtual bool      uploadTexture( Tex* tex, Image const & src ) = 0;
   virtual void      updateTextures() = 0;
   virtual void      clearTextures() = 0;
   virtual void      removeTexture( std::string const & name ) = 0;
   virtual void      removeTexture( Tex* tex ) = 0;

   virtual uint32_t  getMaxTex2DSize() const;

   // #####################
   // StateManager
   // #####################
   virtual State const& getState() const = 0;
   virtual void         setState( State const & state ) = 0;

   // #####################
   // Matrices
   // #####################

   virtual glm::dmat4 const & getModelMatrix() const = 0;
   virtual void setModelMatrix( glm::dmat4 const & ) = 0;

   virtual Camera* getCamera() = 0;
   virtual void setCamera( Camera* ) = 0;
};

VideoDriver* createVideoDriverOpenGL( CreateParams const & params );

} // end namespace GL.
