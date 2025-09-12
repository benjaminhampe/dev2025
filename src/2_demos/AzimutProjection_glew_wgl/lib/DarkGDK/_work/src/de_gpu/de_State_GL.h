#pragma once
#include <de_gpu/de_State.h>
#include <de_gpu/de_GL_debug_layer.h>

namespace de {

struct GL_State
{
   DE_CREATE_LOGGER("de_gpu.State_GL")

   static void
   test()
   {
      /*
      DE_DEBUG( "[Init] CullTest: ", m_Culling.curr.toString() )
      DE_DEBUG( "[Init] DepthTest: ", m_Depth.curr.toString() )
      DE_DEBUG( "[Init] StencilTest: ", m_Stencil.curr.toString() )
      DE_DEBUG( "[Init] BlendTest: ", m_Blend.curr.toString() )
      DE_DEBUG( "[Init] LineWidth: ", m_LineWidth.curr.toString() )
      DE_DEBUG( "[Init] PointSize: ", m_PointSize.curr.toString() )
      DE_DEBUG( "[Init] PolygonOffset: ", m_PolygonOffset.curr.toString() )
      DE_DEBUG( "[Init] RasterizerDiscard: ", m_RasterizerDiscard.curr.toString() )*/
      DE_DEBUG( "[Sizeof] State(", sizeof( State ), ")" )
      DE_DEBUG( "[Sizeof] = Viewport(", sizeof( Viewport ), ")" )
      DE_DEBUG( "[Sizeof] + Scissor(", sizeof( Scissor ), ")" )
      DE_DEBUG( "[Sizeof] + Clear(", sizeof( Clear ), ")" )
      DE_DEBUG( "[Sizeof] + DepthRange(", sizeof( DepthRange ), ")" )
      DE_DEBUG( "[Sizeof] + Culling(", sizeof( Culling ), ")" )
      DE_DEBUG( "[Sizeof] + Blend(", sizeof( Blend ), ")" )
      DE_DEBUG( "[Sizeof] + Depth(", sizeof( Depth ), ")" )
      DE_DEBUG( "[Sizeof] + Stencil(", sizeof( Stencil ), ")" )
      DE_DEBUG( "[Sizeof] + LineWidth(", sizeof( LineWidth ), ")" )
      DE_DEBUG( "[Sizeof] + PointSize(", sizeof( PointSize ), ")" )
      DE_DEBUG( "[Sizeof] + PolygonOffset(", sizeof( PolygonOffset ), ")" )
      DE_DEBUG( "[Sizeof] + RasterizerDiscard(", sizeof( RasterizerDiscard ), ")" )
   }

   static PolygonOffset
   queryPolygonOffset()
   {
      PolygonOffset polyfill;
      polyfill.enabled = ( ::glIsEnabled( GL_POLYGON_OFFSET_FILL ) != GL_FALSE );
      GL_VALIDATE

      GLfloat offsetFactor = 0.0f;
      ::glGetFloatv( GL_POLYGON_OFFSET_FACTOR, &offsetFactor );
      GL_VALIDATE
      polyfill.offsetFactor = offsetFactor;

      GLfloat offsetUnits = 0.0f;
      ::glGetFloatv( GL_POLYGON_OFFSET_UNITS, &offsetUnits );
      GL_VALIDATE
      polyfill.offsetUnits = offsetUnits;

      return polyfill;
   }

   static PolygonOffset
   applyPolygonOffset( PolygonOffset const & alt, PolygonOffset const & neu )
   {
      if ( neu.enabled != alt.enabled )
      {
         if ( neu.enabled )
         {
            ::glEnable( GL_POLYGON_OFFSET_FILL ); GL_VALIDATE
            ::glPolygonOffset( neu.offsetFactor, neu.offsetUnits ); GL_VALIDATE
            DE_DEBUG("PolygonOffsetFill ON")
         }
         else
         {
            ::glDisable( GL_POLYGON_OFFSET_FILL ); GL_VALIDATE
            DE_DEBUG("PolygonOffsetFill OFF")
         }
      }

      return neu;
   }

   // ===========================================================================
   // StateManager
   // ===========================================================================

   static uint32_t
   toClearMask( Clear const & clear )
   {
      uint32_t mask = 0;
      if ( clear.hasColorBit() ) mask |= GL_COLOR_BUFFER_BIT;
      if ( clear.hasDepthBit() ) mask |= GL_DEPTH_BUFFER_BIT;
      if ( clear.hasStencilBit() ) mask |= GL_STENCIL_BUFFER_BIT;
      return mask;
   }

