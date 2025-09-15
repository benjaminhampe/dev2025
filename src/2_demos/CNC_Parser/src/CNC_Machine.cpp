#include "CNC_Machine.hpp"

namespace de {
namespace cnc {

CNC_Machine::CNC_Machine()
   : m_X('X'), m_Y('Y'), m_Z('Z')
   , m_A('A'), m_B('B'), m_C('C')
   , m_E('E'), m_F('F'), m_S('S')
   , m_relativeMode(0)
   , m_imperialMode(0)
   , m_moveMode(0)
   , m_dummy1(0)
   , m_feedSpeed(40.0) // 40mm/min
   , m_travelDistance(0.0)
   , m_parseTimeTotal(0.0)
   , m_parseTimeStart(0.0)
{

}

void
CNC_Machine::parserStarted( std::string uri, std::string sourceText )
{
   m_parseTimeStart = dbTimeInSeconds();
   m_uri = uri;                  // for debugging
   m_sourceText = sourceText;    // for debugging
}
void
CNC_Machine::parserEnded()
{
   m_parseTimeTotal = dbTimeInSeconds() - m_parseTimeStart;
}

//   void parseG00()
//   {
//      m_moveMode = 0;
//   }

// static
void
CNC_Machine::skipBlock( CNC_Block & block )
{
   if ( block.m_inputWords.empty() )
   {
      // Nothing todo, we parsed all words, congrats...
      return;
   }
   for ( size_t i = 0; i < block.m_inputWords.size(); ++i )
   {
      block.m_skippedWords.emplace_back( block.m_inputWords[i] );
   }
   block.m_inputWords.clear();
}

//static
void
CNC_Machine::markWord( CNC_Block & block, s32 i, u32 color )
{
   if ( block.m_inputWords.size() < 1 )
   {
      return;
   }

   if ( i < 0 || i >= s32(block.m_inputWords.size()) )
   {
      std::cout << "[Error] CNC_Machine.markWord(" << i << ") :: "
                   "Invalid index of "<< block.m_inputWords.size() << std::endl;
      return;
   }

   // Add color marker
   block.m_inputWords[ size_t( i ) ].m_color = color;

   // Add word to parsedWords
   block.m_parsedWords.emplace_back( block.m_inputWords[ size_t( i ) ] );

   // Delete word from inputWords
   block.m_inputWords.erase( block.m_inputWords.begin() + i );

   //      if ( n0 != n1 )
   //      {
   //         std::cout << "[Error] CNC_Machine.markParsed(" << i << ") :: "
   //                      "Invalid action: n0("<<n0<<") != n1("<<n1<<")" << i << std::endl;
   //         return;
   //      }
}

//static
void
CNC_Machine::skipWord( CNC_Block & block, s32 i )
{
   if ( block.m_inputWords.size() < 1 )
   {
      return;
   }

   if ( i < 0 || i >= s32(block.m_inputWords.size()) )
   {
      std::cout << "[Error] CNC_Machine.markSkipped(" << i << ") :: "
                   "Invalid index of "<< block.m_inputWords.size() << std::endl;
      return;
   }

   // Add word to skippedWords
   block.m_skippedWords.emplace_back( block.m_inputWords[ size_t( i ) ] );

   // Delete word from inputWords
   block.m_inputWords.erase( block.m_inputWords.begin() + i );

   //      if ( n0 != n1 )
   //      {
   //         std::cout << "[Error] CNC_Machine.markParsed(" << i << ") :: "
   //                      "Invalid action: n0("<<n0<<") != n1("<<n1<<")" << i << std::endl;
   //         return;
   //      }
}

void
CNC_Machine::parseBlock( CNC_Block givenBlock )
{
   // Store first
   m_blocks.emplace_back( givenBlock );
   // The manipulate, if any. By default nothing of block is parsed ( no outputwords )
   CNC_Block & block = m_blocks.back();

   // Copy into work buffer. Remove/parse items from work buffer until empty.
   block.m_inputWords = block.m_originalWords;
   std::vector< CNC_Word > & inputs = block.m_inputWords;

   if ( inputs.size() < 1 )
   {
      // std::cout << "[Error] CNC_Machine.parseBlock("<<block.toString()<<"), no words" << std::endl;
      return;
   }
   s32 MS = CNC_Block::findWord( inputs, '%');
   if ( MS > -1 )
   {
      // std::cout << "[Error] CNC_Machine.parseBlock("<<block.toString()<<"), Found %, interpret as comment" << std::endl;
      skipBlock( block );
      return;
   }

   s32 M = CNC_Block::findWord( inputs, 'M');
   if ( M > -1 )
   {
      // std::cout << "[Error] CNC_Machine.parseBlock("<<block.toString()<<"), Found M, unsupported" << std::endl;
      skipBlock( block );
      return;
   }

   u32 colorG0 = 0xFF008A00;
   u32 colorG1 = 0xFF00AA00;
   u32 colorG20 = 0xFFA06060;
   u32 colorG21 = 0xFFA06060;
   u32 colorG90 = 0xFFA06060;
   u32 colorG91 = 0xFFA06060;

   u32 colorX = 0xFF606080;
   u32 colorY = 0xFF608060;
   u32 colorZ = 0xFF806060;

   u32 colorF = 0xFF6080A0;

   s32 G = CNC_Block::findWord( inputs, 'G');
   while ( G > -1 )
   {
      int Gvalue = inputs[ size_t(G) ].i();
      if ( Gvalue == 20 )
      {
         inputs[ size_t(G) ].m_color = 0xFF0080DF; // state change color
         m_imperialMode = 1;  //
         block.addDebugLog("G20 - Imperial mode (inch)");
         markWord( block, G, colorG20 );
         G = CNC_Block::findWord( inputs, 'G' );
      }
      else if ( Gvalue == 21 )
      {
         m_imperialMode = 0; //word.m_color = 0xFF0080DF; // state change color
         block.addDebugLog("G21 - Metric mode (mm)");
         markWord( block, G, colorG21 );
         G = CNC_Block::findWord( inputs, 'G' );
      }
      else if ( Gvalue == 90 )
      {
         m_relativeMode = 0;  //word.m_color = 0xFF0080DF; // state change color
         block.addDebugLog("G90 - Absolute mode");
         markWord( block, G, colorG90 );
         G = CNC_Block::findWord( inputs, 'G' );
      }
      else if ( Gvalue == 91 )
      {
         m_relativeMode = 1; //word.m_color = 0xFF0080DF; // state change color
         block.addDebugLog("G91 - Relative mode");
         markWord( block, G, colorG91 );
         G = CNC_Block::findWord( inputs, 'G' );
      }
      else if ( Gvalue == 0 || Gvalue == 1 )
      {
         s32 F = CNC_Block::findWord( inputs, 'F' );
         if ( F > -1 && F < s32(inputs.size()) )
         {
            CNC_Word & f = inputs[ size_t(F) ];
            double Fvalue = f.m_value;
            if ( m_imperialMode == 1 ) { Fvalue *= 2.56; } // convert inch to mm
            m_feedSpeed = Fvalue;
            markWord( block, F, colorF );
         }

         if ( Gvalue == 0 )
         {
            m_moveMode = 0;
            std::ostringstream o;
            o << "G00 MoveTo(" << m_feedSpeed << "mm/min)";
            block.addDebugLog(o.str());
            markWord( block, G, colorG0 );
         }
         else
         {
            m_moveMode = 1;
            std::ostringstream o;
            o << "G01 LineTo(" << m_feedSpeed << "mm/min)";
            block.addDebugLog(o.str());
            markWord( block, G, colorG1 );
         }

         glm::dvec3 old_position = m_currPos;
         glm::dvec3 new_position = old_position;

         s32 X = CNC_Block::findWord( inputs, 'X' );
         if ( X > -1 && X < s32(inputs.size()) )
         {
            CNC_Word & x = inputs[ size_t(X) ];
            double Xvalue = x.m_value;
            if ( m_imperialMode == 1 ) { Xvalue *= 2.56; } // convert inch to mm
            m_X.move( Xvalue, m_relativeMode, m_feedSpeed );
            block.addDebugLog( m_X.toString());
            markWord( block, X, colorX );
            new_position.x = m_X.m_currValue;
         }
         s32 Y = CNC_Block::findWord( inputs, 'Y' );
         if ( Y > -1 && Y < s32(inputs.size()) )
         {
            CNC_Word & y = inputs[ size_t(Y) ];
            double Yvalue = y.m_value;
            if ( m_imperialMode == 1 ) { Yvalue *= 2.56; } // convert inch to mm
            m_Y.move( Yvalue, m_relativeMode, m_feedSpeed );
            block.addDebugLog(m_Y.toString());
            markWord( block, Y, colorY );
            new_position.y = m_Y.m_currValue;
         }
         s32 Z = CNC_Block::findWord( inputs, 'Z' );
         if ( Z > -1 && Z < s32(inputs.size()) )
         {
            CNC_Word & z = inputs[ size_t(Z) ];
            double Zvalue = z.m_value;
            if ( m_imperialMode == 1 ) { Zvalue *= 2.56; } // convert inch to mm
            m_Z.move( Zvalue, m_relativeMode, m_feedSpeed );
            block.addDebugLog(m_Z.toString());
            markWord( block, Z, colorZ );
            new_position.z = m_Z.m_currValue;
         }

         u32 lineColor = 0x08FFFFFF;
         double dx = std::abs(new_position.x - old_position.x);
         double dy = std::abs(new_position.y - old_position.y);
         double dz = std::abs(new_position.z - old_position.z);
         double dm = std::max( dx, std::max( dy, dz ) );

         if ( std::abs(dm-dx) < 0.001 )
         {
            lineColor = 0x0800008F;
         }
         else if ( std::abs(dm-dy) < 0.001 )
         {
            lineColor = 0x08008F00;
         }
         else if ( std::abs(dm-dz) < 0.001 )
         {
            lineColor = 0x088F0000;
         }

         m_colorLineStrip.emplace_back( new_position, lineColor );
         m_travelDistance += glm::length( new_position - old_position );
         m_currPos = new_position;
      }
      else
      {
         skipWord( block, G );
      }
      G = CNC_Block::findWord( inputs, 'G' );
   }

   skipBlock( block );
}

glm::ivec4
CNC_Machine::transformLine( glm::vec3 A, glm::vec3 B, glm::mat4 const & mvp, int w, int h )
{
   std::swap(A.y,A.z);
   std::swap(B.y,B.z);
   glm::vec4 clipSpaceA = mvp * glm::vec4(A,1.0f);
   glm::vec4 clipSpaceB = mvp * glm::vec4(B,1.0f);
   clipSpaceA /= clipSpaceA.w;
   clipSpaceB /= clipSpaceB.w;
   glm::vec2 const screenSize( w, h );
   glm::vec2 ndcA = screenSize * ((glm::vec2(clipSpaceA.x,clipSpaceA.y) * 0.5f) + 0.5f);
   glm::vec2 ndcB = screenSize * ((glm::vec2(clipSpaceB.x,clipSpaceB.y) * 0.5f) + 0.5f);

   return glm::ivec4( int(ndcA.x), int(ndcA.y), int(ndcB.x), int(ndcB.y) );
}


glm::ivec4
CNC_Machine::clipLine( glm::ivec4 const & ab, int w, int h )
{
   s32 Ax = ab.x; // gimme timme
   s32 Ay = ab.y;
   s32 Bx = ab.z;
   s32 By = ab.w;

   if ( Ax > Bx ) std::swap(Ax,Bx); // sanitize
   if ( Ay > By ) std::swap(Ay,By);

   float fAx = Ax; // gimme timme
   float fAy = Ay;
   float fBx = Bx;
   float fBy = By;

   float dx = fBx - fAx; // delta-x
   float dy = fBy - fAy; // delta-y

   float m = float( dy ) / float( dx ); // Anstieg m = dy / dx
   float n = ((fAx*fBy) - (fAy*fBx)) / (fAx - fBx); // Benni math

   if ( std::isinf(m) || std::isnan(m) )
   {
      return glm::ivec4(0,0,0,0); // guess we dont draw this line.
   }

   if ( std::isinf(n) || std::isnan(n) )
   {
      return glm::ivec4(0,0,0,0); // guess we dont draw this line.
   }

   // Test benni math: Must be able to generate A,B from y=f(x)=mx+n
   float tAy = fAx * m + n;
   float tBy = fBx * m + n;

   auto almostEqual = [] ( float a, float b, float eps = 1.0 ) -> bool
   {
      return std::abs( b-a ) <= eps;
   };

   if ( !almostEqual(tAy,fAy) )
   {
      std::cout << "tAy("<<tAy<<") != fAy("<<fAy<<")" << std::endl;
   }

   if ( !almostEqual(tBy,fBy) )
   {
      std::cout << "tBy("<<tBy<<") != fBy("<<fBy<<")" << std::endl;
   }

   if ( fAx < 0 ) { fAx = 0; fAy = n; }
   if ( fAx > w ) { fAx = w; fAy = m*w + n; }
   if ( fBx < 0 ) { fBx = 0; fBy = n; }
   if ( fBx > w ) { fBx = w; fBy = m*w + n; }

   if ( fAy < 0 ) { fAx = -n/m; fAy = 0; }
   if ( fAy > h ) { fAx = (h-n)/m; fAy = h; }
   if ( fBy < 0 ) { fBx = -n/m; fBy = 0; }
   if ( fBy > h ) { fBx = (h-n)/m; fBy = h; }
   return glm::ivec4( fAx, fAy, fBx, fBy );
}

void
CNC_Machine::drawColorLine( Image & img,
          glm::vec3 A, glm::vec3 B, u32 colorA, u32 colorB, glm::mat4 const & mvp, bool blend )
{
   glm::vec4 clipSpaceA = mvp * glm::vec4(A,1.0f);
   glm::vec4 clipSpaceB = mvp * glm::vec4(B,1.0f);
   clipSpaceA /= clipSpaceA.w;
   clipSpaceB /= clipSpaceB.w;
   glm::vec2 const screenSize( img.w(), img.h() );
   glm::vec2 ndcA = screenSize * ((glm::vec2(clipSpaceA.x,clipSpaceA.y) * 0.5f) + 0.5f);
   glm::vec2 ndcB = screenSize * ((glm::vec2(clipSpaceB.x,clipSpaceB.y) * 0.5f) + 0.5f);

   ImagePainter::drawLine( img, int(ndcA.x), int(ndcA.y), int(ndcB.x), int(ndcB.y), colorA, blend );

}

void
CNC_Machine::saveMeshJSLines( std::string saveUri )
{
   PerformanceTimer perf;
   perf.start();

   std::ostringstream o;
   o << "var linesCount = " << m_colorLineStrip.size() << ";\n";
   o << "var linesPositions = [\n\t";

   size_t k = 0;
   for ( size_t i = 1; i < m_colorLineStrip.size(); ++i )
   {
      auto const & A = m_colorLineStrip[ i-1 ];
      auto const & B = m_colorLineStrip[ i ];
      if ( i > 1 ) { o << ","; }
      o << A.pos << "," << B.pos;
      k++;
      if ( k > 32 ) { o << "\n\t"; k = 0; }
   }
   o << "];\n";

   std::ofstream file( saveUri.c_str() );
   std::string js = o.str();

   if ( file.is_open() )
   {
      file << js;
      file.close();
   }

   perf.stop();
   DE_DEBUG("Save lines mesh JS needed(", perf.ms(),"ms), ", saveUri, " with ", js.size(), " bytes.")
}

void
CNC_Machine::saveMeshJSLineStrip( std::string saveUri )
{
   PerformanceTimer perf;
   perf.start();

   std::ostringstream o;
   o << "var lineStripCount = " << m_colorLineStrip.size() << ";\n";
   o << "var lineStripPositions = [\n\t";

   size_t k = 0;
   for ( size_t b = 0; b < m_colorLineStrip.size(); ++b )
   {
      CNC_ColorVertex const & vertex = m_colorLineStrip[ b ];
      if ( b > 0 ) { o << ","; }
      o << vertex.pos;
      k++;
      if ( k > 16 ) { o << "\n\t"; k = 0; }
   }
   o << "];\n";

   o << "var lineStripColors = [\n\t";
   k = 0;
   for ( size_t b = 0; b < m_colorLineStrip.size(); ++b )
   {
      CNC_ColorVertex const & vertex = m_colorLineStrip[ b ];
      if ( b > 0 ) { o << ","; }
      o << "0x" << de::StringUtil::hex(vertex.color);
      k++;
      if ( k > 16 ) { o << "\n\t"; k = 0; }
   }
   o << "];\n";

   std::ofstream file( saveUri.c_str() );
   std::string js = o.str();

   if ( file.is_open() )
   {
      file << js;
      file.close();
   }

   perf.stop();
   DE_DEBUG("Save lineStrip mesh JS needed(", perf.ms(),"ms), ", saveUri, " with ", js.size(), " bytes.")
}

// t_in_min
std::string strMinutes(double t )
{
   std::ostringstream o;

   if ( t < 1 )
   {
      o << (t*60.0) << "sec";
   }
   else if ( t >= 60 )
   {
      int hh = int(t / 60.0);
      double mm = t - 60.0*hh;
      o << hh << "h" << int(mm) << "min";
   }
   else
   {
      o << t << "min";
   }
   return o.str();
}

std::string
CNC_Machine::save( std::string loadUri )
{
   DE_DEBUG("Parser handles uri ", loadUri)
   DE_DEBUG("Parser needed ", m_parseTimeTotal, " seconds.")

   std::string title = de::FileSystem::fileName(loadUri);
   std::string htmlUri = loadUri + ".html";
   std::string stripUri = loadUri + ".lineStrip.js";
   std::string linesUri = loadUri + ".lines.js";
   std::string mainUri = loadUri + ".main.js";
   std::string prevUri = loadUri + ".preview.jpg";
   std::string wallUri = loadUri + ".wallpaper.jpg";

   FileSystem::copyFile( "../../src/CNC_Parser/js/main.js", mainUri );

   // Create wallpaper image
   Image wallpaper = CNC_Draw3D::createWallpaperImage( 1024,768,256 );
   dbSaveImage( wallpaper, wallUri );

   // Create line mesh .js // TODO: use strip, save some memory.
   //saveMeshJSLines( linesUri );
   //saveMeshJSLineStrip( stripUri );

   //std::string jsMesh = TextUtil::loadText( saveUri + ".mesh.js");

   // Compute BoundingBox
   auto bbox = CNC_ColorVertex::computeBoundingBox( m_colorLineStrip );
   auto bboxSize = bbox.Max - bbox.Min;

   // Create preview image
   Image previewImage = CNC_Draw3D::createPreviewImage( 1024, 768, m_colorLineStrip );
   dbSaveImage( previewImage, prevUri );

   // Save html text
   PerformanceTimer perf;
   perf.start();
   double minTime = std::max( m_X.m_timeConsumption, std::max( m_Y.m_timeConsumption, m_Z.m_timeConsumption ) );
   double maxDist = std::max( m_X.m_travelDistance, std::max( m_Y.m_travelDistance, m_Z.m_travelDistance ) );
   double expTime = minTime * 1.5;
   std::ostringstream o; o <<
   "<html>\n"
   "<title>CNC_Parser: "<<loadUri<< "</title>\n"
   "<style type=\"text/css\">\n"
   "*    { margin:0; border:0; padding:0; }\n"
   //"html { margin:0; height:100%; }\n"
   "body {\n"
   "     width:100%;\n"
   "     height:100%;\n"
   "     color:rgb(0,0,0);\n"
   //"     background-color:rgb(255,255,255);\n"
   "     background:url(./" << de::FileSystem::fileName(wallUri) << ");\n"
   "     background-repeat: y-repeat;\n"
   "     background-size: cover;\n"
   "     //background-size: 100% 100%;\n"
   "     font-family:monospace;\n"
   "     font-size:16px;\n"
   "}\n"
   "#canvas_div { width:100%; height:100%; display: block; }\n"
   "#canvas { width:800px; height:600px; display: block; }\n"
   "content { position:absolute; top:50px; padding-top:20px; white-space:nowrap; z-index:1;}\n"
   "table { width:100%; margin:1 1 0 0; padding:1 1 0 0; }\n"
   "#c1 { font-size:16pt; font-weight:400; font-style:italic; padding-left:2px; padding-right:2px;}\n"
   "#t1 { background-color:#808080; color:#FFFFFF; padding-right:20px; text-align:right; }\n"
   "#t2 { background-color:#D0D0D0; color:black; padding-right:20px; text-align:left; }\n"
   "#t3 { background-color:#E0E0E0; color:black; padding-right:20px; text-align:left; }\n"
   "#t4 { background-color:#C8C8C8; color:#666666; padding-right:20px; text-align:left; }\n"
   "#stat{background-color:#80AA80; top:0px; left:0px; width:100%; height:auto; z-index:2; }\n"
   "</style>\n"
   //"<script src=\"./" << de::FileSystem::fileName(linesUri) << "\" type=\"text/javascript\"></script>\n"
   //"<script src=\"./" << de::FileSystem::fileName(mainUri) << "\" type=\"text/javascript\"></script>\n"
   "</head>\n"
   "<body onload=\"main()\">\n"

   // ---------------------- title (=loadUri) ----------------------------------
   "<div style=\""
         "background-color:#BD7920; " //opacity:0.5;"
         "position:fixed; "
         "top:0px; "
         "left:0px; "
         "width:100%; "
         //"height:64px; "
         "line-height:32px; "
         "padding-left:10px; "
         "padding-top:10px; "
         "padding-right:20px; "
         "padding-bottom:10px; "
         "z-index:2;\">\n"
   "<table style=\"vertical-align:middle;\">\n"
   "<tr valign=\"middle\">\n"
   "<td align=\"left\">\n"
   "<span style=\"color:black; font-weight:bold; font-size:16px; vertical-align:middle;\">\n"
   << "ExpectedTime(<span id=\"c1\">" << strMinutes(expTime) << "</span>)"
   << "ComputedTime(<span id=\"c1\">" << strMinutes(minTime) << "</span>)</span>\n"
   "</td>\n"

   "<td align=\"center\">\n"
      "<span style=\"color:white; font-weight:bold; font-size:16px; vertical-align:middle;\">\n"
      << title << "</span>\n"
   "</td>\n"

   "<td align=\"left\">\n"
   "<span style=\"color:black; font-weight:bold; font-size:16px; vertical-align:middle;\">\n"
   << "TravelDistance(<span id=\"c1\">" << (0.001*maxDist) << "m</span>)</span>\n"
   "</td>\n"
   "</tr>\n"
   "</table>\n"
   "</div>\n\n";

   // ---------------------- content start ----------------------------------
   o <<
        "<content>\n\n"

   // ---------------------- statistics ----------------------------------
   "<h1>CNC_Parser statistics:</h1>\n"
   "<div style=\""
        //"width:100%;"
        "padding:20px 20px 20px 20px;"
        "background-color:#209920; "
        "z-index:2;\">\n"

   "<div style=\""
         //"width:100%; height:100%;"
         "background-color:#205920; "
         "z-index:2;\">\n"

   "<table style=\"background-color:#80AA80;\">\n"
   "<tr>"
   "<td>BoundingBox</td>"
   "<td align=\"center\"><span id=\"c1\">Size</span></td>"
   "<td>X(<span id=\"c1\">" << bboxSize.x << "mm</span>)</td>"
   "<td>Y(<span id=\"c1\">" << bboxSize.y << "mm</span>)</td>"
   "<td>Z(<span id=\"c1\">" << bboxSize.z << "mm</span>)</td>"
   "</tr>\n"
   "<tr>"
   "<td>BoundingBox</td>"
   "<td align=\"center\"><span id=\"c1\">Min</span></td>"
   "<td>X(<span id=\"c1\">" << bbox.Min.x << "mm</span>)</td>"
   "<td>Y(<span id=\"c1\">" << bbox.Min.y << "mm</span>)</td>"
   "<td>Z(<span id=\"c1\">" << bbox.Min.z << "mm</span>)</td>"
   "</tr>\n"
   "<tr>"
   "<td>BoundingBox</td>"
   "<td align=\"center\"><span id=\"c1\">Max</span></td>"
   "<td>X(<span id=\"c1\">" << bbox.Max.x << "mm</span>)</td>"
   "<td>Y(<span id=\"c1\">" << bbox.Max.y << "mm</span>)</td>"
   "<td>Z(<span id=\"c1\">" << bbox.Max.z << "mm</span>)</td></tr>\n"
   "<tr>"
   "<tr><td>Register</td>"
     "<td align=\"center\"><span id=\"c1\">X</span></td>"
     "<td>TravelDistance(<span id=\"c1\">" << m_X.m_travelDistance * 0.001 << "m</span>)</td>"
     "<td>TimeConsumption(<span id=\"c1\">" << m_X.m_timeConsumption << "min</span>)</td>"
     "<td>FeedSpeedAverage(<span id=\"c1\">" << m_X.m_travelDistance / m_X.m_timeConsumption << "mm/min</span>)</td>"
     "<td>CallCounter(<span id=\"c1\">" << m_X.m_callCounter << "</span>)</td></tr>\n"
   "<tr><td>Register</td>"
      "<td align=\"center\"><span id=\"c1\">Y</span></td>"
      "<td>TravelDistance(<span id=\"c1\">" << m_Y.m_travelDistance * 0.001 << "m</span>)</td>"
      "<td>TimeConsumption(<span id=\"c1\">" << m_Y.m_timeConsumption << "min</span>)</td>"
      "<td>FeedSpeedAverage(<span id=\"c1\">" << m_Y.m_travelDistance / m_Y.m_timeConsumption << "mm/min</span>)</td>"
      "<td>CallCount(<span id=\"c1\">" << m_Y.m_callCounter << "</span>)</td></tr>\n"
   "<tr><td>Register</td>"
      "<td align=\"center\"><span id=\"c1\">Z</span></td>"
      "<td>TravelDistance(<span id=\"c1\">" << m_Z.m_travelDistance * 0.001 << "m</span>)</td>"
      "<td>TimeConsumption(<span id=\"c1\">" << m_Z.m_timeConsumption << "min</span>)</td>"
      "<td>FeedSpeedAverage(<span id=\"c1\">" << m_Z.m_travelDistance / m_Z.m_timeConsumption << "mm/min</span>)</td>"
      "<td>CallCounter(<span id=\"c1\">" << m_Z.m_callCounter << "</span>)</td></tr>\n"
   "</table>\n"
   "</div>\n"
   "</div>\n"

   // ---------------------- preview image ----------------------------------
   "<h1>CNC_Parser: Preview image</h1>\n"
   "<table style=\"background-color:#8080AA;\">\n"
   "<tr>\n"
   "<td align=\"center\"><img src=\"./" << de::FileSystem::fileName(prevUri) << "\" /></td>\n"
   "</tr>"
   "</table>\n"

/*
   // ---------------------- preview lines WebGL mesh ----------------------------------
   "<h3 style=\"flex:center\">CNC_Canvas</h3>\n"
   "<div id=\"canvas_div\">\n"
   "  <canvas id=\"canvas\" width=\"800\" height=\"600\">"
                    "Please use a real browser"
   "  </canvas>\n"
   "</div>\n"
*/

   // ---------------------- javascript mesh ----------------------------------
   //  "<table style=\"background-color:#8080AA;\">\n"
   //  "<tr>\n"
   //  "<td align=\"center\"><pre>" << jsMesh << "</pre></td>\n"
   //  "</tr>"
   //  "</table>\n"

   // ---------------------- wallpaper image ----------------------------------
   //   "<h1>libDarkImage demo wallpaper image:</h1>\n"
   //   "<table style=\"background-color:#8080AA;\">\n"
   //   "<tr>\n"
   //   "<td align=\"center\"><img src=\"" << de::FileSystem::fileName(wallUri) << "\" /></td>\n"
   //   "</tr>"
   //   "</table>\n"


/*
   // ---------------------- CNC_Parser tokens --------------------------------
   "<h1>CNC_Parser result table:</h1>\n"
   "<table style=\"background-color:#AA8080;\">\n"
   "<tr>\n"
   "\t<th width=\"80\">#</th>\n"
   "\t<th width=\"700\">Original line</th>\n"
   "\t<th width=\"700\">Parsed Block</th>\n"
   "\t<th width=\"2400\">DebugLog</th>\n"
   "</tr>\n";

   s64 iPartI = s64( std::min( m_blocks.size(), size_t(100) ) );
   for ( size_t i = 0; i < size_t( iPartI ); ++i )
   {
      CNC_Block const & block = m_blocks[ i ];
      o << "<tr>"
           "<td id=\"t1\">" << i << "</td>"
           "<td id=\"t2\">" << block.m_originalLine << "</td>"
           "<td id=\"t3\">" << block.toHtml() << "</td>"
           "<td id=\"t4\"><pre>";
      for ( size_t d = 0; d < block.m_debugLog.size(); ++d )
      {
         if ( d > 0 ) o << " ";
         o << block.m_debugLog[ d ];
      }
      o << "</pre></td></tr>\n";
   }


   s64 iPartII = s64( m_blocks.size() ) - 100;
   if (iPartII > 99 )
   {
      for ( size_t i = size_t( iPartII ); i < m_blocks.size(); ++i )
      {
         CNC_Block const & block = m_blocks[ i ];
         o << "<tr>"
              "<td id=\"t1\">" << i << "</td>"
              "<td id=\"t2\">" << block.m_originalLine << "</td>"
              "<td id=\"t3\">" << block.toHtml() << "</td>"
              "<td id=\"t4\"><pre>";
         for ( size_t d = 0; d < block.m_debugLog.size(); ++d )
         {
            if ( d > 0 ) o << " ";
            o << block.m_debugLog[ d ];
         }
         o << "</pre></td></tr>\n";
      }

   }


   o << "\t</table>\n"


*/

/*
   for ( size_t b = 0; b < m_blocks.size(); ++b )
   {
      CNC_Block const & block = m_blocks[ b ];
      o << "\t\t<tr>"
           "\t\t\t<td id=\"t1\">" << b << "</td>"
                 "<td id=\"t2\">" << block.m_originalLine << "</td>"
                 "<td id=\"t3\">" << block.toHtml() << "</td>"
                 "<td id=\"t4\">";
      for ( size_t d = 0; d < block.m_debugLog.size(); ++d )
      {
         if ( d > 0 ) o << " ";
         o << block.m_debugLog[ d ];
      }
      o <<
      "</td></tr>\n";
   }
*/

   "\t<br />"
   "\t<br />"
   "\t<br />"
   "\t<br />"
   "\t<br />"
   "</content>\n"

   // ---------------------------- footer ----------------------------------
   "<footer style=\"background-color:#202020; color:white; position:fixed; bottom:0px; left:0px; width:100%; height:50px; line-height:50px; z-index:3;\">\n"
   "<table width=\"100%\" height=\"100%\">\n"
   "<tr valign=\"middle\"><td align=\"center\"><h3 style=\"color:white;\">log-file produced by class GCodeParser</h3></td></tr>\n"
   "<tr valign=\"middle\"><td align=\"center\"><h6 style=\"color:white;\">(c) 2023 by BenjaminHampe@gmx.de</h6></td></tr>\n"
   "</table>\n"
   "</footer>\n"
   "</body>\n"
   "</html>\n";

   std::ofstream file( htmlUri.c_str() );
   std::string htmlContent = o.str();

   if ( file.is_open() )
   {
      file << htmlContent;
      file.close();
   }

   perf.stop();
   DE_DEBUG("Save HTML needed(",perf.ms(),"ms), ",
            htmlUri, " with ", htmlContent.size(), " bytes.")

   return htmlUri;
}

} // end namespace cnc
} // end namespace de
