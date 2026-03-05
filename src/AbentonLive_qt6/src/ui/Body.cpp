#include "Body.h"
#include <QResizeEvent>
#include "App.h"
#include "Draw.h"

Body::Body( App & app, QWidget* parent )
   : QWidget( parent )
   , m_app( app )
{
   setMinimumSize(383,77);
   setMouseTracking( true );

   m_hoverButton = -1;
   m_isDragging = false;
   m_dragMode = 0; // 0 = Idle, 1 = splitterV, 2 = splitterH
   m_dragStartX = 0;
   m_dragStartY = 0;

   //for ( auto & synth : m_synths ) { synth = nullptr; }
   //for ( auto & key : m_touched ) { key = nullptr; }
   //setKeyRange( 24, 100 );

   m_app.m_detailStack = new QStackedWidget( this );

   // Arrangement:
   m_app.m_timeLineOverview = new TimeLineOverview( m_app, this );
   m_app.m_arrangement = new Arrangement( m_app, this );

   m_app.m_btnShowArrangement = createShowArrangementButton();
   m_app.m_btnShowSession = createShowSessionButton();
   m_app.m_arrangeScrollBar = new ScrollBar( m_app, this );
   m_app.m_btnComposeIO = createComposeIOButton();
   m_app.m_btnComposeR = createComposeRButton();
   m_app.m_btnComposeM = createComposeMButton();
   m_app.m_btnComposeD = createComposeDButton();

//   auto v = createVBox();
//   v->setContentsMargins( 0,0,0,0);
//   v->setSpacing( 5 );
//   v->addWidget( m_btnShowArrangement );
//   v->addWidget( m_btnShowSessionButton );
//   v->addWidget( m_arrangeScrollBar,1 );
//   v->addWidget( m_btnComposeIO );
//   v->addWidget( m_btnComposeR );
//   v->addWidget( m_btnComposeM );
//   v->addWidget( m_btnComposeD );
//   setLayout( v );

   //m_app.m_pluginExplorer = new PluginExplorer2( m_app, this );
   //m_deviceBar = new DeviceBar( m_app, this );

   // :: ExplorerPanel ::
   m_app.m_btnShowExplorer = createShowExplorerButton();
   m_app.m_btnShowInternDevices = createShowInternDevicesButton();
   m_app.m_btnShowExternDevices = createShowExternDevicesButton();
   m_app.m_btnShowExplorer1 = createShowExplorer1Button();
   m_app.m_btnShowExplorer2 = createShowExplorer2Button();
   m_app.m_btnShowExplorer3 = createShowExplorer3Button();
   m_app.m_explorerScrollBar = new ScrollBar( m_app, this );
   m_app.m_btnShowGrooves = createShowGroovesButton();
//   auto v = createVBox();
//   v->setContentsMargins( 0,0,0,0);
//   v->setSpacing( 5 );
//   v->addWidget( m_btnShowExplorer );
//   v->addWidget( m_btnShowInternDevices );
//   v->addWidget( m_btnShowExternDevices );
//   v->addWidget( m_btnShowExplorer1 );
//   v->addWidget( m_btnShowExplorer2 );
//   v->addWidget( m_btnShowExplorer3 );
//   v->addWidget( m_scrollBar,1 );
//   v->addWidget( m_btnShowGrooves );
//   setLayout( v );

   //m_explorerPanel = new ExplorerPanel( m_app, this );
   m_app.m_pluginExplorer = new PluginExplorer( m_app, this );

//   auto h = createHBox();
//   h->setContentsMargins( 0,0,0,0);
//   h->setSpacing( 0 );
//   h->addWidget( m_deviceBar );
//   h->addWidget(m_explorerPanel,1 );
//   h->addWidget(m_pluginExplorer,1 );
//   setLayout( h );

   connect( m_app.m_btnShowExplorer, SIGNAL(toggled(bool)),
            this, SLOT(on_btnShowExplorerPanel(bool)) );

}

Body::~Body()
{}

void
Body::on_btnShowExplorerPanel( bool checked )
{
   //DE_DEBUG("Toggled")
   LiveSkin & skin = m_app.m_skin;
   skin.isExplorerVisible = checked;
   updateLayout();
}

void
Body::updateLayout()
{
   int w = width();
   int h = height();

   // std::cout << "Body.updateLayout(" << w << "," << h << ")" << std::endl;

   LiveSkin & skin = m_app.m_skin;

   int p = skin.padding;

   // Min elements
   int headerH = 17 + 2*p;
   int vsplitH = p;
   int footerH = 12 + 16 + 2*p;

   // int minH =
//   int deviceBarWidth = 28;
//   int explorerWidth = m_hsplitterPos;
//   int arrangeWidth = 0;
//   int composeBarWidth = 0;

   skin.rc1 = QRect();
   skin.rcSplitterV = QRect();
   skin.rc2 = QRect();

   // Main vertical layout compute variable heights
   int row1H = 0;
   int row2H = 0;
   if ( skin.isDetailVisible )
   {
      #if 0
      row2H = skin.vsplitterPos; // ClipEditor has variable height determined by splitter v.
      #else
      if ( skin.isClipEdVisible )
      {
         row2H = skin.vsplitterPos; // ClipEditor has variable height determined by splitter v.
      }
      else
      {
         row2H = skin.spurEditorHeight; // Fx-Audio DSP chain has fixed height ( yet )
      }
      #endif
   }
   else
   {
      vsplitH = 0;
   }

   int availH = h - (headerH + vsplitH + footerH);
   if (availH > 0)
   {
      if ( row2H > availH )
      {
         row2H = availH;
      }
      else
      {
         row1H = availH - row2H;
      }
   }

   // Main vertical layout ( header+rc1+vsplit+rc2+footer )
   int y = 0;
   skin.rcHeader = QRect( 0, y, w, headerH );   y += headerH;
   skin.rc1 = QRect( 0, y, w, row1H );          y += row1H;
   skin.rcSplitterV = QRect( 0, y, w, vsplitH );        y += vsplitH;
   skin.rc2 = QRect( 0, y, w, row2H );          y += row2H;
   skin.rcFooter = QRect( 0, y, w, footerH );   y += footerH;

   if ( h != y )
   {
       DE_ERROR("h(",h,") != y(",y,")")
   }

   // HeaderPanel
   if ( m_app.m_headerPanel )
   {
      setWidgetBounds( m_app.m_headerPanel, skin.rcHeader );
      //m_headerPanel->updateLayout();
   }

   // FooterPanel
   if ( m_app.m_footerPanel )
   {
      setWidgetBounds( m_app.m_footerPanel, skin.rcFooter );
   }

   updateTopPanelLayout();
   updateExplorerPanelLayout();
   updateArrangementLayout();
   updateDetailPanelLayout();
   update();
}


