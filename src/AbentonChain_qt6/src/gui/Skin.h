#pragma once
#include <QWidget>
#include <QPainter>
#include <QDragEnterEvent>
#include <QMimeData>
#include <QFileInfo>
#include <QRandomGenerator>
#include <QMenu>
#include <QTimer>
#include <QScrollBar>
#include <QJsonArray>
#include <QJsonDocument>
#include <QFile>
#include <QVBoxLayout>
#include <QScrollArea>
#include <QPushButton>
#include <QSvgRenderer>
#include <QPainter>

#include <DarkImage.h>

// ------------------------------------------------------------
// Utils
// ------------------------------------------------------------
bool
isMouseOver(const QPoint &pos, const QRect &r);

QString
qstr(const QRect &r);

QRect
mkRect(const QRect &r, int b = 1);

QPixmap
mkSvg(const QString &svg, int size = 64);

QPixmap
mkSvg_Power(int buttonWidth, const QColor& fillColor,
            int outlineWidth, const QColor& outlineColor);

QString
toSvg(const QColor& c);

QIcon
toQIcon(const QString& svg, int w, int h);


/*
QString
mkSvg_Power( int d, int p, int r );
*/

void setButtonSvg(QPushButton* btn, const QString &svg, int size );

void setButtonPix(QPushButton* btn, const QPixmap &pix );

inline QColor
toQColor( uint32_t color )
{
   int32_t r = dbRGBA_R(color);
   int32_t g = dbRGBA_G(color);
   int32_t b = dbRGBA_B(color);
   int32_t a = dbRGBA_A(color);
   return QColor( r,g,b,a );
}

inline void
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

inline QColor
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

// ============================================
struct Skin
// ============================================
{
public:
    int zoom = 100; // in percent

    QColor windowColor = QColor(129,129,129);
    QColor windowColorActive = QColor(61,61,61); // Border around window
    QColor panelColor = QColor(165,165,165);
    QColor headerColor = QColor(188,188,188);
    QColor headerColorActive = QColor(205,248,255); // light-blue
    QColor symbolColor = QColor(207,207,207);
    QColor symbolColorActive = QColor(255,185,1); // orange
    QColor comboColor = QColor(223,223,223);
    QColor padColor = QColor(36,36,36);
    QColor textColor = QColor(18,18,18);
    // QColor(36,36,36);

    //QColor m_primary;
    //QColor m_background;
    //QFont m_font;
};


// ============================================
class ISkinnable
// ============================================
{
public:
    virtual void applySkin() = 0;
};
