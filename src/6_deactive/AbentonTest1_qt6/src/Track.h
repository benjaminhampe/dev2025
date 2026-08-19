#pragma once
#include "Plugin.h"
#include <QHBoxLayout>

class Track : public QWidget {
    Q_OBJECT
public:
    explicit Track(QWidget *parent = nullptr)
        : QWidget(parent)
    {
        setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

        auto h = new QHBoxLayout(this);
        h->setContentsMargins(0, 0, 0, 0);
        h->setSpacing(4);

        h->addWidget(new Plugin(this));
        h->addWidget(new Plugin(this));
        h->addWidget(new Plugin(this));
        h->addWidget(new Plugin(this));
        h->addWidget(new Plugin(this));
        h->addWidget(new Plugin(this));
        h->addWidget(new Plugin(this));
        h->addWidget(new Plugin(this));
        h->addWidget(new Plugin(this));

    }

    void setZoomHeight(int h) {

        setFixedHeight(h);   // the only correct call

        updateGeometry();    // notify parent layout

        /*
        setMinimumHeight(h);
        setMaximumHeight(h);

        updateGeometry();   // notify parent layout
        //resize(width(), h); // force resize event

        if (auto lay = layout()) {
            lay->invalidate();
            //lay->activate();
        }
        //update();
*/
    }

    QSize minimumSizeHint() const override {
        return QSize(0, 0);   // CRITICAL: do not propagate minimum height upward
    }
};