void
Body::updateTopPanelLayout()
{
   LiveSkin & skin = m_app.m_skin;
   int p = skin.getInt( LiveSkin::Padding );
   int btnW = skin.getInt( LiveSkin::CircleButtonSize );
   int barW = btnW + 5;
   int explorerW = 0;
   int arrangeW = 0;

   int w = skin.rc1.width();
   int h = skin.rc1.height();
   int x = skin.rc1.x() + p;
   int y = skin.rc1.y();

   // Row1 Content : DeviceBar + PluginExplorer + GMixer + ComposeBar
   //m_deviceBarWidth = 23; // fix width
   //m_explorerWidthMin = 20; // fix width
   //m_arrangementWidth = 0; // computed
   //m_composeBarWidth = 23; // fix width
   int trW = w - 2*p; // - 2*barW;
   int trH = h;

   // H-Splitter
   if ( skin.isExplorerVisible )
   {
      if ( trW < 2*barW + p )
      {

      }
      else
      {
         if ( trW < 2*barW + p + skin.hsplitterPos )
         {
            explorerW = trW - 2*barW - p;
         }
         else
         {
            explorerW = skin.hsplitterPos;
            arrangeW = trW - 2*barW - p - explorerW;
         }
      }

      if ( arrangeW < 100 )
      {
         explorerW -= 100 - arrangeW;
         arrangeW = 100;
         //m_hsplitterPos = explorerW;
      }

      skin.rcTopLeft = QRect( x, y, explorerW + barW, trH );
      //skin.rc1DeviceBar = QRect( x, y, btnW, trH );
      //skin.rc1ExplorerPanel = QRect( x + barW, y, explorerW, trH );
      //skin.rc1ExplorerContent = QRect( x + barW + 6, y + 6, explorerW - 12, trH - 12 );
      x += explorerW + barW;

      skin.rcSplitterH = QRect( x, y, p, trH );
      x += p;

      skin.rcTopRight = QRect( x, y, arrangeW + barW, trH );
      //skin.rc1ArrangementPanel = QRect( x, y, arrangeW, trH );
      //skin.rc1ArrangementContent = QRect( x + 6, y + 6, arrangeW - 12, trH - 12 );
      //skin.rc1ComposeBar = QRect( x + arrangeW + 5, y, btnW, trH );
   }
   else
   {
      arrangeW = trW - barW;
      skin.rcTopLeft = QRect( x, y, barW, trH ); x += barW;
      skin.rcSplitterH = QRect();
      skin.rcTopRight = QRect( x, y, arrangeW, trH ); x += arrangeW;
      //skin.rc1ArrangementPanel = QRect( x, y, arrangeW, trH );
      //skin.rc1ArrangementContent = QRect( x + 6, y + 6, arrangeW - 12, trH - 12 );
      //skin.rc1ComposeBar = QRect( x + arrangeW + 5, y, btnW, trH );
   }
}

void
Body::updateExplorerPanelLayout()
{
   LiveSkin & skin = m_app.m_skin;
   int w = skin.rcTopLeft.width();
   int h = skin.rcTopLeft.height();
   int x = skin.rcTopLeft.x();
   int y = skin.rcTopLeft.y();

   int b = skin.getInt( LiveSkin::CircleButtonSize );
   int s = skin.getInt( LiveSkin::Spacing );
   int bs = b + s;
   //int p = skin.getInt( LiveSkin::Padding );

   //skin.rcTopLeft = QRect( x, y, explorerW + barW, trH );
   skin.rcDeviceBar = QRect( x, y, b, h );

   skin.rcExplorerPanel = QRect( x + bs, y, w - bs, h );

   skin.rcExplorerContent = QRect( skin.rcExplorerPanel.x() + 6,
                                skin.rcExplorerPanel.y() + 6,
                                skin.rcExplorerPanel.width() - 12,
                                skin.rcExplorerPanel.height() - 12 );

   x = skin.rcDeviceBar.x();
   y = skin.rcDeviceBar.y();
   //w = m_rcDeviceBar.x();
   //h = m_rcDeviceBar.y();
   setWidgetBounds( m_app.m_btnShowExplorer, QRect( x, y, b, b ) ); y += bs;
   setWidgetBounds( m_app.m_btnShowInternDevices, QRect( x, y, b, b ) ); y += bs;
   setWidgetBounds( m_app.m_btnShowExternDevices, QRect( x, y, b, b ) ); y += bs;
   setWidgetBounds( m_app.m_btnShowExplorer1, QRect( x, y, b, b ) ); y += bs;
   setWidgetBounds( m_app.m_btnShowExplorer2, QRect( x, y, b, b ) ); y += bs;
   setWidgetBounds( m_app.m_btnShowExplorer3, QRect( x, y, b, b ) ); y += bs;

   int sy = h - 7*bs;
   if ( sy < 50 ) sy = 50;
   skin.rcExplorerScrollBarPanel = QRect( x, y, b, sy );
   skin.rcExplorerScrollBar = QRect( skin.rcExplorerScrollBarPanel.x() + 7,
                                skin.rcExplorerScrollBarPanel.y() + 7,
                                9,
                                skin.rcExplorerScrollBarPanel.height() - 2*7 );

   setWidgetBounds( m_app.m_explorerScrollBar, skin.rcExplorerScrollBar ); y += sy + s;
   setWidgetBounds( m_app.m_btnShowGrooves, QRect( x, y, b, b ) ); y += bs;

   setWidgetBounds( m_app.m_pluginExplorer, skin.rcExplorerContent );
}

