#pragma once
#include <cstdint>
#include <sstream>
#include <de_glm.hpp>

// #include <fstream>
// #include <iostream>

namespace de {

// ////////////////////////////
struct Shader
// ////////////////////////////
{
   virtual ~Shader() = default;

   virtual void destroy() = 0;

   virtual uint32_t getProgramID() const = 0;

   virtual std::string getName() const = 0;

   virtual void setVec1b(std::string const & name, bool value) const = 0;
   virtual void setVec1i(std::string const & name, int value) const = 0;
   virtual void setVec1f(std::string const & name, float value) const = 0;
   virtual void setVec2f(std::string const & name, glm::vec2 const & value ) const = 0;
   virtual void setVec3f(std::string const & name, glm::vec3 const & value ) const = 0;
   virtual void setVec4f(std::string const & name, glm::vec4 const & value ) const = 0;
   virtual void setMat2f(std::string const & name, glm::mat2 const & mat) const = 0;
   virtual void setMat3f(std::string const & name, glm::mat3 const & mat) const = 0;
   virtual void setMat4f(std::string const & name, glm::mat4 const & mat) const = 0;

   inline void setVec2f(std::string const & name, float x, float y) const
   {
      setVec2f( name, glm::vec2(x,y) );
   }
   inline void setVec3f(std::string const & name, float x, float y, float z) const
   {
      setVec3f( name, glm::vec3(x,y,z) );
   }
   inline void setVec4f(std::string const & name, float x, float y, float z, float w) const
   {
      setVec4f( name, glm::vec4(x,y,z,w) );
   }
};

/*
// Load text file
// ////////////////////////////
inline std::string
// ////////////////////////////
loadString( std::string uri )
{
   std::string content;
   std::ifstream file;
   file.exceptions( std::ifstream::failbit | std::ifstream::badbit );
   try
   {
      file.open(uri);
      std::ostringstream s;
      s << file.rdbuf();
      file.close();
      content = s.str();
   }
   catch (std::ifstream::failure e)
   {
      std::cout << "ERROR: loadTextFile() :: " << uri << std::endl;
   }
   return content;
}
*/

} // end namespace GL.
