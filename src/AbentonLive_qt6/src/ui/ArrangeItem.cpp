#include "ArrangeItem.h"
#include "App.h"
#include "Track.h"
#include "Draw.h"
#include <QColorDialog>
#include <QPromt.h>

// ============================================================================
ItemV::ItemV( App & app, QWidget* parent )
   : QWidget( parent )
   , m_app( app )
   , m_hasFocus( false )
   , m_isHovered( false )
   , m_isSelected( false )
   , m_track( nullptr )
   , m_isAudioOnly( true )
   , m_color( 255,0,255 )
{
   setObjectName( "ItemV" );
   setContentsMargins(0,0,0,0);

   //setFont( getFontAwesome( 14, true ));

   m_timeGrid = new QWidget( this );

   m_lblName = new QLabel( "1 Audio", this );
   m_lblName->setFont( getFontAwesome( 14, true ));

   m_lblType = new QLabel( "Track", this );
   m_lblType->setFont( getFontAwesome( 10, false ));

   m_optMaster = new QWidget( this );
   m_optMaster->setContextMenuPolicy(Qt::CustomContextMenu);
      connect( m_optMaster, &QWidget::customContextMenuRequested,
               this, &ItemV::onCustomContextMenuRequested);


   auto optMv = new QVBoxLayout();
   optMv->setContentsMargins( 3, 3, 3, 3 );
   optMv->setSpacing( 3 );
   optMv->addWidget( m_lblName );
   optMv->addWidget( m_lblType );
   optMv->addStretch( 1 );
   m_optMaster->setLayout( optMv );

   m_audioMeter = new AudioMeter2( this );

   m_volumeSlider = new QSlider( Qt::Vertical, this );
   m_volumeSlider->setRange(0,100);
   m_volumeSlider->setValue(100);

   m_timeGrid->setStyleSheet( QString("background-color:rgb(220,220,230);") );
   m_optMaster->setStyleSheet( QString("background-color:rgb(200,200,200);") );

   auto h = new QHBoxLayout();
   h->setContentsMargins( 0, 0, 0, 0 );
   h->setSpacing( 0 );
   h->addWidget( m_timeGrid,1 );
   h->addWidget( m_optMaster );
   h->addWidget( m_volumeSlider );
   h->addWidget( m_audioMeter );
   setLayout( h );

   //connect( &m_app, SIGNAL(activatedTrack(Track*)), this, SLOT(setTrack(Track*)) );

}

