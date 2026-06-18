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
mkSvg(const QString &svg, int w, int h);

QPixmap
mkSvg_Power(int buttonWidth, const QColor& fillColor,
            int outlineWidth, const QColor& outlineColor);

QString
toSvg(const QColor& c);

QIcon
toQIcon(const QString& svg, int w, int h);

void
setButtonSvg(QPushButton* btn, const QString &svg, int w, int h);

void
setButtonPix(QPushButton* btn, const QPixmap &pix);

QColor
toQColor(uint32_t color);

void
drawKey(QPainter & dc, QRect pos, QColor brushColor, QColor penColor );

QColor
blendRGB(QColor from, QColor to, float t);

int
computeBestFontHeight(QFont baseFont, int maxHeight);

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

    bool bIsHeaderVisible = false;
    bool bIsClipEditorVisible = false;
    bool bIsDetailsVisible = false;
    bool bIsQuickHelpVisible = true;
    bool bIsFooterVisible = false;
};


// ============================================
class ISkinnable
// ============================================
{
public:
    virtual void applySkin() = 0;
};
