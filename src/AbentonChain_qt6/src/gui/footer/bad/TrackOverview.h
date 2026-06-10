#pragma once
#include <QWidget>
#include <QImage>
#include <QTimer>
#include <QPainter>
#include <QPaintEvent>
#include <QResizeEvent>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QDebug>
#include <QThread>

// ============================================================================
class TrackOverview : public QWidget
// ============================================================================
{
   Q_OBJECT
   bool m_isHovered;
   bool m_isFocused;
   //PluginExplorer2* m_pluginExplorer;
   QString m_text;
   QSize m_textSize;
   QImage m_imgSrc;  // Big original image laying in memory ( of the full track chain )
   QImage m_imgDst;  // Current drawn image ( small and added with zoom and pos )
public:
   TrackOverview( QWidget* parent = 0 );
   ~TrackOverview() override;

   QSize computeBestSize() const;

signals:
   void clicked( bool );
public slots:
   void setText( QString txt );
protected:
   void resizeEvent( QResizeEvent* event ) override;
   void paintEvent( QPaintEvent* event ) override;
   void focusInEvent( QFocusEvent* event ) override;
   void focusOutEvent( QFocusEvent* event ) override;
/*
   void enterEvent( QEnterEvent* event ) override;
   void leaveEvent( QEvent* event ) override;
//   void hideEvent( QHideEvent* event ) override;
//   void showEvent( QShowEvent* event ) override;

   void mousePressEvent( QMouseEvent* event ) override;
   void mouseReleaseEvent( QMouseEvent* event ) override;
   void mouseMoveEvent( QMouseEvent* event ) override;
   void wheelEvent( QWheelEvent* event ) override;

   void keyPressEvent( QKeyEvent* event ) override;
   void keyReleaseEvent( QKeyEvent* event ) override;
*/
private:
};
