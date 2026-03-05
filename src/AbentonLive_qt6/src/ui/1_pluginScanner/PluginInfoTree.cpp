#include "PluginInfoTree.h"
#include "App.h"

PluginInfoTree::PluginInfoTree( App & app, QWidget* parent )
   : QTreeWidget( parent )
   , m_app( app )
   , m_selectedItem( nullptr )
   , m_isDragging( false )
//   , m_drag( nullptr )
{
   setObjectName( "PluginInfoTree" );
   setContentsMargins(0,0,0,0);
   //setAcceptDrops( true );
   setMouseTracking( true );

   createLiveTreeHeader();
   m_synthNode = new QTreeWidgetItem( this );
   m_synthNode->setText( 0, "Synths (0)" );
   m_synthNode->setIcon( 0, m_app.m_synthIcon );

   m_effectNode = new QTreeWidgetItem( this );
   m_effectNode->setText( 0, "Effects (0)" );
   m_effectNode->setIcon( 0, m_app.m_effectIcon );

//   connect( this, SIGNAL(itemActivated(QTreeWidgetItem*,int)),
//         this, SLOT(on_itemActivated(QTreeWidgetItem*,int)) );

   connect( this, SIGNAL(itemPressed(QTreeWidgetItem*,int)),
         this, SLOT(on_itemPressed(QTreeWidgetItem*,int)) );

   populateFromPluginDb();
}

void
PluginInfoTree::on_itemPressed( QTreeWidgetItem* item, int col )
{
   DE_TRACE("x:",currentIndex().column(),"; "
            "y:",currentIndex().row(),";")
   //m_selectedItem = item;
}

void
PluginInfoTree::createLiveTreeHeader()
{
   auto header = headerItem();
   int k = 0;
   header->setText( k, "Name" ); k++;
   header->setText( k, "Edit" ); k++;
   header->setText( k, "Banks" ); k++;
   header->setText( k, "Params" ); k++;
   header->setText( k, "Out" ); k++;
   header->setText( k, "In" ); k++;
   header->setText( k, "f32" ); k++;
   header->setText( k, "f64" ); k++;
   header->setText( k, "entry" ); k++;
   header->setText( k, "uri" ); k++;

   k = 0;
   setColumnWidth( k, 250 ); k++;
   setColumnWidth( k, 20 ); k++;
   setColumnWidth( k, 20 ); k++;
   setColumnWidth( k, 20 ); k++;
   setColumnWidth( k, 20 ); k++;
   setColumnWidth( k, 20 ); k++;
   setColumnWidth( k, 20 ); k++;
   setColumnWidth( k, 20 ); k++;
   setColumnWidth( k, 100 ); k++;
   setColumnWidth( k, 250 ); k++;
}

void
PluginInfoTree::populateFromPluginDb()
{
   clear();
   createLiveTreeHeader();

   blockSignals( true );

   de::audio::PluginInfoDb & pluginDb = m_app.pluginDb();

   int numSynths = pluginDb.numSynths();
   int numEffects = pluginDb.numEffects();

   m_synthNode = new QTreeWidgetItem( this );
   m_synthNode->setText( 0, QString("Synths (%1)").arg(numSynths) );
   m_synthNode->setIcon( 0, m_app.m_synthIcon );

   m_effectNode = new QTreeWidgetItem( this );
   m_effectNode->setText( 0, QString("Effects (%1)").arg(numEffects) );
   m_effectNode->setIcon( 0, m_app.m_effectIcon );

   addTopLevelItem( m_synthNode );
   addTopLevelItem( m_effectNode );

   DE_WARN("Updating pluginDb.m_all.m_plugins.size() = ",pluginDb.m_all.m_plugins.size())

   std::lock_guard< std::mutex > lg( pluginDb.getMutex() );

   for ( de::audio::PluginInfo const * const pi : pluginDb.m_all.m_plugins )
   {
      if ( !pi ) continue;
      de::audio::PluginInfo const & pluginInfo = *pi;

      // Determine parent Synth/Effect root node.
      QTreeWidgetItem* parent = m_effectNode;
      if ( pluginInfo.m_isSynth )
      {
         parent = m_synthNode;
      }

      // Add child
      auto pin = new QTreeWidgetItem( parent );

      if ( pluginInfo.m_isSynth )
      {
         pin->setIcon( 0, m_app.m_synthIcon );
      }
      else
      {
         pin->setIcon( 0, m_app.m_effectIcon );
      }

      pin->setData( 0, Qt::UserRole, QString::fromStdWString( pluginInfo.m_uri ) );
      pin->setData( 1, Qt::UserRole, int( pluginInfo.m_isSynth ) );

      int k = 0;
      pin->setText( k, QString::fromStdWString( pluginInfo.m_name ) ); k++;
      //pin->setText( k, QString::number( pluginInfo.m_isSynth ) ); k++;
      pin->setText( k, QString::number( pluginInfo.m_hasEditor ) ); k++;
      pin->setText( k, QString::number( pluginInfo.m_numPrograms ) ); k++;
      pin->setText( k, QString::number( pluginInfo.m_numParams ) ); k++;
      pin->setText( k, QString::number( pluginInfo.m_numOutputs ) ); k++;
      pin->setText( k, QString::number( pluginInfo.m_numInputs ) ); k++;
      pin->setText( k, QString::number( pluginInfo.m_can32Bit ) ); k++;
      pin->setText( k, QString::number( pluginInfo.m_can64Bit ) ); k++;
      pin->setText( k, QString::fromStdString( pluginInfo.m_entry ) ); k++;
      pin->setText( k, QString::fromStdWString( pluginInfo.m_uri ) ); k++;
      //parent->addChild( pin );
   }

   blockSignals( false );
}



