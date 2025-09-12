#pragma once
#include <de_gpu/de_IShaderManager.h>
#include <de_os/de_CreateParams.h>
#include <de_image/de_Align.h>
#include <de_image/de_Font5x8.h>

namespace de {

// ////////////////////////////////////
struct IVideoDriver : public IShaderManager
// ////////////////////////////////////
{
   virtual ~IVideoDriver() = default;

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
   //    IShaderManager:
   // #####################

   virtual void clearShaders() = 0;
   virtual bool useShader( IShader* shader ) = 0;
   virtual IShader* getShader() const = 0;
   virtual IShader* getShader( std::string const & name ) const = 0;
   virtual void addShader( IShader* shader ) = 0;
   virtual IShader* createShader(
      std::string const & name,
      std::string const & vsSourceText,
      std::string const & fsSourceText,
      bool debug = false ) = 0;
   virtual IShader* createShaderFromFile(
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
};

IVideoDriver* createVideoDriverOpenGL( CreateParams const & params );

} // end namespace GL.
