#ifndef G_FILE_INFO_HPP
#define G_FILE_INFO_HPP

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
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSlider>
#include <QFileDialog>

#include <QImageWidget.hpp>
#include <DarkImage.h>

// ============================================================================
class GFileInfo : public QWidget
// ============================================================================
{
   Q_OBJECT
   DE_CREATE_LOGGER("GFileInfo")
public:
   GFileInfo( QWidget* parent = 0 );
   ~GFileInfo();
signals:
public slots:
   void load();
private slots:
//   void on_slider_pos( int );
//   void on_clicked_load(bool checked);
//   void on_clicked_play(bool checked);
//   void on_clicked_stop(bool checked);

   // void stopUpdateTimer();
   // void startUpdateTimer( int ms = 67 );
   // void clearInputSignals();
   // void setInputSignal( de::audio::IDspChainElement* input );

protected:
   // void timerEvent( QTimerEvent* event) override;
   // void paintEvent( QPaintEvent* event ) override;

private:
   QLineEdit* m_edtUri;

   QImageWidget* m_coverArt;

   QPushButton* m_btnLoad;
   QPushButton* m_btnPlay;
   QPushButton* m_btnStop;

   QLabel* m_lblPosition;
   QSlider* m_sldPosition;
   QLabel* m_lblDuration;

   std::string m_uri;
};

#endif // G_OSZILLOSKOP_H