   static Clear
   queryClear()
   {
      glm::vec4 color;
      ::glGetFloatv( GL_COLOR_CLEAR_VALUE, glm::value_ptr( color ) ); GL_VALIDATE
      GLfloat depth = 1.0f;
      ::glGetFloatv( GL_DEPTH_CLEAR_VALUE, &depth ); GL_VALIDATE
      GLint stencil = 0;
      ::glGetIntegerv( GL_STENCIL_CLEAR_VALUE, &stencil ); GL_VALIDATE
      Clear clear;
      clear.color = color;
      clear.depth = depth;
      clear.stencil = uint8_t( stencil );
      return clear;
   }

   /*
   static void
   clearImpl( Clear const & state )
   static Clear
   applyClear( Clear const & alt, Clear const & neu )
   {
      // Different clear color
      if ( neu.color != alt.color )
      {
         //DE_DEBUG("Apply new clear color")
         ::glClearColor( neu.color.r, neu.color.g, neu.color.b, neu.color.a );
         GL_VALIDATE
      }
      // Different clear depth value
      if ( neu.depth != alt.depth )
      {
         //DE_DEBUG("Apply new clear depth")
         ::glClearDepthf( neu.depth );
         GL_VALIDATE
      }
      // Different clear stencil value
      if ( neu.stencil != alt.stencil )
      {
         //DE_DEBUG("Apply new clear stencil")
         ::glClearStencil( neu.stencil );
         GL_VALIDATE
      }
      // Actually do clear if mask differs from 0.
   //   if ( neu.mask > 0 )
   //   {
         //DE_DEBUG("Apply clear(", neu.toString(), ")")
         ::glClear( toClearMask( neu ) );
         GL_VALIDATE
   //   }
      return neu;
   }
   */

   static Clear
   applyClear( Clear const & alt, Clear const & neu )
   {
      // Different clear color
      if ( neu.color != alt.color )
      {
         //DE_DEBUG("Apply new clear color")
         ::glClearColor( neu.color.r, neu.color.g, neu.color.b, neu.color.a );
         GL_VALIDATE
      }
      // Different clear depth value
      if ( neu.depth != alt.depth )
      {
         //DE_DEBUG("Apply new clear depth")
         ::glClearDepthf( neu.depth );
         GL_VALIDATE
      }
      // Different clear stencil value
      if ( neu.stencil != alt.stencil )
      {
         //DE_DEBUG("Apply new clear stencil")
         ::glClearStencil( neu.stencil );
         GL_VALIDATE
      }
      // Actually do clear if mask differs from 0.
   //   if ( neu.mask > 0 )
   //   {
         //DE_DEBUG("Apply clear(", neu.toString(), ")")
   //      ::glClear( toClearMask( neu ) );
   //      GL_VALIDATE
   //   }
      return neu;
   }


   static Viewport
   queryViewport()
   {
      GLint vp[ 4 ];
      ::glGetIntegerv( GL_VIEWPORT, vp );
      GL_VALIDATE

      Viewport viewport;
      viewport.m_x = vp[ 0 ];
      viewport.m_y = vp[ 1 ];
      viewport.m_w = vp[ 2 ];
      viewport.m_h = vp[ 3 ];
      return viewport;
   }

   static Viewport
   applyViewport( Viewport const & alt, Viewport const & neu )
   {
      if ( alt != neu )
      {
         ::glViewport( neu.x(), neu.y(), neu.w(), neu.h() );
         GL_VALIDATE
         //std::cout << "Changed Viewport from(" << this->toString() << " ) to( " << other.toString() << " )\n";
      }
      return neu;
   }

   static DepthRange
   queryDepthRange()
   {
      GLfloat depthRange[ 2 ];
      ::glGetFloatv( GL_DEPTH_RANGE, depthRange ); GL_VALIDATE
      return DepthRange( depthRange[ 0 ], depthRange[ 1 ] );
   }

   static DepthRange
   applyDepthRange( DepthRange const & alt, DepthRange const & neu )
   {
      if ( alt != neu )
      {
         ::glDepthRangef( neu.n, neu.f );
         GL_VALIDATE
         //std::cout << "Changed Viewport from(" << this->toString() << " ) to( " << other.toString() << " )\n";
      }
      return neu;
   }

