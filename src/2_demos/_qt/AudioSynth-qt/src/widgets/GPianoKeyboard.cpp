#include "GPianoKeyboard.hpp"
#include <QPainter>
#include <QDebug>

GPianoKeyboard::GPianoKeyboard( QWidget* parent )
   : QWidget( parent ) // , Qt::NoDropShadowWindowHint
   , m_wantImageUpdate( true )
   , m_timerId( 0 )
   , m_keyStart( 0 )
   , m_timeLastNote( 0.0 )
   , m_timeNote( 0.0 )
{
   setContentsMargins( 0,0,0,0 );
   setMinimumSize( 120, 64 );
   setFocusPolicy( Qt::StrongFocus );
   setMouseTracking( true );

   // Piano:
   for ( auto & synth : m_synths ) { synth = nullptr; }
   for ( auto & key : m_touched ) { key = nullptr; }
   setKeyRange( 24, 100 );

//   de::Image imgC = createImage( 0, 24, 64 );
//   dbSaveImage( imgC, "Audio_imgC.png");
//   QImage m_imgC = createKeyImage( 0, 24, 64 );
//   m_imgC.save("Audio_m_imgC.png");
   updateImage();
}

void
GPianoKeyboard::timerEvent( QTimerEvent* event )
{
   QWidget::timerEvent( event );
/*
   if ( event->timerId() == m_timerId )
   {
      update();
   }
*/
}

// Piano:
int32_t
GPianoKeyboard::findKey( int midiNote ) const
{
   auto it = std::find_if( m_keys.begin(), m_keys.end(), [&] (Key const & key) { return key.midiNote == midiNote; });
   if ( it != m_keys.end() )
   {
      return int32_t( std::distance( m_keys.begin(), it ) );
   }
   else
   {
      return -1;
   }
}

// Piano:
void
GPianoKeyboard::setKeyRange( int midiNoteStart, int midiNoteCount )
{
   m_keyStart = midiNoteStart;
   m_keys.clear();

   for ( int i = 0; i < midiNoteCount; i++ )
   {
      int midiNote = m_keyStart + i;
      int oktave = midiNote / 12;
      int semitone = midiNote - 12 * oktave;
      m_keys.emplace_back( midiNote, oktave, semitone );
   }
}

namespace {

void drawKey( QPainter & dc, QRect pos, QColor brushColor, QColor penColor )
{
   int x = pos.x()+1;
   int y = pos.y()+1;
   int w = pos.width()-1;
   int h = pos.height()-2;
   dc.setPen( Qt::NoPen );
   dc.setBrush( QBrush( brushColor ) );
   dc.drawRect( QRect(x+1,y+1,w-2,h-2) );

   dc.setPen( QPen( penColor ) );
   dc.drawLine( x+1,y, x+w-2, y );
   dc.drawLine( x+1,y+h-1, x+w-2, y+h-1 );
   dc.drawLine( x,y+1, x, y+h-2 );
   dc.drawLine( x+w-1,y+1, x+w-1, y+h-2 );
}


} // end namespace

void
GPianoKeyboard::updateImage()
{
   if ( !m_wantImageUpdate )
   {
      return;
   }
   m_wantImageUpdate = false;

   QImage m_image;
   QPainter dc(&m_image);
   QRect r = m_image.rect();
   int w = m_image.width();
   int h = m_image.height();
   int gw = 20;
   int gh = 64;
   int k = w / gw;

   QColor whiteBrushColor(255,255,255);
   QColor whitePenColor(205,205,205);
   QColor blackBrushColor(2,2,2);
   QColor blackPenColor(65,65,65);

   //dc.setRenderHint( QPainter::NonCosmeticDefaultPen );
   dc.fillRect( r, QColor( 10,10,10 ) );

   //m_keys[ keyIndex ].pressed = true;
   //int keyCount = std::min( int(m_keys.size()), k );
   //int whiteCount = std::min( int(m_keys.size()), k );

   int x = 0;
   int y = 0;

   for ( int i = 0; i < k+1; ++i )
   {
      drawKey( dc, QRect(x,y,gw,gh), whiteBrushColor, whitePenColor );
      x += gw;
   }

   x = 12;
   y = 0;

   for ( int i = 0; i < k; ++i )
   {
      int semi = i % 7;
      if ( semi != 2 && semi != 6 )
      {
         drawKey( dc, QRect(x,y,16,32), blackBrushColor, blackPenColor );
      }
      x += 20;
   }

//   m_font5x8.drawText( dc, w/2, 1, QString("L_min(%1), L_max(%2), R_min(%3), R_max(%4)")
//      .arg( L_min).arg( L_max ).arg( R_min).arg( R_max ), 0xFFFFFFFF, de::Align::CenterTop );

   //dc.end();
}

void
GPianoKeyboard::resizeEvent( QResizeEvent* event )
{
   QWidget::resizeEvent( event );
   m_wantImageUpdate = true;
   update();
}

