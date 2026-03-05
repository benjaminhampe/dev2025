#include "Header.h"
#include "App.h"
#include "Draw.h"
#include <de_fontawesome.hpp>

// Header = EngineButtonBar

Header::Header( App & app, QWidget* parent )
   : QWidget(parent)
   , m_app( app )
{
   setObjectName( "TopPanel" );
   setContentsMargins( 8,8,8,8 );
   setMouseTracking( true );

   m_TapButton = createTapButton();
   m_BpmEdit = createBpmEdit();
   m_NudgeSlowButton = createNudgeSlowButton();
   m_NudgeFastButton = createNudgeFastButton();
   m_BeatSignatureEdit = createBeatSignatureEdit();
   m_MetronomToggle = createMetronomToggle();

   m_DisplayFollowToggle = createDisplayFollowToggle();
   m_ArrangmentPosEdit = createArrangmentPosEdit();
   m_PlayButton = createPlayButton();
   m_StopButton = createStopButton();
   m_RecordButton = createRecordButton();
   m_OverdubButton = createOverdubButton();
   m_BackToArrangementButton = createBackToArrangementButton();
   m_BarComboBox = createBarComboBox();
   m_PenButton = createPenButton();

   m_LoopStartEdit = createLoopStartEdit();
   m_PunchInButton = createPunchInButton();
   m_LoopButton = createLoopButton();
   m_PunchOutButton = createPunchOutButton();
   m_LoopLengthEdit = createLoopLengthEdit();

   m_KeyMidiButton = createKeyMidiButton();
   m_KeyMappingButton = createKeyMappingButton();
   m_KeyMidiInButton = createKeyMidiInButton();
   m_KeyMidiOutButton = createKeyMidiOutButton();
   m_MidiMappingButton = createMidiMappingButton();
   m_CpuUsageEdit = createCpuUsageEdit();
   m_DiskUsageEdit = createDiskUsageEdit();
   m_LiveMidiInLED = createLiveMidiInLED();
   m_LiveMidiOutLED = createLiveMidiOutLED();

   auto & g1 = m_btnGroup1;
   g1.emplace_back( m_TapButton ); g1.emplace_back( m_BpmEdit );
   g1.emplace_back( m_NudgeSlowButton ); g1.emplace_back( m_NudgeFastButton );
   g1.emplace_back( m_BeatSignatureEdit ); g1.emplace_back( m_MetronomToggle );

   auto & g2 = m_btnGroup2;
   g2.emplace_back( m_DisplayFollowToggle ); g2.emplace_back( m_ArrangmentPosEdit );
   g2.emplace_back( m_PlayButton ); g2.emplace_back( m_StopButton );
   g2.emplace_back( m_RecordButton ); g2.emplace_back( m_OverdubButton );
   g2.emplace_back( m_BackToArrangementButton ); g2.emplace_back( m_BarComboBox );
   g2.emplace_back( m_PenButton );

   auto & g3 = m_btnGroup3;
   g3.emplace_back( m_LoopStartEdit ); g3.emplace_back( m_PunchInButton );
   g3.emplace_back( m_LoopButton ); g3.emplace_back( m_PunchOutButton );
   g3.emplace_back( m_LoopLengthEdit );

   auto & g4 = m_btnGroup4;
   g4.emplace_back( m_KeyMidiButton ); g4.emplace_back( m_KeyMappingButton );
   g4.emplace_back( m_KeyMidiInButton ); g4.emplace_back( m_KeyMidiOutButton );
   g4.emplace_back( m_MidiMappingButton ); g4.emplace_back( m_CpuUsageEdit );
   g4.emplace_back( m_DiskUsageEdit );
   g4.emplace_back( m_LiveMidiInLED ); g4.emplace_back( m_LiveMidiOutLED );

   connect( m_PlayButton, SIGNAL(clicked(bool)), this, SLOT(on_playButton(bool)) );
   connect( m_StopButton, SIGNAL(clicked(bool)), this, SLOT(on_stopButton(bool)) );
   connect( m_RecordButton, SIGNAL(clicked(bool)), this, SLOT(on_recordButton(bool)) );
}

void
Header::on_playButton( bool checked )
{
   m_app.playAudioMaster();

//   if ( !checked )
//   {
//      m_PlayButton->blockSignals( true );
//      m_PlayButton->setChecked( true );
//      m_PlayButton->blockSignals( false );
//   }
}

void
Header::on_stopButton( bool checked )
{
   m_app.stopAudioMaster();

//   if ( m_PlayButton->isChecked() )
//   {
//      m_PlayButton->blockSignals( true );
//      m_PlayButton->setChecked( false );
//      m_PlayButton->blockSignals( false );
//   }
}

void
Header::on_recordButton( bool checked )
{

}


