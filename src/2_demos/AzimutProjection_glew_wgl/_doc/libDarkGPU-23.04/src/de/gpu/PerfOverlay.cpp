#include "PerfOverlay.hpp"
#include "VideoDriver.hpp"

namespace de {
namespace gpu {

namespace
{
   constexpr float const CONST_Z_INIT = 0.90f; // At 1.0 it disappears, not inside frustum.
   constexpr float const CONST_Z_STEP = -0.00001f; // enough for 200.000 elements, TODO: test more.
}

void
PerfOverlay::draw2D()
{
   if (!m_driver) { DE_ERROR("No driver") return; }
      //std::cout << "PerfOverlay::draw2DPerfOverlay5x8()" << std::endl;
   int w = m_driver->getScreenWidth();
   int h = m_driver->getScreenHeight();
   int mx = m_driver->getMouseX();
   int my = m_driver->getMouseY();

   int p = 2;
   int x = p;
   int y = p;
   int ln = m_driver->getTextSize5x8( "W" ).y + p;
   Align::EAlign align = Align::TopLeft;

   m_driver->resetModelMatrix();

   auto s1 = dbStrJoin("Frames(",m_driver->getFrameCount(),")");
   //DE_DEBUG("w(",w,"), h(",h,"), s1(",s1,")")
   m_driver->draw2DText5x8( x,y, s1, 0xFF00AEFE, align ); y += ln;
   m_driver->draw2DText5x8( x,y, dbStrJoin("Time(",m_driver->getTimer(),")"), 0xFF0030FF, align ); y += ln;
   m_driver->draw2DText5x8( x,y, dbStrJoin("FPS(",m_driver->getFPS(),")"), 0xFF10BB10, align ); y += ln;
   m_driver->draw2DText5x8( x,y, dbStrJoin("Mouse(",mx,",",my,")"), 0xFF10BB10, align ); y += ln;

   m_driver->draw2DText5x8( x,y, dbStrJoin("DrawCalls/Frame(",m_driver->getDrawCallsPerFrame(),")"), 0xFF80BBD0, align ); y += ln;
   m_driver->draw2DText5x8( x,y, dbStrJoin("DrawCalls/Sec(",m_driver->getDrawCallsPerSecond(),")"), 0xFF80BBC0, align ); y += ln;


   auto camera = m_driver->getCamera();
   if ( camera )
   {
      m_driver->draw2DText5x8( x,y, dbStrJoin("Camera-Pos(",glm::ivec3(camera->getPos()),")"), 0xFFFFEEEE, align ); y += ln;
      m_driver->draw2DText5x8( x,y, dbStrJoin("Camera-Target(",glm::ivec3(camera->getTarget()),")"), 0xFFFFEEEE, align ); y += ln;
      {
         std::ostringstream s;
         s << "Camera-Angle(" << int(camera->getAng().x) << ","
                              << int(camera->getAng().y) << ","
                              << int(camera->getAng().z) << ")";
         m_driver->draw2DText5x8( x,y, s.str(), 0xFFFFBBBB, align ); y += ln;
      }



      m_driver->draw2DText5x8( x,y, dbStrJoin("Camera-Dir(",camera->getDir(),")"), 0xFFDD6A6A, align ); y += ln;
      m_driver->draw2DText5x8( x,y, dbStrJoin("Camera-Right(",camera->getRight(),")"), 0xFFCC1212, align ); y += ln;

      m_driver->draw2DText5x8( x,y, dbStrJoin("Camera-FOV(",camera->getFOV(),")"), 0xFFaa5252, align ); y += ln;
      m_driver->draw2DText5x8( x,y, dbStrJoin("Camera-Near(",camera->getNearValue(),")"), 0xFFaa52aa, align ); y += ln;
      m_driver->draw2DText5x8( x,y, dbStrJoin("Camera-Far(",camera->getFarValue(),")"), 0xFFaa72CC, align ); y += ln;
   }

   m_driver->draw2DText5x8( x,y, dbStrJoin("MSAA(",m_driver->getMSAA(),")"), 0xFFee9292, align ); y += ln;
//   auto smgr = m_driver->getSceneManager();
//   if ( smgr )
//   {
//      m_driver->draw2DText5x8( x,y, dbStrJoin("Buffers(",smgr->getBufferCount(),")"), 0xFFaa72CC, align ); y += ln;
//      m_driver->draw2DText5x8( x,y, dbStrJoin("Vertices(",smgr->getVertexCount(),")"), 0xFFaa72CC, align ); y += ln;
//      m_driver->draw2DText5x8( x,y, dbStrJoin("Indices(",smgr->getIndexCount(),")"), 0xFFaa72CC, align ); y += ln;
//      m_driver->draw2DText5x8( x,y, dbStrJoin("Bytes(",dbStrByteCount( smgr->getByteCount() ),")"), 0xFFaa72CC, align ); y += ln;
//      //m_driver->draw2DText5x8( x,y, dbStrJoin("Primitives(",smgr->getByteCount(3),")"), 0xFFaa72CC, align ); y += ln;
//   }

   align = Align::TopRight;
   x = w - 1 - p;
   y = p;

   {  std::ostringstream s;
      s << "Screen(" << m_driver->getScreenWidth() << ","
                     << m_driver->getScreenHeight()<< ","
                     << m_driver->getScreenFormat().toString().c_str() << ")";
      m_driver->draw2DText5x8( x,y, s.str(), 0xFF00EEEE, align ); y += ln;
   }

   align = Align::BottomRight;
   x = w - p;
   y = h - p;
   m_driver->draw2DText5x8( x,y, "(c) 2021 <benjaminhampe@gmx.de>", 0xFFEEEEEE, align ); y += ln;

   if ( camera )
   {
      align = Align::BottomLeft;
      x = p;
      y = h - p - 4*ln;
      m_driver->draw2DText5x8( x,y, dbStrJoin("ViewMatrix(",camera->getViewMatrix(),")"), 0xFF1BAB1B, align ); y += ln;
      m_driver->draw2DText5x8( x,y, dbStrJoin("ProjectionMatrix(",camera->getProjectionMatrix(),")"), 0xFF3CBB3B, align ); y += ln;
      m_driver->draw2DText5x8( x,y, dbStrJoin("ViewProjectionMatrix(",camera->getViewProjectionMatrix(),")"), 0xFF2BAB2B, align ); y += ln;
      m_driver->draw2DText5x8( x,y, dbStrJoin("ModelMatrix(",m_driver->getModelMatrix(),")"), 0xFF1B9BAB, align ); y += ln;
   }


/*

   if ( m_model )
   {
      size_t nV = m_model->getVertexCount();
      size_t nI = m_model->getIndexCount();
      size_t nB = m_model->getBufferCount();

      Box3f::V3 s = m_model->getAbsoluteBoundingBox().getSize();

      //auto & mesh = m_model->getMesh();

      align = Align::TopLeft;
      x = p;
      y = h/2;
      m_driver->draw2DText5x8( x,y, dbStrJoin("MeshSize = ",s), 0xFF00AEFE, align ); y += ln;
      m_driver->draw2DText5x8( x,y, dbStrJoin("Mesh.BufferCount = ",nB), 0xFF10BB10, align ); y += ln;
      m_driver->draw2DText5x8( x,y, dbStrJoin("Mesh.VertexCount = ",nV), 0xFF10BB10, align ); y += ln;
      m_driver->draw2DText5x8( x,y, dbStrJoin("Mesh.IndexCount = ",nI), 0xFF10BB10, align ); y += ln;


   }
   */
}

/*
void
draw2DShadowText( VideoDriver* driver,
   int x, int y, std::wstring const & msg, uint32_t fg_color, uint32_t bg_color,
   Align::EAlign align, Font const & font )
{
   Font small = font;
   small.pixelSize -= 4;

//   TextSize ts_font = getFontAtlas( font )->getTextSize( msg );
//   TextSize ts_small = getFontAtlas( small )->getTextSize( msg );

   //driver->draw2DText( x, y, msg, bg_color, align, font );
   driver->draw2DText( x-1, y-1, msg, bg_color, align, font );
   driver->draw2DText( x-1, y, msg, bg_color, align, font );
   driver->draw2DText( x-1, y+1, msg, bg_color, align, font );
   driver->draw2DText( x, y-1, msg, bg_color, align, font );
   driver->draw2DText( x, y, msg, bg_color, align, font );
   driver->draw2DText( x, y+1, msg, bg_color, align, font );
   driver->draw2DText( x+1, y-1, msg, bg_color, align, font );
   driver->draw2DText( x+1, y, msg, bg_color, align, font );
   driver->draw2DText( x+1, y+1, msg, bg_color, align, font );

   driver->draw2DText( x, y, msg, fg_color, align, font );
}

void
PerfOverlay::draw2DPerfOverlay()
{
   if (!m_driver) return;
   int w = m_driver->getScreenWidth();
   int h = m_driver->getScreenHeight();
   de::gpu::ICamera* camera = m_driver->getActiveCamera();

   int mx = m_driver->getMouseX();
   int my = m_driver->getMouseY();

   m_driver->draw2DLine( mx, 0, mx, h, 0x2FFFFFFF );
   m_driver->draw2DLine( 0, my, w, my, 0x2FFFFFFF );

   Font font( "arial", 24 );
   Font font2( "arial", 14 );
   TextSize ts = dbGetTextSize( L"WM_01°!", font );
   int p = 10;
   int x = p;
   int y = p;
   int ln = ts.lineHeight + p; // line-height
   Align::EAlign align = Align::TopLeft;

   auto drawFancy = [&] ( int px, int py, std::wstring const & msg,
      uint32_t fg_color, uint32_t bg_color, Align::EAlign align, Font const & font )
   {
      draw2DShadowText( m_driver, px,py,msg,fg_color,bg_color, align,font );
   };

   uint32_t grey = 0xFF505050;

   align = Align::TopLeft;
   ln = dbGetTextSize( L"WM_01°!", font ).lineHeight + p;
   x = p;
   y = p;
   {  std::wostringstream w;
      w << L"Frames(" << m_driver->getFrameCount() << L")";
      drawFancy( x,y, w.str(), 0xFF00AEFE, grey, align, font ); y += ln;
   }
   {
      std::wstringstream w;
      w << L"Time(" << m_driver->getTimer() << L")";
      drawFancy( x,y, w.str(), 0xFF0030FF, grey, align, font ); y += ln;
   }
   {
      std::wstringstream w;
      w << L"FPS(" << m_driver->getFPS() << L")";
      drawFancy( x,y, w.str(), 0xFF10BB10, grey, align, font ); y += ln;
   }
   {
      std::wstringstream w;
      w << L"Mouse(" << m_driver->getMouseX() << L"," << m_driver->getMouseY() << L")";
      drawFancy( x,y, w.str(), 0xFF10BB10, grey, align, font ); y += ln;
   }
   if ( camera )
   {
      {
         std::wostringstream w;
         w << L"Pos(" << glm::ivec3(camera->getPos()) << ")";
         drawFancy( x,y, w.str(), 0xFFFFEEEE, grey, align, font ); y += ln;
      }
      {
         std::wostringstream w;
         w << L"Ang(" << int(camera->getAng().x) << ""
           << L"," << int(camera->getAng().y) << ""
           << L"," << int(camera->getAng().z) << ")";
         drawFancy( x,y, w.str(), 0xFFFFBBBB, grey, align, font ); y += ln;
      }
      {
         std::wostringstream w;
         w << L"Dir(" << camera->getDir() << ")";
         drawFancy( x,y, w.str(), 0xFFDD6A6A, grey, align, font ); y += ln;
      }
      {
         std::wostringstream w;
         w << L"Right(" << camera->getRight() << ")";
         drawFancy( x,y, w.str(), 0xFFCC1212, grey, align, font ); y += ln;
      }
      {
         std::wostringstream w;
         w << L"FOV(" << camera->getFOV() << ")";
         drawFancy( x,y, w.str(), 0xFFaa5252, 0xFFFFFFFF, align, font ); y += ln;
      }
      {
         std::wostringstream w;
         w << L"Near(" << camera->getNearValue() << ")";
         drawFancy( x,y, w.str(), 0xFFaa52aa, grey, align, font ); y += ln;
      }
      {
         std::wostringstream w;
         w << L"Far(" << camera->getFarValue() << ")";
         drawFancy( x,y, w.str(), 0xFFaa72CC, grey, align, font ); y += ln;
      }
   }

   {
      std::wostringstream w;
      w << L"MSAA(" << m_driver->getMSAA() << ")";
      drawFancy( x,y, w.str(), 0xFFee9292, 0xFFFFFFFF, align, font ); y += ln;
   }

   {
      std::wostringstream w;
      w << L"Buffers(" << m_driver->getRenderStat(0) << ")";
      drawFancy( x,y, w.str(), 0xFFaa72CC, grey, align, font ); y += ln;
   }

   {
      std::wostringstream w;
      w << L"Vertices(" << m_driver->getRenderStat(1) << ")";
      drawFancy( x,y, w.str(), 0xFFaa72CC, grey, align, font ); y += ln;
   }

   {
      std::wostringstream w;
      w << L"Indices(" << m_driver->getRenderStat(2) << ")";
      drawFancy( x,y, w.str(), 0xFFaa72CC, grey, align, font ); y += ln;
   }

   {
      std::wostringstream w;
      w << L"Primitves(" << m_driver->getRenderStat(3) << ")";
      drawFancy( x,y, w.str(), 0xFFaa72CC, grey, align, font ); y += ln;
   }

   align = Align::TopRight;
   ln = dbGetTextSize( L"WM_01°!", font ).lineHeight + p;
   x = w - p;
   y = p;

   {  std::wostringstream w;
      w << L"Screen(" << m_driver->getScreenWidth() << L","
              << m_driver->getScreenHeight()<< L","
              << m_driver->getScreenFormat().toString().c_str() << L")";
      drawFancy( x,y, w.str(), 0xFF00EEEE, grey, align, font ); y += ln;
   }

   align = Align::BottomRight;
   ln = dbGetTextSize( L"WM_01°!").lineHeight + p;
   x = w - p;
   y = h - p;
   {
      std::wostringstream w; w << L"(c) 2021 <benjaminhampe@gmx.de>";
      drawFancy( x,y, w.str(), 0xFFEEEEEE, grey, align, font ); y += ln;
   }

   if ( camera )
   {
      align = Align::BottomLeft;
      ln = dbGetTextSize( L"WM_01°!", font ).lineHeight + p;
      x = p;
      y = h - p - 3*ln;
      {
         std::wostringstream w;
         w << L"ViewMatrix(" << camera->getViewMatrix() << ")";
         drawFancy( x,y, w.str(), 0xFF1BAB1B, grey, align, font2 ); y += ln;
      }
      {
         std::wostringstream w;
         w << L"ProjectionMatrix(" << camera->getProjectionMatrix() << ")";
         drawFancy( x,y, w.str(), 0xFF3CBB3B, grey, align, font2 ); y += ln;
      }
      {
         std::wostringstream w;
         w << L"ViewProjectionMatrix(" << camera->getViewProjectionMatrix() << ")";
         drawFancy( x,y, w.str(), 0xFF2BAB2B, grey, align, font2 ); y += ln;
      }
//      {
//         std::wostringstream w;
//         w << L"ModelMatrix(" << m_driver->getModelMatrix() << ")";
//         drawFancy( x,y, w.str(), 0xFFAB3B3B, align, font2 ); y += ln;
//      }
   }
}
*/

} // end namespace gpu.
} // end namespace de.