void
PluginInfoTree::enterEvent( QEnterEvent* event )
{
   //m_isHovered = true;

//   if ( m_drag )
//   {
   m_isDragging= false;
   QDrag::cancel();
//      delete m_drag;
//      m_drag = nullptr;
//   }
   update();
   QWidget::enterEvent( event );
}
void
PluginInfoTree::leaveEvent( QEvent* event )
{
   //m_isHovered = false;
   update();
   QWidget::leaveEvent( event );
}

void
PluginInfoTree::focusInEvent( QFocusEvent* event )
{
   //m_hasFocus = true;
   update();
   QWidget::focusInEvent( event );
}

void
PluginInfoTree::focusOutEvent( QFocusEvent* event )
{
   //m_hasFocus = true;
   update();
   QWidget::focusOutEvent( event );
}

void
PluginInfoTree::mousePressEvent( QMouseEvent* event )
{
   QTreeWidget::mousePressEvent( event );

   int x = event->x();
   int y = event->y();
   auto selectedItem = itemAt( x, y );
   if ( m_isDragging )
   {
      if ( selectedItem != m_selectedItem )
      {
         m_isDragging = false;
      }
   }
   else
   {
      if ( selectedItem != m_synthNode  // dont Drag synth root
          && selectedItem != m_effectNode ) // dont Drag effect root
      {
         m_isDragging = true;
         m_dragStartX = x;
         m_dragStartY = y;
      }
   }

   m_selectedItem = selectedItem;
   //std::cout << "mousePressEvent" << std::endl;
   update();
}

void
PluginInfoTree::mouseReleaseEvent( QMouseEvent* event )
{
   QTreeWidget::mouseReleaseEvent( event );

   m_isDragging = false;
   //std::cout << "mouseReleaseEvent" << std::endl;
   update();
}

void
PluginInfoTree::mouseMoveEvent( QMouseEvent* event )
{
   QTreeWidget::mouseMoveEvent( event );

   //std::cout << "mouseMoveEvent" << std::endl;
   if ( m_isDragging && m_selectedItem )
   {
       DE_TRACE("Dragging")
      int dx = event->x() - m_dragStartX;
      int dy = event->y() - m_dragStartY;
      float len = sqrtf( float( (dx*dx) + (dy*dy) ) );
      if ( len > 13.5f )
      {
         auto drag = new QDrag( this );

         auto uri = m_selectedItem->data( 0, Qt::UserRole ).toString();
         bool is_synth = m_selectedItem->data( 1, Qt::UserRole ).toInt() != 0;

         auto mime = new QMimeData;
         mime->setText( uri );
         drag->setMimeData( mime );

         //QPixmap pixmap(34, 34);
         //pixmap.fill( Qt::blue );
         /*
         QPainter dc( &pixmap );
         dc.setRenderHint(QPainter::Antialiasing);
         dc.translate(15, 15);
         paint( &dc, 0, 0 );
         dc.end();

         pixmap.setMask(pixmap.createHeuristicMask());
         */

         if ( is_synth )
         {
            drag->setPixmap( m_app.m_synthIcon.pixmap(32,24) );
         }
         else
         {
            drag->setPixmap( m_app.m_effectIcon.pixmap(32,24) );
         }

         drag->setHotSpot( QPoint( 15, 20 ) );
         drag->exec( Qt::CopyAction ); // Qt::MoveAction |
      }

      update();

   }
//   m_mouseX = event->x();
//   m_mouseY = event->y();
}

void
PluginInfoTree::wheelEvent( QWheelEvent* event )
{
//   int wheel = event->angleDelta().y();
//   DE_DEBUG("MouseWheel = ",wheel )

//   if ( wheel >= 1 )
//   {
//      m_keyStart = std::clamp( m_keyStart - 3, 0, 125 );
//      update();
//   }
//   else if ( wheel <= 1 )
//   {
//      m_keyStart = std::clamp( m_keyStart + 3, 0, 125 );
//      update();
//   }

   QTreeWidget::wheelEvent( event );
}