void
Header::updateLayout()
{
   int x = contentsMargins().top();
   int y = contentsMargins().left();
   int d = contentsMargins().bottom();
   int e = contentsMargins().right();
   updateLayout( QRect( x,y,width()-(x+d),height()-(y+e) ) );
   update();
}


void
Header::updateLayout( QRect const & clipRect )
{
   int clipW = clipRect.width();
   int totalW = getTotalWidth();

   int spacing = 0;
   int spacingR = 0;
   if ( clipW > totalW )
   {
      spacing = (clipW - totalW) / 3;
      spacingR = (clipW - totalW) - 2*spacing;
   }

   int x = clipRect.x();
   int y = clipRect.y();

   int xEnd = clipRect.x() + clipRect.width() - 1;

   for ( auto & p : m_btnGroup1 ) { p->setVisible( false ); }
   for ( auto & p : m_btnGroup2 ) { p->setVisible( false ); }
   for ( auto & p : m_btnGroup3 ) { p->setVisible( false ); }
   for ( auto & p : m_btnGroup4 ) { p->setVisible( false ); }

   bool ok = true;

   for ( auto & p : m_btnGroup1 )
   {
      if ( ok )
      {
         p->setRect( QRect( x,y,p->w(),p->h() ) );
         p->setVisible( true );
      }

      if ( x + p->w() >= xEnd )
      {
         ok = false;
         break;
      }

      x += p->w() + m_spacing;
   }

   x += spacing;


   for ( auto & p : m_btnGroup2 )
   {
      if ( ok )
      {
         p->setRect( QRect( x,y,p->w(),p->h() ) );
         p->setVisible( true );
      }

      if ( x + p->w() >= xEnd )
      {
         ok = false;
         break;
      }

      x += p->w() + m_spacing;
   }

   x += spacing;

   for ( auto & p : m_btnGroup3 )
   {
      if ( ok )
      {
         p->setRect( QRect( x,y,p->w(),p->h() ) );
         p->setVisible( true );
      }

      if ( x + p->w() >= xEnd )
      {
         ok = false;
         break;
      }

      x += p->w() + m_spacing;
   }

   x += spacingR;

   for ( auto & p : m_btnGroup4 )
   {
      if ( ok )
      {
         p->setRect( QRect( x,y,p->w(),p->h() ) );
         p->setVisible( true );
      }

      if ( x + p->w() >= xEnd )
      {
         ok = false;
         break;
      }

      x += p->w() + m_spacing;
   }

}

void
Header::resizeEvent( QResizeEvent* event )
{
   QWidget::resizeEvent( event );
   updateLayout();

}

void
Header::paintEvent( QPaintEvent* event )
{
//   int w = width();
//   int h = height();

//   QPainter dc( this );
//   dc.setRenderHint( QPainter::NonCosmeticDefaultPen );
//   dc.fillRect( rect(), QColor(255,200,100) );

   QWidget::paintEvent( event );
}

inline QFont getHeaderFont()
{
   QFont font("FontAwesome", 9, QFont::Bold, false );
   font.setHintingPreference( QFont::PreferFullHinting );
   font.setKerning( true );
   font.setStyleStrategy( QFont::PreferAntialias );
   return font;
}

ImageButton*
Header::createTapButton()
{
   LiveSkin const & skin = m_app.m_skin;
   auto btn = new ImageButton( this );

   int bw = 32;
   int bh = 17;

   QFont font = getHeaderFont();
   QString msg = "TAP";
   // [idle]
   QImage ico = createImageFromText( 0,0, msg, font, skin.headerBtnTextColor, skin.headerBtnFillColor );
   QImage img = createRectImage( bw,bh, skin.headerBtnPenColor, skin.headerBtnFillColor, ico );
   btn->setImage( 0, img );
   // [idle_hover]
   btn->setImage( 1, img );

   // [active]
   ico = createImageFromText( 1,1, msg, font, skin.headerBtnTextColor, skin.activeColor );
   img = createRectImage( bw,bh, skin.headerBtnPenColor, skin.activeColor, ico );
   btn->setImage( 2, img );
   // [active_hover]
   btn->setImage( 3, img );

   return btn;
}

ImageButton*
Header::createBpmEdit()
{
   LiveSkin const & skin = m_app.m_skin;
   auto & btn = m_BpmEdit;
   btn = new ImageButton( this );

   int bw = 48;
   int bh = 17;

   QFont font = getHeaderFont();
   QString msg = "120.00";
   // [idle]
   QImage ico = createImageFromText( 1,1, msg, font, skin.headerBtnTextColor, skin.headerBtnFillColor );
   QImage img = createRectImage( bw,bh, skin.headerBtnPenColor, skin.headerBtnFillColor, ico );
   btn->setImage( 0, img );
   // [idle_hover]
   btn->setImage( 1, img );

   // [active]
   ico = createImageFromText( 1,1, msg, font, skin.headerBtnTextColor, skin.activeColor );
   img = createRectImage( bw,bh, skin.headerBtnPenColor, skin.activeColor, ico );
   btn->setImage( 2, img );
   // [active_hover]
   btn->setImage( 3, img );
   return btn;
}

