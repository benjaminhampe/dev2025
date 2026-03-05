/// (c) 2017 - 20180 Benjamin Hampe <benjaminhampe@gmx.de>
#pragma once
#include <App.h>

inline void
drawBody( QPainter & dc, QRect pos, LiveSkin const & skin )
{
   int w = pos.width();
   int h = pos.height();
   if ( w < 1 || h < 1 ) return;
   dc.fillRect( pos, skin.windowColor );
}

inline void
drawPanel( QPainter & dc, QRect pos, LiveSkin const & skin, bool hasFocus = false )
{
   int x = pos.x();
   int y = pos.y();
   int w = pos.width();
   int h = pos.height();
   if ( w < 4 ) return;
   if ( h < 4 ) return;

   //auto bgColor = skin.panelBlendColor;
   auto fgColor = hasFocus ? skin.focusColor : skin.panelColor;

   int rx = skin.radius;
   int ry = skin.radius;

   drawRoundRectFill( dc, x, y, w, h, fgColor, rx, ry );
   //drawRoundRectFill( dc, x, y, w, h, bgColor, rx, ry );
   //drawRoundRectFill( dc, x+1, y+1, w-2, h-2, fgColor, rx, ry );
}

inline void
drawShadow( QPainter & dc, QRect pos, LiveSkin const & skin, bool hasFocus = false )
{
   int x = pos.x();
   int y = pos.y();
   int w = pos.width();
   int h = pos.height();

   if ( w < 2 ) return;
   if ( h < 2 ) return;
   auto bgColor = skin.windowColor;

   int rx = skin.radius;
   int ry = skin.radius;

   drawRoundRectFill( dc, x, y, w, h, bgColor, rx, ry );
   //drawRoundRectFill( dc, x+1, y+1, w-2, h-2, fgColor, rx, ry );
}

/*
inline int
drawImageButton( QPainter & dc, Button const & btn )
{
//   std::cout << "Draw Button ID(" << btn.id() << ") pos(" << btn.x() << ", "
//    << btn.y() << ", " << btn.w() << ", " << btn.h() << ")" << std::endl;
   if ( !btn.m_isVisible )
   {
      return 0;
   }

   int x = btn.x();
   int y = btn.y();

   if ( btn.m_isChecked )
   {
      if ( btn.m_isHovered )
      {
         dc.drawImage( x, y, btn.m_imgActiveHover );
      }
      else
      {
         dc.drawImage( x, y, btn.m_imgActive );
      }
   }
   else
   {
      if ( btn.m_isHovered )
      {
         dc.drawImage( x, y, btn.m_imgIdleHover );
      }
      else
      {
         dc.drawImage( x, y, btn.m_imgIdle );
      }
   }

   return btn.w();
}
*/

inline void
drawContent( QPainter & dc, QRect pos, LiveSkin const & skin )
{
   int x = pos.x();
   int y = pos.y();
   int w = pos.width();
   int h = pos.height();
   if ( w < 2 || h < 2 ) return;

   // Draw background
   drawRectFill( dc, QRect( x, y, w, h ), skin.contentColor );
}

inline void
drawHelpView( QPainter & dc, QRect pos, LiveSkin const & skin )
{
   int x = pos.x();
   int y = pos.y();
   int w = pos.width();
   int h = pos.height();
   if ( w < 2 || h < 2 ) return;

   // Draw background
   drawRectFill( dc, QRect( x, y, w, h ), skin.contentColor );

   // Draw title rect
   if ( h > skin.titleH )
   {
      drawRectFill( dc, QRect( x, y, w, skin.titleH ), skin.titleColor );
      if ( h > skin.titleH + skin.tdH )
      {
         drawRectFill( dc, QRect( x, y + skin.titleH, w, skin.tdH ), skin.tdColor );
      }
      else
      {
         drawRectFill( dc, QRect( x, y + skin.titleH, w, h - skin.titleH ), skin.tdColor );
      }
   }
   else
   {
      drawRectFill( dc, QRect( x, y, w, h ), skin.titleColor );
   }

   // Draw table header
   if ( h > skin.titleH + skin.tdH )
   {

   }

}

inline void
drawArrangement( QPainter & dc, App & app, QRect clipRect )
{
/*
   int w = clipRect.width();
   int h = clipRect.height();
   if ( w < 1 || h < 1 )
   {
      return;
   }

   LiveSkin const & skin = app.m_skin;
   BeatGrid const & grid = app.m_beatGrid;
   TrackList const & trackList = m_app.m_trackList;

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
      for ( int c = 0; c < track.clipCount(); ++c )
      {
         Clip const & clip = track.clip( c );
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
}

inline void
drawTimeLineOverview( QPaintEvent* event )
{
/*
   int w = width();
   int h = height();

   if ( w > 1 && h > 1 )
   {
      LiveSkin const & skin = m_app.m_skin;
      BeatGrid const & grid = m_app.m_beatGrid;
      TrackList const & trackList = m_app.m_trackList;

      QPainter dc( this );
      dc.setRenderHint( QPainter::NonCosmeticDefaultPen );

      // Draw gray background
      dc.fillRect( rect(), skin.contentColor );

      // Draw tracks
      int y = 2;
      for ( int i = 0; i < trackList.trackCount(); ++i )
      {

         Track const & track = trackList.track( i );
         for ( int c = 0; c < track.clipCount(); ++c )
         {
            Clip const & clip = track.clip( c );
            int x1 = grid.time2pixel( clip.m_timeBeg );
            int x2 = grid.time2pixel( clip.m_timeEnd );
            dc.fillRect( QRect(x1, y, x2, m_lineHeight ), clip.m_color );
         }

         dc.setBrush( Qt::NoBrush );
         dc.setPen( QPen( skin.panelColor ) );
         dc.drawLine( 0, y+m_lineHeight - 1, w-1, y+m_lineHeight-1 );
         y += m_lineHeight;

      }

      dc.setBrush( Qt::NoBrush );
      dc.setPen( QPen( QColor(0,0,0) ) );
      dc.drawRect( QRect( 0,0,w-1,h-1 ) );
      dc.drawRect( QRect( 1,1,w-3,h-3 ) );
   }
*/
}


