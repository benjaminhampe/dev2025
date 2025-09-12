#include <de_gpu/de_renderer_SMaterial.h>
#include <de_gpu/de_GL_debug_layer.h>
#include <de_core/de_PerformanceTimer.h>
#include <de_core/de_RuntimeError.h>

//#include <de/gpu/smesh/Rect.hpp>
//#include <de/gpu/smesh/RoundRect.hpp>
//#include <de/gpu/smesh/Circle.hpp>
//#include <de/gpu/smesh/Ring.hpp>
//#include <de/gpu/smesh/Sphere.hpp>

namespace de {

SMaterialRenderer::SMaterialRenderer()
   : m_Driver( nullptr )
   , m_ScreenGamma( 1.25f )
{
   addLight( 1,
             glm::vec3(10,10,10),
             glm::vec3( 0, 500, -200 ),
             1000.0f,
             glm::vec3(),
             0xFFEEEEFF ); // Rot


   addLight( 1,
             glm::vec3(10,10,10),
             glm::vec3( -1000, 1000, 11000 ),
             2000.0f,
             glm::vec3(),
             0xFFEEFFFF ); // Gelb


   addLight( 1,
             glm::vec3(100,100,100),
             glm::vec3( 2000, 2000, 1000 ),
             5000.0f,
             glm::vec3(),
             0xFFFFEEEE ); // Blau

   addLight( 1,
             glm::vec3(10,10,10),
             glm::vec3( 0, -2000, -3000 ),
             5000.0f,
             glm::vec3(),
             0xFFEEFFEE ); // Gruen

   m_AnimTime.start = HighResolutionClock::GetTimeInSeconds();
   m_AnimTime.last = m_AnimTime.start;
   m_AnimTime.curr = m_AnimTime.start;
}

SMaterialRenderer::~SMaterialRenderer()
{
}

//bool
//SMaterialRenderer::setMVP( Shader* shader, glm::dmat4 const & mvp )
//{
//   //if ( !m_Driver ) { DE_ERROR("No driver.") return false; }
//   if ( !shader ) { DE_ERROR("No shader.") return false; }
//   shader->setUniformM4f( "u_mvp", mvp );
//   return true;
//}

bool
SMaterialRenderer::setMaterial( SMaterial const & material )
{
   if ( !m_Driver ) { DE_ERROR("No driver.") return false; }

   // [UseProgram]
   Shader* shader = getShader( material );
   if ( !shader )
   {
      DE_ERROR("No shader in use for material(",material.toString(),")")
      return false;
   }

   // [MVP] Common to all shaders
   auto modelMatrix = m_Driver->getModelMatrix();
   auto vp = glm::dmat4( 1.0 );
   auto camera = m_Driver->getCamera();
   if ( camera )
   {
      vp = camera->getViewProjectionMatrix();
      shader->setMat4f( "u_mvp", vp * modelMatrix );
   }
   else
   {
      shader->setMat4f( "u_mvp", modelMatrix );
   }

   // [Gamma] Common to all shaders
   shader->setVec1f( "u_screenGamma", m_ScreenGamma );

   // [Fog]
   if ( material.FogEnable )
   {
      auto camera = m_Driver->getCamera();
      if ( camera )
      {
         shader->setVec3f( "u_camera_pos", camera->getPos() );
      }
   }

   // [Lighting]
   if ( material.Lighting > 0 )
   {
//      if ( material.Lighting > 1 )
//      {
//         shader->setUniformf( "u_shininess", material.Shininess );
//      }

      if ( material.Lighting == 3 )
      {
         for ( size_t i = 0; i < m_Lights.size(); ++i )
         {
            std::string l1 = std::string( "u_light_pos" ) + std::to_string( i );
            std::string l2 = std::string( "u_light_color" ) + std::to_string( i );
            std::string l3 = std::string( "u_light_power" ) + std::to_string( i );

            glm::vec4 clipRaw = glm::vec4( vp * glm::dvec4( glm::dvec3( m_Lights[i].pos ), 1.0 ) );
            glm::vec3 clipPos = glm::vec3( clipRaw ) / clipRaw.w;
            m_Lights[i].clipPos = clipPos;

            shader->setVec3f( l1, m_Lights[i].clipPos );
            shader->setVec3f( l2, m_Lights[i].color );
            shader->setVec1f( l3, m_Lights[i].maxdist );
         }
      }
   }

   // [TexDiffuse]
   TexRef texDiffuse = material.getDiffuseMap();
   if ( texDiffuse.getTexture() )
   {
      int stage = m_Driver->bindTexture( texDiffuse.getTexture() );
      if ( stage < 0 )
      {
         DE_ERROR("texDiffuse not activated")
         return false;
      }

      shader->setVec1i( "u_texDiffuse", stage );
      //shader->setVec2f( "u_texDiffuseSize", texDiffuse.getTexSizef() );
      shader->setVec2f( "u_texDiffuseRepeat", texDiffuse.m_repeat );
      shader->setVec4f( "u_texDiffuseTransform", texDiffuse.getTexTransform() );
   }

   // [TexBump]
   TexRef texBump = material.getBumpMap();
   if ( texBump.getTexture() )
   {
      int stage = m_Driver->bindTexture( texBump.getTexture() );
      if ( stage < 0 )
      {
         DE_ERROR("texBump not activated")
         return false;
      }
      shader->setVec1i( "u_texBump", stage );
      shader->setVec1f( "u_texBumpScale", material.BumpScale );
      shader->setVec2f( "u_texBumpRepeat", texBump.m_repeat );
      shader->setVec4f( "u_texBumpTransform", texBump.getTexTransform() );
   }

   // [State] Cull, Depth, Stencil, Blend, etc...
   State state = material.state;
   if ( material.CloudTransparent )
   {
      state.blend = Blend::alphaBlend();
   }
   m_Driver->setState( state );

   m_Material = material;
   return true;
}


void
SMaterialRenderer::unsetMaterial( SMaterial const & material )
{
   (void)material;
//   for ( size_t i = 0; i < material.getTextureCount(); ++i )
//   {
//      Tex* tex = material.getTexture( i );
//      if ( tex )
//      {
//         m_Driver->deactivateTexture( tex );
//      }
//   }
}

void
SMaterialRenderer::beginFrame()
{
   animate( HighResolutionClock::GetTimeInSeconds() );

   // Geometry stats

}
void
SMaterialRenderer::endFrame()
{
#ifdef USE_RENDER_STATS
   m_TotalFrames++;
   m_TotalBuffers += m_FrameBuffers;
   m_TotalVertices += m_FrameVertices;
   m_TotalIndices += m_FrameIndices;
   m_TotalPrimitives += m_FramePrimitives;
   m_FrameBuffers = 0;
   m_FrameVertices = 0;
   m_FrameIndices = 0;
   m_FramePrimitives = 0;
#endif
}

void
SMaterialRenderer::animate( double pts )
{
   m_AnimTime.curr = pts;
   double dt = m_AnimTime.curr - m_AnimTime.last;

   double speed_y = 0.001;
   //double speed_x = 0.01f;

   // [Light0]
   glm::vec3 center(0,0,0);
   glm::vec3 pos = m_Lights[0].pos;
   glm::vec3 dir = pos-center;
   float radius = glm::length( dir );

   dir = Math::rotateVectorY( dir, float(speed_y * dt) );

   m_Lights[0].pos = center + dir * radius;

   m_AnimTime.last = m_AnimTime.curr;
}




Shader*
SMaterialRenderer::getShader( SMaterial const & material )
{
   if ( !m_Driver ) { DE_ERROR("No driver.") return nullptr; }

   uint32_t id = createShaderId( material );

   Shader* shader = nullptr;

   auto it = m_ShaderLut.find( id );
   if ( it != m_ShaderLut.end() )
   {
      shader = (*it).second;
   }
   else
   {
      // Create by expensive name
      auto name = createShaderName( material );
      shader = m_Driver->getShader( name );
      if ( !shader )
      {
         shader = m_Driver->createShader( name, createVS( material ), createFS( material ), true );
      }

      if ( !shader )
      {
         DE_ERROR("No shader built(",name,")")
         return nullptr;
      }

      m_ShaderLut[ id ] = shader; // Store new and valid shader
   }

   if ( !shader )
   {
      DE_RUNTIME_ERROR(std::to_string(id),"No shader with this Id");
   }

   // [UseProgram]
   if ( !m_Driver->useShader( shader ) )
   {
      DE_ERROR("No active shader(",shader->getName(),")")
      return nullptr;
   }

   return shader;
}


// static
uint32_t
SMaterialRenderer::createShaderId( SMaterial const & material ) const
{
   uint32_t id = 0x00;
   if ( material.Lighting > 0 ) id = uint32_t( material.Lighting & 0x3F );
   if ( material.FogEnable ) id |= 0x40;
   //if ( material.hasAmbientColor() ) id |= 0x04;

   if ( material.hasDiffuseMap() ) id |= 0x100;
   if ( material.hasBumpMap() ) id |= 0x200;
   if ( material.hasNormalMap() ) id |= 0x300;

   if ( material.CloudTransparent ) id |= 0x1000;
   return id;
}

// static
std::string
SMaterialRenderer::createShaderName( SMaterial const & material ) const
{
   std::ostringstream s;
   s << "s3d";
   //if ( material.hasAmbientMap() ) s << "+Ta";
   if ( material.CloudTransparent ) s << "+ct";
   if ( material.hasDiffuseMap() ) s << "+Td";
   if ( material.hasBumpMap() ) s << "+Tb";
   if ( material.hasNormalMap() ) s << "+Tn";
   if ( material.Lighting > 0 ) s << "+illum" << material.Lighting;// ;m_Lights.size();
   if ( material.FogEnable ) s << "+fog";
   return s.str();
}

std::string
SMaterialRenderer::createVS( SMaterial const & material ) const
{
   // Vertex shader depends only on S3DVertex program ( yet ).
   static_cast< void >( material );

   std::ostringstream vs;
   vs << R"(
      precision highp float;

      in vec3 a_pos;
      in vec3 a_normal;
      in lowp vec4 a_color;
      in vec2 a_tex;

      uniform mat4 u_mvp;
   )";

