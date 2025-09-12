#pragma once
#include <QWidget>
#include <QSplitter>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>

// ============================================================================
class Body : public QSplitter
// ============================================================================
{
   Q_OBJECT
public:
   Body( QWidget* parent = 0 );
   ~Body();
signals:
public slots:
protected slots:
   void startUpdateTimer();
   void stopUpdateTimer();
protected:
   void timerEvent( QTimerEvent* event ) override;
   void paintEvent( QPaintEvent* event ) override;
public:
   int m_updateTimerId;
};
