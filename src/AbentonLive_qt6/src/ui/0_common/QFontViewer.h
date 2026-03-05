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
class QFontViewer : public QWidget
// ============================================================================
{
   Q_OBJECT
   QFont m_font;
   int m_glyphIndex = 0xf000;
   //DE_CREATE_LOGGER("QFontViewer")
public:
   QFontViewer( QWidget* parent = 0 );
   ~QFontViewer();
signals:
public slots:
   void setViewFont( QFont font ) { m_font = font; update(); }
   void setGlyphStart( int index = 0xf000 ) { m_glyphIndex = index; update(); }

protected:
private:
   QImage m_imgSource;
   QImage m_imgDisplay;
   bool m_isDisplayDirty;
   bool m_imgScaling;
   bool m_imgPreserveAspectWoH;
};