void ItemV::onCustomContextMenuRequested( QPoint const & pos )
{
   //const QTreeWidgetItem *item = treeWidget->itemAt(pos);
   //if (!item)
   //return;
   //QString url = item->text(1);

   if ( m_track )
   {
      QMenu contextMenu;
      QAction* renameAction = contextMenu.addAction(QString("Rename '%1'").arg(QString::fromStdString(m_track->name())));
      contextMenu.addSeparator();
      QAction* colorAction = contextMenu.addAction(tr("Choose color"));
      contextMenu.addSeparator();
      QAction* removeAction = contextMenu.addAction(tr("Delete/Remove Track"));
      QAction* action = contextMenu.exec( m_optMaster->mapToGlobal(pos) );
      if (action == renameAction)
      {
         QStringInputDialog dlg;
         dlg.setInfoText( "Enter a new track name...");
         dlg.setEditText( QString::fromStdString( m_track->m_trackInfo.m_name ) );
         if ( QDialog::Accepted == dlg.exec() )
         {
            QString neu = dlg.dialogResult();
            if ( !neu.isEmpty() )
            {
               m_track->m_trackInfo.m_name = neu.toStdString();
               m_lblName->setText( neu );
            }

         }
//         QMessageBox msb;
//         msb.setText("Do you want to remove a directory from Plugin Database?");
//         msb.setInformativeText( uri );
//         msb.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
//         msb.setDefaultButton( QMessageBox::Cancel );
//         int ret = msb.exec();
//         if (ret == QMessageBox::Ok)
//         {
//            //event->accept();
//            m_app.m_pluginDb.removeDir( uri.toStdWString() );
//            m_app.m_pluginDirTree->updateFromPluginDb();
//            m_app.m_pluginInfoTree->updateFromPluginDb();
//         }
//         else if (ret == QMessageBox::Cancel)
//         {
//            //m_app.stopAudioMasterBlocking();
//            //m_app.save( false );
//            //event->ignore();
//         }

      }
      else if (action == colorAction)
      {
         QColor initColor( 255,255,255 );
         if ( m_track )
         {
            initColor = m_track->m_trackInfo.m_color;
         }

         QColorDialog::ColorDialogOptions options; // =
            // QFlag( colorDialogOptionsWidget->value() );
         QColor color = QColorDialog::getColor( initColor, this, "Select Color", options);

         if (color.isValid())
         {
            m_color = color;
            if ( m_track )
            {
               m_track->m_trackInfo.m_color = color;
            }
            update();
            //Label->setText(color.name());
            //colorLabel->setPalette(QPalette(color));
            //colorLabel->setAutoFillBackground(true);
         }
//         m_lblName->setText( QString::fromStdString( m_track->name() ) );
      }
      else if (action == removeAction)
      {
         QMessageBox msb;
         msb.setText("Do you want to remove the track");
         msb.setInformativeText( QString::fromStdString( m_track->name() ) );
         msb.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
         msb.setDefaultButton( QMessageBox::Cancel );
         int ret = msb.exec();
         if (ret == QMessageBox::Ok)
         {
            m_app.removeTrack( m_track );
            //delete this;
         }
         else if (ret == QMessageBox::Cancel)
         {
            //m_app.stopAudioMasterBlocking();
            //m_app.save( false );
            //event->ignore();
         }
      }

          //QGuiApplication::clipboard()->setText(url);
      //else if (action == openAction)
          //QDesktopServices::openUrl(QUrl(url));

   }

}

void
ItemV::setTrack( Track* track )
{
   m_track = track;
   updateFromTrack();
}

void
ItemV::updateFromTrack()
{
   m_isSelected = (m_track == m_app.track());
   if ( m_track )
   {
      DE_DEBUG("m_trackName = ",m_track->name() )

      m_isAudioOnly = m_track->isAudioOnly();
      m_lblName->setText( QString::fromStdString( m_track->name() ) );
      m_lblType->setText( m_isAudioOnly ? "Audio-Only" : "Midi + AudioFx" );

      m_color = m_track->m_trackInfo.m_color;

      m_optMaster->setStyleSheet( QString("background-color:rgb(%1,%2,%3);")
               .arg( m_color.red() )
               .arg( m_color.green() )
               .arg( m_color.blue() ) );

      connect( m_track, SIGNAL(audioMeterData(float,float,float,float)),
               m_audioMeter, SLOT(setData(float,float,float,float)), Qt::QueuedConnection );

      m_volumeSlider->setValue( m_track->getVolume() );

      connect( m_volumeSlider, SIGNAL(valueChanged(int)),
               m_track, SLOT(setVolume(int)), Qt::QueuedConnection );
   }
   else
   {
      DE_ERROR("No m_track")
   }
   updateLayout();
}

void
ItemV::updateLayout()
{
   update();
}

void
ItemV::resizeEvent( QResizeEvent* event )
{
   updateLayout();
   QWidget::resizeEvent( event );
}


void
ItemV::dropEvent( QDropEvent* event )
{
    DE_TRACE(event->mimeData()->text().toStdString())

   std::wstring uri = event->mimeData()->text().toStdWString();
   if ( m_track )
   {
      m_app.setActiveTrack( m_track );
   }
   m_app.addPlugin( uri );

   event->acceptProposedAction();
   QWidget::dropEvent( event );
}

void ItemV::dragEnterEvent( QDragEnterEvent* event )
{
   if ( event->mimeData()->hasFormat("text/plain") )
   {
      event->acceptProposedAction();
   }
   DE_TRACE("")
   QWidget::dragEnterEvent( event );
}

void ItemV::dragLeaveEvent( QDragLeaveEvent* event )
{
   DE_TRACE("")
   QWidget::dragLeaveEvent( event );
}

