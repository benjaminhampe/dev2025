#include "DropTarget.h"
#include <QResizeEvent>
#include "App.h"

DropTarget::DropTarget( App & app, QWidget* parent )
   : QWidget(parent)
   , m_app( app )
   , m_isDirty( true )
   , m_isAudioOnly( false )
   , m_hasFocus( false )
   , m_msg1("Ziehen Sie Audio-Effekte hierhin")
   , m_msg2("Ziehen Sie MIDI-Effekte, Audio-Effekte, Instrumente oder Samples hierhin")
{
   setObjectName( "DropTarget" );
   setContentsMargins( 0,0,0,0 );
   setMinimumSize( 3, 4*10 );
   setMaximumSize( 3, 4*10 );
   setMouseTracking( true );
   setAcceptDrops( true );
}

void
DropTarget::setAudioOnly( bool b )
{
   if ( m_isAudioOnly == b ) return;
   //std::cout << "DropTarget::setAudioOnly(" << b << ")" << std::endl;
   m_isAudioOnly = b;
   m_isDirty = true;
   update();
}

void
DropTarget::updateImage()
{
   bool needUpdate = false;
   if ( m_isDirty ||
        m_size.width() != m_lastSize.width() ||
        m_size.height() != m_lastSize.height() )
   {
      needUpdate = true;
   }

   m_lastSize = m_size;

   if ( !needUpdate ) return;

   if ( m_size.width() > m_img.width() || m_size.height() > m_img.height() )
   {
      m_img = QImage( m_size, QImage::Format_ARGB32 );
   }

   //m_img.fill( skin.contentColor );

   QPainter dc;
   if ( dc.begin( &m_img ) )
   {
      dc.fillRect( QRect( 0,0,m_size.width(),m_size.height()), m_app.m_skin.contentColor );
      //dc.setRenderHint( QPainter::NonCosmeticDefaultPen );

      dc.setPen( QPen( m_app.m_skin.symbolColor ) );
      dc.setBrush( QBrush( m_app.m_skin.symbolColor ) );

      //QString msg = "Ziehen Sie Audio-Effekte hierhin";

      QRect br = QRect( 6, 6, m_size.width()-12, m_size.height()-12 );

      if ( m_isAudioOnly )
      {
         dc.drawText( br, Qt::TextWordWrap | Qt::AlignCenter, m_msg1, &br );
      }
      else
      {
         dc.drawText( br, Qt::TextWordWrap | Qt::AlignCenter, m_msg2, &br );
      }

   }

   m_isDirty = false;
}

void
DropTarget::dropEvent( QDropEvent* event )
{
    DE_TRACE("uri = ",event->mimeData()->text().toStdString())
    std::wstring uri = event->mimeData()->text().toStdWString();
    m_app.addPlugin( uri, true );
    event->acceptProposedAction();
    QWidget::dropEvent( event );
}

/*
void
SpurEditor::dropEvent( QDropEvent* event )
{
   std::cout << "SpurEditor::" << __func__ << std::endl;
   std::cout << event->mimeData()->text().toStdString() << std::endl;

   std::wstring uri = event->mimeData()->text().toStdWString();

   addPlugin( uri );

   event->acceptProposedAction();
   QWidget::dropEvent( event );
}
*/

void DropTarget::dragEnterEvent( QDragEnterEvent* event )
{
   if ( event->mimeData()->hasFormat("text/plain") )
   {
      event->acceptProposedAction();
   }
   DE_TRACE("")
   QWidget::dragEnterEvent( event );
}

void DropTarget::dragLeaveEvent( QDragLeaveEvent* event )
{
   DE_TRACE("")
   QWidget::dragLeaveEvent( event );
}

void DropTarget::dragMoveEvent(QDragMoveEvent* event )
{
   DE_TRACE("")
   QWidget::dragMoveEvent( event );
}


void
DropTarget::resizeEvent( QResizeEvent* event )
{
   QWidget::resizeEvent( event );
   m_size = event->size();
   update();
}

void DropTarget::paintEvent( QPaintEvent* event )
{
   int w = width();
   int h = height();

   if ( w > 0 && h > 0 )
   {
      updateImage();
      QPainter dc( this );
      //dc.setRenderHint( QPainter::NonCosmeticDefaultPen );
      dc.drawImage( QPoint(0,0), m_img, rect() );
   }
}

void
DropTarget::focusInEvent( QFocusEvent* event )
{
   m_hasFocus = true;
   update();
   QWidget::focusInEvent( event );
}

void
DropTarget::focusOutEvent( QFocusEvent* event )
{
   m_hasFocus = true;
   update();
   QWidget::focusOutEvent( event );
}

void
DropTarget::enterEvent( QEnterEvent* event )
{
   QWidget::enterEvent( event );
}

void
DropTarget::leaveEvent( QEvent* event )
{
   QWidget::leaveEvent( event );
}
