#pragma once
#include "IMeshBuffer.hpp"
#include <de/Rectf.hpp>

namespace de {
namespace gpu {

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
   virtual Material const & getMaterial( int i ) const = 0;
   virtual Material & getMaterial( size_t i ) = 0;
   virtual std::string getPrimTypesStr() const = 0;
};

} // end namespace gpu.
} // end namespace de.
