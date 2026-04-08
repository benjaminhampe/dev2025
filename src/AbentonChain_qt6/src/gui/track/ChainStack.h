#pragma once
#include <QWidget>
#include <QStackedWidget>
#include <QScrollBar>
#include "gui/track/ChainWrapper.h"

// ============================================
class ChainStack : public QWidget
// ============================================
{
    Q_OBJECT
public:
    ChainStack(QWidget* parent = nullptr);
    ~ChainStack() override;

    void applySkin();

    void addPage(QWidget *page);

protected:
    bool eventFilter(QObject *obj, QEvent *event) override;
    void paintEvent(QPaintEvent* event) override;

protected slots:
private slots:
    void onUpdateScrollRange();
    void onUpdateScrollPosition(int value);

private:
    int m_baseWidth = 284;
    int m_baseHeight = 326;
    int m_baseMargin = 10;

    QColor m_windowColor;

    QStackedWidget* m_stack;
    QScrollBar* m_scrollBar;
};