void ItemV::dragMoveEvent(QDragMoveEvent* event )
{
   DE_TRACE("")
   QWidget::dragMoveEvent( event );
}

void
ItemV::paintEvent( QPaintEvent* event )
{
   int w = width();
   int h = height();

   if ( w > 1 && h > 1 )
   {
      LiveSkin const & skin = m_app.m_skin;

      QPainter dc( this );
      dc.setFont( font() );
      //dc.setRenderHint( QPainter::NonCosmeticDefaultPen );

      drawRectFill( dc, rect(), m_color );

      if ( m_isSelected )
      {
         drawRectFill( dc, rect(), skin.activeColor );
      }

//      else
//      {
//         drawRectFill( dc, rect(), skin.contentColor );
//      }

      drawLine( dc, 0,0,w-1,0, skin.panelColor );

//      {
//      auto ts = getTextSize( dc, m_trackDesc );
//      int x = 5; // grid.time2pixel( i );
//      int y = (h - ts.height()) / 2; // y + m_headHeight - 1 - 8;
//      drawText( dc, x,y, m_trackDesc );
//      }

//      auto ts = getTextSize( dc, m_trackName );
//      int x = w - 5 - ts.width(); // grid.time2pixel( i );
//      int y = (h - ts.height()) / 2; // y + m_headHeight - 1 - 8;
//      drawText( dc, x,y, m_trackName );
   }

   /*
   // Draw background
   int y = 0;
   int pxPerBeat = grid.time2pixel( 1 );
   int k = float( w ) / float( pxPerBeat );
   // Draw beat indices
   dc.fillRect( QRect(0,0,w-1,m_headHeight), skin.panelColor );
   dc.setBrush( Qt::NoBrush );
   dc.setPen( QPen( skin.windowColor ) );
   dc.setFont( QFont("Arial Black", 8) );
   for ( int i = 0; i < k; ++i )
   {
      int x = grid.time2pixel( i );
      int y1 = y + m_headHeight - 1 - 8;
      int y2 = y + m_headHeight - 2;
      drawLine( dc, x, y1, x, y2, skin.windowColor );
      drawLine( dc, x, y1, x+2, y1, skin.windowColor );

      dc.drawText( x + 4, y2-2, QString::number( i ) );
   }
   y += m_headHeight;

   // Draw ContentRect
   QRect r_content(0,m_headHeight,w,h-m_headHeight-m_footHeight);
   dc.fillRect( r_content, skin.contentColor );


   // Draw BeatGrid
   for ( int i = 0; i < k; ++i )
   {
      int x = grid.time2pixel( i );
      int y1 = m_headHeight;
      int y2 = h - 1 - m_headHeight - m_footHeight;
      drawLine( dc, x, y1, x, y2, skin.panelColor );

      x = grid.time2pixel( double(i)+0.5 );
      drawLine( dc, x, y1, x, y2, skin.semiBeatColor );

      dc.drawText( x + 7, y2-2, QString::number( i ) );
   }

   // Draw TimePin
   y = m_headHeight + m_setHeight;
   dc.fillRect( QRect(0,y,w-1,m_pinHeight), skin.panelColor );
   y += m_pinHeight;

   // Draw tracks
   for ( int i = 0; i < trackList.trackCount(); ++i )
   {
      // highlight #3
      if ( i == m_focusedTrack )
      {
         dc.fillRect( QRect(0,y,w-1,m_trackHeight), skin.editBoxColor );
      }

      Track const & track = trackList.track( i );
      Clips const & clips = track.clips();
      for ( int c = 0; c < clips.size(); ++c )
      {
         Clip const & clip = clips[ c ];
         int x1 = grid.time2pixel( clip.m_timeBeg );
         int x2 = grid.time2pixel( clip.m_timeEnd )-1;
         dc.setBrush( QBrush( toQColor( clip.m_color ) ) );
         dc.setPen( QPen( QColor(0,0,0) ) );
         dc.drawRect( QRect( x1, y+1, x2-x1, m_trackHeight-3 ) );
      }

      y += m_trackHeight;

      dc.fillRect( QRect(0,y,w-1,m_trackSpace), skin.panelColor );
      y += m_trackSpace;
   }
  */

   QWidget::paintEvent( event );
}