   if ( material.hasBumpMap() )
   {
      vs << R"(
      uniform float u_texBumpScale;
      uniform vec2 u_texBumpRepeat;
      uniform sampler2D u_texBump;
      uniform vec4 u_texBumpTransform;
      )";
   }

   if ( material.Lighting > 0 )
   {
      for ( size_t i = 0; i < m_Lights.size(); ++i )
      {
         vs << "uniform vec3 u_light_pos" << i << ";\n";
      }
   }

   vs << R"(
      out vec3 v_pos;
      out vec3 v_normal;
      out vec4 v_color;
      out vec2 v_tex;
   )";

   if ( material.Lighting == 3 )
   {
      for ( size_t i = 0; i < m_Lights.size(); ++i )
      {
         vs << "out vec3 v_light_pos" << i << ";\n";
         vs << "out float v_light_dist" << i << ";\n";
      }
   }

   vs << R"(
      void main()
      {
         gl_PointSize = 5.0;
   )";

   if ( material.hasBumpMap() )
   {
      vs << R"(
            vec2 transTb = u_texBumpTransform.xy;
            vec2 scaleTb = u_texBumpTransform.zw;
            float h = texture( u_texBump, scaleTb * fract( u_texBumpRepeat * a_tex ) + transTb ).r;
            vec3 pos = a_pos + (a_normal * h * u_texBumpScale);
      )";
   }
   else
   {
      vs << R"(
            vec3 pos = a_pos;
      )";
   }

   vs << R"(
         gl_Position = u_mvp * vec4( pos, 1.0 );
   )";

   if ( material.Lighting == 3 )
   {
      for ( size_t i = 0; i < m_Lights.size(); ++i )
      {
         vs << "v_light_pos" << i << " = vec3( u_mvp * vec4( u_light_pos" << i << ", 1.0 ) );\n";
         vs << "v_light_dist" << i << " = length( pos - u_light_pos" << i << ");\n";
      }
   }


   vs << R"(
         v_pos = a_pos;
         v_normal = a_normal;
         v_color = clamp( vec4( a_color ) * (1.0 / 255.0), vec4( 0,0,0,0 ), vec4( 1,1,1,1 ) );
         v_tex = a_tex;
      }
   )";

   return vs.str();
}


