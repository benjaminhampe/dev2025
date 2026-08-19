#pragma once
#include "Plugin.h"

class Track : public QWidget {
    Q_OBJECT
public:
    explicit Track(QWidget *parent = nullptr);

    QSize minimumSizeHint() const override;

public slots:
    // void on_skinChanged();

protected:
    // void resizeEvent(QResizeEvent* e) override;

};
