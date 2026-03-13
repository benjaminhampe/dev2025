#include "ShellCommon.h"

ImageButton*
ShellCommon::createEnableButton( const LiveSkin& skin, QWidget* parent )
{
   auto btn = new ImageButton( parent );
   btn->setToolTip("This DSP element is now (e)nabled = not bypassed");

   int b = 13;

   btn->setCheckable( true );
   btn->setChecked( false );

   auto symColor = skin.symbolColor;
   auto bgColor = skin.windowColor;
   auto fgColor = skin.panelColor; // or panelColor

   //QFont font = getFontAwesome( 14 );
   std::string
   msg = "  #####\n"
         " #     #\n"
         "#       #\n"
         "#   #   #\n"
         "#   #   #\n"
         "#   #   #\n"
         "#       #\n"
         " #     #\n"
         "  #####\n";

   // [idle] has active color, means not bypassed
   QImage ico = createAsciiArt( skin.activeColor, fgColor, msg );
   QImage img = createCircleImage( b,b, bgColor, fgColor, ico );
   btn->setImage( 0, img );
   // [idle_hover]
   btn->setImage( 1, img );

   // [active]
   ico = createAsciiArt( symColor, fgColor, msg );
   img = createCircleImage( b,b, bgColor, fgColor, ico );
   btn->setImage( 2, img );
   // [active_hover]
   btn->setImage( 3, img );

   return btn;
}

ImageButton*
ShellCommon::createMoreButton( const LiveSkin& skin, QWidget* parent )
{
   auto btn = new ImageButton( parent );
   btn->setToolTip("All DSP options are visible now");
   int b = 13;
   btn->setCheckable( true );
   btn->setChecked( false );

   auto symColor = skin.symbolColor;
   auto bgColor = skin.windowColor;
   auto fgColor = skin.panelColor; // or panelColor

   //QFont font = getFontAwesome( 14 );
   std::string
   msg = "#######\n"
         " #####\n"
         " #####\n"
         "  ###\n"
         "  ###\n"
         "   #\n"
         "   #\n";
   // [idle]
   QImage ico = createAsciiArt( symColor, fgColor, msg );
   QImage img = createCircleImage( b,b, bgColor, fgColor, ico );
   btn->setImage( 0, img );
   // [idle_hover]
   btn->setImage( 1, img );

   // [active]
   msg = "#\n"
         "###\n"
         "#####\n"
         "#######\n"
         "#####\n"
         "###\n"
         "#\n";
   ico = createAsciiArt( skin.activeColor, fgColor, msg );
   img = createCircleImage( b,b, bgColor, fgColor, ico );
   btn->setImage( 2, img );
   // [active_hover]
   btn->setImage( 3, img );

   return btn;
}


ImageButton*
ShellCommon::createEditorButton( const LiveSkin& skin, QWidget* parent )
{
   auto btn = new ImageButton( parent );
   int b = 13;
   btn->setCheckable( true );
   btn->setChecked( false );
   auto symColor = skin.symbolColor;
   auto bgColor = skin.windowColor;
   auto fgColor = skin.panelColor; // or panelColor

   //QFont font = getFontAwesome( 14 );
   std::string
   msg = "# #####\n"
         " \n"
         "# #####\n"
         " \n"
         "# #####\n";

   // [idle]
   QImage ico = createAsciiArt( symColor, fgColor, msg );
   QImage img = createCircleImage( b,b, bgColor, fgColor, ico );
   btn->setImage( 0, img );
   // [idle_hover]
   btn->setImage( 1, img );

   // [active]
   ico = createAsciiArt( skin.activeColor, fgColor, msg );
   img = createCircleImage( b,b, bgColor, fgColor, ico );
   btn->setImage( 2, img );
   // [active_hover]
   btn->setImage( 3, img );

   return btn;
}

ImageButton*
ShellCommon::createUpdateButton( const LiveSkin& skin, QWidget* parent )
{
   auto btn = new ImageButton( parent );
   int b = 13;
   btn->setCheckable( true );
   btn->setChecked( false );
   auto symColor = skin.symbolColor;
   auto bgColor = skin.windowColor;
   auto fgColor = skin.panelColor; // or panelColor

   //QFont font = getFontAwesome( 14 );
   std::string
   msg = "   ##\n"
         "  #\n"
         " #     #\n"
         "###   ###\n"
         " #     #\n"
         "      #\n"
         "    ##\n";

   // [idle]
   QImage ico = createAsciiArt( symColor, fgColor, msg );
   QImage img = createCircleImage( b,b, bgColor, fgColor, ico );
   btn->setImage( 0, img );
   // [idle_hover]
   btn->setImage( 1, img );

   // [active]
   ico = createAsciiArt( skin.activeColor, fgColor, msg );
   img = createCircleImage( b,b, bgColor, fgColor, ico );
   btn->setImage( 2, img );
   // [active_hover]
   btn->setImage( 3, img );

   return btn;
}


ImageButton*
ShellCommon::createSaveButton( const LiveSkin& skin, QWidget* parent )
{
   auto btn = new ImageButton( parent );
   int b = 13;
   btn->setCheckable( true );
   btn->setChecked( false );
   auto symColor = skin.symbolColor;
   auto bgColor = skin.windowColor;
   auto fgColor = skin.panelColor; // or panelColor

   //QFont font = getFontAwesome( 14 );
   std::string
   msg = "######\n"
         "##   ##\n"
         "##   ##\n"
         "#######\n"
         "#######\n"
         "#######\n"
         "#######\n";

   // [idle]
   QImage ico = createAsciiArt( symColor, fgColor, msg );
   QImage img = createCircleImage( b,b, bgColor, fgColor, ico );
   btn->setImage( 0, img );
   // [idle_hover]
   btn->setImage( 1, img );

   // [active]
   ico = createAsciiArt( skin.activeColor, fgColor, msg );
   img = createCircleImage( b,b, bgColor, fgColor, ico );
   btn->setImage( 2, img );
   // [active_hover]
   btn->setImage( 3, img );

   return btn;
}