ImageButton*
Header::createNudgeSlowButton()
{
   LiveSkin const & skin = m_app.m_skin;
   auto & btn = m_NudgeSlowButton;
   btn = new ImageButton( this );

   int bw = 16;
   int bh = 17;

   btn->setCheckable( true );
   btn->setChecked( false );

   std::string
   msg = "## #  #   #\n"
         "## #  #   #\n"
         "## #  #   #\n"
         "## #  #   #\n"
         "## #  #   #\n"
         "## #  #   #\n"
         "## #  #   #\n";

   // [idle]
   QImage ico = createAsciiArt( skin.headerBtnTextColor, skin.headerBtnFillColor, msg );
   QImage img = createRectImage( bw,bh, skin.headerBtnPenColor, skin.headerBtnFillColor, ico );
   btn->setImage( 0, img );
   // [idle_hover]
   btn->setImage( 1, img );

   // [active]
   ico = createAsciiArt( skin.headerBtnTextColor, skin.activeColor, msg );
   img = createRectImage( bw,bh, skin.headerBtnPenColor, skin.activeColor, ico );
   btn->setImage( 2, img );
   // [active_hover]
   btn->setImage( 3, img );
   return btn;
}

ImageButton*
Header::createNudgeFastButton()
{
   LiveSkin const & skin = m_app.m_skin;
   auto & btn = m_NudgeFastButton;
   btn = new ImageButton( this );

   int bw = 16;
   int bh = 17;

   btn->setCheckable( true );
   btn->setChecked( false );

   std::string
   msg = "#   #  # ##\n"
         "#   #  # ##\n"
         "#   #  # ##\n"
         "#   #  # ##\n"
         "#   #  # ##\n"
         "#   #  # ##\n"
         "#   #  # ##\n";

   // [idle]
   QImage ico = createAsciiArt( skin.headerBtnTextColor, skin.headerBtnFillColor, msg );
   QImage img = createRectImage( bw,bh, skin.headerBtnPenColor, skin.headerBtnFillColor, ico );
   btn->setImage( 0, img );
   // [idle_hover]
   btn->setImage( 1, img );

   // [active]
   ico = createAsciiArt( skin.headerBtnTextColor, skin.activeColor, msg );
   img = createRectImage( bw,bh, skin.headerBtnPenColor, skin.activeColor, ico );
   btn->setImage( 2, img );
   // [active_hover]
   btn->setImage( 3, img );
   return btn;
}

ImageButton*
Header::createBeatSignatureEdit()
{
   LiveSkin const & skin = m_app.m_skin;
   auto & btn = m_BeatSignatureEdit;
   btn = new ImageButton( this );

   int bw = 50;
   int bh = 17;

   QFont font = getHeaderFont();
   QString msg = "  4  /  4  ";
   // [idle]
   QImage ico = createImageFromText( 1,1, msg, font, skin.headerBtnTextColor, skin.headerBtnFillColor );
   QImage img = createRectImage( bw,bh, skin.headerBtnPenColor, skin.headerBtnFillColor, ico );
   btn->setImage( 0, img );
   // [idle_hover]
   btn->setImage( 1, img );

   // [active]
   ico = createImageFromText( 1,1, msg, font, skin.headerBtnTextColor, skin.activeColor );
   img = createRectImage( bw,bh, skin.headerBtnPenColor, skin.activeColor, ico );
   btn->setImage( 2, img );
   // [active_hover]
   btn->setImage( 3, img );
   return btn;
}

ImageButton*
Header::createMetronomToggle()
{
   LiveSkin const & skin = m_app.m_skin;
   auto & btn = m_MetronomToggle;
   btn = new ImageButton( this );

   int bw = 24;
   int bh = 17;

   btn->setCheckable( true );
   btn->setChecked( false );

   std::string msg = "  ###      ###\n"
                  " #   #    #####\n"
                  "#     #  #######\n"
                  "#     #  #######\n"
                  "#     #  #######\n"
                  " #   #    #####\n"
                  "  ###      ###\n";
   // [idle]
   QImage ico = createAsciiArt( skin.headerBtnTextColor, skin.headerBtnFillColor, msg );
   QImage img = createRectImage( bw,bh, skin.headerBtnPenColor, skin.headerBtnFillColor, ico );
   btn->setImage( 0, img );
   // [idle_hover]
   btn->setImage( 1, img );

   // [active]
   ico = createAsciiArt( skin.headerBtnTextColor, skin.activeColor, msg );
   img = createRectImage( bw,bh, skin.headerBtnPenColor, skin.activeColor, ico );
   btn->setImage( 2, img );
   // [active_hover]
   btn->setImage( 3, img );
   return btn;
}

