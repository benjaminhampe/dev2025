#pragma once
#include <QWidget>

// ============================================
class QuickHelp : public QWidget
// ============================================
{
    Q_OBJECT
public:
    QuickHelp(QWidget* parent = nullptr);
    void applySkin();
    void updateLayout();
protected:
    void resizeEvent(QResizeEvent* e) override;
    void paintEvent(QPaintEvent* e) override;
private:
    int m_zoom = 100;
    QColor m_windowColor;
    QColor m_panelColor;
    QColor m_textColor;
    QFont m_helpFont;
    QString m_helpText = "QuickHelp Window shall display\nuseful information about widgets\nunder the current mouse-cursor.";
};
