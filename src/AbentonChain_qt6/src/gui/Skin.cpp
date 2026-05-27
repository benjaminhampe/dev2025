#include "Skin.h"

// ------------------------------------------------------------
// Utils
// ------------------------------------------------------------
bool
isMouseOver(const QPoint &pos, const QRect &r)
{
    const int m = pos.x();
    const int n = pos.y();
    const int x1 = r.x();
    const int y1 = r.y();
    const int x2 = r.x() + r.width() - 1;
    const int y2 = r.y() + r.height() - 1;
    return (m >= x1) && (m <= x2)
           && (n >= y1) && (n <= y2);
}

QString
qstr(const QRect &r)
{
    const int x = r.x();
    const int y = r.y();
    const int w = r.width();
    const int h = r.height();
    return QString("%1,%2,%3,%4").arg(x).arg(y).arg(w).arg(h);
}

QRect
mkRect(const QRect &r, int b)
{
    const int x = r.x();
    const int y = r.y();
    const int w = r.width();
    const int h = r.height();
    return QRect(x + b,
                 y + b,
                 w - (b+1), h - (b+1));
}

QPixmap mkSvg(const QString &svg, int size)
{
    QSvgRenderer renderer(svg.toUtf8());
    QPixmap pix(size, size);
    pix.fill(Qt::transparent);

    QPainter p(&pix);
    renderer.render(&p);

    return pix;
}

QString toSvg(const QColor& c)
{
    if (c.alpha() < 255) {
        return QString("rgba(%1,%2,%3,%4)")
        .arg(c.red())
            .arg(c.green())
            .arg(c.blue())
            .arg(c.alphaF(), 0, 'f', 3); // Alpha als float 0–1
    }

    return QString("rgb(%1,%2,%3)")
        .arg(c.red())
        .arg(c.green())
        .arg(c.blue());
}

QIcon toQIcon(const QString& svg, int w, int h)
{
    QSvgRenderer renderer(svg.toUtf8());
    QPixmap pix(w, h);
    pix.fill(Qt::transparent);
    QPainter p(&pix);
    renderer.render(&p);
    return QIcon(pix);
}

/*
QString
mkSvg_Power( int d, int p, int r )
{
    auto s = QString(R"(
    <svg width="%1" height="%1" viewBox="0 0 %1 %1"
         xmlns="http://www.w3.org/2000/svg">
      <rect x="4" y="4" width="56" height="56" rx="10"
            fill="#2196F3" stroke="black" stroke-width="3"/>
      <text x="32" y="38" font-size="24" text-anchor="middle" fill="white">A</text>
    </svg>
    )";

}
*/

void
setButtonSvg(QPushButton* btn, const QString &svg, int size)
{
    btn->setFlat(true);
    // btn->setStyleSheet("background: transparent; border: none;");
    btn->setIcon(QIcon(mkSvg(svg, size)));
    btn->setIconSize(QSize(size, size));
    btn->setFixedSize( size, size );
}

void
setButtonPix(QPushButton* btn, const QPixmap &pix)
{
    btn->setFlat(true);
    // btn->setStyleSheet("background: transparent; border: none;");
    btn->setIcon(QIcon(pix));
    btn->setIconSize(pix.size());
    btn->setFixedSize(pix.size());
}
