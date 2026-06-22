#pragma once
#include <QWidget>
#include <QStackedWidget>
#include <QScrollBar>
#include "ChainWrapper.h"

// ============================================
class ChainStack : public QWidget
// ============================================
{
    Q_OBJECT
public:
    ChainStack(QWidget* parent = nullptr);
    ~ChainStack() override;
    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

    void applySkin();
    void addPage(ChainWrapper* page);

protected:
    bool eventFilter(QObject *obj, QEvent *event) override;
    void paintEvent(QPaintEvent* event) override;

signals:
    void newTrackOverview(QPixmap pix); // for Footer

protected slots:
private slots:
    void onUpdateScrollRange();
    void onUpdateScrollPosition(int value);

private:
    int m_baseWidth = 284;
    int m_baseHeight = 376 + 20;
    int m_baseMargin = 10;

    int m_height;

    QColor m_windowColor;

    QStackedWidget* m_stack;
    QScrollBar* m_scrollBar;
};
