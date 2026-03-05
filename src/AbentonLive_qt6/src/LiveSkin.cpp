#include "LiveSkin.h"

void
LiveSkin::update()
{
   panelBlendColor = blendRGB( windowColor, panelColor, 0.25f );
}

QColor
LiveSkin::getColor( eColor color )
{
   switch ( color )
   {
      case WindowColor: return windowColor;
      case SymbolColor: return symbolColor;
      case PanelColor: return panelColor;
      case FocusColor: return focusColor;
      case ActiveColor: return activeColor;
      case ContentColor: return contentColor;
      case TitleColor: return titleColor;
      case TitleTextColor: return titleTextColor;
      case EditBoxColor: return editBoxColor;
      case TdColor: return tdColor;
      default: return windowColor;
   }
}

int
LiveSkin::getInt( eInt param ) const
{
   if ( zoomUI != 1.0f )
   {
      switch( param )
      {
         case Padding: return int(zoomUI * float(padding));
         case Spacing: return int(zoomUI * float(spacing));
         case Radius: return int(zoomUI * float(radius));
         case EngineButtonH: return int(zoomUI * float(engineButtonH));
         case CircleButtonSize: return int(zoomUI * float(circleButtonSize));
         case ScrollButtonSize: return int(zoomUI * float(scrollButtonSize));
         case SmallCircleButtonSize: return int(zoomUI * float(smallCircleButtonSize));
         case Margin: return int(zoomUI * float(margin));
         case TitleH: return int(zoomUI * float(titleH));
         case SearchBarH: return int(zoomUI * float(searchBarH));
         case TdH: return int(zoomUI * float(tdH));
         default: return 0;
      }
   }
   else
   {
      switch( param )
      {
         case Padding: return padding;
         case Spacing: return spacing;
         case Radius: return radius;
         case EngineButtonH: return engineButtonH;
         case CircleButtonSize: return circleButtonSize;
         case ScrollButtonSize: return scrollButtonSize;
         case SmallCircleButtonSize: return smallCircleButtonSize;
         case Margin: return margin;
         case TitleH: return titleH;
         case SearchBarH: return searchBarH;
         case TdH: return tdH;
         default: return 0;
      }
   }
}
