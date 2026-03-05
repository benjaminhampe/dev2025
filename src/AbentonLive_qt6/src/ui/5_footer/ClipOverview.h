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

struct App;

// ============================================================================
class ClipOverview : public QWidget
// ============================================================================
{
   Q_OBJECT
   //DE_CREATE_LOGGER("ClipOverview")
   App & m_app;
   bool m_isHovered;
   bool m_isFocused;
   QString m_text;
//   PluginExplorer2* m_pluginExplorer;
   QImage m_imgSrc;  // Big original image laying in memory ( of the full track chain )
   QImage m_imgDst;  // Current drawn image ( small and added with zoom and pos )
public:
   ClipOverview( App & app, QWidget* parent = 0 );
   ~ClipOverview() override {}

   QSize computeBestSize() const;

signals:
   void clicked( bool );
public slots:
protected:

   void resizeEvent( QResizeEvent* event ) override;
   void paintEvent( QPaintEvent* event ) override;
   void enterEvent( QEnterEvent* event ) override;
   void leaveEvent( QEvent* event ) override;

   void focusInEvent( QFocusEvent* event ) override;
   void focusOutEvent( QFocusEvent* event ) override;
//   void hideEvent( QHideEvent* event ) override;
//   void showEvent( QShowEvent* event ) override;

   void mousePressEvent( QMouseEvent* event ) override;
   void mouseReleaseEvent( QMouseEvent* event ) override;
   void mouseMoveEvent( QMouseEvent* event ) override;
   void wheelEvent( QWheelEvent* event ) override;

   void keyPressEvent( QKeyEvent* event ) override;
   void keyReleaseEvent( QKeyEvent* event ) override;
private:
};