   static Scissor
   queryScissor()
   {
      Scissor scissor;
      scissor.enabled = ( ::glIsEnabled( GL_SCISSOR_TEST ) != GL_FALSE );
      GL_VALIDATE
      return scissor;
   }

   static Scissor
   applyScissor( Scissor const & alt, Scissor const & neu )
   {
      if ( neu == alt ) { return neu; }
      if ( neu.enabled != alt.enabled )
      {
         ::glScissor( neu.x, neu.y, neu.w, neu.h );
         GL_VALIDATE

         ::glEnable( GL_SCISSOR_TEST );
         GL_VALIDATE
      }
      else
      {
         ::glDisable( GL_SCISSOR_TEST );
         GL_VALIDATE
      }

      return neu;
   }

   static Culling
   queryCulling()
   {
      Culling culling;
      bool enabled = glIsEnabledDE( GL_CULL_FACE );
      culling.setEnabled( enabled );
      int cullMode = glGetIntegerDE( GL_CULL_FACE_MODE ); // GL_BACK|GL_FRONT|GL_FRONT_AND_BACK
      switch ( cullMode )
      {
         case GL_BACK: culling.back(); break;
         case GL_FRONT: culling.front(); break;
         case GL_FRONT_AND_BACK: culling.frontAndBack(); break;
         default: culling.back(); DE_ERROR("Unknown Cull mode, maybe not init") break;
      }

      bool isCW = ( GL_CW == glGetIntegerDE( GL_FRONT_FACE ) ); // GL_CCW|GL_CW
      if ( isCW )
         culling.cw(); // GL_CCW|GL_CW
      else
         culling.ccw(); // GL_CCW|GL_CW

      return culling;
   }

   static Culling
   applyCulling( Culling const & alt, Culling const & neu )
   {
      if ( neu == alt )
      {
         return neu;
      }

      if ( neu.isEnabled() )
      {
         ::glEnable( GL_CULL_FACE );  GL_VALIDATE

         // [mode] GL_BACK, GL_FRONT
         GLenum mode = GL_BACK;
         if ( neu.isFrontAndBack() ) { mode = GL_FRONT_AND_BACK; }
         else if ( neu.isFront() ) { mode = GL_FRONT; }
         ::glCullFace( mode ); GL_VALIDATE

         // [windingOrder] CW,CCW
         ::glFrontFace( neu.isCCW() ? GL_CCW : GL_CW ); GL_VALIDATE

         //DE_DEBUG("Culling ON ", neu.toString())
      }
      else
      {
         ::glDisable( GL_CULL_FACE ); GL_VALIDATE
         //DE_DEBUG("Culling OFF")
      }

      return neu;
   }


   static Depth::EFunc
   toDepthFunction( GLint value )
   {
      switch ( value )
      {
         case GL_LESS: return Depth::Less;
         case GL_LEQUAL: return Depth::LessEqual;
         case GL_GREATER: return Depth::Greater;
         case GL_GEQUAL: return Depth::GreaterEqual;
         case GL_EQUAL: return Depth::Equal;
         case GL_NOTEQUAL: return Depth::NotEqual;
         case GL_ALWAYS: return Depth::AlwaysPass;
         case GL_NEVER: return Depth::Never;
         default: return Depth::LessEqual;
      }
   }

   static uint32_t
   fromDepthFunction( Depth::EFunc value  )
   {
      switch ( value )
      {
         case Depth::Less: return GL_LESS;
         case Depth::LessEqual: return GL_LEQUAL;
         case Depth::Greater: return GL_GREATER;
         case Depth::GreaterEqual: return GL_GEQUAL;
         case Depth::Equal: return GL_EQUAL;
         case Depth::NotEqual: return GL_NOTEQUAL;
         case Depth::AlwaysPass: return GL_ALWAYS;
         case Depth::Never: return GL_NEVER;
         default: return GL_LEQUAL;
      }
   }


   static Depth
   queryDepth()
   {
      bool enabled = glIsEnabledDE( GL_DEPTH_TEST );
      bool zwrite = glGetBoolDE( GL_DEPTH_WRITEMASK );
      Depth::EFunc depthFunc = toDepthFunction( glGetIntegerDE( GL_DEPTH_FUNC ) );
      return Depth( enabled, zwrite, depthFunc );
   }

