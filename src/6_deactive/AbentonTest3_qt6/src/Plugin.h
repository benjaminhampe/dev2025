#pragma once
#include <QWidget>

class Plugin : public QWidget {
    Q_OBJECT
public:
    explicit Plugin(std::string debugName, QWidget *parent = nullptr);
    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;
    QSize calcWishSize() const;
signals:
    void requestsNewLayout();

public slots:
    void on_skinChanged();

protected:
    void resizeEvent(QResizeEvent *) override;
    void paintEvent(QPaintEvent *) override;

    std::string m_debugName;
};