void
Body::updateArrangementLayout()
{
   LiveSkin & skin = m_app.m_skin;
   int p = skin.getInt( LiveSkin::Padding );
   int b = skin.getInt( LiveSkin::CircleButtonSize );
   int s = skin.getInt( LiveSkin::Spacing );
   int bs = b + s;
   int c = skin.getInt( LiveSkin::SmallCircleButtonSize );

   int w = skin.rcTopRight.width();
   int h = skin.rcTopRight.height();
   int x = skin.rcTopRight.x();
   int y = skin.rcTopRight.y();

   skin.rcArrangePanel = QRect( x, y, w - bs, h );
   skin.rcArrangePanelContent = QRect( x+6, y+6, w - bs - 12, h - 12 );
   skin.rcComposeBar = QRect( x+w-1-b, y, b, h );

   int m_timeLineH = m_app.m_timeLineOverview->computeBestHeight();

   skin.rcArrangeOverview = QRect( skin.rcArrangePanelContent.x(),
                         skin.rcArrangePanelContent.y(),
                         skin.rcArrangePanelContent.width(),
                         m_timeLineH );

   setWidgetBounds( m_app.m_timeLineOverview, skin.rcArrangeOverview );

   skin.rcArrangeContent = QRect( skin.rcArrangePanelContent.x(),
                            skin.rcArrangePanelContent.y() + m_timeLineH + p,
                            skin.rcArrangePanelContent.width(),
                            skin.rcArrangePanelContent.height() - m_timeLineH - p );

   setWidgetBounds( m_app.m_arrangement, skin.rcArrangeContent );

   x = skin.rcComposeBar.x();
   y = skin.rcComposeBar.y();
   //w = skin.rcDeviceBar.x();
   //h = skin.rcDeviceBar.y();
   setWidgetBounds( m_app.m_btnShowArrangement, QRect( x, y, b, b ) ); y += bs;
   setWidgetBounds( m_app.m_btnShowSession, QRect( x, y, b, b ) ); y += bs;

   int sy = h - 2*bs - s - 4*(c+s)-s;
   if ( sy < 50 ) sy = 50;

   x += 7;
   skin.rcArrangeScrollBarPanel = QRect( x, y, bs, sy );

   skin.rcArrangeScrollBar = QRect( skin.rcArrangeScrollBarPanel.x(),
                                skin.rcArrangeScrollBarPanel.y() + 7,
                                9,
                                skin.rcArrangeScrollBarPanel.height() - 2*7 );

   setWidgetBounds( m_app.m_arrangeScrollBar, skin.rcArrangeScrollBar ); y += sy + s;

   x -= 1;
   setWidgetBounds( m_app.m_btnComposeIO, QRect( x, y, c, c ) ); y += c+s;
   setWidgetBounds( m_app.m_btnComposeR, QRect( x, y, c, c ) ); y += c+s;
   setWidgetBounds( m_app.m_btnComposeM, QRect( x, y, c, c ) ); y += c+s;
   setWidgetBounds( m_app.m_btnComposeD, QRect( x, y, c, c ) ); y += c+s;

   //if ( w < 4 ) return;
   //if ( h < 4 ) return;
}


