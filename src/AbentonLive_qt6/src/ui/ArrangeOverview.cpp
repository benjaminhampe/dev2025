#include "ArrangeOverview.h"
#include "App.h"

// ============================================================================
TimeLineOverview::TimeLineOverview( App & app, QWidget* parent )
// ============================================================================
   : QWidget( parent )
   , m_app( app )
{
   m_lineHeight = 3;
   setObjectName( "TimeLineOverview" );
   setContentsMargins(0,0,0,0);
   setMinimumWidth( 100 );
   setMinimumHeight( m_lineHeight );
   updateFromTrackList();
}

int
TimeLineOverview::computeBestHeight() const
{
#if 0
   int h = m_lineHeight;
   int n = m_app.m_trackList.size();
   if ( n > 0 )
   {
      h = n * m_lineHeight;
   }
   return h+4;
#else
   return m_lineHeight;
#endif
}

void
TimeLineOverview::updateFromTrackList()
{
   int h = computeBestHeight();
   setMinimumHeight( h );
   setMaximumHeight( h );
   update();
}

//void
//TimeLineOverview::resizeEvent( QResizeEvent* event )
//{
//   update();
//   QWidget::resizeEvent( event );
//}

void
TimeLineOverview::paintEvent( QPaintEvent* event )
{
#if 0
   int w = width();
   int h = height();

   if ( w > 1 && h > 1 )
   {
      LiveSkin const & skin = m_app.m_skin;
      BeatGrid const & grid = m_app.m_beatGrid;
      TrackList const & tl = m_app.m_trackList;

      QPainter dc( this );
      dc.setRenderHint( QPainter::NonCosmeticDefaultPen );

      // Draw gray background
      dc.fillRect( rect(), skin.contentColor );

      // Draw tracks
      int y = 2;
      for ( int i = 0; i < tl.m_user.size(); ++i )
      {
         Track const & track = *tl.m_user[ i ];
         Clips const & clips = track.clips();
         for ( int c = 0; c < clips.size(); ++c )
         {
            Clip const & clip = clips[ c ];
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
#endif
   QWidget::paintEvent( event );
}
