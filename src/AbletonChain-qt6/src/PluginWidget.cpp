#include "PluginWidget.h"

// ------------------------------------------------------------
// PluginWidget
// ------------------------------------------------------------
PluginWidget::PluginWidget(const QString &fileName, QWidget* parent)
    : QWidget(parent)
    , m_strFileName(fileName)
    , m_isDragging(false)
{
    setFixedSize(300, 200);

    m_fillColor = QColor(25,25,25);

    m_bg = QColor(
        QRandomGenerator::global()->bounded(256),
        QRandomGenerator::global()->bounded(256),
        QRandomGenerator::global()->bounded(256)
    );

    int brightness = (m_bg.red()*299 + m_bg.green()*587 + m_bg.blue()*114) / 1000;
    m_fg = (brightness < 128) ? Qt::white : Qt::black;

    setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this, &QWidget::customContextMenuRequested,
            this, &PluginWidget::showContextMenu);
}

QString
PluginWidget::fileName() const
{
    return m_strFileName;
}

void
PluginWidget::paintEvent(QPaintEvent *)
{
    QPainter dc(this);

    // [Draw] Panel
    dc.fillRect(rect(), m_fillColor);

    // [Draw] RoundRect
    dc.setPen(m_bg);
    dc.setBrush(m_bg);
    dc.drawRoundedRect(rect(), 16,16);

    // [Draw] Text
    dc.setPen(m_fg);
    dc.setFont(QFont("Arial", 14, QFont::Bold));
    auto r_body = QRect(rect().x() + 10,
                        rect().y() + 26,
                        rect().width() - 20,
                        rect().height() - 36);

    // [Draw] Text1
    auto s = QString("%1, Pos(%2,%3)")
            .arg(m_strIndex)
            .arg(m_pos.x())
            .arg(m_pos.y());

    dc.drawText(r_body, Qt::AlignTop | Qt::AlignHCenter | Qt::TextWordWrap, s);

    // [Draw] Text2
    dc.drawText(rect(), Qt::AlignCenter | Qt::TextWordWrap, m_strFileName);

    // [Draw] Text3
    dc.drawText(rect(), Qt::AlignBottom | Qt::AlignHCenter | Qt::TextWordWrap,
                QString("rect{%1,%2,%3,%4}")
                .arg(rect().x())
                .arg(rect().y())
                .arg(rect().width())
                .arg(rect().height()) );

    if (m_isDragging)
    {
        dc.setBrush(Qt::NoBrush);
        dc.setPen(QPen(QColor(255,255,255,200), 5.0));
        dc.drawRect(rect());
    }
}

void
PluginWidget::setIsDragging(bool isDragging)
{
    if (m_isDragging != isDragging)
    {
        m_isDragging = isDragging;
        update();
    }
}

void
PluginWidget::setPluginPos(int x, int y)
{
    m_pos = QPoint(x,y);
    update();
}

void
PluginWidget::setPluginIndex(int i)
{
    m_strIndex = QString::number(i);
    update();
}


void
PluginWidget::showContextMenu(const QPoint &pos)
{
    QMenu menu;
    QAction *removeAct = menu.addAction("Entfernen");
    QAction *chosen = menu.exec(mapToGlobal(pos));

    if (chosen == removeAct)
        emit requestRemoval(this);
}
