#include <de_gpu/de_renderer_ColorLine3D.h>
#include <de_gpu/de_GL_debug_layer.h>
#include <de_gpu/de_FVF_GL.h>

namespace de {

ColorLine3DMeshBuffer::ColorLine3DMeshBuffer()
   : m_primType(PrimitiveType::Points)
   , m_vao(0)
   , m_vbo(0)
   , m_shouldUpload( false )
{}

ColorLine3DMeshBuffer::~ColorLine3DMeshBuffer()
{
   if ( m_vao )
   {
      std::ostringstream o;
      if ( m_debugName.size() > 0 ) o << "[" << m_debugName << "] ";
      o << "Forgot to call destroy()";
      DE_ERROR(o.str())
   }
}

void ColorLine3DMeshBuffer::destroy()
{
   if ( m_vao )
   {
      de_glDeleteVertexArrays(1, &m_vao);
      m_vao = 0;
   }

   if ( m_vbo )
   {
      de_glDeleteBuffers(1, &m_vbo);
      m_vbo = 0;
   }
}

size_t ColorLine3DMeshBuffer::computeMemoryConsumption() const
{
   size_t n = sizeof(*this);
   n += m_vertices.size() * sizeof( ColorLine3DVertex );
   return n;
}

void ColorLine3DMeshBuffer::clear()
{
   m_vertices.clear();
   //m_shouldUpload = true;
}

void ColorLine3DMeshBuffer::upload()
{
   if ( m_vertices.empty() )
   {
      m_shouldUpload = false;
      return; // Nothing to draw
   }

   if ( !m_vao )
   {
      de_glGenVertexArrays(1, &m_vao);
      m_shouldUpload = true;
   }

   if ( !m_vbo )
   {
      de_glGenBuffers(1, &m_vbo);
      m_shouldUpload = true;
   }

   if ( m_shouldUpload )
   {
      m_shouldUpload = false;

      size_t const vertexCount = m_vertices.size();
      size_t const vertexSize = sizeof(ColorLine3DVertex);
      size_t const vertexBytes = vertexCount * vertexSize;
      uint8_t* vertices = reinterpret_cast< uint8_t* >( m_vertices.data() );

      de_glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );

      de_glBindVertexArray(m_vao);
      de_glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
      de_glBufferData(GL_ARRAY_BUFFER, GLsizeiptr(vertexBytes), vertices, GL_STATIC_DRAW);

      // VertexAttribute[0] Position3D_XYZf
      de_glEnableVertexAttribArray( 0 );
      de_glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, vertexSize, reinterpret_cast<void*>(0) );

      // VertexAttribute[1] Color4D_RGBAub
      de_glEnableVertexAttribArray( 1 );
      de_glVertexAttribPointer( 1, 4, GL_UNSIGNED_BYTE, GL_FALSE, vertexSize, reinterpret_cast<void*>(12) );

      de_glBindVertexArray(0);

      de_glDisableVertexAttribArray( 0 );
      de_glDisableVertexAttribArray( 1 );
      de_glBindBuffer( GL_ARRAY_BUFFER, 0 );
      //glDisable(GL_TEXTURE_2D);      GL_VALIDATE

#ifdef USE_COLORLINE3D_DEBUG
      std::ostringstream o;
      if ( m_debugName.size() > 0 ) o << "[" << m_debugName << "] ";
      o << "Uploaded to GPU " << StringUtil::byteCount(computeMemoryConsumption());
      DE_DEBUG(o.str())
#endif
   }
}

void ColorLine3DMeshBuffer::render()
{
   upload();

   // glDisable(GL_CULL_FACE); //GL_VALIDATE
   // //glDisable(GL_TEXTURE_2D); GL_VALIDATE
   // glDisable(GL_DEPTH_TEST); //GL_VALIDATE

   // Draw call
   GLenum const primType = GL_PrimitiveType::toOpenGL(m_primType);
   GLint const elementStart = 0;
   GLint const elementCount = GLint(m_vertices.size());

   de_glBindVertexArray(m_vao);
   de_glDrawArrays( primType, elementStart, elementCount );
   de_glBindVertexArray(0);
   //  GL_VALIDATE
}

void
ColorLine3DMeshBuffer::addLine( glm::vec3 a, glm::vec3 b, uint32_t color_a, uint32_t color_b )
{
   m_primType = PrimitiveType::Lines;
   m_vertices.emplace_back( a, color_a );
   m_vertices.emplace_back( b, color_b );
   m_shouldUpload = true;
}

void
ColorLine3DMeshBuffer::addTriangle( glm::vec3 a, glm::vec3 b, glm::vec3 c,
             uint32_t color_a, uint32_t color_b, uint32_t color_c )
{
   m_primType = PrimitiveType::Triangles;
   m_vertices.emplace_back( a, color_a );
   m_vertices.emplace_back( b, color_b );
   m_vertices.emplace_back( c, color_c );
   m_shouldUpload = true;
}




ColorLine3DRenderer::ColorLine3DRenderer() {}
ColorLine3DRenderer::~ColorLine3DRenderer() {}

void
ColorLine3DRenderer::init( VideoDriver* driver )
{
   m_driver = driver;
}

void
ColorLine3DRenderer::destroy()
{
}

void
ColorLine3DRenderer::render( ColorLine3DMeshBuffer & meshBuffer, glm::mat4 const & mvp )
{
   if ( !m_driver )
   {
      return;
   }

   if ( !m_shader )
   {
      std::string vs = R"(
         //precision highp float;

         layout(location = 0) in vec3 a_pos;          // VertexAttrib[0] POSITION_XYZ
         layout(location = 1) in lowp vec4 a_color;   // VertexAttrib[1] COLOR_RGBA

         uniform mat4 u_mvp;                          // Uniform[0] ModelViewProjectionMatrix

         out vec4 v_color;                            // Varying[0] COLOR_RGBAf

         void main()
         {
            gl_Position = u_mvp * vec4( a_pos, 1.0 );
            v_color = clamp( vec4( a_color ) * (1.0 / 255.0), vec4( 0,0,0,0 ), vec4( 1,1,1,1 ) );
         }
      )";

      std::string fs = R"(
         //precision highp float;

         in vec4 v_color;                             // Varying[0] COLOR_RGBAf

         layout(location = 0) out vec4 color;         // gl_FragColor

         void main()
         {
            color = v_color;
            //color = clamp( color, vec4( 0,0,0,0 ), vec4( 1,1,1,1 ) );
         }
      )";

      m_shader = m_driver->createShader( "color3d", vs, fs );

      // std::cout << "CreateShader 'color3d' m_shader.ID = " << m_shader.ID << std::endl;
   }

   m_driver->useShader( m_shader );

   m_shader->setMat4f( "u_mvp", mvp );

   de_glDisable(GL_CULL_FACE); //GL_VALIDATE
   //glDisable(GL_TEXTURE_2D); GL_VALIDATE
   de_glEnable(GL_DEPTH_TEST); //GL_VALIDATE

   meshBuffer.render();
}

} // end namespace GL.