std::string
SMaterialRenderer::createFS( SMaterial const & material ) const
{
   std::ostringstream fs;
   fs << R"(
      precision highp float;

      in vec3 v_pos;
      in vec3 v_normal;
      in vec4 v_color;
      in vec2 v_tex;

      out vec4 color;

      uniform float u_screenGamma;
   )";


   if ( material.Lighting == 3 )
   {
      for ( size_t i = 0; i < m_Lights.size(); ++i )
      {
         fs << "in vec3 v_light_pos" << i << ";\n";
         fs << "in float v_light_dist" << i << ";\n";
      }
   }

   if ( material.hasDiffuseMap() )
   {
      fs << R"(
      uniform sampler2D u_texDiffuse;
      //uniform vec2 u_texDiffuseSize;
      uniform vec2 u_texDiffuseRepeat;    // All mesh coords are in range [0,1], always.
      uniform vec4 u_texDiffuseTransform;
      )";
   }

   if ( material.FogEnable )
   {
      fs << R"(
      uniform vec3 u_camera_pos;
      )";
   }

//   if ( material.Lighting > 1 )
//   {
//      fs << R"(
//      uniform float u_shininess;
//      )";
//   }

   if ( material.Lighting == 3 )
   {
      for ( size_t i = 0; i < m_Lights.size(); ++i )
      {
         fs << "uniform vec3 u_light_color" << i << ";\n";
         fs << "uniform float u_light_power" << i << ";\n";
      }
   }

   fs << R"(
      void main()
      {
         //const float u_screenGamma = 1.2; // Assume the monitor is calibrated to the sRGB color space
         vec4 diffuseColor = v_color;
   )";

   if ( material.hasDiffuseMap() )
   {
      fs << R"(
         //vec2 bias = vec2(0.5,0.5) / u_texDiffuseSize;
         //vec2 biasEnd = vec2(1.0,1.0) - bias;
         //vec2 biasDelta = vec2(1.0,1.0) - (bias*4.0);
         vec2 transTd = u_texDiffuseTransform.xy;
         vec2 scaleTd = u_texDiffuseTransform.zw; // * biasDelta;
         vec2 texCoords = scaleTd * fract( u_texDiffuseRepeat * v_tex ) + transTd;
         vec4 colorTd = texture( u_texDiffuse, texCoords );
      )";
      if ( material.CloudTransparent )
      {
         fs << R"(
         float red_channel = colorTd.r;
         diffuseColor *= vec4(red_channel, red_channel, red_channel, red_channel);
         )";
      }
      else
      {
         fs << R"(
         diffuseColor *= colorTd;
         )";
      }
   }

   if ( material.Lighting == 0 )
   {
      fs << R"(
         vec3 colorLinear = diffuseColor.rgb;
      )";
   }
   else if ( material.Lighting == 1 )
   {
      fs << R"(
         const vec3 u_Ka = vec3(0.033, 0.033, 0.033);

         vec3 lightDir1 = normalize( vec3(0.6,1,-0.8) );
         vec3 lightDir2 = normalize( vec3(1,0,0) );
         vec3 lightDir3 = normalize( vec3(-1,-1,-.2) );
         vec3 lightDir4 = normalize( vec3(1,-1,-1) );
         vec3 lightDir5 = normalize( vec3(0.7,1,2) );

         vec3 normal = normalize( v_normal );
         float lambert1 = max( dot(lightDir1, normal), 0.0 );
         float lambert2 = max( dot(lightDir2, normal), 0.0 );
         float lambert3 = max( dot(lightDir3, normal), 0.0 );
         float lambert4 = max( dot(lightDir4, normal), 0.0 );
         float lambert5 = max( dot(lightDir5, normal), 0.0 );

         vec3 colorLinear = u_Ka
                      + diffuseColor.rgb * 0.3*( lambert1 + lambert2 + lambert3 + lambert4 + lambert5 );
      )";
   }
   else if ( material.Lighting == 2 )
   {
      fs << R"(
         const vec3 u_Ka = vec3(0.033, 0.033, 0.033);
         const vec3 u_Ks = vec3(1.0, 1.0, 1.0);
         float shininess = 128.0; //clamp( u_shininess, 0.1, 10000.0 );
         vec3 colorLinear = u_Ka;

         vec3 normal = normalize( v_normal );

         vec3 lightColor1 = vec3(1,1,1);
         vec3 lightColor2 = vec3(1,0.9,0.9);
         vec3 lightColor3 = vec3(1,1,0.9);
         vec3 lightColor4 = vec3(1,1,1);

         vec3 lightDir1 = normalize( vec3(0,-1,0) );
         vec3 lightDir2 = normalize( vec3(1,1,1) );
         vec3 lightDir3 = normalize( vec3(1,0,2) );
         vec3 lightDir4 = normalize( vec3(1,0,-2) );

         float lambert1 = max( dot(lightDir1, normal), 0.0 );
         float lambert2 = max( dot(lightDir2, normal), 0.0 );
         float lambert3 = max( dot(lightDir3, normal), 0.0 );
         float lambert4 = max( dot(lightDir4, normal), 0.0 );

         // blinn phong
         vec3 viewDir = normalize(v_pos);
         if (lambert1 > 0.0)
         {
            vec3 halfDir = normalize(lightDir1 + viewDir);
            float angle1 = max(dot(halfDir, normal), 0.0);
            float spec1 = pow( angle1, shininess );
            colorLinear += (diffuseColor.rgb * lambert1 + 0.125 * u_Ks * spec1)*lightColor1;
         }
         if (lambert2 > 0.0)
         {
            vec3 halfDir = normalize(lightDir2 + viewDir);
            float angle2 = max(dot(halfDir, normal), 0.0);
            float spec2 = pow( angle2, shininess );
            colorLinear += (diffuseColor.rgb * lambert2 + 0.125 * u_Ks * spec2)*lightColor2;
         }
         if (lambert3 > 0.0)
         {
            vec3 halfDir = normalize(lightDir3 + viewDir);
            float angle3 = max(dot(halfDir, normal), 0.0);
            float spec3 = pow( angle3, shininess );
            colorLinear += (diffuseColor.rgb * lambert3 + 0.125 * u_Ks * spec3)*lightColor3;
         }
         if (lambert4 > 0.0)
         {
            vec3 halfDir = normalize(lightDir4 + viewDir);
            float angle4 = max(dot(halfDir, normal), 0.0);
            float spec4 = pow( angle4, shininess );
            colorLinear += (diffuseColor.rgb * lambert4 + 0.125 * u_Ks * spec4)*lightColor4;
         }
      )";
   }
   else if ( material.Lighting == 3 )
   {

      fs << R"(
         const vec3 u_Ka = vec3(0.033, 0.033, 0.033);
         const vec3 u_Ks = vec3(1.0, 1.0, 1.0);
         vec3 colorLinear = u_Ka;
         vec3 normal = normalize( v_normal );
      )";

      fs << "float f = 1.0 / float(" << m_Lights.size() << ");\n";

      for ( size_t i = 0; i < m_Lights.size(); ++i )
      {
      fs << "{\n";
         fs << "vec3 lightDir = v_pos - v_light_pos" << i << ";\n";
      fs << R"(
         float distance = 1.1; //length(lightDir);
         //distance = distance * distance;
         lightDir = normalize(lightDir);

         float lambertian = max( dot(lightDir, normal), 0.0 );
         vec3 reflectDir = reflect(lightDir, normal);
         float specAngle = max(dot(reflectDir, lightDir), 0.0);
         float specular = pow( specAngle, 4.0 );
         )";
      fs << "\t\tfloat power = 1.0;\n";
      fs << "\t\tvec3 lightColor = u_light_color" << i << " * power;\n";
      fs << "\t\tcolorLinear += diffuseColor.rgb * lambertian * f;\n";
      fs << "\t\tcolorLinear += u_Ks * 0.0125 * specular * f;\n";


      fs << "}\n";
      }

   }


   // apply gamma correction (assume ambientColor, diffuseColor and u_Ks
   // have been linearized, i.e. have no gamma correction in them)
   // use the gamma corrected color in the fragment
   fs << R"(
         vec3 colorGammaCorrected = pow(colorLinear, vec3(1.0 / u_screenGamma));
   )";

   if ( material.hasDiffuseMap() &&
        ( material.CloudTransparent ||
          material.getDiffuseMap().getTexture()->hasTransparency() ) )
   {
      fs << R"(
         color = vec4(colorGammaCorrected, diffuseColor.a * v_color.a );
      )";
   }
   else {
      fs << R"(
         color = vec4(colorGammaCorrected, v_color.a );
      )";
   }

   if ( material.FogEnable )
   {
      fs << R"(
         vec3 fog_color = vec3(0.2,0.5,0.1);
         float dx = u_camera_pos.x - v_pos.x;
         float dz = u_camera_pos.z - v_pos.z;
         float d = clamp( sqrt( dx*dx + dz*dz ) / 1500.0, 0.0, 1.0 );
         if ( d > 0.99 )
         {
            color = vec4( fog_color, color.a );
         }
         else if ( d < 0.85 )
         {
            // color = vec4( mix( color.rgb, fog_color, d ), color.a );
         }
         else
         {
            color = vec4( mix( color.rgb, fog_color, (d - 0.85)/0.14 ), color.a );
         }
      )";
   }

   fs << R"(
         color = clamp( color, vec4( 0,0,0,0 ), vec4( 1,1,1,1 ) );
      }
   )";

   return fs.str();
}

} // end namespace de.



