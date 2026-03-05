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
class LongText : public QWidget
// ============================================================================
{
   Q_OBJECT
   QColor m_fillColor;
   QColor m_textColor;
   QFont m_font;
   int m_ascent;
   QString m_longText;
public:
   LongText( QWidget* parent = 0 );
   ~LongText() override {}
signals:
public slots:
   void setFillColor( QColor fillColor ) 
   { 
      m_fillColor = fillColor; 
      update();
   }
   void setTextColor( QColor textColor ) 
   { 
      m_textColor = textColor; 
      update();
   }
   void setLongText( QString txt ) 
   { 
      m_longText = txt; 
      update();
   }
protected slots:
protected:
   //void resizeEvent( QResizeEvent* event ) override;
   void paintEvent( QPaintEvent* event ) override;

   // void enterEvent( QEvent* event ) override;
   // void leaveEvent( QEvent* event ) override;

   // void focusInEvent( QFocusEvent* event ) override;
   // void focusOutEvent( QFocusEvent* event ) override;

   // void mousePressEvent( QMouseEvent* event ) override;
   // void mouseReleaseEvent( QMouseEvent* event ) override;
   // void mouseMoveEvent( QMouseEvent* event ) override;
   // void wheelEvent( QWheelEvent* event ) override;

   // void keyPressEvent( QKeyEvent* event ) override;
   // void keyReleaseEvent( QKeyEvent* event ) override;

};
