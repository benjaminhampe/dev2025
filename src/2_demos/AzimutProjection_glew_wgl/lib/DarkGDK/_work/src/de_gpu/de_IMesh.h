#pragma once
#include <de_gpu/de_IMeshBuffer.h>
#include <de_image/de_Rectf.h>

namespace de {

// ===========================================================================
struct IMesh
// ===========================================================================
{
   virtual ~IMesh() = default;
   virtual void clear() = 0;
   virtual void upload() = 0;
   virtual glm::dmat4 const & getModelMatrix() const = 0;
   virtual void setModelMatrix( glm::dmat4 const & modelMat ) = 0;
   virtual std::string getName() const = 0;
   virtual void setName( std::string const & name ) = 0;
   virtual bool isVisible() const = 0;
   virtual void setVisible( bool visible ) = 0;
   virtual Box3f const & getBoundingBox() const = 0;
   virtual void setBoundingBox( Box3f const & box ) = 0;
   virtual int getMaterialCount() const = 0;
   virtual SMaterial const & getMaterial( int i ) const = 0;
   virtual SMaterial & getMaterial( int i ) = 0;
   virtual std::string getPrimTypesStr() const = 0;
};

} // end namespace de.
