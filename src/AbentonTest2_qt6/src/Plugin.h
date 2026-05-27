#pragma once
#include <QWidget>

class Plugin : public QWidget {
    Q_OBJECT
public:
    explicit Plugin(QWidget *parent = nullptr);
    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;
    QSize calcWishSize() const;
public slots:
    void on_skinChanged();

protected:
    void resizeEvent(QResizeEvent *) override;
    void paintEvent(QPaintEvent *) override;
};