void
Body::updateDetailPanelLayout()
{
   LiveSkin & skin = m_app.m_skin;
   int p = skin.padding;

   int w = skin.rc2.width();
   int h = skin.rc2.height();
   int x = skin.rc2.x() + p;
   int y = skin.rc2.y();

   // Row1 Content : DeviceBar + PluginExplorer + GMixer + ComposeBar
   //m_deviceBarWidth = 23; // fix width
   //m_explorerWidthMin = 20; // fix width
   //m_arrangementWidth = 0; // computed
   //m_composeBarWidth = 23; // fix width

   // Bottom Rc2 sub rects
   skin.rcQuickHelpPanel = QRect();
   skin.rcQuickHelpContent = QRect();
   skin.rcDetailPanel = QRect();
   skin.rcDetailContent = QRect();

   //DE_DEBUG( "skin.isQuickHelpVisible = ", skin.isQuickHelpVisible)

   if ( skin.isDetailVisible )
   {
      int drW = w - 2*p;
      int drH = h;
      int helpW = 0;

      if ( skin.isQuickHelpVisible )
      {
         helpW = skin.quickHelpWidth;

         skin.rcQuickHelpPanel = QRect( x, y, helpW, drH );

         x += helpW + p;

         int clipW = drW - helpW - p;
         skin.rcDetailPanel = QRect( x, y, clipW, drH );
      }
      else
      {
         int clipW = drW;
         skin.rcDetailPanel = QRect( x, y, clipW, drH );
      }
   }

   skin.rcQuickHelpContent = QRect( skin.rcQuickHelpPanel.x() + 6,
                                 skin.rcQuickHelpPanel.y() + 6,
                                 skin.rcQuickHelpPanel.width() - 12,
                                 skin.rcQuickHelpPanel.height() - 12 );

   skin.rcDetailContent = QRect( skin.rcDetailPanel.x() + 6,
                                 skin.rcDetailPanel.y() + 6,
                                 skin.rcDetailPanel.width() - 12,
                                 skin.rcDetailPanel.height() - 12 );

   // Toggle <=> {ClipEditor|SpurEditor}
   if ( skin.isDetailVisible )
   {
      if ( skin.isClipEdVisible )
      {
         setWidgetBounds( m_app.m_clipContent, skin.rcDetailContent );  // Show ClipEditor
         setWidgetBounds( m_app.m_detailStack, QRect() );  // Hide TrackEditor
      }
      else // if ( !m_app.m_isClipEdVisible )
      {
         setWidgetBounds( m_app.m_clipContent, QRect() );  // Hide ClipEditor
         setWidgetBounds( m_app.m_detailStack, skin.rcDetailContent );  // Show TrackEditor
      }

   }
   else
   {
      setWidgetBounds( m_app.m_clipContent, QRect() );  // Hide ClipEditor
      setWidgetBounds( m_app.m_detailStack, QRect() );  // Hide TrackEditor
   }
}

/*
int32_t
Body::findKey( int midiNote ) const
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

void
Body::setKeyRange( int midiNoteStart, int midiNoteCount )
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
*/

void
Body::resizeEvent( QResizeEvent* event )
{
   int w = event->size().width();
   int h = event->size().height();
   topLevelWidget()->setWindowTitle( m_app.appName
                        + QString(" | WindowSize(%1,%2)").arg(w).arg(h) );
   updateLayout();

   QWidget::resizeEvent( event );
}

void
Body::paintEvent( QPaintEvent* event )
{
   int w = width();
   int h = height();
   if ( w > 1 && h > 1 )
   {
      LiveSkin const & skin = m_app.m_skin;
      int r = skin.radius;
      int s = skin.getInt( LiveSkin::Spacing );
      int b = skin.getInt( LiveSkin::CircleButtonSize );
      int p = skin.getInt( LiveSkin::Padding );

      QPainter dc( this );
      //dc.setRenderHint( QPainter::NonCosmeticDefaultPen );
      //dc.fillRect( rect(), skin.windowColor );

      //QColor c = skin.panelColor; //  hasFocus() ? skin.focusColor :
      //QColor f = skin.focusColor; //  hasFocus() ? skin.focusColor :


      // ||| ExplorerPanel |||
      if ( m_app.m_focusPanel == eLivePanelExplorer )
      {
         drawRoundRectFill( dc, skin.rcExplorerPanel, skin.focusColor, r, r );
      }
      else
      {
         drawRoundRectFill( dc, skin.rcExplorerPanel, skin.panelColor, r, r );
      }

      // ||| ArrangePanel |||
      if ( m_app.m_focusPanel == eLivePanelArrange )
      {
         drawRoundRectFill( dc, skin.rcArrangePanel, skin.focusColor, r, r );
      }
      else
      {
         drawRoundRectFill( dc, skin.rcArrangePanel, skin.panelColor, r, r );
      }

      // ||| QuickHelpPanel |||
      if ( m_app.m_focusPanel == eLivePanelQuickHelp
            && skin.isQuickHelpVisible )
      {
         drawRoundRectFill( dc, skin.rcQuickHelpPanel, skin.focusColor, r, r );
      }
      else
      {
         drawRoundRectFill( dc, skin.rcQuickHelpPanel, skin.panelColor, r, r );
      }

      // ||| DetailPanel |||
      if ( m_app.m_focusPanel == eLivePanelDetails )
      {
         drawRoundRectFill( dc, skin.rcDetailPanel, skin.focusColor, r, r );
      }
      else
      {
         drawRoundRectFill( dc, skin.rcDetailPanel, skin.panelColor, r, r );
      }

      // ||| DeviceBar :: ScrollBar
      int x = skin.rcExplorerScrollBarPanel.x();
      int y = skin.rcExplorerScrollBarPanel.y();
      w = skin.rcExplorerScrollBarPanel.width() + 3*s;
      h = skin.rcExplorerScrollBarPanel.height();
      drawRoundRectFill( dc, QRect(x,y,w,h), skin.panelColor, r, r );

      // ||| ComposeBar |||
      int bs = b + s;
      x = skin.rcComposeBar.x() - 2* p;
      y = skin.rcComposeBar.y() + 2 * bs;
      int m = skin.rcComposeBar.width() + 2*p;
      int n = h - y;
      drawRoundRectFill( dc, QRect(x,y,m,n), skin.panelColor, r, r );

      // ||| SplitterH |||
      drawRectBorder( dc, skin.rcSplitterH, QColor(255,129,30) );
      if ( m_app.m_isOverSplitH )
      {
         drawRectBorder( dc, skin.rcSplitterH, 0xFFFFFFFF );
      }

      // ||| SplitterV |||
      drawRectBorder( dc, skin.rcSplitterV, QColor(255,129,30) );
      if ( m_app.m_isOverSplitV )
      {
         drawRectBorder( dc, skin.rcSplitterV, 0xFFFFFFFF );
      }

   }

   QWidget::paintEvent( event );

}

void
Body::enterEvent( QEnterEvent* event )
{
   QWidget::enterEvent( event );
}