/*
void
ItemV::dropEvent( QDropEvent* event )
{
   std::cout << __func__ << std::endl;
   std::cout << event->mimeData()->text().toStdString() << std::endl;

   std::wstring uri = event->mimeData()->text().toStdWString();

//   if ( m_chain )
//   {
//      m_chain->addPlugin( -1, uri );
//   }

   event->acceptProposedAction();
}

void
ItemV::dragEnterEvent( QDragEnterEvent* event )
{
   if ( event->mimeData()->hasFormat("text/plain") )
   {
      event->acceptProposedAction();
   }
   std::cout << __func__ << std::endl;
}

void
ItemV::dragLeaveEvent( QDragLeaveEvent* event )
{
   std::cout << __func__ << std::endl;
}

void
ItemV::dragMoveEvent( QDragMoveEvent* event )
{
   std::cout << __func__ << std::endl;
}

*/
void
ItemV::enterEvent( QEnterEvent* event )
{
   m_isHovered = true;
   update();
   QWidget::enterEvent( event );
}
void
ItemV::leaveEvent( QEvent* event )
{
   m_isHovered = false;
   update();
   QWidget::leaveEvent( event );
}

void
ItemV::focusInEvent( QFocusEvent* event )
{
   m_hasFocus = true;
   update();
   QWidget::focusInEvent( event );
}

void
ItemV::focusOutEvent( QFocusEvent* event )
{
   m_hasFocus = true;
   update();
   QWidget::focusOutEvent( event );
}



void
ItemV::mouseMoveEvent( QMouseEvent* event )
{
   int mx = event->x();
   int my = event->y();

   updateLayout();
   QWidget::mouseMoveEvent( event );
}

void
ItemV::mousePressEvent( QMouseEvent* event )
{
   int mx = event->x();
   int my = event->y();

//   if ( m_isHovered )
//   {
// TODO:
      m_app.m_arrangement->m_selected = this;
      m_app.setActiveTrack( m_track );
      m_app.showSpurEditor();
//   }

   updateLayout();
   QWidget::mousePressEvent( event );
}

void
ItemV::mouseReleaseEvent( QMouseEvent* event )
{
   //   int mx = event->x();
   //   int my = event->y();

   QWidget::mouseReleaseEvent( event );
}


void
ItemV::wheelEvent( QWheelEvent* event )
{
   //   int mx = event->x();
   //   int my = event->y();
   //   me.m_wheelY = event->angleDelta().y();
   //   if ( me.m_wheelX != 0.0f )
   //   {
   //      me.m_flags |= de::MouseEvent::WheelX;
   //   }
   //   if ( me.m_wheelY != 0.0f )
   //   {
   //      me.m_flags |= de::MouseEvent::WheelY;
   //   }

   QWidget::wheelEvent( event );
}

void
ItemV::keyPressEvent( QKeyEvent* event )
{
   //DE_DEBUG("KeyPress(",event->key(),")")
   QWidget::keyPressEvent( event );
}

void
ItemV::keyReleaseEvent( QKeyEvent* event )
{
   //DE_DEBUG("KeyRelease(",event->key(),")")
   QWidget::keyReleaseEvent( event );
}

