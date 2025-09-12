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
#include <QSlider>
#include <QDial>
#include <QLabel>

#include <QFont5x8.hpp>

// ============================================================================
class GSliderV : public QWidget
// ============================================================================
{
   Q_OBJECT
   DE_CREATE_LOGGER("GSliderV")
public:
   explicit
   GSliderV( QString name, int value, int min = 0, int max = 1, QString unit = "", int step = 1, QWidget* parent = 0 );
   ~GSliderV();

   int value() const { return m_slider->value(); }
   int minimum() const { return m_slider->minimum(); }
   int maximum() const { return m_slider->maximum(); }
   int singleStep() const { return m_slider->singleStep(); }
   QString name() const { return m_title->text(); }
   QString unit() const { return m_unit; }

signals:
   void valueChanged( int value );

public slots:
   void setValue(int v)
   {
      m_slider->blockSignals( true );
      m_slider->setValue( v );
      m_slider->blockSignals( false );
      on_sliderChanged( v );
   }
   void setMinimum(int v)
   {
      m_slider->blockSignals( true );
      m_slider->setMinimum( v );
      m_slider->blockSignals( false );
   }

   void setMaximum(int v) { m_slider->blockSignals( true ); m_slider->setMaximum( v ); m_slider->blockSignals( false ); }
   void setSingleStep(int v) { m_slider->blockSignals( true ); m_slider->setSingleStep( v ); m_slider->blockSignals( false ); }
   void setName(QString title) { m_title->blockSignals( true ); m_title->setText( title ); m_slider->blockSignals( false ); }
   void setUnit(QString unit) { m_unit = unit; }

private slots:
   void on_sliderChanged( int value );
   // void on_sliderPressed();
   // void on_sliderReleased();

protected:
   void timerEvent( QTimerEvent* event) override;
   void paintEvent( QPaintEvent* event ) override;
   void resizeEvent( QResizeEvent* event ) override;
   void keyPressEvent( QKeyEvent* event ) override;
   void keyReleaseEvent( QKeyEvent* event ) override;
   void mousePressEvent( QMouseEvent* event ) override;
   void mouseReleaseEvent( QMouseEvent* event ) override;
   void mouseMoveEvent( QMouseEvent* event ) override;
   void wheelEvent( QWheelEvent* event ) override;

private:
   QLabel* m_title;
   QSlider* m_slider;
   QLabel* m_amount;
   int m_default;
   QString m_unit;
   QString m_desc; // Tooltip Info
};
