#include "ArrangeDropTarget.h"
#include <QResizeEvent>
#include "App.h"

ArrangeDropTarget::ArrangeDropTarget( App & app, QWidget* parent )
   : QWidget(parent)
   , m_app( app )
{
   setObjectName( "ArrangeDropTarget" );
   setContentsMargins( 0,0,0,0 );
   setMouseTracking( true );
   setAcceptDrops( true );
}

// void
// ArrangeDropTarget::resizeEvent( QResizeEvent* event )
// {
   // QWidget::resizeEvent( event );
// }

void ArrangeDropTarget::paintEvent( QPaintEvent* event )
{
   int w = width();
   int h = height();

   if ( w > 0 && h > 0 )
   {
      QPainter dc( this );
      //dc.setRenderHint( QPainter::NonCosmeticDefaultPen );

      auto const & skin = m_app.m_skin;
      dc.fillRect( rect(), skin.contentColor );
      dc.setPen( QPen( skin.symbolColor ) );
      dc.setBrush( QBrush( skin.symbolColor ) );

      //QString msg = "Ziehen Sie Audio-Effekte hierhin";

      QRect br = QRect( 6, 6, w-12, h-12 );
      dc.drawText( br, Qt::TextWordWrap | Qt::AlignCenter, "Drop synth plugin to create Midi Track\n"
                                                           "Drop audio plugin to create Audio-Only Track", &br );
   }
}


void
ArrangeDropTarget::dropEvent( QDropEvent* event )
{
    DE_DEBUG("ArrangeDropTarget::dropEvent()")
    DE_DEBUG(event->mimeData()->text().toStdString())

   // FIND PLUGIN uri IN DATABASE

   std::wstring uri = event->mimeData()->text().toStdWString();

   auto p = m_app.getPluginInfo( uri );
   if ( p )
   {
      int id = p->isSynth() ? 3000 + m_app.m_userMidiTracks.size() : 2000 + m_app.m_userAudoTracks.size();
      std::string name = QString("%1 %2").arg(id).arg(QString::fromStdWString(p->m_name) ).toStdString();
      auto tt = p->isSynth() ? TrackType::Midi : TrackType::Audio;
      QColor color = toQColor( de::randomColorRGB() );
      m_app.addTrack( id, name, tt, color );
      m_app.addPlugin( uri, true );
   }
   else
   {
       DE_DEBUG("No plugin info found in db ",de_mbstr(uri))
   }


   event->acceptProposedAction();
   QWidget::dropEvent( event );
}

void ArrangeDropTarget::dragEnterEvent( QDragEnterEvent* event )
{
   if ( event->mimeData()->hasFormat("text/plain") )
   {
      event->acceptProposedAction();
   }
   DE_DEBUG("")
   QWidget::dragEnterEvent( event );
}

void ArrangeDropTarget::dragLeaveEvent( QDragLeaveEvent* event )
{
    DE_DEBUG("")
   QWidget::dragLeaveEvent( event );
}

void ArrangeDropTarget::dragMoveEvent(QDragMoveEvent* event )
{
   DE_DEBUG("")
   QWidget::dragMoveEvent( event );
}

void
ArrangeDropTarget::focusInEvent( QFocusEvent* event )
{
//   m_hasFocus = true;
   update();
   QWidget::focusInEvent( event );
}

void
ArrangeDropTarget::focusOutEvent( QFocusEvent* event )
{
//   m_hasFocus = true;
   update();
   QWidget::focusOutEvent( event );
}

void
ArrangeDropTarget::enterEvent( QEnterEvent* event )
{
   QWidget::enterEvent( event );
}

void
ArrangeDropTarget::leaveEvent( QEvent* event )
{
   QWidget::leaveEvent( event );
}
