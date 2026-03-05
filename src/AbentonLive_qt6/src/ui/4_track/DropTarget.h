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

#include "LiveSkin.h"

class App;

// ============================================================================
class DropTarget : public QWidget
// ============================================================================
{
   Q_OBJECT
   //DE_CREATE_LOGGER("DropTarget")
   App& m_app;
   bool m_isDirty;
   bool m_isAudioOnly;
   bool m_hasFocus;
   //   ClipEditor* m_clipEditor;
   QSize m_size;
   QSize m_lastSize;
   QImage m_img;

   QString m_msg1;
   QString m_msg2;
public:
   DropTarget( App& app, QWidget* parent = 0 );
   ~DropTarget() override {}
   bool hasFocus() const { return m_hasFocus; }
signals:
public slots:
   void setAudioOnly( bool b );
protected:
   void updateImage();
   void resizeEvent( QResizeEvent* event ) override;
   void paintEvent( QPaintEvent* event ) override;
   void enterEvent( QEnterEvent* event ) override;
   void leaveEvent( QEvent* event ) override;
   void focusInEvent( QFocusEvent* event ) override;
   void focusOutEvent( QFocusEvent* event ) override;
   void dropEvent( QDropEvent* event ) override;
   void dragEnterEvent( QDragEnterEvent* event ) override;
   void dragLeaveEvent( QDragLeaveEvent* event ) override;
   void dragMoveEvent(QDragMoveEvent* event ) override;
private:
};
