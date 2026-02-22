#pragma once
#include <HideOnCloseWidget.h>

#include <QApplication>
#include <QWidget>
#include <QTreeView>
#include <QAbstractListModel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QToolButton>
#include <QDoubleSpinBox>
#include <QColorDialog>
#include <QPainter>
#include <QFileDialog>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>
#include <QHeaderView>
#include <QStyle>
#include <QImage>

#include <vector>
#include <de_glm.hpp>

// Minimal glm::vec4 replacement if you don't want to pull glm in this snippet.
// Replace with real glm::vec4 in your code.
/*
namespace glm {
    struct vec4 {
        float r, g, b, a;
        vec4(float R = 0.f, float G = 0.f, float B = 0.f, float A = 1.f)
            : r(R), g(G), b(B), a(A) {}
    };
}
*/

struct ColorStop {
    glm::vec4 color;
    float t;
};

class GradientStopsModel : public QAbstractListModel
{
    Q_OBJECT
public:
    const std::vector<ColorStop> &stops() const;

    void resetStops(const std::vector<ColorStop>& newStops)
    {
        beginResetModel();
        m_stops = newStops;
        endResetModel();
    }

    explicit GradientStopsModel(std::vector<ColorStop> &stops, QObject *parent = nullptr);
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role) override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    QStringList mimeTypes() const override;
    QMimeData *mimeData(const QModelIndexList &indexes) const override;
    bool dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int, const QModelIndex &) override;
    Qt::DropActions supportedDropActions() const override;
    //bool insertRow(int row, const QModelIndex &parent = QModelIndex()) override;
    bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;
    //bool removeRow(int row, const QModelIndex &parent = QModelIndex()) override;
    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;

private:
    std::vector<ColorStop> m_stops;
};

// Simple gradient preview widget based on QImage
class GradientPreview : public QWidget
{
    Q_OBJECT
public:
    explicit GradientPreview(std::vector<ColorStop> &stops, QWidget *parent = nullptr);
public slots:
    void updateGradient();
protected:
    void paintEvent(QPaintEvent *) override;
private:
    std::vector<ColorStop> &m_stops;
};

class ControlWidget_ColorGradient : public HideOnCloseWidget
{
    Q_OBJECT
public:
    explicit ControlWidget_ColorGradient(QWidget *parent = nullptr);

    const std::vector<ColorStop> &stops() const;

private slots:
    void onCurrentChanged(const QModelIndex &current, const QModelIndex &);
    void onChooseColor();
    void onApplyEdit();
    void onAddStop();
    void onRemoveStop();
    void onMoveUp();
    void onMoveDown();
    void onNew();
    void onLoad();
    void onSave() ;

private:
    void moveRow(int from, int to);
    void updateColorButton();

    std::vector<ColorStop> m_stops;
    GradientStopsModel *m_model;
    QTreeView *m_view;
    GradientPreview *m_preview;

    QDoubleSpinBox *m_spinT;
    QPushButton *m_btnColor;
    QPushButton *m_btnApply;
    QPushButton *m_btnAdd;
    QPushButton *m_btnRemove;
    QPushButton *m_btnUp;
    QPushButton *m_btnDown;

    QColor m_currentColor;
};

/*
int main(int argc, char **argv)
{
    QApplication app(argc, argv);

    ControlWidget_ColorGradient w;
    w.show();

    return app.exec();
}
*/

// #include "main.moc"