   static Depth
   applyDepth( Depth const & alt, Depth const & neu )
   {
      if ( neu == alt ) { return neu; }

   //   if ( neu == alt )
   //   {
   //      DE_DEBUG("Nothing todo neu(",neu.toString(),")")
   //      return neu; // Nothing todo
   //   }

      if ( neu.isEnabled() )
      {
         ::glEnable( GL_DEPTH_TEST );
         GL_VALIDATE

         ::glDepthFunc( fromDepthFunction( neu.getFunc() ) );
         GL_VALIDATE

         ::glDepthMask( neu.isZWriteEnabled() ? GL_TRUE : GL_FALSE );
         GL_VALIDATE

         //DE_DEBUG("DepthTest ON")
      }
      else
      {
         ::glDisable( GL_DEPTH_TEST );
         GL_VALIDATE
         //DE_DEBUG("DepthTest OFF")
      }

      return neu;
   }

   static Stencil
   queryStencil()
   {
      Stencil stencil;
      stencil.enabled = glIsEnabledDE( GL_STENCIL_TEST );
      return stencil;
   }

   static Stencil
   applyStencil( Stencil const & alt, Stencil const & neu )
   {
      if ( neu == alt )
      {
         return neu; // No difference, nothing todo.
      }

      if ( neu.enabled )
      {
         ::glDisable( GL_STENCIL_TEST ); GL_VALIDATE
         //::glEnable( GL_STENCIL_TEST ); GL_VALIDATE
         //::glStencilFunc( to ); GL_VALIDATE
         //::glStencilMask(); GL_VALIDATE
         //::glStencilOp(); GL_VALIDATE
         //DE_DEBUG( "Stencil ON ", neu.toString() )
      }
      else
      {
         ::glDisable( GL_STENCIL_TEST ); GL_VALIDATE
         //DE_DEBUG( "Stencil OFF" )
      }

   //      std::cout << "Changed Stencil from(" << this->toString() << " ) to( " << neu.toString() << " )\n";
   //      m_StencilFail = neu.m_StencilFail;
   //      m_StencilKeep = neu.m_StencilKeep;
      return neu;
   }

   static Blend::EEquation
   toBlendEquation( GLint value )
   {
      switch ( value )
      {
         case GL_FUNC_ADD: return Blend::Add;
         case GL_FUNC_SUBTRACT: return Blend::Sub;
         case GL_FUNC_REVERSE_SUBTRACT: return Blend::ReverseSub;
         case GL_MIN: return Blend::Min;
         case GL_MAX: return Blend::Max;
         default: return Blend::Add;
      }
   }

   static uint32_t
   fromBlendEquation( Blend::EEquation value  )
   {
      switch ( value )
      {
         case Blend::Add: return GL_FUNC_ADD;
         case Blend::Sub: return GL_FUNC_SUBTRACT;
         case Blend::ReverseSub: return GL_FUNC_REVERSE_SUBTRACT;
         case Blend::Min: return GL_MIN;
         case Blend::Max: return GL_MAX;
         default: return GL_FUNC_ADD;
      }
   }

   static Blend::EFunction
   toBlendFunction( GLint value )
   {
      switch ( value )
      {
         case GL_SRC_ALPHA: return Blend::SrcAlpha;
         case GL_ONE_MINUS_SRC_ALPHA: return Blend::OneMinusSrcAlpha;

         case GL_ZERO: return Blend::Zero;
         case GL_ONE: return Blend::One;

         case GL_DST_ALPHA: return Blend::DstAlpha;
         case GL_ONE_MINUS_DST_ALPHA: return Blend::OneMinusDstAlpha;
         case GL_SRC_ALPHA_SATURATE: return Blend::SrcAlphaSaturate;
         case GL_CONSTANT_ALPHA: return Blend::ConstantAlpha;

         case GL_SRC_COLOR: return Blend::SrcColor;
         case GL_ONE_MINUS_SRC_COLOR: return Blend::OneMinusSrcColor;
         case GL_DST_COLOR: return Blend::DstColor;
         case GL_ONE_MINUS_DST_COLOR: return Blend::OneMinusDstColor;

         case GL_CONSTANT_COLOR: return Blend::ConstantColor;
         case GL_ONE_MINUS_CONSTANT_COLOR: return Blend::OneMinusConstantColor;

         case GL_BLEND_COLOR: return Blend::BlendColor;
   //         case GL_SRC1_ALPHA_EXT: return Blend::Src1Alpha;
   //         case GL_ONE_MINUS_SRC1_ALPHA_EXT: return Blend::OneMinusSrc1Alpha;
   //         case GL_SRC1_COLOR_EXT: return Blend::Src1Color;
   //         case GL_ONE_MINUS_SRC1_COLOR_EXT: return Blend::OneMinusSrc1Color;
         default: return Blend::Zero;
      }
   }

