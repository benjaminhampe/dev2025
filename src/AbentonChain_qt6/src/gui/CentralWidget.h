#pragma once
#include <QWidget>
#include <gui/header/Header.h>
#include <gui/arrange/ArraCentral.h>
#include <gui/viz/GL_Canvas.h>
#include <gui/clip/ClipEditor.h>
#include <gui/track/TrackStack.h>
#include <gui/footer/Footer.h>

// ============================================================================
class CentralWidget : public QWidget
// ============================================================================
{
    Q_OBJECT
public:
    Header*     m_header;
    ArraCentral*m_arraCentral;
    QWidget*    m_canvasContainer;
    GL_Canvas*  m_canvas;
    ClipEditor* m_clipEditor; // PianoRoll
    TrackStack* m_trackStack;
    Footer*     m_footer;

public:
    CentralWidget(QWidget* parent = 0);
    ~CentralWidget() override;
    void applySkin();
    void updateLayout();

signals:
public slots:
protected slots:

protected:
    bool event(QEvent* event) override;
    void resizeEvent( QResizeEvent* event ) override;
    void paintEvent( QPaintEvent* event ) override;
private:
    int m_zoom = 100;

    int m_baseClipEditorHeight = 400;
    int m_lastClipEditorHeight = 400;
};