void
Body::leaveEvent( QEvent* event )
{
//   if ( m_app.m_isOverSplitV )
//   {
//      m_isOverSplitV = false;
//      updateLayout();
//   }
   QWidget::leaveEvent( event );
}

void
Body::mouseMoveEvent( QMouseEvent* event )
{
   int mx = event->x();
   int my = event->y();

   LiveSkin & skin = m_app.m_skin;

   // === Find hover splitter ===
   m_app.m_isOverSplitV = isMouseOverRect( mx, my, skin.rcSplitterV );
   m_app.m_isOverSplitH = isMouseOverRect( mx, my, skin.rcSplitterH );
   //m_app.m_isOverSplitH = isMouseOverRect( mx, my, m_app.m_rc1Splitter );

   if ( isMouseOverRect( mx,my, skin.rcExplorerPanel ) )
   {
      m_app.m_hoverPanel = eLivePanelExplorer;
   }
   else if ( isMouseOverRect( mx,my, skin.rcArrangePanel ) )
   {
      m_app.m_hoverPanel = eLivePanelArrange;
   }
   else if ( isMouseOverRect( mx,my, skin.rcQuickHelpPanel ) )
   {
      m_app.m_hoverPanel = eLivePanelQuickHelp;
   }
   else if ( isMouseOverRect( mx,my, skin.rcDetailPanel ) )
   {
      m_app.m_hoverPanel = eLivePanelDetails;
   }
   else
   {
      m_app.m_hoverPanel = eLivePanelCount;
   }

   if ( m_app.m_isOverSplitV )
   {
      if ( cursor().shape() != Qt::SizeVerCursor )
      {
         setCursor( QCursor( Qt::SizeVerCursor ) );
      }
   }
   else if ( m_app.m_isOverSplitH )
   {
      if ( cursor().shape() != Qt::SizeHorCursor )
      {
         setCursor( QCursor( Qt::SizeHorCursor ) );
      }
   }
   else
   {
      if ( cursor().shape() != Qt::ArrowCursor )
      {
         setCursor( QCursor( Qt::ArrowCursor ) );
      }
   }

   // === Update layout when splitting h ===
   if ( m_dragMode == 2 ) // hsplit
   {
      skin.hsplitterPos = m_dragData + (mx - m_dragStartX);
      int k = skin.padding + skin.circleButtonSize + 5;
      int x1 = k;
      int x2 = width() - 1 - k;
      if ( skin.hsplitterPos < x1 )
      {
         skin.hsplitterPos = x1;
      }
      else if ( skin.hsplitterPos >= x2 )
      {
         skin.hsplitterPos = x2;
      }

      updateLayout();
   }



   if ( skin.isDetailVisible && skin.isClipEdVisible )
   {
      // === Update layout when splitting v ===
      if ( m_dragMode == 1 ) // vsplit
      {
         skin.vsplitterPos = m_dragData - (my - m_dragStartY);
         int y1 = skin.rcHeader.y() + skin.rcHeader.height();
         int y2 = skin.rcFooter.y();
         if ( skin.vsplitterPos < y1 )
         {
            skin.vsplitterPos = y1;
         }
         else if ( skin.vsplitterPos > y2 )
         {
            skin.vsplitterPos = y2;
         }

         updateLayout();
      }
   }


   QWidget::mouseMoveEvent( event );
}


void
Body::mousePressEvent( QMouseEvent* event )
{
   int mx = event->x();
   int my = event->y();

   LiveSkin & skin = m_app.m_skin;

   if ( m_dragMode > 0 )
   {

   }
   else
   {
      if ( m_app.m_isOverSplitH )
      {
         m_dragMode = 2;
         //m_app.m_dragData = m_app.m_hsplitterPos; // Store original pos.
         m_dragData = skin.rcSplitterH.x(); // Store original pos.
         m_dragStartX = mx;
         m_dragStartY = my;
         updateLayout();
      }

      if ( skin.isDetailVisible && skin.isClipEdVisible )
      {
         if ( m_app.m_isOverSplitV )
         {
            m_dragMode = 1;
            m_dragData = skin.vsplitterPos; // Store original pos.
            //m_app.m_dragData = m_app.m_rcSplitterV.y(); // Store original pos.
            m_dragStartX = mx;
            m_dragStartY = my;
            updateLayout();
         }
      }

   }

   QWidget::mousePressEvent( event );
}

void
Body::mouseReleaseEvent( QMouseEvent* event )
{
   if ( m_dragMode > 0 )
   {
      if ( m_dragMode == 1 )
      {
      }
      else if ( m_dragMode == 2 )
      {
      }

      updateLayout();
      m_dragMode = 0;
   }

   int mx = event->x();
   int my = event->y();

   LiveSkin & skin = m_app.m_skin;

   if ( isMouseOverRect( mx,my, skin.rcExplorerPanel ) )
   {
      m_app.m_focusPanel = eLivePanelExplorer;
      update();
   }
   else if ( isMouseOverRect( mx,my, skin.rcArrangePanel ) )
   {
      m_app.m_focusPanel = eLivePanelArrange;
      update();
   }
   else if ( isMouseOverRect( mx,my, skin.rcQuickHelpPanel ) )
   {
      m_app.m_focusPanel = eLivePanelQuickHelp;
      update();
   }
   else if ( isMouseOverRect( mx,my, skin.rcDetailPanel ) )
   {
      m_app.m_focusPanel = eLivePanelDetails;
      update();
   }
//   else
//   {
//      m_app.m_focusPanel = eLivePanelCount;
//      update();
//   }
   QWidget::mouseReleaseEvent( event );
}


void
Body::wheelEvent( QWheelEvent* event )
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

