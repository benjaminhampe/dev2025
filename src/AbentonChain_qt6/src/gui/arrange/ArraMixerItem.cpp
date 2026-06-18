#include "ArraMixerItem.h"
#include <App.h>

ArraMixerItem::ArraMixerItem(QWidget* parent )
   : QWidget( parent )
{
   setObjectName( "ArraMixerItem" );
   setMouseTracking( true );

   //m_pianoRoll = new PianoRoll(this);

   applySkin();
}

void ArraMixerItem::applySkin()
{
    const auto& skin = App::instance()->getSkin();
    m_zoom = skin.zoom;
    m_margin = (8* m_zoom)/100;
    m_windowColor = skin.windowColor;
    m_panelColor = skin.panelColor;
    updateLayout();
}

void ArraMixerItem::updateLayout()
{
    const int w = width();
    const int h = height();
    if (w < 1) return;
    if (h < 1) return;

    int m = 2*m_margin; // inner margin
    int dx = std::max(w - 2*m,0);
    int dy = std::max(h - 2*m,0);
    //m_pianoRoll->setGeometry(m,m,dx,dy);
    update();
}

void ArraMixerItem::resizeEvent(QResizeEvent* e)
{
    const int w = e->size().width();
    const int h = e->size().height();
    if (w < 1) return;
    if (h < 1) return;
    updateLayout();
}

void ArraMixerItem::paintEvent( QPaintEvent* event )
{
    const int w = width();
    const int h = height();
    if (w < 1) return;
    if (h < 1) return;
    QPainter dc( this );
    dc.fillRect(rect(), m_windowColor );

    int m = m_margin;
    int dx = std::max(0, w - 2*m);
    int dy = std::max(0, h - 2*m);
    if (dx > 0 && dy > 0)
    {
        dc.setRenderHint(QPainter::Antialiasing);
        dc.setPen(Qt::NoPen);
        dc.setBrush(QBrush(m_panelColor));
        dc.drawRoundedRect(QRect(m,m,dx,dy),m,m);
    }
}

#if 0

void
ArraMixerItem::hideEvent( QHideEvent* event )
{
   QWidget::hideEvent( event );
}

void
ArraMixerItem::showEvent( QShowEvent* event )
{
   //DE_DEBUG("")
   QWidget::showEvent( event );
   //event->accept();
}


void
ArraMixerItem::mouseMoveEvent( QMouseEvent* event )
{
   m_mouseX = event->x();
   m_mouseY = event->y();

   m_isOverPianoBar = isMouseOverPianoBar();
   m_isOverBeatGrid = isMouseOverBeatGrid();
   m_detectedKeyIndex = getKeyIndexFromMousePos();
   m_hoveredNote = computeHoveredNote();

   if ( m_dragMode == 1 ) // drawing
   {
//      int sy = m_mouseY - m_dragStartY;
//      DE_DEBUG("StretchY(",sy,")")
   }
   if ( m_dragMode == 2 ) // stretch y
   {
      int sy = m_mouseY - m_dragStartY;
      DE_DEBUG("StretchY(",sy,")")
   }

   update();
   QWidget::mouseMoveEvent( event );
}

void
ArraMixerItem::wheelEvent( QWheelEvent* event )
{
   int wheel = event->angleDelta().y();
   DE_DEBUG("MouseWheel = ",wheel )

   if ( wheel >= 1 )
   {
      m_keyStart = std::clamp( m_keyStart - 3, 0, 125 );
      update();
   }
   else if ( wheel <= 1 )
   {
      m_keyStart = std::clamp( m_keyStart + 3, 0, 125 );
      update();
   }

   QWidget::wheelEvent( event );

//   if ( m_driver )
//   {
//      de::SEvent post;
//      post.type = de::EET_MOUSE_EVENT;
//      post.mouseEvent.m_Flags = de::SMouseEvent::Wheel;
//      post.mouseEvent.m_X = event->x();
//      post.mouseEvent.m_Y = event->y();
//      post.mouseEvent.m_Wheel = event->angleDelta().y(); //    //QPoint delta = event->pixelDelta();
//      // DE_DEBUG("post.mouseEvent.m_Wheel = ",post.mouseEvent.m_Wheel)
//      m_driver->postEvent( post );
//   }

   //event->accept();
}


