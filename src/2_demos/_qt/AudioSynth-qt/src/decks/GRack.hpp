#pragma once
#include <QWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>

// ============================================================================
class GRack : public QWidget
// ============================================================================
{
   Q_OBJECT
public:
   GRack( QWidget* parent = 0 );
   ~GRack();
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