/*

void
GMixer::drawTrack( QPainter & dc, QRect const & rect, Track const & track )
{
   //if ( !track ) return;

   LiveSkin const & skin = m_app.m_skin;

   if ( &track == m_app.m_trackList.current() )
   {
      drawRectFill( dc, rect, skin.activeColor );
   }
   else
   {
      drawRectFill( dc, rect, skin.contentColor );
   }

   int x = rect.topLeft().x() + rect.width() - 100; // grid.time2pixel( i );
   int y = rect.topLeft().y() + 1; // y + m_headHeight - 1 - 8;
   drawText( dc,x,y, QString::fromStdWString( track.m_name ) );
}

int w = width();
int h = height();

if ( w > 1 && h > 1 )
{
   LiveSkin const & skin = m_app.m_skin;
   BeatGrid const & grid = m_app.m_beatGrid;
   TrackList const & trackList = m_app.m_trackList;

   QPainter dc( this );
   dc.setRenderHint( QPainter::NonCosmeticDefaultPen );


   // Draw tracks
   //auto const * ctrack = m_app.m_trackList.current();
   auto const & tracks = m_app.m_trackList.tracks();

   int avail = h - 3 * 18;
   int x = 0;
   int y = 100;

   for ( int i = 0; i < tracks.size(); ++i )
   {
      auto const & track = tracks[ i ];
      drawTrack( dc, QRect( x,y,w,17 ), track ); y += 17 + 1;

   }

   auto const & rA = m_app.m_trackList.m_returnA;
   drawTrack( dc, QRect( x,h - 1-3*18,w,17 ), rA );

   auto const & rB = m_app.m_trackList.m_returnB;
   drawTrack( dc, QRect( x,h - 1-2*18,w,17 ), rB );

   auto const & master = m_app.m_trackList.m_master;
   drawTrack( dc, QRect( x,h - 1-18,w,17 ), master );


   // Draw background
   int y = 0;
   int pxPerBeat = grid.time2pixel( 1 );
   int k = float( w ) / float( pxPerBeat );
   // Draw beat indices
   dc.fillRect( QRect(0,0,w-1,m_headHeight), skin.panelColor );
   dc.setBrush( Qt::NoBrush );
   dc.setPen( QPen( skin.windowColor ) );
   dc.setFont( QFont("Arial Black", 8) );
   for ( int i = 0; i < k; ++i )
   {
      int x = grid.time2pixel( i );
      int y1 = y + m_headHeight - 1 - 8;
      int y2 = y + m_headHeight - 2;
      drawLine( dc, x, y1, x, y2, skin.windowColor );
      drawLine( dc, x, y1, x+2, y1, skin.windowColor );

      dc.drawText( x + 4, y2-2, QString::number( i ) );
   }
   y += m_headHeight;

   // Draw ContentRect
   QRect r_content(0,m_headHeight,w,h-m_headHeight-m_footHeight);
   dc.fillRect( r_content, skin.contentColor );


   // Draw BeatGrid
   for ( int i = 0; i < k; ++i )
   {
      int x = grid.time2pixel( i );
      int y1 = m_headHeight;
      int y2 = h - 1 - m_headHeight - m_footHeight;
      drawLine( dc, x, y1, x, y2, skin.panelColor );

      x = grid.time2pixel( double(i)+0.5 );
      drawLine( dc, x, y1, x, y2, skin.semiBeatColor );

      dc.drawText( x + 7, y2-2, QString::number( i ) );
   }

   // Draw TimePin
   y = m_headHeight + m_setHeight;
   dc.fillRect( QRect(0,y,w-1,m_pinHeight), skin.panelColor );
   y += m_pinHeight;

   // Draw tracks
   for ( int i = 0; i < trackList.trackCount(); ++i )
   {
      // highlight #3
      if ( i == m_focusedTrack )
      {
         dc.fillRect( QRect(0,y,w-1,m_trackHeight), skin.editBoxColor );
      }

      Track const & track = trackList.track( i );
      Clips const & clips = track.clips();
      for ( int c = 0; c < clips.size(); ++c )
      {
         Clip const & clip = clips[ c ];
         int x1 = grid.time2pixel( clip.m_timeBeg );
         int x2 = grid.time2pixel( clip.m_timeEnd )-1;
         dc.setBrush( QBrush( toQColor( clip.m_color ) ) );
         dc.setPen( QPen( QColor(0,0,0) ) );
         dc.drawRect( QRect( x1, y+1, x2-x1, m_trackHeight-3 ) );
      }

      y += m_trackHeight;

      dc.fillRect( QRect(0,y,w-1,m_trackSpace), skin.panelColor );
      y += m_trackSpace;
   }
}
*/