void
ArraMixerItem::mousePressEvent( QMouseEvent* event )
{
//   m_X = event->x();
//   m_Y = event->y();
//   m_Wheel = 0.0f;
   m_mouseX = event->x();
   m_mouseY = event->y();


   int x = event->x();
   int y = event->y();

   if ( m_dragMode > 0 )
   {
      DE_ERROR("Already dragging ", m_dragMode)
   }
   else
   {
      m_dragStartX = x;
      m_dragStartY = y;
      //m_selectedNote = m_hoveredNote;

      if ( m_isOverBeatGrid )
      {
         m_dragMode = 1;
         DE_DEBUG("DrawMode ", m_dragMode)
      }

      if ( m_isOverPianoBar )
      {
         m_dragMode = 2;
         DE_DEBUG("StretchMode ", m_dragMode)
      }
      update();
   }

   QWidget::mousePressEvent( event );
}

void
ArraMixerItem::mouseReleaseEvent( QMouseEvent* event )
{
   m_mouseX = event->x();
   m_mouseY = event->y();

//   m_isOverPianoBar = isMouseOverPianoBar();
//   m_isOverBeatGrid = isMouseOverBeatGrid();

   // Finish Drawing
   if ( m_dragMode == 1 && m_detectedKeyIndex > -1 )
   {
      auto a = pixel2time( m_dragStartX );
      auto b = pixel2time( m_mouseX );
      if ( a > b )
      {
         std::swap( a, b );
      }
      if ( a < b )
      {
         Note note;
         note.timeBeg = a;
         note.timeEnd = b;
         note.color = de::randomColorRGB();
         Key & key = m_keys[ m_detectedKeyIndex ];
         key.m_notes.emplace_back( std::move( note ) );
         DE_DEBUG("Added Note ")
      }
   }

   // Finish StretchY
   if ( m_dragMode == 2 && m_isOverPianoBar )
   {
      int sy = m_mouseY - m_dragStartY;
      DE_DEBUG("StretchY Final(",sy,")")
      m_keyHeight = std::clamp( m_keyHeight + sy/4, 2, 48 );
   }

   // Reset all
   m_dragMode = 0;

   // Enqueue ( async ) request for redraw.
   update();

   QWidget::mouseReleaseEvent( event );
}


void
ArraMixerItem::keyPressEvent( QKeyEvent* event )
{
   //DE_DEBUG("KeyPress(",event->key(),")")

//   if ( m_driver )
//   {
//      de::SEvent post;
//      post.type = de::EET_KEY_EVENT;
//      post.keyEvent.Key = de::KEY_UNKNOWN;
//      post.keyEvent.Flags = de::SKeyEvent::Pressed;
//      if ( event->modifiers() & Qt::ShiftModifier ) { post.keyEvent.Flags |= de::SKeyEvent::Shift; }
//      if ( event->modifiers() & Qt::ControlModifier ) { post.keyEvent.Flags |= de::SKeyEvent::Ctrl; }
//      if ( event->modifiers() & Qt::AltModifier ) { post.keyEvent.Flags |= de::SKeyEvent::Alt; }
//      parseQtKey( event, post );
//      m_driver->postEvent( post );
//   }

   QWidget::keyPressEvent( event );
}

void
ArraMixerItem::keyReleaseEvent( QKeyEvent* event )
{
   //DE_DEBUG("KeyRelease(",event->key(),")")

//   if ( m_driver )
//   {
//      de::SEvent post;
//      post.type = de::EET_KEY_EVENT;
//      post.keyEvent.Key = de::KEY_UNKNOWN;
//      post.keyEvent.Flags = de::SKeyEvent::None;
//      if ( event->modifiers() & Qt::ShiftModifier ) { post.keyEvent.Flags |= de::SKeyEvent::Shift; }
//      if ( event->modifiers() & Qt::ControlModifier ) { post.keyEvent.Flags |= de::SKeyEvent::Ctrl; }
//      if ( event->modifiers() & Qt::AltModifier ) { post.keyEvent.Flags |= de::SKeyEvent::Alt; }
//      parseQtKey( event, post );
//      m_driver->postEvent( post );
//   }

//   event->accept();
   QWidget::keyReleaseEvent( event );
}

#endif