//   m_app.m_clipEditor.wheelEvent( me );

   QWidget::wheelEvent( event );
}

void
Body::keyPressEvent( QKeyEvent* event )
{
   //DE_DEBUG("KeyPress(",event->key(),")")
   QWidget::keyPressEvent( event );
}

void
Body::keyReleaseEvent( QKeyEvent* event )
{
   //DE_DEBUG("KeyRelease(",event->key(),")")
   QWidget::keyReleaseEvent( event );
}



// ExplorerPanel


ImageButton*
Body::createShowExplorerButton()
{
   auto btn = new ImageButton( this );
   btn->setCheckable( true );
   btn->setChecked( true );

   LiveSkin const & skin = m_app.m_skin;
   int bw = skin.circleButtonSize;
   int bh = skin.circleButtonSize;
   auto symColor = skin.symbolColor;
   auto bgColor = skin.windowColor;
   auto fgColor = skin.panelColor; // or panelColor

   //QFont font = getFontAwesome( 14 );
   std::string
   msg = "#########\n"
         "#########\n"
         " #######\n"
         " #######\n"
         "  #####\n"
         "  #####\n"
         "   ###\n"
         "   ###\n"
         "    #\n";
   // [idle]
   QImage ico = createAsciiArt( symColor, fgColor, msg );
   QImage img = createCircleImage( bw,bh, bgColor, fgColor, ico );
   btn->setImage( 0, img );
   // [idle_hover]
   btn->setImage( 1, img );

   // [active]
   msg = "##\n"
         "####\n"
         "######\n"
         "########\n"
         "#########\n"
         "########\n"
         "######\n"
         "####\n"
         "##\n";

   ico = createAsciiArt( symColor, fgColor, msg );
   img = createCircleImage( bw,bh, bgColor, fgColor, ico );
   btn->setImage( 2, img );
   // [active_hover]
   btn->setImage( 3, img );

   return btn;
}

ImageButton*
Body::createShowInternDevicesButton()
{
   auto btn = new ImageButton( this );
   btn->setCheckable( true );
   btn->setChecked( false );
   btn->setEnabled( false );

   LiveSkin const & skin = m_app.m_skin;
   int bw = skin.circleButtonSize;
   int bh = skin.circleButtonSize;
   auto symColor = skin.symbolColor;
   auto bgColor = skin.windowColor;
   auto fgColor = skin.panelColor;

   //QFont font = getFontAwesome( 14 );
   std::string
   msg = " ########### \n"
         "##         ##\n"
         "##         ##\n"
         "#############\n"
         "#############\n"
         "#############\n"
         "#############\n"
         "#############\n"
         "#############\n"
         " ########### \n";
   // [idle]
   QImage ico = createAsciiArt( symColor, fgColor, msg );
   QImage img = createCircleImage( bw,bh, bgColor, fgColor, ico );
   btn->setImage( 0, img );
   // [idle_hover]
   btn->setImage( 1, img );

   // [active]
   ico = createAsciiArt( skin.activeColor, fgColor, msg );
   img = createCircleImage( bw,bh, bgColor, fgColor, ico );
   btn->setImage( 2, img );
   // [active_hover]
   btn->setImage( 3, img );
   return btn;
}

ImageButton*
Body::createShowExternDevicesButton()
{
   auto btn = new ImageButton( this );
   btn->setCheckable( true );
   btn->setChecked( false );
   //btn->setEnabled( false );

   LiveSkin const & skin = m_app.m_skin;
   int bw = skin.circleButtonSize;
   int bh = skin.circleButtonSize;

   auto symColor = skin.symbolColor;
   auto bgColor = skin.windowColor;
   auto fgColor = skin.panelColor;

   //QFont font = getFontAwesome( 14 );
   std::string
   msg = "      ####\n"
         "   #  #######\n"
         "   #######\n"
         "##########\n"
         "   #######\n"
         "   #  #######\n"
         "      ####\n";
   // [idle]
   QImage ico = createAsciiArt( symColor, fgColor, msg );
   QImage img = createCircleImage( bw,bh, bgColor, fgColor, ico );
   btn->setImage( 0, img );
   // [idle_hover]
   btn->setImage( 1, img );

   // [active]
   ico = createAsciiArt( skin.activeColor, fgColor, msg );
   img = createCircleImage( bw,bh, bgColor, fgColor, ico );
   btn->setImage( 2, img );
   // [active_hover]
   btn->setImage( 3, img );
   return btn;
}

ImageButton*
Body::createShowExplorer1Button()
{
   auto btn = new ImageButton( this );
   btn->setCheckable( true );
   btn->setChecked( false );
   btn->setEnabled( false );

   LiveSkin const & skin = m_app.m_skin;
   int bw = skin.circleButtonSize;
   int bh = skin.circleButtonSize;
   auto symColor = skin.symbolColor;
   auto bgColor = skin.windowColor;
   auto fgColor = skin.panelColor;

   //QFont font = getFontAwesome( 14 );
   std::string
   msg = "#####\n"
         "#####\n"
         "###########\n"
         "###     ###\n"
         "### ##  ###\n"
         "###  #  ###\n"
         "###  #  ###\n"
         "###     ###\n"
         "###########\n";
   // [idle]
   QImage ico = createAsciiArt( symColor, fgColor, msg );
   QImage img = createCircleImage( bw,bh, bgColor, fgColor, ico );
   btn->setImage( 0, img );
   // [idle_hover]
   btn->setImage( 1, img );

   // [active]
   ico = createAsciiArt( skin.activeColor, fgColor, msg );
   img = createCircleImage( bw,bh, bgColor, fgColor, ico );
   btn->setImage( 2, img );
   // [active_hover]
   btn->setImage( 3, img );
   return btn;
}

