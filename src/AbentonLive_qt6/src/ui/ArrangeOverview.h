#pragma once
#include "LiveSkin.h"

struct App;

// TrackListOverview
// ============================================================================
class TimeLineOverview : public QWidget
// ============================================================================
{
   DE_CREATE_LOGGER("TimeLineOverview")
   Q_OBJECT
   App & m_app;
   bool m_isHighlighted;
   int m_lineHeight;

public:
   TimeLineOverview( App & app, QWidget* parent = 0 );
   ~TimeLineOverview() override {}

   void reset()
   {
      m_isHighlighted = false;
   }
   int computeBestHeight() const;

signals:
public slots:
protected slots:
protected:
   void updateFromTrackList();
   //void resizeEvent( QResizeEvent* event ) override;
   void paintEvent( QPaintEvent* event ) override;
};
