#include "Skin.h"

// ------------------------------------------------------------
// Utils
// ------------------------------------------------------------
bool
isMouseOver(const int mx, const int my, const QRect &r)
{
    const int x1 = r.x();
    const int y1 = r.y();
    const int x2 = r.x() + r.width() - 1;
    const int y2 = r.y() + r.height() - 1;
    return dbMouseOver(mx,my,x1,y1,x2,y2);
}

bool
isMouseOver(const QPoint &pos, const QRect &r)
{
    const int x1 = r.x();
    const int y1 = r.y();
    const int x2 = r.x() + r.width() - 1;
    const int y2 = r.y() + r.height() - 1;
    return dbMouseOver(pos.x(),pos.y(),x1,y1,x2,y2);
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

QPixmap mkSvg(const QString &svg, int w, int h)
{
    QSvgRenderer renderer(svg.toUtf8());
    QPixmap pix(w,h);
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
setButtonSvg(QPushButton* btn, const QString &svg, int w, int h)
{
    btn->setFlat(true);
    // btn->setStyleSheet("background: transparent; border: none;");
    btn->setIcon(QIcon(mkSvg(svg,w,h)));
    btn->setIconSize(QSize(w,h));
    //btn->setFixedSize(w,h);
}

void
setButtonPix(QPushButton* btn, const QPixmap &pix)
{
    btn->setFlat(true);
    // btn->setStyleSheet("background: transparent; border: none;");
    btn->setIcon(QIcon(pix));
    btn->setIconSize(pix.size());
    //btn->setFixedSize(pix.size());
}


QColor
toQColor( uint32_t color )
{
    int32_t r = dbRGBA_R(color);
    int32_t g = dbRGBA_G(color);
    int32_t b = dbRGBA_B(color);
    int32_t a = dbRGBA_A(color);
    return QColor( r,g,b,a );
}

void
drawKey( QPainter & dc, QRect pos, QColor brushColor, QColor penColor )
{
    int x = pos.x();
    int y = pos.y();
    int w = pos.width()-1;
    int h = pos.height()-1;

    if ( h < 6 )
    {
        dc.setPen( Qt::NoPen );
        dc.setBrush( QBrush( brushColor ) );
        dc.drawRect( pos );
    }
    else
    {
        dc.setPen( Qt::NoPen );
        dc.setBrush( QBrush( brushColor ) );
        dc.drawRect( QRect(x+1,y+1,w-2,h-2) );

        dc.setPen( QPen( penColor ) );
        dc.drawLine( x+1,y, x+w-2, y );
        dc.drawLine( x+1,y+h-1, x+w-2, y+h-1 );
        dc.drawLine( x,y+1, x, y+h-2 );
        dc.drawLine( x+w-1,y+1, x+w-1, y+h-2 );
    }
}

QColor
blendRGB( QColor from, QColor to, float t )
{
    int32_t r = from.red();
    int32_t g = from.green();
    int32_t b = from.blue();
    //int32_t a = from.alpha();
    int32_t dr = int( to.red() ) - r;
    int32_t dg = int( to.green() ) - g;
    int32_t db = int( to.blue() ) - b;
    //int32_t da = int( to.alpha() ) - a;

    r = std::clamp( int32_t( t * dr + float( r ) ), 0, 255 );
    g = std::clamp( int32_t( t * dg + float( g ) ), 0, 255 );
    b = std::clamp( int32_t( t * db + float( b ) ), 0, 255 );
    //a = std::clamp( int32_t( t * da + float( a ) ), 0, 255 );
    return QColor( r, g, b );
}

int
computeBestFontHeight(QFont baseFont, int maxHeight)
{
    QFont tf = baseFont;
    tf.setPixelSize(maxHeight);

    QFontMetrics fm(tf);

    static const QString testText = "1WCgp";
    int i = 0;
    int textHeight = fm.boundingRect(testText).height();

    // DE_OK("Start "
    //       "fontSize(",tf.pixelSize(), "), "
    //       "maxHeight(", maxHeight,"), "
    //       "textHeight(",textHeight, ")")

    while (textHeight < maxHeight)
    {
        int lastSize = tf.pixelSize() + 1;
        if (lastSize > 64)
        {
            tf.setPixelSize(64);
            break;
        }
        tf.setPixelSize(lastSize);
        fm = QFontMetrics(tf);
        textHeight = fm.boundingRect(testText).height();

        // DE_OK("[",i,"] "
        //   "fontSize(",tf.pixelSize(), "), "
        //   "maxHeight(", maxHeight,"), "
        //   "textHeight(",textHeight, ")")
        i++;
    }

    while (textHeight > maxHeight)
    {
        int lastSize = tf.pixelSize() - 1;
        if (lastSize < 8)
        {
            tf.setPixelSize(8);
            break;
        }
        tf.setPixelSize(lastSize);
        fm = QFontMetrics(tf);
        textHeight = fm.boundingRect(testText).height();

        // DE_OK("[",i,"] "
        //   "fontSize(",tf.pixelSize(), "), "
        //   "maxHeight(", maxHeight,"), "
        //   "textHeight(",textHeight, ")")

        i++;
    }

    int pixelSize = tf.pixelSize();
    //DE_OK("Computed best Font.pixelSize = ",pixelSize," after ",i," tries")
    return pixelSize;
}