ImageButton*
Header::createDisplayFollowToggle()
{
   LiveSkin const & skin = m_app.m_skin;
   auto & btn = m_DisplayFollowToggle;
   btn = new ImageButton( this );

   int bw = 24;
   int bh = 17;

   btn->setCheckable( true );
   btn->setChecked( false );

   //QFont font = getHeaderFont();
   std::string
   msg = "        #\n"
         "     #  ##\n"
         " #  ### ###\n"
         "     #  ##\n"
         "        #\n";

   // [idle]
   QImage ico = createAsciiArt( skin.headerBtnTextColor, skin.headerBtnFillColor, msg );
   QImage img = createRectImage( bw,bh, skin.headerBtnPenColor, skin.headerBtnFillColor, ico );
   btn->setImage( 0, img );
   // [idle_hover]
   btn->setImage( 1, img );

   // [active]
   ico = createAsciiArt( skin.headerBtnTextColor, skin.activeColor, msg );
   img = createRectImage( bw,bh, skin.headerBtnPenColor, skin.activeColor, ico );
   btn->setImage( 2, img );
   // [active_hover]
   btn->setImage( 3, img );
   return btn;
}

ImageButton*
Header::createArrangmentPosEdit()
{
   LiveSkin const & skin = m_app.m_skin;
   auto & btn = m_ArrangmentPosEdit;
   btn = new ImageButton( this );

   int bw = 86;
   int bh = 17;

   QFont font = getHeaderFont();
   QString msg = "    1 .   1 .   1";
   // [idle]
   QImage ico = createImageFromText( 1,1, msg, font, skin.headerBtnTextColor, skin.headerBtnFillColor );
   QImage img = createRectImage( bw,bh, skin.headerBtnPenColor, skin.headerBtnFillColor, ico );
   btn->setImage( 0, img );
   // [idle_hover]
   btn->setImage( 1, img );

   // [active]
   ico = createImageFromText( 1,1, msg, font, skin.headerBtnTextColor, skin.activeColor );
   img = createRectImage( bw,bh, skin.headerBtnPenColor, skin.activeColor, ico );
   btn->setImage( 2, img );
   // [active_hover]
   btn->setImage( 3, img );
   return btn;
}

ImageButton*
Header::createPlayButton()
{
   LiveSkin const & skin = m_app.m_skin;
   auto & btn = m_PlayButton;
   btn = new ImageButton( this );

   int bw = 17;
   int bh = 17;

   btn->setCheckable( true );
   btn->setChecked( false );

   QFont font = getHeaderFont();
   QString msg = QChar(static_cast<ushort>(fa::play));

   // [idle]
   QImage ico = createImageFromText( 1,1, msg, font, skin.headerBtnTextColor, skin.headerBtnFillColor );
   QImage img = createRectImage( bw,bh, skin.headerBtnPenColor, skin.headerBtnFillColor, ico, 0, 1 );
   btn->setImage( 0, img );
   // [idle_hover]
   btn->setImage( 1, img );

   // [active]
   ico = createImageFromText( 1,1, msg, font, QColor(79,254,29), skin.headerBtnFillColor );
   img = createRectImage( bw,bh, skin.headerBtnPenColor, skin.headerBtnFillColor, ico, 0, 1 );
   btn->setImage( 2, img );
   // [active_hover]
   btn->setImage( 3, img );
   return btn;
}

ImageButton*
Header::createStopButton()
{
   LiveSkin const & skin = m_app.m_skin;
   auto & btn = m_StopButton;
   btn = new ImageButton( this );

   int bw = 17;
   int bh = 17;

   btn->setCheckable( false );
   btn->setChecked( false );

   QFont font = getHeaderFont();
   QString msg = QChar(static_cast<ushort>(fa::stop));

   // [idle]
   QImage ico = createImageFromText( 1,1, msg, font, skin.headerBtnTextColor, skin.headerBtnFillColor );
   QImage img = createRectImage( bw,bh, skin.headerBtnPenColor, skin.headerBtnFillColor, ico, 0, 1 );
   btn->setImage( 0, img );
   // [idle_hover]
   btn->setImage( 1, img );

   // [active]
   ico = createImageFromText( 1,1, msg, font, QColor(249,210,14), skin.headerBtnFillColor );
   img = createRectImage( bw,bh, skin.headerBtnPenColor, skin.headerBtnFillColor, ico, 0, 1 );
   btn->setImage( 2, img );
   // [active_hover]
   btn->setImage( 3, img );
   return btn;
}


