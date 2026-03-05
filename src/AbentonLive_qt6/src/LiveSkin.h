/// (c) 2017 - 20180 Benjamin Hampe <benjaminhampe@gmx.de>

#pragma once
#include "LiveCommon.h"

struct LiveSkin
{
   enum eInt
   {
      Padding = 0, Spacing, Radius, EngineButtonH,
      CircleButtonSize, ScrollButtonSize, SmallCircleButtonSize,
      Margin, TitleH, SearchBarH, TdH, eIntCount
   };

   enum eColor
   {
      WindowColor, PanelColor, SymbolColor, FocusColor, ActiveColor,
      TitleColor, TitleTextColor, ContentColor, EditBoxColor, TdColor,
      // BlackEditColor, WhiteEditColor, PenColor, FillColor, SemiBeatColor, TextColor,
      HeaderBtnPenColor, HeaderBtnFillColor, HeaderBtnTextColor, eColorCount
   };

   // WindowColors
   QColor windowColor = QColor(0,0,0,0);  // transparent
   QColor symbolColor = QColor(24,30,35);  // dark blue
   QColor panelColor = QColor(103,116,140);  // semi dark blue
   QColor focusColor = QColor(127,137,147); // darker gray
   QColor activeColor = QColor( 238, 246, 13 ); // yellow highlight
   QColor titleColor = QColor(217,221,226);   // light white blue
   QColor titleTextColor = windowColor;  // same as Window
   QColor contentColor = QColor(170,178,183); // greyish
   QColor editBoxColor = QColor(232,232,232);  // almost white
   QColor tdColor = QColor(140,153,175);  // table header
   // HeaderButtons
   QColor headerBtnPenColor = QColor( 166, 181, 186 ); // Engine Button
   QColor headerBtnFillColor = QColor( 146, 157, 168 );
   QColor headerBtnTextColor = QColor( 29, 31, 33 );
   // ClipEditor
   QColor blackEditColor = QColor(217,221,226);
   QColor whiteEditColor = QColor(196,200,204);
   // QColor semiBeatColor = QColor(146,156,167);  // light grey between main beat grid
   // QColor playColor = QColor(79,254,29);
   // QColor recColor = QColor(254,49,34);
   QColor panelBlendColor = blendRGB( windowColor, panelColor, 0.25f );

   int padding = 8;
   int spacing = 5;
   int radius = 8;
   int engineButtonH = 17;
   int circleButtonSize = 23;
   int scrollButtonSize = 9;
   int smallCircleButtonSize = 11;
   int margin = 4;
   int titleH = 12;
   int searchBarH = 24;
   int tdH = 12;
   float zoomUI = 1.0f;

   //bool m_isExplorerVisible;
   //bool m_isQuickHelpVisible;
   bool isBigHelpVisible = false;
   bool isExplorerVisible = true;    // Left of top panel
   bool isQuickHelpVisible = true; // Left of bottom panel
   bool isDetailVisible = true; // Right of bottom panel
   bool isClipEdVisible = true; // Right of bottom panel
   int deviceBarWidth = 23; // default init width
   int explorerWidth = 200; // default init width
   int spurEditorHeight = 190 + 12; // default height
   int clipEditorHeight = 300; // default height by splitter
   int quickHelpWidth = 190 + 12; // 220, default width -> no splitter here to control it so far.
   int hsplitterPos = 200; // default splitter h pos ( between LeftExplorer and Arrangement )
   int vsplitterPos = spurEditorHeight; // default splitter v pos ( between BottomClipEditor and TopExplorerArrangement )


   //Body V
   QRect rcHeader; // fix height
   QRect rc1; // variable height row1 ( plugin explorer + arrangement )
   QRect rcSplitterV; // fix height,splitter v rect between m_rc1 and m_rc2
   QRect rc2; // variable height row1 ( quick help + clip editor )
   QRect rcFooter; // fix height
   QRect rcCover; // rcV + rc2 + rcFooter to draw backround

   //TopPanel
   QRect rcTopLeft;
   QRect rcSplitterH;
   QRect rcTopRight;

   //DetailPanel
   QRect rcQuickHelpPanel;
   QRect rcQuickHelpContent;
   QRect rcDetailPanel;
   QRect rcDetailContent;

   // ExplorerPanel
   QRect rcDeviceBar; // Computed, buttons
   QRect rcExplorerScrollBarPanel; // Computed
   QRect rcExplorerScrollBar; // Computed
   QRect rcExplorerPanel; // Computed
   QRect rcExplorerContent;

   // Arrangement
   QRect rcArrangePanel; // Computed ( arrange + composebar = special panel )
   QRect rcArrangePanelContent;
   QRect rcComposeBar; // Computed, buttons
   QRect rcArrangeScrollBarPanel; // Computed
   QRect rcArrangeScrollBar; // Computed
   QRect rcArrangeOverview;     // upper sub-rect of m_rcViewContent
   QRect rcArrangeContent;  // lower sub-rect of m_rcViewContent

   void update();

   QColor getColor( eColor color );
   int getInt( eInt param ) const;



};