ImageButton*
Body::createShowExplorer2Button()
{
   auto btn = new ImageButton( this );
   btn->setCheckable( true );
   btn->setChecked( false );
   btn->setEnabled( false );

   LiveSkin const & skin = m_app.m_skin;
   int bw = skin.circleButtonSize;
   int bh = skin.circleButtonSize;
   auto symColor = skin.symbolColor;
   auto bgColor = skin.windowColor;
   auto fgColor = skin.panelColor;

   //QFont font = getFontAwesome( 14 );
   std::string
   msg = "#####\n"
         "#####\n"
         "###########\n"
         "####   ####\n"
         "###  #  ###\n"
         "##  # #  ##\n"
         "##    #  ##\n"
         "##   #   ##\n"
         "### ### ###\n"
         "####   ####\n"
         "###########\n";

   // [idle]
   QImage ico = createAsciiArt( symColor, fgColor, msg );
   QImage img = createCircleImage( bw,bh, bgColor, fgColor, ico );
   btn->setImage( 0, img );
   // [idle_hover]
   btn->setImage( 1, img );

   // [active]
   ico = createAsciiArt( skin.activeColor, fgColor, msg );
   img = createCircleImage( bw,bh, bgColor, fgColor, ico );
   btn->setImage( 2, img );
   // [active_hover]
   btn->setImage( 3, img );
   return btn;
}

ImageButton*
Body::createShowExplorer3Button()
{
   auto btn = new ImageButton( this );
   btn->setCheckable( true );
   btn->setChecked( false );
   btn->setEnabled( false );

   LiveSkin const & skin = m_app.m_skin;
   int bw = skin.circleButtonSize;
   int bh = skin.circleButtonSize;
   auto symColor = skin.symbolColor;
   auto bgColor = skin.windowColor;
   auto fgColor = skin.panelColor;

   //QFont font = getFontAwesome( 14 );
   std::string
   msg = "#####\n"
         "#####\n"
         "###########\n"
         "###     ###\n"
         "### ##  ###\n"
         "###  #  ###\n"
         "###  #  ###\n"
         "###     ###\n"
         "###########\n";
   // [idle]
   QImage ico = createAsciiArt( symColor, fgColor, msg );
   QImage img = createCircleImage( bw,bh, bgColor, fgColor, ico );
   btn->setImage( 0, img );
   // [idle_hover]
   btn->setImage( 1, img );

   // [active]

   ico = createAsciiArt( skin.activeColor, fgColor, msg );
   img = createCircleImage( bw,bh, bgColor, fgColor, ico );
   btn->setImage( 2, img );
   // [active_hover]
   btn->setImage( 3, img );
   return btn;
}

ImageButton*
Body::createShowGroovesButton()
{
   auto btn = new ImageButton( this );
   btn->setCheckable( true );
   btn->setChecked( false );
   //btn->setEnabled( false );

   LiveSkin const & skin = m_app.m_skin;
   int bw = skin.circleButtonSize;
   int bh = skin.circleButtonSize;
   auto symColor = skin.symbolColor;
   auto bgColor = skin.windowColor;
   auto fgColor = skin.panelColor;

   //QFont font = getFontAwesome( 14 );
   std::string
   msg = "  ##\n"
         " #  #\n"
         "#    #    #\n"
         "      #  #\n"
         "       ##\n"
         "  ##\n"
         " #  #\n"
         "#    #    #\n"
         "      #  #\n"
         "       ##\n";

   // [idle]
   QImage ico = createAsciiArt( symColor, fgColor, msg );
   QImage img = createCircleImage( bw,bh, bgColor, fgColor, ico );
   btn->setImage( 0, img );
   // [idle_hover]
   btn->setImage( 1, img );

   // [active]
   ico = createAsciiArt( skin.activeColor, fgColor, msg );
   img = createCircleImage( bw,bh, bgColor, fgColor, ico );
   btn->setImage( 2, img );
   // [active_hover]
   btn->setImage( 3, img );
   return btn;
}













// ArrangementPanel


ImageButton*
Body::createShowArrangementButton()
{
   auto btn = new ImageButton( this );
   LiveSkin const & skin = m_app.m_skin;

   int cbs = skin.getInt( LiveSkin::CircleButtonSize );

   btn->setCheckable( true );
   btn->setChecked( true );

   auto symColor = skin.symbolColor;
   auto bgColor = skin.windowColor;
   auto fgColor = skin.focusColor; // or panelColor

   //QFont font = getFontAwesome( 14 );
   std::string
   msg = "###########\n"
         "###########\n"
         "###########\n"
         " \n"
         " \n"
         "###########\n"
         "###########\n"
         "###########\n"
         " \n"
         " \n"
         "###########\n"
         "###########\n"
         "###########\n"
         ;

   // [idle]
   QImage ico = createAsciiArt( symColor, fgColor, msg );
   QImage img = createCircleImage( cbs,cbs, bgColor, fgColor, ico );
   btn->setImage( 0, img );
   // [idle_hover]
   btn->setImage( 1, img );

   // [active]
   ico = createAsciiArt( skin.activeColor, fgColor, msg );
   img = createCircleImage( cbs,cbs, bgColor, fgColor, ico );
   btn->setImage( 2, img );
   // [active_hover]
   btn->setImage( 3, img );
   return btn;
}