ImageButton*
Header::createRecordButton()
{
   LiveSkin const & skin = m_app.m_skin;
   auto & btn = m_RecordButton;
   btn = new ImageButton( this );

   int bw = 17;
   int bh = 17;

   btn->setCheckable( true );
   btn->setChecked( false );

   QFont font = getHeaderFont();
   QString msg = QChar(static_cast<ushort>(fa::circle));

   // [idle]
   QImage ico = createImageFromText( 1,1, msg, font, skin.headerBtnTextColor, skin.headerBtnFillColor );
   QImage img = createRectImage( bw,bh, skin.headerBtnPenColor, skin.headerBtnFillColor, ico, 0, 1 );
   btn->setImage( 0, img );
   // [idle_hover]
   btn->setImage( 1, img );

   // [active]
   ico = createImageFromText( 1,1, msg, font, QColor(254,49,34), skin.headerBtnFillColor );
   img = createRectImage( bw,bh, skin.headerBtnPenColor, skin.headerBtnFillColor, ico, 0, 1 );
   btn->setImage( 2, img );
   // [active_hover]
   btn->setImage( 3, img );
   return btn;
}




ImageButton*
Header::createOverdubButton()
{
   LiveSkin const & skin = m_app.m_skin;
   auto & btn = m_OverdubButton;
   btn = new ImageButton( this );

   int bw = 32;
   int bh = 17;
   btn->setCheckable( true );
   btn->setChecked( false );

   QFont font = getHeaderFont();
   QString msg = "OVR";

   // [idle]
   QImage ico = createImageFromText( 1,1, msg, font, skin.headerBtnTextColor, skin.headerBtnFillColor );
   QImage img = createRectImage( bw,bh, skin.headerBtnPenColor, skin.headerBtnFillColor, ico );
   btn->setImage( 0, img );
   // [idle_hover]
   btn->setImage( 1, img );

   // [active]
   ico = createImageFromText( 1,1, msg, font, skin.headerBtnTextColor, skin.activeColor );
   img = createRectImage( bw,bh, skin.headerBtnPenColor, skin.activeColor, ico );
   btn->setImage( 2, img );
   // [active_hover]
   btn->setImage( 3, img );
   return btn;
}

ImageButton*
Header::createBackToArrangementButton()
{
   LiveSkin const & skin = m_app.m_skin;
   auto & btn = m_BackToArrangementButton;
   btn = new ImageButton( this );

   int bw = 24;
   int bh = 17;
//   btn->setCheckable( true );
//   btn->setChecked( false );

   std::string
   msg = "#########\n"
         "             #\n"
         "            ##\n"
         "#########  #######\n"
         "            ##\n"
         "             #\n"
         "#########\n";
   // [idle]
   QImage ico = createAsciiArt( skin.headerBtnTextColor, skin.headerBtnFillColor, msg );
   QImage img = createRectImage( bw,bh, skin.headerBtnPenColor, skin.headerBtnFillColor, ico );
   btn->setImage( 0, img );
   // [idle_hover]
   btn->setImage( 1, img );

   // [active]
   ico = createAsciiArt( skin.headerBtnTextColor, skin.activeColor, msg );
   img = createRectImage( bw,bh, skin.headerBtnPenColor, skin.activeColor, ico );
   btn->setImage( 2, img );
   // [active_hover]
   btn->setImage( 3, img );
   return btn;
}

ImageButton*
Header::createBarComboBox()
{
   LiveSkin const & skin = m_app.m_skin;
   auto & btn = m_BarComboBox;
   btn = new ImageButton( this );

   int bw = 48;
   int bh = 17;

//   btn->setCheckable( true );
//   btn->setChecked( false );

   QFont font = getHeaderFont();
   QString msg = "1 Bar v";

   // [idle]
   QImage ico = createImageFromText( 1,1, msg, font, skin.headerBtnTextColor, skin.headerBtnFillColor );
   QImage img = createRectImage( bw,bh, skin.headerBtnPenColor, skin.headerBtnFillColor, ico );
   btn->setImage( 0, img );
   // [idle_hover]
   btn->setImage( 1, img );

   // [active]
   ico = createImageFromText( 1,1, msg, font, skin.headerBtnTextColor, skin.activeColor );
   img = createRectImage( bw,bh, skin.headerBtnPenColor, skin.activeColor, ico );
   btn->setImage( 2, img );
   // [active_hover]
   btn->setImage( 3, img );
   return btn;
}

ImageButton*
Header::createPenButton()
{
   LiveSkin const & skin = m_app.m_skin;
   auto & btn = m_PenButton;
   btn = new ImageButton( this );

   int bw = 24;
   int bh = 17;

   btn->setCheckable( true );
   btn->setChecked( false );

   //QFont font = getHeaderFont();
   std::string msg = "      #\n"
                  "     ###\n"
                  "    ## ##\n"
                  "   ## ####\n"
                  "  ## ####\n"
                  " ## ####\n"
                  "## ####\n"
                  "# ####\n"
                  "#  ##\n"
                  "####\n";
   // [idle]
   QImage ico = createAsciiArt( skin.headerBtnTextColor, skin.headerBtnFillColor, msg );
   QImage img = createRectImage( bw,bh, skin.headerBtnPenColor, skin.headerBtnFillColor, ico );
   btn->setImage( 0, img );
   // [idle_hover]
   btn->setImage( 1, img );

   // [active]
   ico = createAsciiArt( skin.headerBtnTextColor, skin.activeColor, msg );
   img = createRectImage( bw,bh, skin.headerBtnPenColor, skin.activeColor, ico );
   btn->setImage( 2, img );
   // [active_hover]
   btn->setImage( 3, img );
   return btn;
}


