#include "ArraTracks.h"
#include <App.h>

ArraTracks::ArraTracks(QWidget* parent)
   : QWidget( parent )
{
    setObjectName("ArraTracks");
    setMouseTracking( true );
    applySkin();
}

void ArraTracks::updateFromSession()
{

    updateLayout();
}

void ArraTracks::applySkin()
{
    const auto& skin = App::instance()->getSkin();
    m_zoom = skin.zoom;
    m_margin = (8* m_zoom)/100;
    m_windowColor = skin.windowColor;
    m_panelColor = skin.panelColor;
    m_alternatingPanelColor = m_panelColor.lighter(50);
    m_headerHeight = (48 * m_zoom) / 100;
    updateLayout();
}

void ArraTracks::updateLayout()
{
    if (width() < 1) return;
    if (height() < 1) return;

    const int m = 2*m_margin; // inner margin
    const int w = std::max(width() - 2*m,0);
    const int h = std::max(height() - 2*m,0);

    using de::session::SharedTrack;
    const auto& session = App::instance()->m_session;

    //const int b = (50 * m_zoom) / 100;

    int x = m;
    int y = m;
    int w2 = w;
    if (m_bVertical)
    {
        SharedTrack masterTrack = session.getMasterTrack();
        int mw = masterTrack->m_width;
        masterTrack->m_rect = QRect(x+w-mw,y,mw,h);

        int nUser = 0;
        for (int i = 0; i < session.m_tracks.size(); ++i)
        {
            SharedTrack track = session.m_tracks[ i ];
            if (track->getTrackType() != de::session::Track::User)
            {
                continue;
            }

            int tw = track->m_width;
            track->m_rect = QRect(x,y,tw,h);
            x += tw;
            nUser++;
        }
    }
    else // Horizontal
    {
        int nUser = 0;
        for (int i = 0; i < session.m_tracks.size(); ++i)
        {
            SharedTrack track = session.m_tracks[ i ];
            if (track->getTrackType() != de::session::Track::User)
            {
                continue;
            }

            const int th = track->m_height;
            track->m_rect = QRect(x,y,w,th);
            y += th;
        }
    }

    update();
}

void ArraTracks::resizeEvent(QResizeEvent* e)
{
    const int w = e->size().width();
    const int h = e->size().height();
    if (w < 1) return;
    if (h < 1) return;
    updateLayout();
}

void ArraTracks::paintEvent( QPaintEvent* event )
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

    using de::session::SharedTrack;
    const auto& session = App::instance()->m_session;

    int nUser = 0;
    for (int i = 0; i < session.m_tracks.size(); ++i)
    {
        SharedTrack track = session.m_tracks[ i ];

        if (track->getTrackType() != de::session::Track::User)
        {
            continue;
        }

        auto fillColor = (nUser % 2 == 0) ? m_panelColor : m_alternatingPanelColor;
        drawTrack(dc, track, fillColor);
        nUser++;
    }

    SharedTrack track = session.getMasterTrack();
    drawTrack(dc, track, QColor(200,200,200));
}

void ArraTracks::drawTrack(QPainter & dc,
    de::session::SharedTrack track, QColor fillColor) const
{
    auto r_track = track->m_rect;
    dc.setPen(QPen(track->m_trackColor));
    dc.setBrush(QBrush(fillColor));
    dc.drawRect(r_track);

    QRect r_text = r_track;
    dc.drawText(r_text,0, track->m_trackName, &r_text);

    auto & clip = track->m_clips[0];

    float zoom_x = 64.0f / 960.0f;
    float zoom_y = 1.0f;

    dc.setPen(Qt::NoPen);
    for (int i = 0; i < clip->m_notes.size(); ++i)
    {
        const auto & note = clip->m_notes[i];
        int x1 = r_track.x() + std::lroundf(zoom_x * note.ppqNoteOn);
        int y1 = r_track.y() + r_track.height() - note.midiNote;
        int x2 = r_track.x() + std::lroundf(zoom_x * note.ppqNoteOff);
        int y2 = y1 - 1;
        const QColor color = toQColor(note.color);
        dc.setBrush(QBrush(color));
        dc.drawRect(x1,y1,x2-x1,y2-y1);
    }
}

#if 0

void
ArraTracks::hideEvent( QHideEvent* event )
{
   QWidget::hideEvent( event );
}

void
ArraTracks::showEvent( QShowEvent* event )
{
   //DE_DEBUG("")
   QWidget::showEvent( event );
   //event->accept();
}


void
ArraTracks::mouseMoveEvent( QMouseEvent* event )
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
ArraTracks::wheelEvent( QWheelEvent* event )
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
ArraTracks::mousePressEvent( QMouseEvent* event )
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
ArraTracks::mouseReleaseEvent( QMouseEvent* event )
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
ArraTracks::keyPressEvent( QKeyEvent* event )
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
ArraTracks::keyReleaseEvent( QKeyEvent* event )
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
