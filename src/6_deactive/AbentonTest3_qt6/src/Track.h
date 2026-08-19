#pragma once
#include "Plugin.h"

class Track : public QWidget {
    Q_OBJECT
public:
    explicit Track(QWidget *parent = nullptr);
    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

    void addPlugin(std::string debugName);
public slots:
    // void on_skinChanged();
    void on_requestsNewLayout();
protected:
    void relayoutChildren();
    void resizeEvent(QResizeEvent* e) override;
    void paintEvent(QPaintEvent* e) override;

    std::vector< Plugin* > m_plugins;

    bool m_layoutDirty = true;
};