ImageButton*
Header::createLoopStartEdit()
{
   LiveSkin const & skin = m_app.m_skin;
   auto & btn = m_LoopStartEdit;
   btn = new ImageButton( this );

   int bw = 86;
   int bh = 17;

   QFont font = getHeaderFont();
   QString msg = "    1 .   1 .   1";
   // [idle]
   QImage ico = createImageFromText( 1,1, msg, font, skin.headerBtnTextColor, skin.headerBtnFillColor );
   QImage img = createRectImage( bw,bh, skin.headerBtnPenColor, skin.headerBtnFillColor, ico );
   btn->setImage( 0, img );
   // [idle_hover]
   btn->setImage( 1, img );

   // [active]
   ico = createImageFromText( 1,1, msg, font, skin.headerBtnTextColor, skin.activeColor );
   img = createRectImage( bw,bh, skin.headerBtnPenColor, skin.activeColor, ico );
   btn->setImage( 2, img );
   // [active_hover]
   btn->setImage( 3, img );
   return btn;
}


ImageButton*
Header::createLoopLengthEdit()
{
   LiveSkin const & skin = m_app.m_skin;
   auto & btn = m_LoopLengthEdit;
   btn = new ImageButton( this );

   int bw = 86;
   int bh = 17;

   QFont font = getHeaderFont();
   QString msg = "    4 .   0 .   0";
   // [idle]
   QImage ico = createImageFromText( 1,1, msg, font, skin.headerBtnTextColor, skin.headerBtnFillColor );
   QImage img = createRectImage( bw,bh, skin.headerBtnPenColor, skin.headerBtnFillColor, ico );
   btn->setImage( 0, img );
   // [idle_hover]
   btn->setImage( 1, img );

   // [active]
   ico = createImageFromText( 1,1, msg, font, skin.headerBtnTextColor, skin.activeColor );
   img = createRectImage( bw,bh, skin.headerBtnPenColor, skin.activeColor, ico );
   btn->setImage( 2, img );
   // [active_hover]
   btn->setImage( 3, img );
   return btn;
}

ImageButton*
Header::createPunchInButton()
{
   LiveSkin const & skin = m_app.m_skin;
   auto & btn = m_PunchInButton;
   btn = new ImageButton( this );

   int bw = 24;
   int bh = 17;
//   btn->setCheckable( true );
//   btn->setChecked( false );

   //QFont font = getHeaderFont();
   std::string msg = "######\n"
                     "      #\n"
                     "       #\n"
                     "        #\n"
                     "         ######\n";
   // [idle]
   QImage ico = createAsciiArt( skin.headerBtnTextColor, skin.headerBtnFillColor, msg );
   QImage img = createRectImage( bw,bh, skin.headerBtnPenColor, skin.headerBtnFillColor, ico );
   btn->setImage( 0, img );
   // [idle_hover]
   btn->setImage( 1, img );

   // [active]
   ico = createAsciiArt( skin.headerBtnTextColor, skin.activeColor, msg );
   img = createRectImage( bw,bh, skin.headerBtnPenColor, skin.activeColor, ico );
   btn->setImage( 2, img );
   // [active_hover]
   btn->setImage( 3, img );
   return btn;
}

ImageButton*
Header::createLoopButton()
{
   LiveSkin const & skin = m_app.m_skin;
   auto & btn = m_LoopButton;
   btn = new ImageButton( this );

   int bw = 24;
   int bh = 17;

   btn->setCheckable( true );
   btn->setChecked( false );

   std::string msg = " ###########\n"
                     "#           #\n"
                     "#           #\n"
                     "#           #\n"
                     "#           #\n"
                     "#       #   #\n"
                     "#      ##   #\n"
                     " ###  ######\n"
                     "       ##\n"
                     "        #\n";
   // [idle]
   QImage ico = createAsciiArt( skin.headerBtnTextColor, skin.headerBtnFillColor, msg );
   QImage img = createRectImage( bw,bh, skin.headerBtnPenColor, skin.headerBtnFillColor, ico );
   btn->setImage( 0, img );
   // [idle_hover]
   btn->setImage( 1, img );

   // [active]
   ico = createAsciiArt( skin.headerBtnTextColor, skin.activeColor, msg );
   img = createRectImage( bw,bh, skin.headerBtnPenColor, skin.activeColor, ico );
   btn->setImage( 2, img );
   // [active_hover]
   btn->setImage( 3, img );
   return btn;
}