   static uint32_t
   fromBlendFunction( Blend::EFunction value )
   {
      switch ( value )
      {
         case Blend::SrcAlpha: return GL_SRC_ALPHA;
         case Blend::OneMinusSrcAlpha: return GL_ONE_MINUS_SRC_ALPHA;

         case Blend::Zero: return GL_ZERO;
         case Blend::One: return GL_ONE;

         case Blend::DstAlpha: return GL_DST_ALPHA;
         case Blend::OneMinusDstAlpha: return GL_ONE_MINUS_DST_ALPHA;
         case Blend::SrcAlphaSaturate: return GL_SRC_ALPHA_SATURATE;
         case Blend::ConstantAlpha: return GL_CONSTANT_ALPHA;

         case Blend::SrcColor: return GL_SRC_COLOR;
         case Blend::OneMinusSrcColor: return GL_ONE_MINUS_SRC_COLOR;
         case Blend::DstColor: return GL_DST_COLOR;
         case Blend::OneMinusDstColor: return GL_ONE_MINUS_DST_COLOR;

         case Blend::ConstantColor: return GL_CONSTANT_COLOR;
         case Blend::OneMinusConstantColor: return GL_ONE_MINUS_CONSTANT_COLOR;

         case Blend::BlendColor: return GL_BLEND_COLOR;
   //         case Blend::Src1Alpha: return GL_SRC1_ALPHA_EXT;
   //         case Blend::OneMinusSrc1Alpha: return GL_ONE_MINUS_SRC1_ALPHA_EXT;
   //         case Blend::Src1Color: return GL_SRC1_COLOR_EXT;
   //         case Blend::OneMinusSrc1Color: return GL_ONE_MINUS_SRC1_COLOR_EXT;
         default: return GL_ZERO;
      }
   }

   static Blend
   queryBlend()
   {
      Blend blend;
      // Blend enabled
      blend.enabled = glIsEnabledDE( GL_BLEND );
      // Blend equation
      blend.equation = toBlendEquation( glGetIntegerDE( GL_BLEND_EQUATION ) );
      // Blend source RGB factor
      blend.src_rgb = toBlendFunction( glGetIntegerDE( GL_BLEND_SRC_RGB ) );
      // Blend destination RGB factor
      blend.dst_rgb = toBlendFunction( glGetIntegerDE( GL_BLEND_DST_RGB ) );
      // Blend source Alpha factor
      blend.src_a = toBlendFunction( glGetIntegerDE( GL_BLEND_SRC_ALPHA ) );
      // Blend destination Alpha factor
      blend.dst_a = toBlendFunction( glGetIntegerDE( GL_BLEND_DST_ALPHA ) );
      return blend;
   }

   static Blend
   applyBlend( Blend const & alt, Blend const & neu )
   {
      if ( neu == alt )
      {
         return neu; // Nothing todo
      }

   //   if ( neu.enabled != alt.enabled )
   //   {
      if ( neu.enabled )
      {
         ::glEnable( GL_BLEND ); GL_VALIDATE

         ::glBlendEquation(
               fromBlendEquation( neu.equation ) ); GL_VALIDATE
         ::glBlendFunc(
               fromBlendFunction( neu.src_a ),
               fromBlendFunction( neu.dst_a ) ); GL_VALIDATE
   //         ::glBlendEquationSeparate(
   //               fromBlendEquation( neu.equation ),
   //               fromBlendEquation( neu.equation ) ); GL_VALIDATE
   //         ::glBlendFuncSeparate(
   //               fromBlendFunction( neu.src_rgb ),
   //               fromBlendFunction( neu.dst_rgb ),
   //               fromBlendFunction( neu.src_a ),
   //               fromBlendFunction( neu.dst_a ) ); GL_VALIDATE
         //DE_DEBUG("Blending ON")
      }
      else
      {
         ::glDisable( GL_BLEND ); GL_VALIDATE
         //DE_DEBUG("Blending OFF")
      }

      return neu;
   }

