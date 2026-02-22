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

// ------------------------------------------------------------
// PluginWidget
// ------------------------------------------------------------
class PluginWidget : public QWidget
{
    Q_OBJECT
public:
    PluginWidget(const QString &fileName, QWidget *parent = nullptr);

    QString fileName() const;

protected:
    void paintEvent(QPaintEvent *) override;

public slots:
    void setIsDragging(bool);
    void setPluginPos(int x, int y);
    void setPluginIndex(int i);
private slots:
    void showContextMenu(const QPoint &pos);

signals:
    void requestRemoval(PluginWidget *self);

private:
    QString m_strFileName;
    QString m_strIndex;
    QPoint m_pos;
    QColor m_fillColor;
    QColor m_bg;
    QColor m_fg;
    bool m_isDragging;

};