ImageButton*
Header::createPunchOutButton()
{
   LiveSkin const & skin = m_app.m_skin;
   auto & btn = m_PunchOutButton;
   btn = new ImageButton( this );

   int bw = 24;
   int bh = 17;

   btn->setCheckable( true );
   btn->setChecked( false );

   std::string msg = "         ######\n"
                     "        #\n"
                     "       #\n"
                     "      #\n"
                     "######\n";
   // [idle]
   QImage ico = createAsciiArt( skin.headerBtnTextColor, skin.headerBtnFillColor, msg );
   QImage img = createRectImage( bw,bh, skin.headerBtnPenColor, skin.headerBtnFillColor, ico );
   btn->setImage( 0, img );
   // [idle_hover]
   btn->setImage( 1, img );

   // [active]
   ico = createAsciiArt( skin.headerBtnTextColor, skin.activeColor, msg );
   img = createRectImage( bw,bh, skin.headerBtnPenColor, skin.activeColor, ico );
   btn->setImage( 2, img );
   // [active_hover]
   btn->setImage( 3, img );
   return btn;
}

ImageButton*
Header::createKeyMidiButton()
{
   LiveSkin const & skin = m_app.m_skin;
   auto & btn = m_KeyMidiButton;
   btn = new ImageButton( this );

   int bw = 16;
   int bh = 17;
   btn->setCheckable( true );
   btn->setChecked( false );

   std::string msg = "ooooooooooo\n"
                     "o o#o o#o o\n"
                     "o o#o o#o o\n"
                     "o o#o o#o o\n"
                     "o o#o o#o o\n"
                     "o o#o o#o o\n"
                     //"o o#o o#o o\n"
                     "o  o   o  o\n"
                     "o  o   o  o\n"
                     "o  o   o  o\n"
                     "o  o   o  o\n"
                     "ooooooooooo\n";
   // [idle]
   QImage ico = createAsciiArt( skin.headerBtnTextColor, skin.headerBtnFillColor, msg );
   QImage img = createRectImage( bw,bh, skin.headerBtnPenColor, skin.headerBtnFillColor, ico );
   btn->setImage( 0, img );
   // [idle_hover]
   btn->setImage( 1, img );

   // [active]
   ico = createAsciiArt( skin.headerBtnTextColor, skin.activeColor, msg );
   img = createRectImage( bw,bh, skin.headerBtnPenColor, skin.activeColor, ico );
   btn->setImage( 2, img );
   // [active_hover]
   btn->setImage( 3, img );
   return btn;
}

ImageButton*
Header::createKeyMidiInButton()
{
   LiveSkin const & skin = m_app.m_skin;
   auto & btn = m_KeyMidiInButton;
   btn = new ImageButton( this );

   int bw = 8;
   int bh = 8;
   btn->setCheckable( true );
   btn->setChecked( false );

   // [idle]
   QImage img = createRectImage( bw,bh, skin.headerBtnPenColor, skin.headerBtnFillColor );
   btn->setImage( 0, img );
   // [idle_hover]
   btn->setImage( 1, img );

   // [active]
   img = createRectImage( bw,bh, skin.headerBtnPenColor, skin.activeColor );
   btn->setImage( 2, img );
   // [active_hover]
   btn->setImage( 3, img );

   return btn;
}

ImageButton*
Header::createKeyMidiOutButton()
{
   LiveSkin const & skin = m_app.m_skin;
   auto & btn = m_KeyMidiOutButton;
   btn = new ImageButton( this );

   int bw = 8;
   int bh = 8;

   btn->setCheckable( true );
   btn->setChecked( false );

   // [idle]
   QImage img = createRectImage( bw,bh, skin.headerBtnPenColor, skin.headerBtnFillColor );
   btn->setImage( 0, img );
   // [idle_hover]
   btn->setImage( 1, img );

   // [active]
   img = createRectImage( bw,bh, skin.headerBtnPenColor, skin.activeColor );
   btn->setImage( 2, img );
   // [active_hover]
   btn->setImage( 3, img );

   return btn;
}


ImageButton*
Header::createKeyMappingButton()
{
   LiveSkin const & skin = m_app.m_skin;
   auto & btn = m_KeyMappingButton;
   btn = new ImageButton( this );

   int bw = 32;
   int bh = 17;

   QFont font = getHeaderFont();
   QString msg = "KEY";
   // [idle]
   QImage ico = createImageFromText( 0,0, msg, font, skin.headerBtnTextColor, skin.headerBtnFillColor );
   QImage img = createRectImage( bw,bh, skin.headerBtnPenColor, skin.headerBtnFillColor, ico );
   btn->setImage( 0, img );
   // [idle_hover]
   btn->setImage( 1, img );

   // [active]
   ico = createImageFromText( 1,1, msg, font, skin.headerBtnTextColor, skin.activeColor );
   img = createRectImage( bw,bh, skin.headerBtnPenColor, skin.activeColor, ico );
   btn->setImage( 2, img );
   // [active_hover]
   btn->setImage( 3, img );

   return btn;
}