   static RasterizerDiscard
   queryRasterizerDiscard()
   {
      RasterizerDiscard rdiscard;
      rdiscard.enabled = ( ::glIsEnabled( GL_RASTERIZER_DISCARD ) != GL_FALSE );
      GL_VALIDATE
      return rdiscard;
   }

   static RasterizerDiscard
   applyRasterizerDiscard( RasterizerDiscard const & alt,
                                 RasterizerDiscard const & neu )
   {
      if ( neu.enabled != alt.enabled )
      {
         if ( neu.enabled )
         {
            ::glEnable( GL_RASTERIZER_DISCARD ); GL_VALIDATE
            DE_DEBUG("RasterizerDiscard ON")
         }
         else
         {
            ::glDisable( GL_RASTERIZER_DISCARD ); GL_VALIDATE
            DE_DEBUG("RasterizerDiscard OFF")
         }
      }
      return neu;
   }

   static LineWidth
   queryLineWidth()
   {
      LineWidth state;

      GLfloat line_width;
      ::glGetFloatv( GL_LINE_WIDTH, &line_width ); GL_VALIDATE
      state.m_Now = line_width;

      GLfloat line_range[2];
      ::glGetFloatv( GL_ALIASED_LINE_WIDTH_RANGE, line_range ); GL_VALIDATE
      state.m_Min = line_range[ 0 ];
      state.m_Max = line_range[ 1 ];

      return state;
   }

   static LineWidth
   applyLineWidth( LineWidth const & alt, LineWidth const & neu )
   {
      if ( std::abs( neu.m_Now - alt.m_Now ) > std::numeric_limits< float >::epsilon() )
      {
         //DE_DEBUG( "glLineWidth(", neu.m_Now, ")" )
         ::glLineWidth( neu.m_Now );
         bool ok = GL_VALIDATE;
         if ( !ok )
         {
            DE_ERROR( "glLineWidth(", neu.m_Now, ") invalid value" )
         }

      }
      return neu;
   }

   static PointSize
   queryPointSize()
   {
      PointSize state;
      //GLfloat ps_now;
      //::glGetFloatv( GL_ALPOINT_SIZE, &ps_now ); GL_VALIDATE
      state.m_Now = 1.0f;

      GLfloat ps_range[2];
      ::glGetFloatv( GL_ALIASED_POINT_SIZE_RANGE, ps_range ); GL_VALIDATE
      state.m_Min = ps_range[ 0 ];
      state.m_Max = ps_range[ 1 ];
      return state;
   }

   static PointSize
   applyPointSize( PointSize const & alt, PointSize const & neu )
   {
      // if ( other == *this )
      // {
         // return;
      // }

      // //::glPointSize( other.m_Now );
      // std::cout << "Changed PointSize from(" << this->toString() << " ) to( " << other.toString() << " )\n";
      // m_Now = other.m_Now;
      return neu;
   }

   static State
   queryState()
   {
      State state;
      //state.viewport = queryViewport();
      //state.scissor = queryScissor();
      //state.clear = queryClear();
      state.culling = queryCulling();
      state.depth = queryDepth();
      state.stencil = queryStencil();
      state.blend = queryBlend();
      state.pointSize = queryPointSize();
      state.lineWidth = queryLineWidth();
      state.rasterizerDiscard = queryRasterizerDiscard();
      state.polygonOffset = queryPolygonOffset();
      return state;
   }

   static State
   applyState( State const & alt, State const & neu )
   {
      State state;
      // Dont apply viewport
      // Dont apply scissor
      // Dont apply clear
      //state.depthRange = applyCulling( alt.depthRange, neu.depthRange );
      state.culling = applyCulling( alt.culling, neu.culling );
      state.depth = applyDepth( alt.depth, neu.depth );
      state.stencil = applyStencil( alt.stencil, neu.stencil );
      state.blend = applyBlend( alt.blend, neu.blend );
      state.pointSize = applyPointSize( alt.pointSize, neu.pointSize );
      state.lineWidth = applyLineWidth( alt.lineWidth, neu.lineWidth );
      state.rasterizerDiscard = applyRasterizerDiscard( alt.rasterizerDiscard, neu.rasterizerDiscard );
      state.polygonOffset = applyPolygonOffset( alt.polygonOffset, neu.polygonOffset );

      return state;
   }
};

} // end namespace de
