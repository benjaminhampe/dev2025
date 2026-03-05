/// (c) 2017 - 20180 Benjamin Hampe <benjaminhampe@gmx.de>

#pragma once
#include "LiveSkin.h"
#include "ImageButton.h"

struct App;

// ============================================================================
class Body : public QWidget
// ============================================================================
{
   DE_CREATE_LOGGER("Body")
   Q_OBJECT
public:
   Body( App & app, QWidget* parent = nullptr );
   ~Body() override;

   //int32_t findKey( int midiNote ) const;
public slots:
   void updateLayout();
   //void setKeyRange( int midiNoteStart, int midiNoteCount );
protected slots:
   void on_btnShowExplorerPanel( bool checked );

protected:
   void updateTopPanelLayout();
   void updateExplorerPanelLayout();
   void updateArrangementLayout();
   void updateDetailPanelLayout();

   void resizeEvent( QResizeEvent* event ) override;
   void paintEvent( QPaintEvent* event ) override;

   void enterEvent( QEnterEvent* event ) override;
   void leaveEvent( QEvent* event ) override;

//   void hideEvent( QHideEvent* event ) override;
//   void showEvent( QShowEvent* event ) override;
   void keyPressEvent( QKeyEvent* event ) override;
   void keyReleaseEvent( QKeyEvent* event ) override;
   void mousePressEvent( QMouseEvent* event ) override;
   void mouseReleaseEvent( QMouseEvent* event ) override;
   void mouseMoveEvent( QMouseEvent* event ) override;
   void wheelEvent( QWheelEvent* event ) override;

public:
   App & m_app;

//   //Body V
//   QRect m_rcHeader; // fix height
//   QRect m_rc1; // variable height row1 ( plugin explorer + arrangement )
//   QRect m_rcSplitterV; // fix height,splitter v rect between m_rc1 and m_rc2
//   QRect m_rc2; // variable height row1 ( quick help + clip editor )
//   QRect m_rcFooter; // fix height
//   QRect m_rcCover; // rcV + rc2 + rcFooter to draw backround

//   //TopPanel
//   QRect m_rcTopLeft;
//   QRect m_rcSplitterH;
//   QRect m_rcTopRight;

//   //DetailPanel
//   QRect m_rcQuickHelpPanel;
//   QRect m_rcQuickHelpContent;
//   QRect m_rcDetailPanel;
//   QRect m_rcDetailContent;

//   // ExplorerPanel
//   QRect m_rcDeviceBar; // Computed, buttons
//   QRect m_rcExplorerScrollBarPanel; // Computed
//   QRect m_rcExplorerScrollBar; // Computed
//   QRect m_rcExplorerPanel; // Computed
//   QRect m_rcExplorerContent;

//   // Arrangement
//   QRect m_rcArrangePanel; // Computed ( arrange + composebar = special panel )
//   QRect m_rcArrangePanelContent;
//   QRect m_rcComposeBar; // Computed, buttons
//   QRect m_rcArrangeScrollBarPanel; // Computed
//   QRect m_rcArrangeScrollBar; // Computed
//   QRect m_rcArrangeOverview;     // upper sub-rect of m_rcViewContent
//   QRect m_rcArrangeContent;  // lower sub-rect of m_rcViewContent

   int m_hoverButton;
   bool m_isDragging;
   int m_dragMode;
   int m_dragData;
   int m_dragStartX;
   int m_dragStartY;


//   struct Key
//   {
//      bool pressed;
//      int midiNote;
//      int oktave;
//      int semitone;
//      Key()
//         : pressed( false ), midiNote( 0 ), oktave( 0 ), semitone( 0 ) {}
//      Key( int iNote, int iOktave, int iSemi )
//         : pressed( false ), midiNote( iNote ), oktave( iOktave ), semitone( iSemi )    {}
//   };

//   int m_timerId;
//   int m_keyStart;
//   double m_timeLastNote;
//   double m_timeNote;

//   std::vector< Key > m_keys;
//   std::array< Key*, 10 > m_touched;
//   //std::array< de::audio::IDspChainElement*, 8 > m_synths;
//   std::array< QImage, 12 > m_images;

private:
   ImageButton* createShowArrangementButton();
   ImageButton* createShowSessionButton();
   ImageButton* createScrollUpButton();
   ImageButton* createScrollDownButton();
   ImageButton* createComposeIOButton();
   ImageButton* createComposeRButton();
   ImageButton* createComposeMButton();
   ImageButton* createComposeDButton();

private:
   ImageButton* createShowExplorerButton();
   ImageButton* createShowInternDevicesButton();
   ImageButton* createShowExternDevicesButton();
   ImageButton* createShowExplorer1Button();
   ImageButton* createShowExplorer2Button();
   ImageButton* createShowExplorer3Button();
   ImageButton* createShowGroovesButton();
};
