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
#include <ClipEditor.h>
//#include <BottomLeftPanel.h>
//#include <ClipContent.h>

// ============================================================================
class ClipContent : public QWidget
// ============================================================================
{
   Q_OBJECT
   LiveSkin & m_skin;
   bool m_hasFocus;
   ClipEditor* m_clipEditor;
public:
   ClipContent( LiveSkin & skin, QWidget* parent = 0 );
   ~ClipContent();
   bool hasFocus() const { return m_hasFocus; }
signals:
public slots:
protected:
   void resizeEvent( QResizeEvent* event ) override;
   void paintEvent( QPaintEvent* event ) override;
   void enterEvent( QEnterEvent* event ) override;
   void leaveEvent( QEvent* event ) override;
   void focusInEvent( QFocusEvent* event ) override;
   void focusOutEvent( QFocusEvent* event ) override;
private:
};