ImageButton*
Body::createShowSessionButton()
{
   auto btn = new ImageButton( this );
   LiveSkin const & skin = m_app.m_skin;

   int cbs = skin.getInt( LiveSkin::CircleButtonSize );

   btn->setCheckable( true );
   btn->setChecked( false );

   auto symColor = skin.symbolColor;
   auto bgColor = skin.windowColor;
   auto fgColor = skin.focusColor; // or panelColor

   std::string
   msg = "###  ###  ###\n"
         "###  ###  ###\n"
         "###  ###  ###\n"
         "###  ###  ###\n"
         "###  ###  ###\n"
         "###  ###  ###\n"
         "###  ###  ###\n"
         "###  ###  ###\n"
         "###  ###  ###\n"
         ;

   // [idle]
   QImage ico = createAsciiArt( symColor, fgColor, msg );
   QImage img = createCircleImage( cbs,cbs, bgColor, fgColor, ico );
   btn->setImage( 0, img );
   // [idle_hover]
   btn->setImage( 1, img );

   // [active]
   ico = createAsciiArt( skin.activeColor, fgColor, msg );
   img = createCircleImage( cbs,cbs, bgColor, fgColor, ico );
   btn->setImage( 2, img );
   // [active_hover]
   btn->setImage( 3, img );
   return btn;
}


ImageButton*
Body::createComposeIOButton()
{
   auto btn = new ImageButton( this );
   LiveSkin const & skin = m_app.m_skin;

   int bs = skin.getInt( LiveSkin::SmallCircleButtonSize );

   btn->setCheckable( true );
   btn->setChecked( true );

   auto symColor = skin.symbolColor;
   auto bgColor = skin.panelColor;
   auto fgColor = skin.windowColor;

   std::string
   msg = "#  ###\n"
         "# #   #\n"
         "# #   #\n"
         "# #   #\n"
         "#  ###\n";

   // [idle]
   QImage ico = createAsciiArt( skin.titleColor, symColor, msg );
   QImage img = createCircleImage( bs,bs, QColor(0,0,0,0), symColor, ico );
   btn->setImage( 0, img );
   // [idle_hover]
   btn->setImage( 1, img );

   // [active]
   ico = createAsciiArt( symColor, skin.activeColor, msg );
   img = createCircleImage( bs,bs, QColor(0,0,0,0), skin.activeColor, ico );
   btn->setImage( 2, img );
   // [active_hover]
   btn->setImage( 3, img );
   return btn;
}

ImageButton*
Body::createComposeRButton()
{
   auto btn = new ImageButton( this );
   LiveSkin const & skin = m_app.m_skin;
   int bs = skin.getInt( LiveSkin::SmallCircleButtonSize );

   btn->setCheckable( true );
   btn->setChecked( true );

   auto symColor = skin.symbolColor;
   auto bgColor = skin.panelColor;
   auto fgColor = skin.windowColor;

   std::string
   msg = "####\n"
         "#   #\n"
         "####\n"
         "# #\n"
         "#  ##\n";

   // [idle]
   QImage ico = createAsciiArt( skin.titleColor, symColor, msg );
   QImage img = createCircleImage( bs,bs, QColor(0,0,0,0), symColor, ico );
   btn->setImage( 0, img );
   // [idle_hover]
   btn->setImage( 1, img );

   // [active]
   ico = createAsciiArt( symColor, skin.activeColor, msg );
   img = createCircleImage( bs,bs, QColor(0,0,0,0), skin.activeColor, ico );
   btn->setImage( 2, img );
   // [active_hover]
   btn->setImage( 3, img );
   return btn;
}

ImageButton*
Body::createComposeMButton()
{
   auto btn = new ImageButton( this );
   LiveSkin const & skin = m_app.m_skin;
   int bs = skin.getInt( LiveSkin::SmallCircleButtonSize );

   btn->setCheckable( true );
   btn->setChecked( true );

   auto symColor = skin.symbolColor;
   auto bgColor = skin.panelColor;
   auto fgColor = skin.windowColor;

   std::string
   msg = "## ##\n"
         "# # #\n"
         "# # #\n"
         "#   #\n"
         "#   #\n";

   // [idle]
   QImage ico = createAsciiArt( skin.titleColor, symColor, msg );
   QImage img = createCircleImage( bs,bs, QColor(0,0,0,0), symColor, ico );
   btn->setImage( 0, img );
   // [idle_hover]
   btn->setImage( 1, img );

   // [active]
   ico = createAsciiArt( symColor, skin.activeColor, msg );
   img = createCircleImage( bs,bs, QColor(0,0,0,0), skin.activeColor, ico );
   btn->setImage( 2, img );
   // [active_hover]
   btn->setImage( 3, img );
   return btn;
}

ImageButton*
Body::createComposeDButton()
{
   auto btn = new ImageButton( this );
   LiveSkin const & skin = m_app.m_skin;
   int bs = skin.getInt( LiveSkin::SmallCircleButtonSize );

   btn->setCheckable( true );
   btn->setChecked( false );

   auto symColor = skin.symbolColor;
   auto bgColor = skin.panelColor;
   auto fgColor = skin.windowColor;

   std::string
   msg = "####\n"
         "#   #\n"
         "#   #\n"
         "#   #\n"
         "####\n";

   // [idle]
   QImage ico = createAsciiArt( skin.titleColor, symColor, msg );
   QImage img = createCircleImage( bs,bs, QColor(0,0,0,0), symColor, ico );
   btn->setImage( 0, img );
   // [idle_hover]
   btn->setImage( 1, img );

   // [active]
   ico = createAsciiArt( symColor, skin.activeColor, msg );
   img = createCircleImage( bs,bs, QColor(0,0,0,0), skin.activeColor, ico );
   btn->setImage( 2, img );
   // [active_hover]
   btn->setImage( 3, img );
   return btn;
}