ImageButton*
Header::createMidiMappingButton()
{
   LiveSkin const & skin = m_app.m_skin;
   auto & btn = m_MidiMappingButton;
   btn = new ImageButton( this );

   int bw = 32;
   int bh = 17;

   QFont font = getHeaderFont();
   QString msg = "MIDI";
   // [idle]
   QImage ico = createImageFromText( 0,0, msg, font, skin.headerBtnTextColor, skin.headerBtnFillColor );
   QImage img = createRectImage( bw,bh, skin.headerBtnPenColor, skin.headerBtnFillColor, ico );
   btn->setImage( 0, img );
   // [idle_hover]
   btn->setImage( 1, img );

   // [active]
   ico = createImageFromText( 1,1, msg, font, skin.headerBtnTextColor, skin.activeColor );
   img = createRectImage( bw,bh, skin.headerBtnPenColor, skin.activeColor, ico );
   btn->setImage( 2, img );
   // [active_hover]
   btn->setImage( 3, img );

   return btn;
}

ImageButton*
Header::createCpuUsageEdit()
{
   LiveSkin const & skin = m_app.m_skin;
   auto & btn = m_CpuUsageEdit;
   btn = new ImageButton( this );

   int bw = 32;
   int bh = 17;

   QFont font = getHeaderFont();
   QString msg = "1 %";
   // [idle]
   QImage ico = createImageFromText( 0,0, msg, font, skin.headerBtnTextColor, skin.headerBtnFillColor );
   QImage img = createRectImage( bw,bh, skin.headerBtnPenColor, skin.headerBtnFillColor, ico );
   btn->setImage( 0, img );
   // [idle_hover]
   btn->setImage( 1, img );

   // [active]
   ico = createImageFromText( 1,1, msg, font, skin.headerBtnTextColor, skin.activeColor );
   img = createRectImage( bw,bh, skin.headerBtnPenColor, skin.activeColor, ico );
   btn->setImage( 2, img );
   // [active_hover]
   btn->setImage( 3, img );

   return btn;
}

ImageButton*
Header::createDiskUsageEdit()
{
   LiveSkin const & skin = m_app.m_skin;
   auto & btn = m_DiskUsageEdit;
   btn = new ImageButton( this );

   int bw = 17;
   int bh = 17;

   QFont font = getHeaderFont();
   QString msg = "D";
   // [idle]
   QImage ico = createImageFromText( 0,0, msg, font, skin.headerBtnTextColor, skin.headerBtnFillColor );
   QImage img = createRectImage( bw,bh, skin.headerBtnPenColor, skin.headerBtnFillColor, ico );
   btn->setImage( 0, img );
   // [idle_hover]
   btn->setImage( 1, img );

   // [active]
   ico = createImageFromText( 1,1, msg, font, skin.headerBtnTextColor, skin.activeColor );
   img = createRectImage( bw,bh, skin.headerBtnPenColor, skin.activeColor, ico );
   btn->setImage( 2, img );
   // [active_hover]
   btn->setImage( 3, img );

   return btn;
}

ImageButton*
Header::createLiveMidiInLED()
{
   LiveSkin const & skin = m_app.m_skin;
   auto & btn = m_LiveMidiInLED;
   btn = new ImageButton( this );
   btn->setCheckable( true );
   btn->setChecked( false );

   int bw = 8;
   int bh = 8;

   // [idle]
   QImage img = createRectImage( bw,bh, skin.headerBtnPenColor, skin.headerBtnFillColor );
   btn->setImage( 0, img );
   // [idle_hover]
   btn->setImage( 1, img );

   // [active]
   img = createRectImage( bw,bh, skin.headerBtnPenColor, skin.activeColor );
   btn->setImage( 2, img );
   // [active_hover]
   btn->setImage( 3, img );

   return btn;
}

ImageButton*
Header::createLiveMidiOutLED()
{
   LiveSkin const & skin = m_app.m_skin;
   auto & btn = m_LiveMidiOutLED;
   btn = new ImageButton( this );

   int bw = 8;
   int bh = 8;
   btn->setCheckable( true );
   btn->setChecked( false );

   // [idle]
   QImage img = createRectImage( bw,bh, skin.headerBtnPenColor, skin.headerBtnFillColor );
   btn->setImage( 0, img );
   // [idle_hover]
   btn->setImage( 1, img );

   // [active]
   img = createRectImage( bw,bh, skin.headerBtnPenColor, skin.activeColor );
   btn->setImage( 2, img );
   // [active_hover]
   btn->setImage( 3, img );

   return btn;
}