void
GPianoKeyboard::paintEvent( QPaintEvent* event )
{
   updateImage();

   QPainter dc(this);
   //dc.setRenderHint( QPainter::NonCosmeticDefaultPen );
   //dc.fillRect( rect(), QColor( 10,10,10 ) );

   int w = width();
   int h = height();

   dc.drawImage( m_image.rect(), m_image,
                  m_image.rect(), Qt::NoOpaqueDetection );

//   m_font5x8.drawText( dc, w/2, 1, QString("L_min(%1), L_max(%2), R_min(%3), R_max(%4)")
//      .arg( L_min).arg( L_max ).arg( R_min).arg( R_max ), 0xFFFFFFFF, de::Align::CenterTop );

}

void
GPianoKeyboard::keyPressEvent( QKeyEvent* event )
{
   QWidget::keyPressEvent( event );
}

void
GPianoKeyboard::keyReleaseEvent( QKeyEvent* event )
{
   QWidget::keyReleaseEvent( event );
}

void
GPianoKeyboard::mouseMoveEvent( QMouseEvent* event )
{
   QWidget::mouseMoveEvent( event );
// m_MousePos = glm::ivec2( event->x(), event->y() ); // current mouse pos
// if ( m_MousePos != m_LastMousePos ) // Some button is clicked...
// {
   // m_MouseMove = m_MousePos - m_LastMousePos; // current mouse pos
   // m_LastMousePos = m_MousePos;
// }
// else
// {
   // m_MouseMove = { 0,0 };
// }
}

void
GPianoKeyboard::mousePressEvent( QMouseEvent* event )
{
   QWidget::mousePressEvent( event );
// if ( m_Driver )
// {
   // de::SEvent post;
   // post.type = de::EET_MOUSE_EVENT;
   // post.mouseEvent.m_Flags = de::SMouseEvent::Pressed;
   // post.mouseEvent.m_X = event->x();
   // post.mouseEvent.m_Y = event->y();
   // post.mouseEvent.m_Wheel = 0.0f;

   // if ( event->button() == Qt::LeftButton )
   // {
      // post.mouseEvent.m_Flags |= de::SMouseEvent::BtnLeft;
   // }
   // else if ( event->button() == Qt::RightButton )
   // {
      // post.mouseEvent.m_Flags |= de::SMouseEvent::BtnRight;
   // }
   // else if ( event->button() == Qt::MiddleButton )
   // {
      // post.mouseEvent.m_Flags |= de::SMouseEvent::BtnMiddle;
   // }
   // m_Driver->postEvent( post );

   // auto camera = m_Driver->getActiveCamera();
   // if ( camera )
   // {
      // de::gpu::Ray3d ray = camera->computeRay( event->x(), event->y() );
      // auto a = ray.getPos() + ray.getDir() * 10.0;
      // auto b = ray.getPos() + ray.getDir() * 1000.0;
      // m_LineRenderer.add3DLine( a,b, de::randomColor() );
   // }

// }
//m_MousePos = glm::ivec2( event->x(), event->y() ); // current mouse pos
}

void
GPianoKeyboard::mouseReleaseEvent( QMouseEvent* event )
{
   QWidget::mouseReleaseEvent( event );
// if ( m_Driver )
// {
   // de::SEvent post;
   // post.type = de::EET_MOUSE_EVENT;
   // post.mouseEvent.m_Flags = de::SMouseEvent::Released;
   // post.mouseEvent.m_X = event->x();
   // post.mouseEvent.m_Y = event->y();
   // post.mouseEvent.m_Wheel = 0.0f;
   // if ( event->button() == Qt::LeftButton )
   // {
      // post.mouseEvent.m_Flags |= de::SMouseEvent::BtnLeft;
   // }
   // else if ( event->button() == Qt::RightButton )
   // {
      // post.mouseEvent.m_Flags |= de::SMouseEvent::BtnRight;
   // }
   // else if ( event->button() == Qt::MiddleButton )
   // {
      // post.mouseEvent.m_Flags |= de::SMouseEvent::BtnMiddle;
   // }
   // m_Driver->postEvent( post );
// }

//m_MousePos = glm::ivec2( event->x(), event->y() ); // current mouse pos
//m_MouseMove = { 0,0 };
}


void
GPianoKeyboard::wheelEvent( QWheelEvent* event )
{
   QWidget::wheelEvent( event );
// if ( m_Driver )
// {
   // de::SEvent post;
   // post.type = de::EET_MOUSE_EVENT;
   // post.mouseEvent.m_Flags = de::SMouseEvent::Wheel;
   // post.mouseEvent.m_X = event->x();
   // post.mouseEvent.m_Y = event->y();
   // post.mouseEvent.m_Wheel = event->angleDelta().y(); //    //QPoint delta = event->pixelDelta();
   // // DE_DEBUG("post.mouseEvent.m_Wheel = ",post.mouseEvent.m_Wheel)
   // m_Driver->postEvent( post );
// }

//event->accept();
}