#if 0
   if ( material.Lighting > 0 )
   {
      fs << R"(
         vec3 normal = normalize( v_normal );
         vec3 colorLinear = u_Ka;
                        // + diffuseColor.rgb * lambertian * l0_finalColor
                        // + u_Ks * specular * l0_finalColor;

      )";
      for ( size_t i = 0; i < m_Lights.size(); ++i )
      {
         std::string l1 = std::string( "u_light_pos" ) + std::to_string( i );
         std::string l2 = std::string( "u_light_color" ) + std::to_string( i );
         std::string l3 = std::string( "u_light_power" ) + std::to_string( i );
      fs << "{\n"
         "\t\tvec3 lightDir = " << l1 << ";"; //  << l1 << " - v_pos;";
      fs << R"(
         float distance = length(lightDir);
         //distance = distance * distance;
         lightDir = normalize(lightDir);

         float specular = 0.0;

         float lambertian = max(dot(lightDir, normal), 0.0);
         if (lambertian > 0.0)
         {
            vec3 viewDir = normalize(v_pos);
            // this is blinn phong
            vec3 halfDir = normalize(lightDir + viewDir);
            float specAngle = max(dot(halfDir, normal), 0.0);
            specular = pow(specAngle, u_shininess);
            // this is phong (for comparison)
            if ( u_illum > 1.5 )
            {
               vec3 reflectDir = reflect(lightDir, normal);
               specAngle = max(dot(reflectDir, viewDir), 0.0);
               // note that the exponent is different here
               specular = pow(specAngle, u_shininess/4.0);
            }
         }
      )";
      fs << "\t\tfloat l_intensity = " << l3 << " / distance;";
      fs << "\t\tvec3 l_color = " << l2 << " * l_intensity;";
      fs << R"(
         colorLinear += (diffuseColor.rgb * lambertian + u_Ks * specular ) * l_color;
         }
      )";
      }
   }
   /* BK
    *       fs << R"(
      vec3 colorLinear = u_Ka;
      vec3 normal = normalize( v_normal );
   )";

   for ( size_t i = 0; i < m_Lights.size(); ++i )
   {
      std::string l1 = std::string( "u_light_pos" ) + std::to_string( i );
      std::string l2 = std::string( "u_light_color" ) + std::to_string( i );
      std::string l3 = std::string( "u_light_power" ) + std::to_string( i );

      fs << "{\n";
      fs << "vec3 lightDir = " << l1 << ";";
   fs << R"(
      float distance = length(lightDir);
      //distance = distance * distance;
      lightDir = normalize(lightDir);

      float lambertian = dot(-lightDir, normal);

      if ( u_illum > 1.5 )
      {
         float specular = 0.0;

         if (lambertian > 0.0)
         {
            vec3 viewDir = normalize(lightDir);
            // this is blinn phong
            vec3 halfDir = normalize(lightDir + viewDir);
            float specAngle = max(dot(halfDir, normal), 0.0);
            specular = pow(specAngle, u_shininess);
            // this is phong (for comparison)
            if ( u_illum > 2.5 )
            {
               vec3 reflectDir = reflect(lightDir, normal);
               specAngle = max(dot(reflectDir, viewDir), 0.0);
               // note that the exponent is different here
               specular = pow(specAngle, u_shininess);
            }
         }
      )";
      fs << "\t\tfloat l_intensity = " << l3 << " / (distance*distance);\n";
      fs << "\t\tvec3 l_color = " << l2 << " * l_intensity;\n";
      fs << "\t\tcolorLinear += (diffuseColor.rgb * lambertian + u_Ks * specular ) * l_color;\n";
      fs << "} else {\n";
      fs << "\t\tcolorLinear += diffuseColor.rgb * lambertian;\n";
      fs << "}\n";
      fs << "}\n";
   }

   */
      fs << R"(
         vec3 colorLinear = u_Ka;
         vec3 normal = normalize( v_normal );


         vec3 lightDir = u_light_pos; // - v_pos;

         float distance = length(lightDir);
         //distance = distance * distance;
         lightDir = normalize(lightDir);

         float lambertian = dot(lightDir, normal);

         if ( u_illum > 1.5 )
         {
            float specular = 0.0;

            if (lambertian > 0.0)
            {
               vec3 viewDir = normalize(lightDir);
               // this is blinn phong
               vec3 halfDir = normalize(lightDir + viewDir);
               float specAngle = max(dot(halfDir, normal), 0.0);
               specular = pow(specAngle, u_shininess);
               // this is phong (for comparison)
               if ( u_illum > 2.5 )
               {
                  vec3 reflectDir = reflect(lightDir, normal);
                  specAngle = max(dot(reflectDir, viewDir), 0.0);
                  // note that the exponent is different here
                  specular = pow(specAngle, u_shininess/4.0);
               }
            }
            float l_intensity = u_light_power / distance;
            vec3 l_color = u_light_color * l_intensity;
            colorLinear += (diffuseColor.rgb * lambertian + u_Ks * specular ) * l_color;
         }
         else
         {
            vec3 l_color = u_light_color;
            colorLinear += ( diffuseColor.rgb * lambertian ); // + u_Ks * specular ) * l_color;
         }
      )";

#endif




